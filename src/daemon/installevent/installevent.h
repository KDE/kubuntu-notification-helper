/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#ifndef INSTALLEVENT_H
#define INSTALLEVENT_H

#include "../event.h"

// Qt includes
#include <QtCore/QMap>

class InstallGui;

class InstallEvent : public Event
{
    Q_OBJECT
public:
    InstallEvent(QObject* parent, const QString &name);

    virtual ~InstallEvent();

public slots:
    void getInfo(const QString application, const QString package);
    void show();

private slots:
    void run();
    void addPackages(QMap<QString, QString>* packageList);

private:
    QString m_applicationName;
    QMap<QString, QString> m_multimediaPackages;
    QMap<QString, QString> m_screensaverPackages;
    QMap<QString, QString> m_packageList;
    InstallGui* m_installGui;
};

#endif
