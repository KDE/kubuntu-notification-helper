/***************************************************************************
 *   Copyright © 2015 Harald Sitter <sitter@kde.org>                       *
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

#include <QObject>
#include <QtTest>

#include "../src/daemon/hookevent/hook.h"

class HookTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void init();

    void ctor();
    void validFile();
    void invalidFile();

private:
    QString data(const QString func);

    QString m_dataPath;
};

void HookTest::initTestCase()
{
    m_dataPath = QStringLiteral(TEST_DATA) + QStringLiteral("/hooktest");
}

void HookTest::init()
{
    setlocale(LC_ALL, "en_US.UTF-8");
}

void HookTest::ctor()
{
    Hook h(nullptr, QStringLiteral("/"));
}

void HookTest::validFile()
{
    Hook h(nullptr, data("validFile"));
    QVERIFY(h.isValid());
    QCOMPARE(h.getField("Name"), QString("apt-file update needed"));
    QCOMPARE(h.getField("Terminal"), QString("True"));
    QCOMPARE(h.getField("Command"), QString("\"/usr/share/apt-file/do-apt-file-update\""));
    h.setLocale("de_DE.UTF-8");
    QCOMPARE(h.getField("Name"), QString("sauerkraut lederhosen"));
    QCOMPARE(h.getField("Terminal"), QString("True"));
    QCOMPARE(h.getField("Command"), QString("\"/usr/share/apt-file/do-apt-file-update\""));
    h.setLocale("fr_FR.UTF-8");
    QCOMPARE(h.getField("Name"), QString("Échec du téléchargement des données supplémentaires"));
}

void HookTest::invalidFile()
{
    Hook h(nullptr, data("invalidFile"));
    QVERIFY(!h.isValid());
}

QString HookTest::data(const QString func)
{
    return m_dataPath + "/" + func;
}

QTEST_GUILESS_MAIN(HookTest);

#include "hooktest.moc"
