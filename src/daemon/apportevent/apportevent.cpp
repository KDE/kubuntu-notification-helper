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

#include "apportevent.h"

#include <QDebug>
#include <QStandardPaths>
#include <QDir>

#include <KProcess>
#include <KToolInvocation>
#include <KDirWatch>

ApportEvent::ApportEvent(QObject* parent)
        : Event(parent, "Apport")
{
    const bool apportKde = QFile::exists("/usr/share/apport/apport-kde");
    const bool apportGtk = QFile::exists("/usr/share/apport/apport-gtk");
    qDebug() << "ApportEvent ::"
             << "apport-kde=" << apportKde
             << "apport-gtk=" << apportGtk;
    if (!apportKde && !apportGtk) {
        return;
    }
    qDebug() << "Using ApportEvent";

    auto apportDirWatch =  new KDirWatch(this);
    apportDirWatch->addDir("/var/crash/");
    connect(apportDirWatch, &KDirWatch::dirty, this, &ApportEvent::onDirty);

    // Force check, we just started up and there might have been crashes on reboot
    show();
}

ApportEvent::~ApportEvent()
{
}

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
    if (isHidden()) {
        return;
    }

    if (!reportsAvailable()) {
        qDebug() << "no reports available, aborting";
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

void ApportEvent::batchUploadAllowed()
{
    const QString script = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                  "kubuntu-notification-helper/whoopsie-upload-all");
    if (script.isEmpty()) {
        qWarning() << "ApportEvent: whoopsie-upload-all not found";
        return;
    }
    qDebug() << "running" << script;
    KToolInvocation::kdeinitExec(script);
}

void ApportEvent::run()
{
    KToolInvocation::kdeinitExec("/usr/share/apport/apport-kde");
    Event::run();
}

void ApportEvent::apportDirEvent()
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
        batchUploadAllowed();
    }

    if (foundCrashFile) {
        show();
    }
}

void ApportEvent::onDirty(const QString &path)
{
    if (isHidden()) {
        return;
    }

    qDebug() << path;
    if (path.isEmpty()) { // Check whole directory for possible crash files.
        apportDirEvent();
        return;
    }

    // Check param path for validity.
    CrashFile f(path);
    if (f.isAutoUploadAllowed()) {
        batchUploadAllowed();
    } else if (f.isValid()) {
        show();
    }
}
