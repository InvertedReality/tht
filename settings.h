/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStringList>
#include <QDataStream>
#include <QByteArray>
#include <QDateTime>
#include <QSettings>
#include <QVariant>
#include <QString>
#include <QPoint>
#include <QList>
#include <QHash>
#include <QMap>

Q_DECLARE_METATYPE(QList<QPoint>)
Q_DECLARE_METATYPE(Qt::AlignmentFlag)

#define SETTINGS_GET_BOOL Settings::instance()->value<bool>
#define SETTINGS_SET_BOOL Settings::instance()->setValue<bool>

#define SETTINGS_GET_INT Settings::instance()->value<int>
#define SETTINGS_SET_INT Settings::instance()->setValue<int>

#define SETTINGS_GET_UINT Settings::instance()->value<uint>
#define SETTINGS_SET_UINT Settings::instance()->setValue<uint>

#define SETTINGS_GET_LONG Settings::instance()->value<long>
#define SETTINGS_SET_LONG Settings::instance()->setValue<long>

#define SETTINGS_GET_ULONG Settings::instance()->value<unsigned long>
#define SETTINGS_SET_ULONG Settings::instance()->setValue<unsigned long>

#define SETTINGS_GET_LONG_LONG Settings::instance()->value<qlonglong>
#define SETTINGS_SET_LONG_LONG Settings::instance()->setValue<qlonglong>

#define SETTINGS_GET_ULONG_LONG Settings::instance()->value<qulonglong>
#define SETTINGS_SET_ULONG_LONG Settings::instance()->setValue<qulonglong>

#define SETTINGS_GET_FLOAT Settings::instance()->value<float>
#define SETTINGS_SET_FLOAT Settings::instance()->setValue<float>

#define SETTINGS_GET_DOUBLE Settings::instance()->value<double>
#define SETTINGS_SET_DOUBLE Settings::instance()->setValue<double>

#define SETTINGS_GET_DATE Settings::instance()->value<QDate>
#define SETTINGS_SET_DATE Settings::instance()->setValue<QDate>

#define SETTINGS_GET_TIME Settings::instance()->value<QTime>
#define SETTINGS_SET_TIME Settings::instance()->setValue<QTime>

#define SETTINGS_GET_DATE_TIME Settings::instance()->value<QDateTime>
#define SETTINGS_SET_DATE_TIME Settings::instance()->setValue<QDateTime>

#define SETTINGS_GET_BYTE_ARRAY Settings::instance()->value<QByteArray>
#define SETTINGS_SET_BYTE_ARRAY Settings::instance()->setValue<QByteArray>

#define SETTINGS_GET_BIT_ARRAY Settings::instance()->value<QBitArray>
#define SETTINGS_SET_BIT_ARRAY Settings::instance()->setValue<QBitArray>

#define SETTINGS_GET_CHAR Settings::instance()->value<QChar>
#define SETTINGS_SET_CHAR Settings::instance()->setValue<QChar>

#define SETTINGS_GET_STRING Settings::instance()->value<QString>
#define SETTINGS_SET_STRING Settings::instance()->setValue<QString>

#define SETTINGS_GET_STRING_LIST Settings::instance()->value<QStringList>
#define SETTINGS_SET_STRING_LIST Settings::instance()->setValue<QStringList>

#define SETTINGS_GET_COLOR Settings::instance()->value<QColor>
#define SETTINGS_SET_COLOR Settings::instance()->setValue<QColor>

#define SETTINGS_GET_ALIGNMENT_FLAG Settings::instance()->value<Qt::AlignmentFlag>
#define SETTINGS_SET_ALIGNMENT_FLAG Settings::instance()->setValue<Qt::AlignmentFlag>

#define SETTINGS_GET_POINTS Settings::instance()->value<QList<QPoint> >
#define SETTINGS_SET_POINTS Settings::instance()->setValue<QList<QPoint> >

#define SETTINGS_GET_SIZE Settings::instance()->value<QSize>
#define SETTINGS_SET_SIZE Settings::instance()->setValue<QSize>

#define SETTINGS_GET_SIZEF Settings::instance()->value<QSizeF>
#define SETTINGS_SET_SIZEF Settings::instance()->setValue<QSizeF>

#define SETTINGS_GET_POINT Settings::instance()->value<QPoint>
#define SETTINGS_SET_POINT Settings::instance()->setValue<QPoint>

#define SETTINGS_GET_POINTF Settings::instance()->value<QPointF>
#define SETTINGS_SET_POINTF Settings::instance()->setValue<QPointF>

