/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "notificationhelperconfigmodule.h"

// Qt includes
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>

// KDE includes
#include <KAboutData>
#include <KDialog>
#include <KGenericFactory>

K_PLUGIN_FACTORY(NotificationHelperConfigFactory,
                 registerPlugin<NotificationHelperConfigModule>();)
K_EXPORT_PLUGIN(NotificationHelperConfigFactory("notificationhelperconfigmodule"))

NotificationHelperConfigModule::NotificationHelperConfigModule(QWidget* parent, const QVariantList&)
    : KCModule(NotificationHelperConfigFactory::componentData(), parent)
    , m_apportCheckBox(0)
    , m_hookCheckBox(0)
    , m_installCheckBox(0)
    , m_rebootCheckBox(0)
{
    KAboutData *about =
    new KAboutData(I18N_NOOP("kcmnotificationhelper"), 0,
            ki18n("Kubuntu Notification Helper Configuration"),
            "0.4.90", KLocalizedString(), KAboutData::License_GPL,
            ki18n("(C) 2009 Jonathan Thomas, (C) 2009 Harald Sitter"), KLocalizedString(),
            "http://kubuntu.org", "https://bugs.launchpad.net/ubuntu");

    about->addAuthor(ki18n("Jonathan Thomas"), KLocalizedString(), "echidnaman@kubuntu.org");
    about->addAuthor(ki18n("Harald Sitter"), KLocalizedString(), "apachelogger@ubuntu.com");
    setAboutData(about);
    setButtons(Apply);
    setQuickHelp(i18n("Configure the behavior of Kubuntu Notification Helper"));

    KConfig kdedrc("notificationhelper", KConfig::NoGlobals);

    QVBoxLayout *lay = new QVBoxLayout(this);

    QLabel *label = new QLabel(this);
    label->setText(i18n("Show notifications for:"));

    m_apportCheckBox = new QCheckBox(i18n("Application crashes"), this);
    m_hookCheckBox = new QCheckBox(i18n("Upgrade information"), this);
    m_installCheckBox = new QCheckBox(i18n("Restricted codec availability"), this);
    m_rebootCheckBox = new QCheckBox(i18n("Required reboots"), this);

    connect(m_apportCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_hookCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_installCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_rebootCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));

    lay->addWidget(label);
    lay->addWidget(m_apportCheckBox);
    lay->addWidget(m_hookCheckBox);
    lay->addWidget(m_installCheckBox);
    lay->addWidget(m_rebootCheckBox);
    lay->addStretch();
}

NotificationHelperConfigModule::~NotificationHelperConfigModule()
{
}

void NotificationHelperConfigModule::load()
{
    KConfig cfg("notificationhelper", KConfig::NoGlobals);
    KConfigGroup notifyGroup(&cfg, "Event");

    m_apportCheckBox->setChecked(!notifyGroup.readEntry("hideApportNotifier", false));
    m_hookCheckBox->setChecked(!notifyGroup.readEntry("hideHookNotifier", false));
    m_installCheckBox->setChecked(!notifyGroup.readEntry("hideInstallNotifier", false));
    m_rebootCheckBox->setChecked(!notifyGroup.readEntry("hideRestartNotifier", false));
}

void NotificationHelperConfigModule::save()
{
    KConfig cfg("notificationhelper", KConfig::NoGlobals);
    KConfigGroup notifyGroup(&cfg, "Event");

    notifyGroup.writeEntry("hideApportNotifier", !m_apportCheckBox->isChecked());
    notifyGroup.writeEntry("hideHookNotifier", !m_hookCheckBox->isChecked());
    notifyGroup.writeEntry("hideInstallNotifier", !m_installCheckBox->isChecked());
    notifyGroup.writeEntry("hideRestartNotifier", !m_rebootCheckBox->isChecked());

    cfg.sync();
    notifyGroup.sync();

    QDBusMessage message = QDBusMessage::createMethodCall("org.kubuntu.NotificationHelper",
                           "/NotificationHelper",
                           "org.kubuntu.NotificationHelper",
                           "reloadConfig");
    QDBusConnection::sessionBus().send(message);
}

void NotificationHelperConfigModule::defaults()
{
    m_apportCheckBox->setChecked(true);
    m_hookCheckBox->setChecked(true);
    m_installCheckBox->setChecked(true);
    m_rebootCheckBox->setChecked(true);
}

void NotificationHelperConfigModule::configChanged()
{
  emit changed(true);
}

#include "notificationhelperconfigmodule.moc"
