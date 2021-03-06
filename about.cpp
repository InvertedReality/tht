/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QKeySequence>
#include <QShortcut>
#include <QTimer>
#include <QMovie>

#include "updatechecker.h"
#include "settings.h"
#include "thttools.h"
#include "tools.h"
#include "about.h"

#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About),
    m_showExt(true)
{
    setObjectName("about");
    setWindowTitle(THTTools::aboutThtTitle());

    ui->setupUi(this);

    ui->pushOk->setText(Tools::oKTitle());

    ui->label->setPixmap(QIcon(":/images/chart.ico").pixmap(48, 48));
    ui->labelVersion->setText(QString("THT %1 (THT-lib %2)").arg(NVER_STRING).arg(Tools::thtLibVersion()));

    ui->labelCopyright->setText(QString("(C) 2012 %1 &lt;<a href=\"mailto:dmitrymq@gmail.com\">"
                                        "dmitrymq@gmail.com</a>&gt;").arg(tr("Dmitry Baryshev")));

    ui->labelUrl->setText(QString("<a href=\"%1\">%2</a> <a href=\"%3\">%4</a> <a href=\"%5\">%6</a> <a href=\"%7\">%8</a>")
                            .arg(HTTPROOT "/blob/master/HOWTO.md")
                            .arg(tr("Documentation"))
                            .arg(DOWNLOADROOT)
                            //: Noun in the plural
                            .arg(tr("Downloads"))
                            .arg("http://www.transifex.com/projects/p/traders-home-task")
                            .arg(tr("Translations"))
                            .arg("https://www.youtube.com/user/thtng")
                            .arg(tr("Youtube"))
                            );

    // set "wait" icon
    m_timer = new QTimer(this);

    m_timer->setSingleShot(true);
    m_timer->setInterval(750);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotSetWaitIcon()));

    // extension
    QLabel *label = new QLabel(QString(
                                   "<table align=center>"
                                   "<tr><td align=right><b>%1: </b></td><td>%2</td></tr>"
                                   "<tr><td align=right><b>%3: </b></td><td>%4</td></tr>"
                                   "</table>"
                                   )
                               //: Means "Fixed database on the hard drive"
                               .arg(tr("Persistent database"))
                               .arg(Settings::instance()->persistentDatabaseTimestamp().toString(Settings::instance()->databaseTimestampFormat()))
                               //: Means "Database which is downloaded from the website"
                               .arg(tr("Downloadable database"))
                               .arg(Settings::instance()->mutableDatabaseTimestamp().toString(Settings::instance()->databaseTimestampFormat()))
                               );

    label->setObjectName("aboutExtension");
    label->setContentsMargins(0, 0, 0, 12);

    setOrientation(Qt::Vertical);
    setExtension(label);

    new QShortcut(QKeySequence::HelpContents, this, SLOT(slotExtendedAbout()));

    // update checker
    m_checker = new UpdateChecker(this);

    connect(m_checker, SIGNAL(newVersion(QString)), this, SLOT(slotNewVersion(QString)));
    connect(m_checker, SIGNAL(error(QString)), this, SLOT(slotError(QString)));

    m_checker->start();
    m_timer->start();
}

About::~About()
{
    delete ui;
}

void About::slotNewVersion(const QString &newVersion)
{
    QString tooltip;
    QString text;

    m_timer->stop();

    if(!newVersion.isEmpty())
    {
        text = "<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
                "<body><a href=\"" DOWNLOADROOT "\">"
                    "<img src=\":/images/update.png\"></img>"
                "</a>"
                "</body></html>";
        //: Means "A new version of THT is availabe at the website"
        tooltip = tr("Update available");
    }
    else
    {
        text = "<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
                "<body>"
                "<img src=\":/images/ok.png\"></img>"
                "</body></html>";
        //: "THT" is the name of the application. THT will check for updates and show this message when there are no new version available
        tooltip = tr("THT is up to date");
    }

    delete ui->labelUpdate->movie();

    ui->labelUpdate->setText(text);
    ui->labelUpdate->setToolTip(tooltip);
}

void About::slotError(const QString &err)
{
    m_timer->stop();

    delete ui->labelUpdate->movie();

    ui->labelUpdate->setPixmap(QPixmap(":/images/error.png"));
    //: %1 will be replaced with the error code by the application. It will look like "Cannot check for updates (Server is unavailable)"
    ui->labelUpdate->setToolTip(tr("Cannot check for updates (%1)").arg(err));
}

void About::slotExtendedAbout()
{
    showExtension(m_showExt);
    m_showExt = !m_showExt;
}

void About::slotSetWaitIcon()
{
    QMovie *movie = new QMovie(":/images/wait.gif", "GIF", this);
    ui->labelUpdate->setMovie(movie);
    movie->start();

    ui->labelUpdate->setToolTip(tr("Checking for updates..."));
}
