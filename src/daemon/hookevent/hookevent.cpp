/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *   Copyright © 2009-2021 Harald Sitter <apachelogger@ubuntu.com>              *
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

#include "hookevent.h"

// Qt includes
#include <QDir>

#include <KDirWatch>

// Own includes
#include "hook.h"
#include "hookgui.h"

HookEvent::HookEvent(QObject* parent)
        : Event(parent, "Hook")
        , m_hooks()
        , m_hookGui(0)
{
    auto hooksDirWatch = new KDirWatch(this);
    hooksDirWatch->addDir("/var/lib/update-notifier/user.d/");
    connect(hooksDirWatch, &KDirWatch::dirty, this, &HookEvent::show);

    // Sometimes hooks are for the first boot, so force a check
    show(); // noop when not applicable
}

HookEvent::~HookEvent()
{
}

void HookEvent::show()
{
    if (isHidden()) {
        return;
    }

    qDeleteAll(m_hooks);
    m_hooks.clear();
    QDir hookDir(QLatin1String("/var/lib/update-notifier/user.d/"));
    QStringList fileList = hookDir.entryList(QDir::Files);
    foreach(const QString &fileName, fileList) {
        Hook *hook = new Hook(this, hookDir.filePath(fileName));
        if (hook->isValid() && hook->isNotificationRequired()) {
            m_hooks << hook;
        } else {
            hook->deleteLater();
        }
    }

    if (!m_hooks.isEmpty()) {
        QString icon = QLatin1String("help-hint");
        QString text(i18nc("Notification when an upgrade requires the user to do something",
                           "Software upgrade notifications are available"));
        QStringList actions;
        actions << i18nc("Opens a dialog with more details", "Details");
        actions << i18nc("User declines an action", "Ignore");
        actions << i18nc("User indicates he never wants to see this notification again",
                         "Never show again");
        Event::show(icon, text, actions);
    }
}

void HookEvent::run()
{
    if (!m_hookGui) {
        m_hookGui = new HookGui(this);
    }
    m_hookGui->showDialog(m_hooks);
    Event::run();
}
