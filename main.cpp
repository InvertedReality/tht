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

#include <QDesktopServices>
#include <QTranslator>
#include <QSslSocket>
#include <QMetaType>
#include <QDateTime>
#include <QLocale>
#include <QFile>
#include <QDir>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QStandardPaths>
#include <QByteArray>
#endif

#include <cstdlib>
#include <cstdio>

#include <windows.h>

#include "qtsingleapplication.h"

#ifndef THT_NO_LOG
#include "qtlockedfile.h"
#endif

#include "linkpointsession.h"
#include "pluginloader.h"
#include "linkedwindow.h"
#include "linkpoint.h"
#include "settings.h"
#include "thttools.h"
#include "tools.h"
#include "tht.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
static void thtOutput(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    Q_UNUSED(context)
    QByteArray msg = message.toLatin1();
#else
static void thtOutput(QtMsgType type, const char *msg)
{
#endif
    Q_UNUSED(type)

    fprintf(stderr, "THT: %s\n", static_cast<const char *>(msg));

#ifndef THT_NO_LOG
    static bool noLog = qgetenv("THT_NO_LOG") == "1";

    if(noLog)
        return;

    static QtLockedFile log(
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                     QStandardPaths::writableLocation(QStandardPaths::TempLocation)
#else
                     QDesktopServices::storageLocation(QDesktopServices::TempLocation)
#endif
                     + QDir::separator()
                     + QCoreApplication::applicationName().toLower()
                     + ".log");

    static bool failed = false;

    if(!log.isOpen() && !failed)
    {
        failed = (!log.open(QIODevice::ReadWrite | QIODevice::Append)
                  || log.isLocked()
                  || !log.lock(QtLockedFile::WriteLock, false)
                  || !log.resize(0)
                  );

        if(failed)
            fprintf(stderr, "THT: Log file is unavailable\n");
    }

    if(!failed)
    {
        // truncate
        if(log.size() > 1*1024*1024) // 1 Mb
        {
            fprintf(stderr, "THT: Truncating log\n");

            log.seek(0);

            char buf[1024];
            int lines = 0;
            char c = 'x';

            while(log.readLine(buf, sizeof(buf)) > 0 && lines++ < 30)
            {}

            log.resize(log.pos());

            // check if '\n' is last
            if(log.seek(log.pos()-1))
            {
                log.getChar(&c);

                if(c != '\n')
                    log.write("\n");
            }

            log.write("...\n<overwrite>\n...\n");
        }

        log.write(static_cast<const char *>(msg));
        log.write("\n");
    }
#endif // THT_NO_LOG
}

static void copyDb()
{
    QString oldDb = Settings::instance()->mutableDatabasePath();
    QString newDb = oldDb + ".new";

    QString oldTs = oldDb + ".timestamp";
    QString newTs = oldTs + ".new";

    if(!QFile::exists(newDb) || !QFile::exists(newTs))
    {
        QFile::remove(newDb);
        QFile::remove(newTs);

        qDebug("No new database found locally");
        return;
    }

    qDebug("Copying new database");

    QFile::remove(oldDb);

    if(!QFile::copy(newDb, oldDb))
    {
        qDebug("Cannot copy new database");
        return;
    }

    QFile::remove(oldTs);

    if(!QFile::copy(newTs, oldTs))
        qDebug("Cannot copy new timestamp");

    QFile::remove(newDb);
    QFile::remove(newTs);
}

/******************************************/

int main(int argc, char *argv[])
{
    setbuf(stderr, 0);

    const qint64 v = QDateTime::currentMSecsSinceEpoch();

    QCoreApplication::setApplicationName("THT");
    QCoreApplication::setOrganizationName("THT");
    QCoreApplication::setApplicationVersion(NVER_STRING);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    qInstallMessageHandler(thtOutput);
#else
    qInstallMsgHandler(thtOutput);
#endif

    qDebug("Starting at %s", qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));

    QtSingleApplication app(argc, argv);

    if(Tools::hasOption("--preload"))
    {
        qDebug("Preloading");
        return 0;
    }

    AllowSetForegroundWindow(ASFW_ANY);

    const int isIpc = app.arguments().indexOf("--ipc");

    if(isIpc >= 0)
    {
        const QStringList arguments = app.arguments();
        QStringList ipcArguments;

        for(int i = isIpc+1;i < arguments.size();i++)
        {
            if(arguments.at(i).startsWith('-'))
                break;

            ipcArguments.append(arguments.at(i));
        }

        if(!ipcArguments.isEmpty())
            return app.sendMessage(ipcArguments.join(" "));
    }

    if(app.sendMessage("activate-window"))
        return 0;

    // workaround to speed up the SSL initialization
    const bool haveSsl = QSslSocket::supportsSsl();

    qDebug("SSL %s supported", haveSsl ? "is" : "is not");

    qRegisterMetaTypeStreamOperators<LinkPointSession>("LinkPointSession");
    qRegisterMetaTypeStreamOperators<QList<LinkPointSession> >("QList<LinkPointSession>");

    qRegisterMetaTypeStreamOperators<LinkedWindow>("LinkedWindow");
    qRegisterMetaTypeStreamOperators<QList<LinkedWindow> >("QList<LinkedWindow>");

    qRegisterMetaTypeStreamOperators<LinkPoint>("LinkPoint");
    qRegisterMetaTypeStreamOperators<QList<LinkPoint> >("QList<LinkPoint>");

    // plugins can register their own datastream operators,
    // so we need to be sure that all of them are loaded
    // before Settings is created
    PluginLoader::instance();

    // copy a new ticker database if any
    copyDb();

    // reread database timestamps
    Settings::instance()->rereadTimestamps();

    // load translations
    QString locale = QLocale::system().name();

    QString ts = SETTINGS_GET_STRING(SETTING_TRANSLATION);
    QString dir = QCoreApplication::applicationDirPath() + QDir::separator() + "translations";

    qDebug("Locale \"%s\", translation \"%s\"", qPrintable(locale), qPrintable(ts));

    ts = ts.isEmpty() ? locale : (ts + ".qm");

    QTranslator translator_tht_lib;
    qDebug("Loading THT library translation: %s", translator_tht_lib.load("tht_lib_" + ts, dir) ? "ok" : "failed");

    QTranslator translator_tht;
    qDebug("Loading THT translation: %s", translator_tht.load("tht_" + ts, dir) ? "ok" : "failed");

    QTranslator translator_qt;
    qDebug("Loading Qt translation: %s", translator_qt.load("qt_" + ts, dir) ? "ok" : "failed");

    app.setProperty("tht-translation", ts);
    app.installTranslator(&translator_qt);
    app.installTranslator(&translator_tht);
    app.installTranslator(&translator_tht_lib);

    app.setQuitOnLastWindowClosed(false);

    // application style
    THTTools::resetStyle(THTTools::ResetStyleOnError);

    // main window
    THT w;
    w.show();

    QObject::connect(&app, SIGNAL(messageReceived(QString)), &w, SLOT(slotMessageReceived(QString)));

    qDebug("Initialized in %ld ms.", static_cast<long int>(QDateTime::currentMSecsSinceEpoch() - v));

    int code = app.exec();

    PluginLoader::instance()->unload();

    qDebug("Goodbye at %s (exit code %d)", qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")), code);

    return code;
}
