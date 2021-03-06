/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2012 by Dmitry 'Krasu' Baryshev                         *
 *   dmitrymq@gmail.com                                                    *
 ***************************************************************************/

// This TU looks like a crap

#include <QDesktopWidget>
#include <QTextDocument>
#include <QStylePainter>
#include <QApplication>
#include <QStyleOption>
#include <QPointer>
#include <QEvent>
#include <QLabel>
#include <QStyle>
#include <QTimer>

#include "tickerinformationtooltip.h"
#include "tickerinformationfetcher.h"

class TickerInformationToolTipLabel : public QLabel
{
    Q_OBJECT

public:
    TickerInformationToolTipLabel(const QString &text, bool ticker, bool persist, QWidget *w);
    ~TickerInformationToolTipLabel();

    static TickerInformationToolTipLabel *instance;

    bool eventFilter(QObject *, QEvent *);

    QBasicTimer hideTimer, expireTimer;
    bool fadingOut;
    bool persistent;

    void reuseTip(const QString &text, bool isTicker);
    void hideTip();
    void hideTipImmediately();
    void restartExpireTimer();
    bool tipChanged(const QString &text);
    void placeTip(const QPoint &pos);

    static int getTipScreen(const QPoint &pos);

protected:
    void timerEvent(QTimerEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

public slots:
    void styleSheetParentDestroyed()
    {
        setProperty("_q_stylesheet_parent", QVariant());
        styleSheetParent = 0;
    }

private slots:
    void slotFetcherDone(const QString &, const QString &, const QString &, const QString &);

private:
    QWidget *styleSheetParent;
    TickerInformationFetcher *fetcher;
    QString data;
    QPoint lastPos;
    QString ticker;
};

TickerInformationToolTipLabel *TickerInformationToolTipLabel::instance = 0;

TickerInformationToolTipLabel::TickerInformationToolTipLabel(const QString &text, bool ticker, bool persist, QWidget *w)
    : QLabel(w, Qt::ToolTip | Qt::BypassGraphicsProxyWidget), styleSheetParent(0)
{
    delete TickerInformationToolTipLabel::instance;
    TickerInformationToolTipLabel::instance = this;

    fetcher = new TickerInformationFetcher(this);

    connect(fetcher, SIGNAL(done(QString,QString,QString,QString)),
            this, SLOT(slotFetcherDone(QString,QString,QString,QString)));

    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    ensurePolished();
    setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
    setFrameStyle(QFrame::NoFrame);
    setAlignment(Qt::AlignLeft);
    setIndent(1);
    qApp->installEventFilter(this);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / qreal(255.0));
    setMouseTracking(true);
    fadingOut = false;
    persistent = persist;
    reuseTip(text, ticker);
}

void TickerInformationToolTipLabel::restartExpireTimer()
{
    int time = 10000 + 40 * qMax(0, text().length()-100);
    expireTimer.start(time, this);
    hideTimer.stop();
}

void TickerInformationToolTipLabel::reuseTip(const QString &text, bool isTicker)
{
    if(styleSheetParent)
    {
        disconnect(styleSheetParent, SIGNAL(destroyed()),
                   TickerInformationToolTipLabel::instance, SLOT(styleSheetParentDestroyed()));
        styleSheetParent = 0;
    }

    setWordWrap(Qt::mightBeRichText(text));
    setText(isTicker ? (text + "...") : text);

    QFontMetrics fm(font());
    QSize extra(1, 0);

    // make it look good with the default ToolTip font on Mac, which has a small descent.
    if(fm.descent() == 2 && fm.ascent() >= 11)
        ++extra.rheight();

    resize(sizeHint() + extra);

    if(!isTicker)
        return;

    ticker = QString(text).replace('.', '-');

    fetcher->fetch(ticker);
}

void TickerInformationToolTipLabel::slotFetcherDone(const QString &name,
                                                    const QString &exchange,
                                                    const QString &sector,
                                                    const QString &industry)
{
    if(name.isEmpty())
    {
        //: In the neuter, means "Something is not found"
        TickerInformationToolTip::showText(QPoint(), tr("Not found"), false);
        restartExpireTimer();
        return;
    }

    TickerInformationToolTip::showText(QPoint(), name + '\n' + exchange + " / " + sector + " / " + industry, false);
    restartExpireTimer();
}

void TickerInformationToolTipLabel::paintEvent(QPaintEvent *ev)
{
    QStylePainter p(this);
    QStyleOptionFrame opt;

    opt.init(this);

    p.drawPrimitive(QStyle::PE_PanelTipLabel, opt);
    p.end();

    QLabel::paintEvent(ev);
}

void TickerInformationToolTipLabel::resizeEvent(QResizeEvent *e)
{
    QStyleHintReturnMask frameMask;
    QStyleOption option;

    option.init(this);

    if(style()->styleHint(QStyle::SH_ToolTip_Mask, &option, this, &frameMask))
        setMask(frameMask.region);

    QLabel::resizeEvent(e);
}

