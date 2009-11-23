/***************************************************************************
 *   Copyright © 2009 by Jonathan Thomas <echidnaman@kubuntu.org>          *
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

#include "installevent.h"

InstallEvent::InstallEvent(QObject* parent, QString name)
        : Event(parent, name)
        , applicationName(0)
        , packageName(0)
        , iGui(0)
{}

InstallEvent::~InstallEvent()
{
    delete iGui;
}

void InstallEvent::show()
{
    QPixmap icon = KIcon("download").pixmap(48, 48);
    QString text(i18nc("Notification when a package wants to install extra software", "Extra packages for restricted multimedia functionality are available"));
    QStringList actions;
    actions << i18nc("Opens a dialog with more details", "Details");
    actions << i18nc("User declines an action", "Ignore");
    actions << i18nc("User indicates he never wants to see this notification again", "Ignore forever");

    Event::show(icon, text, actions);
}

void InstallEvent::getInfo(const QString application, const QString package)
{
    applicationName = application;
    packageName = package;

    // My super funtime pseudocode-so-I-don't-forget-overnight routine
    // Check to see if we have packages to offer
    // bool anythingToShow = checkIfAnythingToShow();

    // If we do, show the user, populate list for iGui
    // if (anythingToShow) {
    // QStringList packageList = getPackageList();
    // show();
    // }
}

void InstallEvent::run()
{
    // TODO: Pass QStringList of packages to install to Gui
    iGui = new InstallGui(this);
    Event::run();
}

#include "installevent.moc"
