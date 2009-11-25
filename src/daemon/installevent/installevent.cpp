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

// Qt includes
#include <QtCore/QFile>

// Own includes
#include "installgui.h"

InstallEvent::InstallEvent(QObject* parent, QString name)
        : Event(parent, name)
        , m_applicationName(0)
        , m_multimediaCategory()
        , m_semanticCategory()
        , m_packageList()
        , m_installGui(0)
{
    m_multimediaCategory["flashplugin-installer"] = i18nc("The name of the Adobe Flash plugin", "Flash");
    m_multimediaCategory["libxine1-ffmpeg"] = i18n("MPEG Plugins");
    m_multimediaCategory["libavcodec-unstripped-52"] = i18n("Video Codecs");
    m_multimediaCategory["libdvdread4"] = i18n("DVD Reading");
    m_multimediaCategory["libk3b6-extracodecs"] = i18n("K3b CD Codecs");
    m_multimediaCategory["libmp3lame0"] = i18n("MP3 Encoding");

    m_semanticCategory["soprano-backend-sesame"] = i18n("Improved Semantic Desktop Plugin");
}

InstallEvent::~InstallEvent()
{
    delete m_installGui;
}

void InstallEvent::show()
{
    QPixmap icon = KIcon("download").pixmap(NOTIFICATION_ICON_SIZE);
    QString text(i18nc("Notification when a package wants to install extra software",
                       "Extra packages can be installed to enhance functionality for %1", m_applicationName));
    QStringList actions;
    actions << i18nc("Opens a dialog with more details", "Details");
    actions << i18nc("User declines an action", "Ignore");
    actions << i18nc("User indicates he never wants to see this notification again",
                     "Ignore forever");

    Event::show(icon, text, actions);
}

void InstallEvent::getInfo(const QString application, const QString package)
{
    m_applicationName = application;
    m_packageList.clear();

    if (m_multimediaCategory.contains(package)) {
        QMap<QString, QString>::const_iterator packageIter = m_multimediaCategory.constBegin();
        while (packageIter != m_multimediaCategory.end()) {
            if (!QFile::exists("/var/lib/dpkg/info/" + packageIter.key() + ".list")) {
                m_packageList[packageIter.key()] = packageIter.value();
            }
            ++packageIter;
        }
    } else if (m_semanticCategory.contains(package)) {
        QMap<QString, QString>::const_iterator packageIter = m_semanticCategory.constBegin();
        while (packageIter != m_semanticCategory.end()) {
            if (!QFile::exists("/var/lib/dpkg/info/" + packageIter.key() + ".list")) {
                m_packageList[packageIter.key()] = packageIter.value();
            }
            ++packageIter;
        }
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
