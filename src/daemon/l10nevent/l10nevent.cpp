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

#include <QDebug>

#include <KConfigGroup>
#include <KToolInvocation>
#include <KSharedConfig>

#include <Kubuntu/l10n_language.h>
#include <Kubuntu/l10n_languagecollection.h>

L10nEvent::L10nEvent(QObject *parent)
    : Event(parent, "L10n")
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
    const KSharedConfig::Ptr userConfig = KSharedConfig::openConfig("kdeglobals", KConfig::IncludeGlobals);
    const KConfigGroup userSettings = KConfigGroup(userConfig, "Locale");
    const QString languageConfigString = userSettings.readEntry("Language", QString());
    const QStringList kdeLanguageList = languageConfigString.split(QLatin1Char(':'),
                                                                   QString::SkipEmptyParts);
    qDebug() << "KDE Languages:" << kdeLanguageList;

    m_missingPackages.clear();

    // languages() at the time of writing has no caching capability, so make sure
    // that it is not called more than necessary.
    const QSet<Kubuntu::Language *> languages = m_languageCollection->languages();

    // Check if we can find the kde languages and if they are complete.
    foreach (const QString &kdeLanguage, kdeLanguageList) {
        foreach (Kubuntu::Language *language, languages) {
            if (kdeLanguage == language->kdeLanguageCode()) {
                qDebug() << "matched" << kdeLanguage;
                checkForMissingPackages(language);
            }
        }
    }

    // Additionally check the system locale.
    // This is necessary because
    //   - after installation there won't be a KCM config, but a possibly
    //     incomplete language support.
    //   - if the user mangles his locale manually or through other tools
    //     we still want him to complete his language support.
    const QStringList matchables = systemLocaleMatchables();
    qDebug() << "System Language Matchables:" << matchables;
    bool matched = false;
    foreach (const QString &matchable, matchables) {
        foreach (Kubuntu::Language *language, languages) {
            if (matchable == language->kdeLanguageCode()) {
                qDebug() << "matched" << matchable;
                checkForMissingPackages(language);
                // If we had a match we abort as we only want the most generic
                // match.
                // e.g. we want 'ca@valencia' but not 'ca'.
                matched = true;
            }
        }
        if (matched) {
            continue;
        }
    }

    m_missingPackages.removeDuplicates();

    if (m_missingPackages.isEmpty()) {
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
    qDebug() << m_missingPackages;
    if (!m_missingPackages.isEmpty()) {
        QStringList args;
        args.append("--install");
        args.append(m_missingPackages);
        KToolInvocation::kdeinitExec("qapt-batch", args);
    }
    Event::run();
}

bool L10nEvent::checkForMissingPackages(Kubuntu::Language *language)
{
    // Not cached, so cache here.
    const bool isSupportComplete = language->isSupportComplete();
    qDebug() << "  completeness:" << isSupportComplete;
    if (!isSupportComplete) {
        m_missingPackages.append(language->missingPackages());
        return true;
    }
    return false;
}

QStringList L10nEvent::systemLocaleMatchables() const
{
    const QString systemLocale = qgetenv("LANG");

    // Valid locales may be C or LANGUAGE followed by country and/or encoding
    // and/or variant. Leading to the following regex:
    // Please note that the regex is a bit blown up because greedy matching
    // requires us to except the seperator characters to get the intended result.
    //                   v language exactly once
    //                   .            v country one match at the most
    //                   .            .               v encoding one match at the most
    //                   .            .               .              v variant one match at the most
    QRegExp localeRegex("([^_\\.\\@]+)(_([^\\.\\@]+))?(\\.([^\\@]+))?(\\@(.+))?");
    if (localeRegex.indexIn(systemLocale) == -1) {
        return QStringList();
    }
    qDebug() << localeRegex.capturedTexts();

    const QString language = localeRegex.capturedTexts().at(1);
    const QString country = localeRegex.capturedTexts().at(3);
    // Not useful for language resolution:
    // const QString encoding = localeRegex.capturedTexts().at(5);
    const QString variant = localeRegex.capturedTexts().at(7);

    QStringList matchables;
    matchables.reserve(4); // We can only ever match 4.

    if (!language.isEmpty()) { // Language cannot really be empty, but oh well.
        matchables.prepend(language);
    }
    if (!language.isEmpty() && !country.isEmpty()) {
        matchables.prepend(QString::fromLatin1("%1_%2").arg(language, country));
    }
    if (!language.isEmpty() && !variant.isEmpty()) {
        matchables.prepend(QString::fromLatin1("%1@%2").arg(language, variant));
    }
    if (!language.isEmpty() && !country.isEmpty() && !variant.isEmpty()) {
        matchables.prepend(QString::fromLatin1("%1_%2@%3").arg(language, country, variant));
    }
    return matchables;
}
