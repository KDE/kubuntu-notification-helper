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

#include "hookevent.h"

// Qt includes
#include <QtCore/QDir>

// Own includes
#include "hookparser.h"
#include "hookgui.h"

HookEvent::HookEvent(QObject* parent, QString name)
        : Event(parent, name)
        , m_parser(0)
        , m_parsedHookMap()
        , m_hookGui(0)
{}

HookEvent::~HookEvent()
{}

void HookEvent::show()
{
    QDir hookDir("/var/lib/update-notifier/user.d/");

    QStringList fileList;
    fileList << hookDir.entryList(QDir::Files);

    m_parser = new HookParser(this);
    foreach(const QString &fileName, fileList) {
        QMap<QString, QString> fileResult = m_parser->parseHook(hookDir.filePath(fileName));
        if (!fileResult.isEmpty()) {
            // Add parsed hook to map
            m_parsedHookMap[fileName] = fileResult;
        }
    }

    if (!m_parsedHookMap.isEmpty()) {
        QPixmap icon = KIcon("help-hint").pixmap(48, 48);
        QString text(i18nc("Notification when an upgrade requires the user to do something", "Software upgrade notifications are available"));
        QStringList actions;
        actions << i18nc("Opens a dialog with more details", "Details");
        actions << i18nc("User declines an action", "Ignore");
        actions << i18nc("User indicates he never wants to see this notification again", "Never show again");
        Event::show(icon, text, actions);
    }
}

void HookEvent::run()
{
    m_hookGui = new HookGui(this, m_parsedHookMap);
    Event::run();
}

#include "hookevent.moc"
