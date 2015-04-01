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

#define private public
#include "../src/daemon/hookevent/locale.h"
#undef private

class LocaleTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void en();
    void enEncoding();
    void en_US();
    void en_USVariant();
    void en_USVariantEncoding();
    void en_USEncoding();
};

void LocaleTest::en()
{
    Locale l(QStringLiteral("en"));
    QCOMPARE(l.m_language, QStringLiteral("en"));
    QCOMPARE(l.m_country, QString());
    QCOMPARE(l.m_variant, QString());
    QCOMPARE(l.m_encoding, QString());
    QCOMPARE(l.combinations(), QStringList() << "en");
}

void LocaleTest::enEncoding()
{
    Locale l(QStringLiteral("en.UTF-8"));
    QCOMPARE(l.m_language, QStringLiteral("en"));
    QCOMPARE(l.m_country, QString());
    QCOMPARE(l.m_variant, QString());
    QCOMPARE(l.m_encoding, QStringLiteral("UTF-8"));
    QCOMPARE(l.combinations(), QStringList() << "en.UTF-8" << "en");
}


void LocaleTest::en_US()
{
    Locale l(QStringLiteral("en_US"));
    QCOMPARE(l.m_language, QStringLiteral("en"));
    QCOMPARE(l.m_country, QStringLiteral("US"));
    QCOMPARE(l.m_variant, QString());
    QCOMPARE(l.m_encoding, QString());
    QCOMPARE(l.combinations(), QStringList() << "en_US" << "en");
}

void LocaleTest::en_USVariant()
{
    Locale l(QStringLiteral("en_US@foo"));
    QCOMPARE(l.m_language, QStringLiteral("en"));
    QCOMPARE(l.m_country, QStringLiteral("US"));
    QCOMPARE(l.m_variant, QStringLiteral("foo"));
    QCOMPARE(l.m_encoding, QString());
    QCOMPARE(l.combinations(), QStringList() << "en_US@foo" << "en@foo" << "en_US" << "en");
}

void LocaleTest::en_USVariantEncoding()
{
    Locale l(QStringLiteral("en_US@foo.UTF-8"));
    QCOMPARE(l.m_language, QStringLiteral("en"));
    QCOMPARE(l.m_country, QStringLiteral("US"));
    QCOMPARE(l.m_variant, QStringLiteral("foo"));
    QCOMPARE(l.m_encoding, QStringLiteral("UTF-8"));
    QCOMPARE(l.combinations(), QStringList() << "en_US@foo.UTF-8" << "en_US@foo" << "en@foo.UTF-8" << "en@foo" << "en_US.UTF-8" << "en_US" << "en.UTF-8" << "en");
}

void LocaleTest::en_USEncoding()
{
    Locale l(QStringLiteral("en_US.UTF-8"));
    QCOMPARE(l.m_language, QStringLiteral("en"));
    QCOMPARE(l.m_country, QStringLiteral("US"));
    QCOMPARE(l.m_variant, QString());
    QCOMPARE(l.m_encoding, QStringLiteral("UTF-8"));
    QCOMPARE(l.combinations(), QStringList() << "en_US.UTF-8" << "en_US" << "en.UTF-8" << "en");
}

QTEST_GUILESS_MAIN(LocaleTest);

#include "localetest.moc"
