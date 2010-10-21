/***************************************************************************
 *   Copyright © 2009-2010 Jonathan Thomas <echidnaman@kubuntu.org>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef INSTALLGUI_H
#define INSTALLGUI_H

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtGui/QListWidget>

class QProcess;
class KDialog;

class InstallGui : public QObject
{
    Q_OBJECT
public:
    InstallGui(QObject* parent, const QString &application,
               const QMap<QString, QString> packageList);
    ~InstallGui();

private slots:
    void packageToggled(QListWidgetItem *item);
    void runPackageInstall();
    void installFinished();
    void cleanUpDialog();

private:
    KDialog *m_dialog;
    QProcess *m_installProcess;
    QString m_applicationName;
    QStringList m_toInstallList;
};

#endif
