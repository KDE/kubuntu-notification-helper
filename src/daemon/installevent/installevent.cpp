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

#include "installevent.h"

// Qt includes
#include <QtCore/QFile>

// Own includes
#include "installgui.h"

InstallEvent::InstallEvent(QObject* parent, const QString &name)
        : Event(parent, name)
        , m_applicationName()
        , m_multimediaPackages()
        , m_screensaverPackages()
        , m_packageList()
        , m_installGui(0)
{
    m_multimediaPackages["flashplugin-installer"] = i18nc("The name of the Adobe Flash plugin", "Flash");
    m_multimediaPackages["libxine1-ffmpeg"] = i18n("MPEG Plugins");
    m_multimediaPackages["libavcodec-extra-52"] = i18n("Video Encoding");
    m_multimediaPackages["libdvdread4"] = i18n("DVD Reading");
    m_multimediaPackages["libk3b6-extracodecs"] = i18n("K3b CD Codecs");
    m_multimediaPackages["libmp3lame0"] = i18n("MP3 Encoding");

    m_screensaverPackages["kscreensaver"] = i18n("Set of default screensavers");
}

InstallEvent::~InstallEvent()
{
}

void InstallEvent::show()
{
    QPixmap icon = KIcon("download").pixmap(NOTIFICATION_ICON_SIZE);
    QString text(i18nc("Notification when a package wants to install extra software",
                       "Extra packages can be installed to enhance functionality for %1",
                       m_applicationName));
    QStringList actions;
    actions << i18nc("Opens a dialog with more details", "Details");
    actions << i18nc("Button to dismiss this notification once", "Ignore for now");
    actions << i18nc("Button to make this notification never show up again",
                     "Never show again");

    Event::show(icon, text, actions);
}

void InstallEvent::addPackages(QMap<QString, QString>* packageList)
{
    QMap<QString, QString>::const_iterator packageIter = packageList->constBegin();
    QMap<QString, QString>::const_iterator end = packageList->constEnd();
    for ( ; packageIter != end; ++packageIter) {
        // check for .md5sums as .list exists even when the package is removed (but not purged)
        if (!QFile::exists("/var/lib/dpkg/info/" + packageIter.key() + ".md5sums")) {
            m_packageList[packageIter.key()] = packageIter.value();
        }
    }
}

void InstallEvent::getInfo(const QString application, const QString package)
{
    m_applicationName = application;
    m_packageList.clear();

    if (m_multimediaPackages.contains(package)) {
        addPackages(&m_multimediaPackages);
    } else if (m_screensaverPackages.contains(package)) {
        addPackages(&m_screensaverPackages);
    }

    if (!m_packageList.isEmpty()) {
       show();
    }
}

void InstallEvent::run()
{
    m_installGui = new InstallGui(this, m_applicationName, m_packageList);
    Event::run();
}

#include "installevent.moc"