#define SETTINGS_GET_RECT Settings::instance()->value<QRect>
#define SETTINGS_SET_RECT Settings::instance()->setValue<QRect>

#define SETTINGS_GET_RECTF Settings::instance()->value<QRectF>
#define SETTINGS_SET_RECTF Settings::instance()->setValue<QRectF>

#define SETTINGS_GET_LINE Settings::instance()->value<QLine>
#define SETTINGS_SET_LINE Settings::instance()->setValue<QLine>

#define SETTINGS_GET_LINEF Settings::instance()->value<QLineF>
#define SETTINGS_SET_LINEF Settings::instance()->setValue<QLineF>

#define SETTINGS_GET_URL Settings::instance()->value<QUrl>
#define SETTINGS_SET_URL Settings::instance()->setValue<QUrl>

#define SETTINGS_REMOVE Settings::instance()->remove

#define SETTING_RESTORE_LINKS_AT_STARTUP     "restore-links-at-startup"
#define SETTING_SHOW_COMMENTS                "show-comments"
#define SETTING_LIST_HEADER                  "list-header"
#define SETTING_LIST_BUTTONS                 "list-buttons"
#define SETTING_TRANSLATION                  "translation"
#define SETTING_STYLE                        "style"
#define SETTING_RESTORE_NEIGHBORS_AT_STARTUP "restore-neighbors-at-startup"
#define SETTING_SHOW_NEIGHBORS_AT_STARTUP    "neighbors-at-startup"
#define SETTING_MINI_TICKER_ENTRY            "mini-ticker-entry"
#define SETTING_ALLOW_DUPLICATES             "allow-duplicates"
#define SETTING_ONTOP                        "ontop"
#define SETTING_HIDE_TO_TRAY                 "tray"
#define SETTING_SAVE_GEOMETRY                "save-geometry"
#define SETTING_SIZE                         "size"
#define SETTING_POSITION                     "position"
#define SETTING_SAVE_TICKERS                 "save-tickers"
#define SETTING_GLOBAL_HOTKEY_SCREENSHOT     "global-hotkey-screenshot"
#define SETTING_GLOBAL_HOTKEY_RESTORE        "global-hotkey-restore"
#define SETTING_IS_WINDOW_SQUEEZED           "is-window-squeezed"

class SettingsPrivate;

/*
 *  Class to query the application settings. You can use
 *  the macroses above to query the appropriate values
 *  like that:
 *
 *      qDebug() << SETTINGS_GET_STRING(SETTING_TRANSLATION);
 */
class Settings
{
public:
    static Settings* instance();

    ~Settings();

    enum SyncType { NoSync, Sync };

    /*
     *  Get the key value. If the value is not found, then
     *  the appropriate value from the known default values
     *  is returned. If the default value for the key is unknown,
     *  the C++ default value is returned ('0' for 'int', 'false' for 'bool' etc.)
     */
    template <typename T>
    T value(const QString &key);

    /*
     *  Get the key value. Return 'def' if the key is not found
     */
    template <typename T>
    T value(const QString &key, const T &def);

    /*
     *  Same as value(key), but will save the key as a serialized
     *  QByteArray. Sometimes this is needed for plugins due to QSettings limitation.
     *  Consider the following example:
     *
     *  1) you wrote a plugin with just one complex setting, say MyCustomStruct
     *  2) you registered MyCustomStruct with Q_DECLARE_METATYPE and wrote the data stream operators,
     *     so your type is working fine with QSettings
     *  3) somewhere in the plugin code you set the key with the type MyCustomStruct, for example
     *     MyCustomStruct m = ...;
     *     Settings::instance()->setValue<MyCustomStruct>("my", m);
     *  4) run THT and test, everything works fine
     *  5) now remove your plugin from plugins/ subdirectory in the THT distribution
     *  6) run THT, then close it
     *  7) bring the plugin back to plugins/ subdirectory in the THT distribution
     *  8) run THT again and try to read the key "my"
     *
     *  Result: your setting is lost. Why? When you removed the plugin you also
     *          removed the data stream operators for the type MyCustomStruct,
     *          and QSettings doesn't know what to do with the unknown key "my",
     *          and thus writes an invalid QVariant instead of it.
     *
     *  Solution: serialize the key "my" into QByteArray and write QByteArray instead of
     *            actual value.
     *
     *  API: binaryValue() to read such keys, setBinaryValue() to write such keys
     */
    template <typename T>
    T binaryValue(const QString &key);

