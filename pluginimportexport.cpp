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

#include <QApplication>
#include <QAction>
#include <QWidget>
#include <QMenu>

#include "pluginimportexport.h"

class PluginImportExportPrivate
{
public:
    PluginImportExport::Embeds embeds;
};

/**************************************/

PluginImportExport::PluginImportExport() :
    Plugin()
{
    d = new PluginImportExportPrivate;
}

PluginImportExport::~PluginImportExport()
{
    Embeds::const_iterator itEnd = d->embeds.end();

    for(Embeds::const_iterator it = d->embeds.begin();it != itEnd;++it)
    {
        delete it.value();
    }

    delete d;
}

bool PluginImportExport::unembed(int list)
{
    Embeds::iterator it = d->embeds.find(list);

    if(it == d->embeds.end())
        return false;

    qDebug("Unembedding plugin \"%s\" from list #%d", qPrintable(property(THT_PLUGIN_PROPERTY_NAME).toString()), list);

    delete it.value();
    d->embeds.erase(it);

    return true;
}

QList<Hotkey> PluginImportExport::supportedHotkeysInList() const
{
    return QList<Hotkey>();
}

void PluginImportExport::listHotkeyActivated(int list, const Hotkey &ke)
{
    Q_UNUSED(list)
    Q_UNUSED(ke)
}

bool PluginImportExport::exportTickers(const QStringList &tickers)
{
    Q_UNUSED(tickers)

    return true;
}

PluginImportExport::Embeds& PluginImportExport::embeds() const
{
    return d->embeds;
}

int PluginImportExport::senderStandaloneActionToList() const
{
    int list = -1;

    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return list;

    Embeds::const_iterator itEnd = d->embeds.end();

    for(Embeds::const_iterator it = d->embeds.begin();it != itEnd;++it)
    {
        if(it.value() == a)
        {
            list = it.key();
            break;
        }
    }

    return list;
}

int PluginImportExport::senderMenuActionToList() const
{
    int list = -1;

    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return list;

    QMenu *menu = qobject_cast<QMenu *>(a->parent());

    if(!menu)
        return list;

    Embeds::const_iterator itEnd = d->embeds.end();

    for(Embeds::const_iterator it = d->embeds.begin();it != itEnd;++it)
    {
        if(it.value() == menu)
        {
            list = it.key();
            break;
        }
    }

    return list;
}
