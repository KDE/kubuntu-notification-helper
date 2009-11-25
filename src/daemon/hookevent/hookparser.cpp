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

#include "hookparser.h"

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>

// KDE includes
#include <KProcess>

HookParser::HookParser(QObject* parent)
        : QObject(parent)
{}

HookParser::~HookParser()
{}

QMap<QString, QString> HookParser::parseHook(const QString &hookPath)
{
    const QMap<QString, QString> emptyMap;

    QFile file(hookPath);
    if (!file.open(QFile::ReadOnly)) {
        return emptyMap;
    }

    // See https://wiki.kubuntu.org/InteractiveUpgradeHooks for details on the hook format
    QMap<QString, QString> hookMap;
    QTextStream stream(&file);

    QString lastKey;
    QString line;
    do {
        line = stream.readLine();

        if (line.contains(':')) {
            QStringList list = line.split(": ");
            QString key = list.at(0);
            QString value = list.at(1);
            hookMap[key] = value;
            lastKey = key;
        } else if (line.startsWith(' ')) {
            if (lastKey.isEmpty()) {
                continue;
            }
            hookMap[lastKey] += line;
        } else if (line.isEmpty()) {
            // Handle empty newline(s) at the end of files
            continue;
        } else {
            // Not an upgrade hook or malformed
            return emptyMap;
        }
    } while (!line.isNull());

    // TODO: Check if already shown, keep track via KConfig
    // TODO: Find a more sane way to get that information?
    if (hookMap.contains("DontShowAfterReboot") && hookMap["DontShowAfterReboot"] == "True") {
        QFile uptimeFile("/proc/uptime");
        if (uptimeFile.open(QFile::ReadOnly)) {
            QTextStream stream(&uptimeFile);
            QString uptimeLine = stream.readLine();
            QStringList uptimeStringList = uptimeLine.split(' ');
            // We don't need the last part of /proc/uptime
            uptimeStringList.removeLast();
            QString uptimeString = uptimeStringList.first();
            float uptime = uptimeString.toFloat();
            const QDateTime now = QDateTime::currentDateTime();

            QDateTime statTime = QFileInfo(hookPath).lastModified();

            if (uptime > 0 && ((now.toTime_t() - statTime.toTime_t()) > uptime)) {
                return emptyMap;
            }
        }
    }

    if (hookMap.contains("DisplayIf")) {
        KProcess *displayIfProcess = new KProcess();
        displayIfProcess->setProgram(hookMap["DisplayIf"]);

        int programResult = displayIfProcess->execute();
        if (programResult != 0) {
            return emptyMap;
        }
    }

    return hookMap;
}

#include "hookparser.moc"