    /*
     *  Same as value(key, def), but will save the key as a serialized
     *  QByteArray
     */
    template <typename T>
    T binaryValue(const QString &key, const T &def);

    /*
     *  Set the value of the key. If 'sync' is 'Sync', then call sync()
     */
    template <typename T>
    void setValue(const QString &key, const T &value, SyncType sync = Sync);

    /*
     *  Same as setValue(key, def), but will save the key as a serialized
     *  QByteArray
     */
    template <typename T>
    void setBinaryValue(const QString &key, const T &value, SyncType sync = Sync);

    /*
     *  Add your default values for your settings
     */
    void addDefaultValues(const QHash<QString, QVariant> &defaultValues);

    /*
     *  Returns the default value of the setting 'key'. Returns
     *  an invalid QVariant if the settings is not found in the
     *  default values
     */
    QVariant defaultValue(const QString &key) const;

    /*
     *  Returns 'true' if the setting 'key' exists. If 'key' starts with '/'
     *  the setting is returned from "settings" group (almost all settings are stored there).
     *
     *  For example:
     *
     *  contains("list")  - will look for the "list" key in the global section
     *  contains("/list") - will look for the "list" key in the "settings" section
     */
    bool contains(const QString &key) const;

    /*
     *  Remove the specified key from the section "settings"
     */
    void remove(const QString &key, SyncType sync = Sync);

    /*
     *  Sync with the storage
     */
    void sync();

    /*
     *  Available translations, hardcoded
     */
    QMap<QString, QString> translations();

    void rereadTimestamps();

    QString databaseTimestampFormat() const;

    QDateTime persistentDatabaseTimestamp() const;
    QDateTime mutableDatabaseTimestamp() const;

    QString persistentDatabaseName() const;
    QString persistentDatabasePath() const;

    QString mutableDatabaseName() const;
    QString mutableDatabasePath() const;

    void setCheckBoxState(const QString &checkbox, bool checked, SyncType sync = Sync);
    int checkBoxState(const QString &checkbox);

    int maximumNumberOfLists() const;
    void setNumberOfLists(int, SyncType sync = Sync);
    int numberOfLists();

    void setTickersForGroup(int group, const QStringList &tickers, SyncType sync = Sync);
    QStringList tickersForGroup(int group);
    void removeTickers(int group, SyncType sync = Sync);

    void setHeaderForGroup(int group, const QString &header, SyncType sync = Sync);
    QString headerForGroup(int group);

private:
    Settings();

    QDateTime readTimestamp(const QString &fileName) const;

    void fillTranslations();

    QHash<QString, QVariant> &defaultValues();

    QSettings *settings();

private:
    SettingsPrivate *d;
};

/**********************************/

template <typename T>
T Settings::value(const QString &key)
{
    T def = T();
    QHash<QString, QVariant>::iterator it = defaultValues().find(key);

    if(it != defaultValues().end())
        def = it.value().value<T>();

    return value<T>(key, def);
}

template <typename T>
T Settings::value(const QString &key, const T &def)
{
    QSettings *s = settings();

    s->beginGroup("settings");
    QVariant value = s->value(key, QVariant::fromValue(def));
    s->endGroup();

    return value.value<T>();
}

template <typename T>
T Settings::binaryValue(const QString &key)
{
    T def = T();
    QHash<QString, QVariant>::iterator it = defaultValues().find(key);

    if(it != defaultValues().end())
        def = it.value().value<T>();

    return binaryValue<T>(key, def);
}

template <typename T>
T Settings::binaryValue(const QString &key, const T &def)
{
    T result = def;
    QByteArray bin = value<QByteArray>(key);

    if(bin.isEmpty())
        return result;

    QDataStream ds(&bin, QIODevice::ReadOnly);
    ds.setVersion(QDataStream::Qt_4_0);
    ds >> result;

    return result;
}

template <typename T>
void Settings::setValue(const QString &key, const T &value, Settings::SyncType sync)
{
    QSettings *s = settings();

    s->beginGroup("settings");
    s->setValue(key, QVariant::fromValue(value));
    s->endGroup();

    if(sync == Sync)
        s->sync();
}

template <typename T>
void Settings::setBinaryValue(const QString &key, const T &value, Settings::SyncType sync)
{
    QByteArray bin;
    QDataStream ds(&bin, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_4_0);
    ds << value;

    setValue(key, bin, sync);
}

#endif // SETTINGS_H
