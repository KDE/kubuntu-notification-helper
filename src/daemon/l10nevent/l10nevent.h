/*
  Copyright (C) 2014 Harald Sitter <apachelogger@kubuntu.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef L10NEVENT_H
#define L10NEVENT_H

#include "../event.h"

namespace Kubuntu {
class Language;
class LanguageCollection;
}

class L10nEvent : public Event
{
    Q_OBJECT
public:
    L10nEvent(QObject *parent);

    virtual ~L10nEvent();

public slots:
    void show();

private slots:
    void showOnLanguageCollectionUpdated();
    void run();

private:
    bool checkForMissingPackages(Kubuntu::Language *languages);
    QStringList systemLocaleMatchables() const;

    Kubuntu::LanguageCollection *m_languageCollection;
    QStringList m_missingPackages;
};

#endif // L10NEVENT_H
