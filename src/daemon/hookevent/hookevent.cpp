/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "hookevent.h"

// Qt includes
#include <QtCore/QDir>

// Own includes
#include "hook.h"
#include "hookgui.h"

HookEvent::HookEvent(QObject* parent, const QString &name)
        : Event(parent, name)
        , m_hooks()
        , m_hookGui(0)
{}

HookEvent::~HookEvent()
{
}

void HookEvent::show()
{
    qDeleteAll(m_hooks);
    m_hooks.clear();
    QDir hookDir("/var/lib/update-notifier/user.d/");
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
        QString icon = QString("help-hint");
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

#include "hookevent.moc"
