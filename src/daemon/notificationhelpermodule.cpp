/***************************************************************************
 *   Copyright © 2009 by Jonathan Thomas <echidnaman@kubuntu.org>          *
 *   Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>              *
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

#include "notificationhelpermodule.h"

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTimer>

// KDE includes
#include <KAboutData>
#include <KDirWatch>
#include <KLocalizedString>
#include <KPluginFactory>

// Own includes
#include "apportevent/apportevent.h"
#include "hookevent/hookevent.h"
#include "installevent/installevent.h"
#include "rebootevent/rebootevent.h"

#include "configwatcher.h"
#include "installevent/installdbuswatcher.h"

K_PLUGIN_FACTORY(NotificationHelperModuleFactory,
                 registerPlugin<NotificationHelperModule>();
                )
K_EXPORT_PLUGIN(NotificationHelperModuleFactory("notificationhelper"))


NotificationHelperModule::NotificationHelperModule(QObject* parent, const QList<QVariant>&)
        : KDEDModule(parent)
        , aEvent(0)
        , hEvent(0)
        , iEvent(0)
        , rEvent(0)
        , installWatcher(0)
{
    KAboutData aboutData("notificationhelper", "notificationhelper", ki18n("Kubuntu Notification Helper"),
                         "0.4", ki18n("A Notification Helper for Kubuntu"),
                         KAboutData::License_GPL, ki18n("(C) 2009 Jonathan Thomas, (C) 2009 Harald Sitter"),
                         KLocalizedString(), "http://kubuntu.org", "https://bugs.launchpad.net/ubuntu");

    configWatcher = new ConfigWatcher(this);

    aEvent = new ApportEvent(this, "Apport");
    hEvent = new HookEvent(this, "Hook");
    iEvent = new InstallEvent(this, "Install" );
    rEvent = new RebootEvent(this, "Restart");

    if (!aEvent->hidden && (QFile::exists("/usr/share/apport/apport-kde") || QFile::exists("/usr/share/apport/apport-gtk"))) {
        KDirWatch *apportDirWatch =  new KDirWatch(this);
        apportDirWatch->addDir("/var/crash/");
        connect(apportDirWatch, SIGNAL(dirty(const QString &)), this, SLOT(apportEvent()));
        connect(configWatcher, SIGNAL(reloadConfigCalled()), aEvent, SLOT(reloadConfig()));

        // Force check since we just started up and there might have been crashes on reboot
        QTimer::singleShot(5000, this, SLOT(apportEvent()));
    }

    if (!hEvent->hidden) {
        KDirWatch *hooksDirWatch = new KDirWatch(this);
        hooksDirWatch->addDir("/var/lib/update-notifier/user.d/");
        connect(hooksDirWatch, SIGNAL(dirty(const QString &)), this, SLOT(hookEvent()));
        connect(configWatcher, SIGNAL(reloadConfigCalled()), hEvent, SLOT(reloadConfig()));
    }

    if ( !iEvent->hidden )
    {
        installWatcher = new InstallDBusWatcher(this);
        connect(installWatcher, SIGNAL(installRestrictedCalled(const QString &, const QString &)),
                                       this, SLOT(installEvent(const QString &, const QString &)));
        connect(configWatcher, SIGNAL(reloadConfigCalled()), iEvent, SLOT(reloadConfig()));
    }

    if (!rEvent->hidden) {
        KDirWatch *stampDirWatch = new KDirWatch(this);
        stampDirWatch->addFile("/var/lib/update-notifier/dpkg-run-stamp");
        connect(stampDirWatch, SIGNAL(dirty(const QString &)), this, SLOT(rebootEvent()));
        connect(configWatcher, SIGNAL(reloadConfigCalled()), rEvent, SLOT(reloadConfig()));

        rebootEvent();
    }
}

NotificationHelperModule::~NotificationHelperModule()
{
    delete aEvent;
    delete hEvent;
    delete iEvent;
    delete rEvent;
    delete installWatcher;
}

void NotificationHelperModule::apportEvent()
{
    // We could be too fast for apport, so wait a bit before showing the notification
    QTimer::singleShot(2000, aEvent, SLOT(show()));
}

void NotificationHelperModule::hookEvent()
{
    hEvent->show();
}

void NotificationHelperModule::installEvent(const QString application, const QString package)
{
    iEvent->getInfo(application, package);
}

void NotificationHelperModule::rebootEvent()
{
    if (!QFile::exists("/var/run/reboot-required")) {
        return;
    }

    rEvent->show();
}

#include "notificationhelpermodule.moc"
