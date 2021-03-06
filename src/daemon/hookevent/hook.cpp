/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *   Copyright © 2009 Amichai Rothman <amichai2@amichais.net>              *
 *   Copyright © 2014-2015 Harald Sitter <sitter@kde.org>                  *
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

#include "hook.h"

// Qt includes
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>
#include <QStringBuilder>

// KDE includes
#include <KProcess>
#include <KToolInvocation>
#include <KConfig>
#include <KConfigGroup>

#include "locale.h"

float getUptime()
{
    float uptime = 0;
    QFile uptimeFile("/proc/uptime");
    if (uptimeFile.open(QFile::ReadOnly)) {
        QTextStream stream(&uptimeFile);
        QString uptimeLine = stream.readLine();
        QStringList uptimeStringList = uptimeLine.split(' ');
        // We don't need the last part of /proc/uptime
        uptimeStringList.removeLast();
        QString uptimeString = uptimeStringList.first();
        uptime = uptimeString.toFloat();
    }
    return uptime;
}

QString trimLeft(QString str, int start = 0)
{
    int len = str.length();
    while (start < len && str[start].isSpace())
        start++;
    return str.mid(start);
}

Hook::Hook(QObject *parent, const QString &hookPath)
    : QObject(parent)
    , m_hookPath(hookPath)
    , m_finished(false)
    , m_locale(QLatin1String(setlocale(LC_ALL, NULL)))
{
    m_fields = parse(hookPath);
    loadConfig();
}

Hook::~Hook()
{}

QString Hook::locale()
{
    return m_locale;
}

void Hook::setLocale(const QString &locale)
{
    m_locale = locale;
}

QString Hook::getField(const QString &name) const
{
    // Try to lookup the field with -LOCALE appended, then -LANGUAGE then without
    // suffix.
    Locale locale(m_locale);
    QString prefix = name % QChar('-');
    QString value;
    for (QString locale_combo: locale.combinations()) {
        value = m_fields.value(prefix + locale_combo);
        if (!value.isEmpty()) {
            break;
        }
    }
    if (value.isEmpty()) {
        value = m_fields.value(name);
    }
    return value;
}

bool Hook::isValid() const
{
    return !m_fields.isEmpty();
}

void Hook::runCommand()
{
    QString command = getField("Command");
    if (getField("Terminal") == "True") {
        // if command is quoted, invokeTerminal will refuse to interpret it properly
        if (command.startsWith('\"') && command.endsWith('\"')) {
            command = command.mid(1, command.length() - 2);
        }
        KToolInvocation::invokeTerminal(command);
    } else {
        KProcess process;
        process.setShellCommand(command);
        process.startDetached();
    }
}

void Hook::setFinished()
{
    m_finished = true;
    saveConfig();
}

void Hook::loadConfig()
{
    QString signature = calculateSignature();
    KConfig config("notificationhelper", KConfig::NoGlobals);
    KConfigGroup group(&config, "updateNotifications");

    m_finished = group.readEntry(signature, false);

    // remain backward compatibile with update-notifier-kde
    // so that after upgrade old notifications are not resurrected
    if (!m_finished) {
        KConfig oldconfig("update-notifier-kderc", KConfig::NoGlobals);
        KConfigGroup oldgroup(&oldconfig, "updateNotifications");
        QFileInfo fileinfo(m_hookPath);
        QString oldsignature = fileinfo.fileName();
        m_finished = oldgroup.readEntry(oldsignature, false);
        if (m_finished)
            saveConfig(); // copy over to new configuration
    }
}

void Hook::saveConfig()
{
    QString signature = calculateSignature();
    KConfig config("notificationhelper", KConfig::NoGlobals);
    KConfigGroup group(&config, "updateNotifications");

    group.writeEntry(signature, m_finished);
    group.sync();
}

QString Hook::calculateSignature() const
{
    // this is used to uniquely identify a hook so that
    // it is not shown again after it has been executed
    QFile file(m_hookPath);
    QFileInfo fileinfo(m_hookPath);
    QString timestamp = fileinfo.lastModified().toString(Qt::ISODate);
    QString filename = fileinfo.fileName();

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(filename.toUtf8());
    hash.addData(timestamp.toUtf8());
    hash.addData(&file);
    return hash.result();
}

QMap<QString, QString> Hook::parse(const QString &hookPath)
{
    const QMap<QString, QString> emptyMap;

    QFile file(hookPath);
    if (!file.open(QFile::ReadOnly)) {
        return emptyMap;
    }

    // See https://wiki.kubuntu.org/InteractiveUpgradeHooks for details on the hook format
    QMap<QString, QString> fields;
    QTextStream stream(&file);
    stream.setCodec("UTF-8"); // as required by spec
    stream.setAutoDetectUnicode(true); // just in case

    QString lastKey;
    QString line;
    do {
        line = stream.readLine();
        if (line.isEmpty()) {
            continue; // skip empty lines, e.g. at end of file
        } else if (line.at(0).isSpace()) {
            line = trimLeft(line);
            if (line.isEmpty())
                continue; // treat it like empty line (lenient)
            if (lastKey.isEmpty())
                return emptyMap; // not a valid upgrade hook
            QString value = fields[lastKey];
            if (!value.isEmpty())
                value += ' ';
            fields[lastKey] = value + line;
        } else {
            int split = line.indexOf(':');
            if (split <= 0) {
                return emptyMap; // not a valid upgrade hook
            }
            QString key = line.left(split);
            QString value = trimLeft(line, split + 1);
            fields[key] = value;
            lastKey = key;
        }
    } while (!line.isNull());

    return fields;
}

bool Hook::isNotificationRequired() const
{
    if (m_finished) {
        return false;
    }

    if (getField("DontShowAfterReboot") == "True") {
        float uptime = getUptime();
        if (uptime > 0) {
            const QDateTime now = QDateTime::currentDateTime();
            QDateTime statTime = QFileInfo(m_hookPath).lastModified();
            if (now.toTime_t() - statTime.toTime_t() > uptime) {
                return false;
            }
        }
    }

    QString condition = getField("DisplayIf");
    if (!condition.isEmpty()) {
        KProcess displayIfProcess;
        // Do not ever try to use a program call here. The spec defines that
        // DisplayIf is a shell command, if one tries to evaluate a somewhat
        // complex shell command as a program KProcess will die a horrible death.
        displayIfProcess.setShellCommand(condition);
        int programResult = displayIfProcess.execute();
        if (programResult != 0) {
            return false;
        }
    }

    return true;
}
