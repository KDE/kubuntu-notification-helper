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

#ifndef HOOKPARSER_H
#define HOOKPARSER_H


#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>

class Hook : public QObject
{
    Q_OBJECT
public:
    Hook(QObject* parent, const QString& hookPath);

    virtual ~Hook();

public slots:
    bool isValid();
    bool isNotificationRequired();
    QString getField(const QString& name, const QString &language);
    QString getField(const QString& name);
    void runCommand();
    
private:
    QMap<QString, QString> parse(const QString &hookPath);
    float getUptime();
    
    QString m_hookPath;
    QMap<QString, QString> m_fields;
};

#endif
