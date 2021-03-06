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

#ifndef REMOTEDATE_H
#define REMOTEDATE_H

#include <QDateTime>

#include <windows.h>

class QString;

class RemoteDatePrivate;

/*
 *  Class to get a date and time in a foreign time zone. Zone names
 *  can be taken from HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Time Zones\
 */
class RemoteDate
{
public:
    RemoteDate(const QString &zone);
    ~RemoteDate();

    /*
     * Current date and time in timezone 'zone'
     */
    QDateTime dateTime();

private:
    TCHAR *stringToTChar(const QString &s) const;

private:
    RemoteDatePrivate *d;
};

#endif // REMOTEDATE_H
