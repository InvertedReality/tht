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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>

namespace Ui
{
    class Options;
}

class QListWidgetItem;

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = 0);
    ~Options();

    void saveSettings() const;

private:
    void load();
    void setIcon(QListWidgetItem *i, const QString &rcIcon, int width);

private slots:
    void slotSomethingImportantChanged();

private:
    Ui::Options *ui;
    int m_startTranslationIndex;
    int m_startStyleIndex;
    QString m_currentStyle;
};

#endif // OPTIONS_H