TickerInformationToolTipLabel::~TickerInformationToolTipLabel()
{
    TickerInformationToolTipLabel::instance = 0;
    qDebug("Closing ticker info");
}

void TickerInformationToolTipLabel::hideTip()
{
    if(!hideTimer.isActive())
        hideTimer.start(300, this);
}

void TickerInformationToolTipLabel::hideTipImmediately()
{
    close(); // to trigger QEvent::Close which stops the animation
    deleteLater();
}

void TickerInformationToolTipLabel::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == hideTimer.timerId() || e->timerId() == expireTimer.timerId())
    {
        hideTimer.stop();
        expireTimer.stop();
        hideTipImmediately();
    }
}

bool TickerInformationToolTipLabel::eventFilter(QObject *o, QEvent *e)
{
    switch(e->type())
    {
        case QEvent::Leave:
            hideTip();
        break;

        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::Wheel:
        case QEvent::KeyPress:
            hideTipImmediately();
        break;

        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
            if(!persistent)
                hideTipImmediately();
        break;

        default:
        break;
    }

    return QObject::eventFilter(o, e);
}

inline
int TickerInformationToolTipLabel::getTipScreen(const QPoint &pos)
{
    return QApplication::desktop()->isVirtualDesktop()
            ? QApplication::desktop()->screenNumber(pos)
            : QApplication::desktop()->screenNumber();
}

void TickerInformationToolTipLabel::placeTip(const QPoint &ps)
{
#ifndef QT_NO_STYLE_STYLESHEET
    if(testAttribute(Qt::WA_StyleSheet))
    {
        //the stylesheet need to know the real parent
        TickerInformationToolTipLabel::instance->setProperty("_q_stylesheet_parent", QVariant::fromValue(0));

        //we force the style to be the QStyleSheetStyle, and force to clear the cache as well.
        TickerInformationToolTipLabel::instance->setStyleSheet(QLatin1String("/* */"));

        // Set up for cleaning up this later...
        TickerInformationToolTipLabel::instance->styleSheetParent = 0;
    }
#endif

    QPoint pos = ps.isNull() ? lastPos : ps;
    QRect screen = QApplication::desktop()->screenGeometry(getTipScreen(pos));

    lastPos = pos;

    if(pos.x() + this->width() > screen.x() + screen.width())
        pos.rx() -= 4 + this->width();
    if(pos.y() + this->height() > screen.y() + screen.height())
        pos.ry() -= 24 + this->height();
    if(pos.y() < screen.y())
        pos.setY(screen.y());
    if(pos.x() + this->width() > screen.x() + screen.width())
        pos.setX(screen.x() + screen.width() - this->width());
    if(pos.x() < screen.x())
        pos.setX(screen.x());
    if(pos.y() + this->height() > screen.y() + screen.height())
        pos.setY(screen.y() + screen.height() - this->height());

    move(pos);
}

bool TickerInformationToolTipLabel::tipChanged(const QString &text)
{
    if(TickerInformationToolTipLabel::instance->text() != text)
        return true;

    return false;
}

void TickerInformationToolTip::showText(const QPoint &pos, const QString &text, bool ticker)
{
    TickerInformationToolTip::showTextPrivate(pos, text, ticker, false);
}

void TickerInformationToolTip::showPersistentText(const QPoint &pos, const QString &text)
{
    TickerInformationToolTip::showTextPrivate(pos, text, false, true);
}

void TickerInformationToolTip::showTextPrivate(const QPoint &pos, const QString &text, bool ticker, bool persist)
{
    if(TickerInformationToolTipLabel::instance && TickerInformationToolTipLabel::instance->isVisible())
    {
        if(text.isEmpty())
        {
            TickerInformationToolTipLabel::instance->hideTip();
            return;
        }
        else if(!TickerInformationToolTipLabel::instance->fadingOut)
        {
            if(TickerInformationToolTipLabel::instance->tipChanged(text))
            {
                TickerInformationToolTipLabel::instance->persistent = persist;
                TickerInformationToolTipLabel::instance->reuseTip(text, ticker);
                TickerInformationToolTipLabel::instance->placeTip(pos);
            }

            return;
        }
    }

    if(!text.isEmpty())
    {
        new TickerInformationToolTipLabel(text,
                                          ticker,
                                          persist,
                                          QApplication::desktop()->screen(TickerInformationToolTipLabel::getTipScreen(pos)));

        TickerInformationToolTipLabel::instance->placeTip(pos);
        TickerInformationToolTipLabel::instance->setObjectName(QLatin1String("tooltip"));
        TickerInformationToolTipLabel::instance->show();

        if(!ticker && persist)
            TickerInformationToolTipLabel::instance->restartExpireTimer();
    }
}

#include "tickerinformationtooltip.moc"
