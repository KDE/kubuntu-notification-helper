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

#ifndef LOCALE_H
#define LOCALE_H

#include <QStringList>

/**
 * @brief Helper class dealing with hook locale strings
 * Hook locale strings are somewhat random nonesense (e.g. they can be
 *  language.encoding even though the string encoding always is utf-8 anyway).
 * So, this class allows building all combinations off of a locale that could
 * potentially be used for the locale identifier in a hook.
 * This list is ordered by preference.
 */
class Locale
{
public:
    Locale(const QString &locale);

    QStringList combinations();

private:
    QString countryfy(const QString &str);
    QString variantify(const QString &str);
    QString encodify(const QString &str);

    QString m_language;
    QString m_country;
    QString m_variant;
    QString m_encoding;
};

#endif // LOCALE_H
