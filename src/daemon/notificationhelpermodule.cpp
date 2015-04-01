/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *   Copyright © 2009-2014 Harald Sitter <apachelogger@ubuntu.com>         *
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
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTimer>

// KDE includes
#include <KDirWatch>
#include <KLocalizedString>
#include <KPluginFactory>

// Own includes
#include "apportevent/apportevent.h"
#include "hookevent/hookevent.h"
#include "installevent/installevent.h"
#include "l10nevent/l10nevent.h"
#include "rebootevent/rebootevent.h"
#include "driverevent/driverevent.h"

#include "configwatcher.h"
#include "installevent/installdbuswatcher.h"

K_PLUGIN_FACTORY(NotificationHelperModuleFactory,
                 registerPlugin<NotificationHelperModule>();
                )

#ifndef START_TIMEOUT
#define START_TIMEOUT 1000
#endif

NotificationHelperModule::NotificationHelperModule(QObject* parent, const QList<QVariant>&)
    : KDEDModule(parent)
    , m_apportEvent(nullptr)
    , m_driverEvent(nullptr)
    , m_hookEvent(nullptr)
    , m_installEvent(nullptr)
    , m_rebootEvent(nullptr)
    , m_configWatcher(nullptr)
    , m_installWatcher(nullptr)
{
    QTimer::singleShot(START_TIMEOUT, this, SLOT(init()));
}

NotificationHelperModule::~NotificationHelperModule()
{
}

void NotificationHelperModule::init()
{
    qDebug();
    m_configWatcher = new ConfigWatcher(this);

    m_apportEvent = new ApportEvent(this, "Apport");
    m_driverEvent = new DriverEvent(this, "Driver");
    m_hookEvent = new HookEvent(this, "Hook");
    m_installEvent = new InstallEvent(this, "Install");
    m_l10nEvent = new L10nEvent(this, "L10n");
    m_rebootEvent = new RebootEvent(this, "Restart");

    const bool apportHidden = m_apportEvent->isHidden();
    const bool apportKde = QFile::exists("/usr/share/apport/apport-kde");
    const bool apportGtk = QFile::exists("/usr/share/apport/apport-gtk");
    qDebug() << "ApportEvent ::"
             << "hidden=" << apportHidden
             << "apport-kde=" << apportKde
             << "apport-gtk=" << apportGtk;
    if (!apportHidden && (apportKde || apportGtk)) {
        qDebug() << "Using ApportEvent";

        KDirWatch *apportDirWatch =  new KDirWatch(this);
        apportDirWatch->addDir("/var/crash/");
        connect(apportDirWatch, SIGNAL(dirty(const QString &)),
                this, SLOT(apportEvent(QString)));
        connect(m_configWatcher, SIGNAL(reloadConfigCalled()),
                m_apportEvent, SLOT(reloadConfig()));

        // Force check, we just started up and there might have been crashes on reboot
        apportEvent(QString());
    }

    if (!m_driverEvent->isHidden()) {
        m_driverEvent->show();
    }

    if (!m_hookEvent->isHidden()) {
        KDirWatch *hooksDirWatch = new KDirWatch(this);
        hooksDirWatch->addDir("/var/lib/update-notifier/user.d/");
        connect(hooksDirWatch, SIGNAL(dirty(const QString &)),
                this, SLOT(hookEvent()));
        connect(m_configWatcher, SIGNAL(reloadConfigCalled()),
                m_hookEvent, SLOT(reloadConfig()));

        // Sometimes hooks are for the first boot, so force a check
        hookEvent();
    }

    if (!m_installEvent->isHidden()) {
        // this normally gets called by applications calling it through dbus when they start
        m_installWatcher = new InstallDBusWatcher(this);
        connect(m_installWatcher, SIGNAL(installRestrictedCalled(const QString &, const QString &)),
                this, SLOT(installEvent(const QString &, const QString &)));
        connect(m_configWatcher, SIGNAL(reloadConfigCalled()),
                m_installEvent, SLOT(reloadConfig()));

        //but we can't/don't patch firefox so call it on start regardless
        installEvent("firefox", "flashplugin-installer");
    }

    if (!m_l10nEvent->isHidden()) {
        // We only want this notification once for now.
        // NOTE: might be viable to watch the config and apt lock to
        //       issue a notification when the user installs software that
        //       requires additional language support packages.
        m_l10nEvent->show();
    }

    if (!m_rebootEvent->isHidden()) {
        KDirWatch *stampDirWatch = new KDirWatch(this);
        stampDirWatch->addFile("/var/lib/update-notifier/dpkg-run-stamp");
        connect(stampDirWatch, SIGNAL(dirty(const QString &)),
                this, SLOT(rebootEvent()));
        connect(m_configWatcher, SIGNAL(reloadConfigCalled()),
                m_rebootEvent, SLOT(reloadConfig()));

        rebootEvent();
    }
}

void NotificationHelperModule::apportDirEvent()
{
    qDebug();

    QDir dir(QLatin1String("/var/crash"));
    dir.setNameFilters(QStringList() << QLatin1String("*.crash"));

    bool foundCrashFile = false;
    bool foundAutoUpload = false;
    foreach (const QFileInfo &fileInfo, dir.entryInfoList()) {
        CrashFile f(fileInfo);
        if (f.isAutoUploadAllowed())
            foundAutoUpload = true;
            continue;
        if (f.isValid()) {
            foundCrashFile = true;
            continue;
        }
    }

    qDebug() << "foundCrashFile" << foundCrashFile
             << "foundAutoUpload" << foundAutoUpload;

    if (foundAutoUpload) {
        m_apportEvent->batchUploadAllowed();
    }

    if (foundCrashFile) {
        m_apportEvent->show();
    }
}

void NotificationHelperModule::apportEvent(const QString &path)
{
    qDebug() << path;
    if (path.isEmpty()) { // Check whole directory for possible crash files.
        apportDirEvent();
        return;
    }

    // Check param path for validity.
    CrashFile f(path);
    if (f.isAutoUploadAllowed()) {
        m_apportEvent->batchUploadAllowed();
    } else if (f.isValid()) {
        m_apportEvent->show();
    }
}

void NotificationHelperModule::hookEvent()
{
    m_hookEvent->show();
}

void NotificationHelperModule::installEvent(const QString &app, const QString &package)
{
    m_installEvent->getInfo(app, package);
}

void NotificationHelperModule::rebootEvent()
{
    m_rebootEvent->show();
}

#include "notificationhelpermodule.moc"
