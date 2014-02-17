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

#include "l10nevent.h"

#include <KConfigGroup>
#include <KDebug>
#include <KToolInvocation>
#include <KSharedConfig>

#include <Kubuntu/l10n_language.h>
#include <Kubuntu/l10n_languagecollection.h>

L10nEvent::L10nEvent(QObject *parent, const QString &name)
    : Event(parent, name)
    , m_languageCollection(nullptr)
{
}

L10nEvent::~L10nEvent()
{
}

void L10nEvent::show()
{
    if (!m_languageCollection) {
        m_languageCollection = new Kubuntu::LanguageCollection(this);
        connect(m_languageCollection, SIGNAL(updated()),
                this, SLOT(showOnLanguageCollectionUpdated()));
    }
    if (!m_languageCollection->isUpdated()) {
        m_languageCollection->update();
        return;
    }
    showOnLanguageCollectionUpdated();
}

void L10nEvent::showOnLanguageCollectionUpdated()
{
    const KSharedConfig::Ptr userConfig = KSharedConfig::openConfig("kcmlocale-user", KConfig::IncludeGlobals);
    const KConfigGroup userSettings = KConfigGroup(userConfig, "Locale");
    const QString languageConfigString = userSettings.readEntry("Language", QString());
    const QStringList languageList = languageConfigString.split(QLatin1Char(':'),
                                                                QString::SkipEmptyParts);
    kDebug() << languageList;

    m_missingPackages.clear();
    int incompleteLanguages = 0;
    foreach (Kubuntu::Language *language, m_languageCollection->languages()) {
        if (languageList.contains(language->kdeLanguageCode())) {
            kDebug() << language->kdeLanguageCode() << "is complete:" << language->isSupportComplete();
            if (!language->isSupportComplete()) {
                m_missingPackages.append(language->missingPackages());
                ++incompleteLanguages;
            }
        }
    }
    m_missingPackages.removeDuplicates();

    if (incompleteLanguages == 0) {
        return;
    }

    QString icon = QString("preferences-desktop-locale");
    QString text(i18nc("Notification when additional packages are required for complete system localization",
                       "Language support is incomplete, additional packages are required"));
    QStringList actions;
    actions << i18nc("Installs additional localization packages", "Install");
    actions << i18nc("Button to dismiss this notification once", "Ignore for now");
    actions << i18nc("Button to make this notification never show up again",
                     "Never show again");
    Event::show(icon, text, actions);
}

void L10nEvent::run()
{
    kDebug() << m_missingPackages;
    if (!m_missingPackages.isEmpty()) {
        QStringList args;
        args.append("--install");
        args.append(m_missingPackages);
        KToolInvocation::kdeinitExec("qapt-batch", args);
    }
    Event::run();
}
