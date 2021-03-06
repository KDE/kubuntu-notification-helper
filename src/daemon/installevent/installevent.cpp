/***************************************************************************
 *   Copyright © 2009-2010 Jonathan Thomas <echidnaman@kubuntu.org>        *
 *   Copyright © 2021 Harald Sitter <sitter@kde.org>                       *
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
#include <QDebug>

// Own includes
#include "installgui.h"
#include "installdbuswatcher.h"

InstallEvent::InstallEvent(QObject *parent)
    : Event(parent, "Install")
    #warning this isn't localized and makes no sense where it is used
    , m_applicationName("Install")
    , m_installGui(0)
{
    // this normally gets called by applications calling it through dbus when they start
    auto installWatcher = new InstallDBusWatcher(this);
    connect(installWatcher,
            &InstallDBusWatcher::installRestrictedCalled,
            this,
            [this](const QString &application, const QString &package) { getInfo(application, package); });

    m_webBrowserPackages["flashplugin-installer"] = i18nc("The name of the Adobe Flash plugin", "Flash");

    m_multimediaEncodingPackages["libk3b6-extracodecs"] = i18n("K3b CD Codecs");
    m_multimediaEncodingPackages["libmp3lame0"] = i18n("MP3 Encoding");

    m_packageMapList.append(m_webBrowserPackages);
    m_packageMapList.append(m_multimediaEncodingPackages);
}

InstallEvent::~InstallEvent()
{
}

void InstallEvent::show()
{
    if (isHidden()) {
        return;
    }

    QString icon = QString("muondiscover");
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

void InstallEvent::addPackages(const QMap<QString, QString> &packageList)
{
    QMap<QString, QString>::const_iterator packageIter = packageList.constBegin();
    while (packageIter != packageList.constEnd()) {
        // check for .md5sums as .list exists even when the package is removed (but not purged)
        if (!QFile::exists("/var/lib/dpkg/info/" + packageIter.key() + ".md5sums") &&
            !QFile::exists("/var/lib/dpkg/info/" + packageIter.key() + ":i386.md5sums") &&
            !QFile::exists("/var/lib/dpkg/info/" + packageIter.key() + ":amd64.md5sums")) {
            m_packageList[packageIter.key()] = packageIter.value();
        }
        ++packageIter;
    }
}

void InstallEvent::getInfo(const QString &application, const QString &package)
{
    m_applicationName = application;
    m_packageList.clear();

    QList<QMap<QString, QString> >::const_iterator packageMapIter = m_packageMapList.constBegin();
    while (packageMapIter != m_packageMapList.constEnd()) {
        if ((*packageMapIter).contains(package)) {
            addPackages(*packageMapIter);
            break;
        }

        ++packageMapIter;
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
