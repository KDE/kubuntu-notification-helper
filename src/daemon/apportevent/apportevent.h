/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *   Copyright © 2009-2013 Harald Sitter <apachelogger@kubuntu.org>        *
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

#ifndef APPORTEVENT_H
#define APPORTEVENT_H

#include "../event.h"

#include <QtCore/QFileInfo>

class CrashFile
{
public:
    CrashFile(const QString &path)
        : m_path(path)
        , m_info(QFileInfo(path))
    {
    }

    CrashFile(const QFileInfo &info)
        : m_path(info.absoluteFilePath())
        , m_info(info)
    {
    }

    bool isAutoUploadAllowed() const {
        QString acceptPath = m_path;
        acceptPath.replace(QLatin1String(".crash"), QLatin1String(".drkonqi-accept"));
        return QFile(acceptPath).exists();
    }

    bool isValid() const {
        QString uploadPath = m_path; // Marked for upload -> ignore.
        uploadPath.replace(QLatin1String(".crash"), QLatin1String(".upload"));
        QString uploadedPath = m_path; // Alraedy uploaded -> ignore even more.
        uploadedPath.replace(QLatin1String(".crash"), QLatin1String(".uploaded"));
        return (!QFile(uploadPath).exists() &&
                !QFile(uploadedPath).exists() &&
                ((m_info.suffix() == QLatin1String("crash")) &&
                 (m_info.permission(QFile::ReadUser))));
    }

private:
    QString m_path;
    QFileInfo m_info;
};

class ApportEvent : public Event
{
    Q_OBJECT
public:
    ApportEvent(QObject* parent);

    virtual ~ApportEvent();

public slots:
    void show();
    void batchUploadAllowed();

private slots:
    bool reportsAvailable();
    void run();
};

#endif
