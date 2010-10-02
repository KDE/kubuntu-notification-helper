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

#include "apportevent.h"

#include <KProcess>
#include <KToolInvocation>

ApportEvent::ApportEvent(QObject* parent, const QString &name)
        : Event(parent, name)
{}

ApportEvent::~ApportEvent()
{}

bool ApportEvent::reportsAvailable()
{
// TODO: there is also a --system arg for checkreports, update-notifier does seem to use
//       that in an either-or combo... so question is why does that --system arg exist at
//       all if we are supposed to either-or the results of two runs anyway?
    KProcess *apportProcess = new KProcess();
    apportProcess->setProgram(QStringList() << "/usr/share/apport/apport-checkreports");

    if (apportProcess->execute() == 0) {
        return true;
    }
    return false;
}

void ApportEvent::show()
{
    if (!reportsAvailable()) {
        return;
    }

    QString icon = QString("apport");
    QString text(i18nc("Notification when apport detects a crash",
                       "An application has crashed on your system (now or in the past)"));
    QStringList actions;
    actions << i18nc("Opens a dialog with more details", "Details");
    actions << i18nc("Button to dismiss this notification once", "Ignore for now");
    actions << i18nc("Button to make this notification never show up again",
                     "Never show again");

    Event::show(icon, text, actions);
}

void ApportEvent::run()
{
    KToolInvocation::kdeinitExec("/usr/share/apport/apport-kde");
}

#include "apportevent.moc"
