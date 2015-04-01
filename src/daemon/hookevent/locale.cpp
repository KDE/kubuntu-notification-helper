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

#include "locale.h"

#include <algorithm>

Locale::Locale(const QString &locale)
    : m_language(QString())
    , m_country(QString())
    , m_variant(QString())
    , m_encoding(QString())
{
    QString tmp = locale;
    QStringList parts;

    parts = tmp.split(QChar('.'));
    if (parts.size() > 1) {
        m_encoding = parts.last();
    }
    tmp = parts.first();

    parts = tmp.split(QChar('@'));
    if (parts.size() > 1) {
        m_variant = parts.last();
    }
    tmp = parts.first();

    parts = tmp.split(QChar('_'));
    if (parts.size() > 1) {
        m_country = parts.last();
    }
    m_language = parts.first();
}

// This function returns a combination of possible locale values.
// The way this works is that it fills a list with all possibly combination on
// top of language. All combiners can return a noop version of the string
// passed into the combiner if the combinee is empty. This ultimately results
// in duplicated entries (e.g. if variant is empty the list would contain
// language and langauge.encoding twice as their respective variantified version
// comes back without variant).
// Once all combinations are attempted the by-design-duplicates are removed
// and we have a nice clean list of language combinations.
// This has slight overhead since we are partially working with duplicated
// strings at times, it does however need substantially less code and less
// iffing.
QStringList Locale::combinations()
{
    QStringList list;
    // language_country@variant.encoding
    // language_country@variant
    // language@variant.encoding
    // language@variant
    // language_country.encoding
    // language_country
    // language.encoding
    // language
    // ^ all built in reverse order for improved readability
    QString tmp;
    if (!m_language.isEmpty()) {
        list << (tmp = m_language);
        list << encodify(tmp);

        list << (tmp = countryfy(m_language));
        list << encodify(tmp);

        list << (tmp = variantify(m_language));
        list << encodify(tmp);

        list << (tmp = variantify(countryfy(m_language)));
        list << encodify(tmp);
    }
    list.removeDuplicates();
    std::reverse(std::begin(list), std::end(list));
    return list;
}

QString Locale::countryfy(const QString &str)
{
    if (m_country.isEmpty())
        return str;
    return str + '_' + m_country;
}

QString Locale::variantify(const QString &str)
{
    if (m_variant.isEmpty())
        return str;
    return str + '@' + m_variant;
}

QString Locale::encodify(const QString &str)
{
    if (m_encoding.isEmpty())
        return str;
    return str + '.' + m_encoding;
}
