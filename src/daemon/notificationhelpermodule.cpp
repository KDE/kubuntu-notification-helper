/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *   Copyright © 2009-2021 Harald Sitter <apachelogger@ubuntu.com>         *
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
#include <QTimer>

// KDE includes
#include <KLocalizedString>
#include <KPluginFactory>
#include <KConfigWatcher>

// Own includes
#include "apportevent/apportevent.h"
#include "hookevent/hookevent.h"
#include "installevent/installevent.h"
#include "l10nevent/l10nevent.h"
#include "rebootevent/rebootevent.h"
#include "driverevent/driverevent.h"

K_PLUGIN_FACTORY(NotificationHelperModuleFactory,
                 registerPlugin<NotificationHelperModule>();
                )

#ifndef START_TIMEOUT
#define START_TIMEOUT 1000
#endif

NotificationHelperModule::NotificationHelperModule(QObject* parent, const QList<QVariant>&)
    : KDEDModule(parent)
{
    QTimer::singleShot(START_TIMEOUT, this, SLOT(init()));
}

NotificationHelperModule::~NotificationHelperModule()
{
}

void NotificationHelperModule::init()
{
    qDebug();

    const QVector<Event *> events = {
        new ApportEvent(this),
        new DriverEvent(this),
        new HookEvent(this),
        new InstallEvent(this),
        new L10nEvent(this),
        new RebootEvent(this),
    };

    // Todo could hold a watcher in every event really.
    m_configWatcher = KConfigWatcher::create(KSharedConfig::openConfig("notificationhelper"));
    connect(m_configWatcher.get(), &KConfigWatcher::configChanged, this, [events] {
        for (auto *event : events) {
            event->reloadConfig();
        }
    });
}

#include "notificationhelpermodule.moc"
