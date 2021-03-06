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
#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

// KDE includes
#include <KAboutData>
#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>

#warning oh god the class name

K_PLUGIN_FACTORY(NotificationHelperConfigFactory,
                 registerPlugin<NotificationHelperConfigModule>();)

NotificationHelperConfigModule::NotificationHelperConfigModule(QWidget* parent, const QVariantList&)
    : KCModule(parent)
    , m_apportCheckBox(nullptr)
    , m_driverCheckBox(nullptr)
    , m_hookCheckBox(nullptr)
    , m_installCheckBox(nullptr)
    , m_l10nCheckBox(nullptr)
    , m_rebootCheckBox(nullptr)
{
    KAboutData *aboutData = new KAboutData("kcmnotificationhelper",
                                        i18n("Kubuntu Notification Helper Configuration"),
                                        VERSION_STRING,
                                        QStringLiteral(""),
                                        KAboutLicense::LicenseKey::GPL,
                                        i18n("(C) 2009-2010 Jonathan Thomas, (C) 2009-2014 Harald Sitter"));

    aboutData->addAuthor(i18n("Jonathan Thomas"), QString(), "echidnaman@kubuntu.org");
    aboutData->addAuthor(i18n("Harald Sitter"), QString(), "apachelogger@ubuntu.com");

    setAboutData(aboutData);

    setButtons(Apply);
    setQuickHelp(i18n("Configure the behavior of Kubuntu Notification Helper"));

    KConfig kdedrc("notificationhelper", KConfig::NoGlobals);

    QVBoxLayout *lay = new QVBoxLayout(this);

    QLabel *label = new QLabel(this);
    label->setText(i18n("Show notifications for:"));

    m_apportCheckBox = new QCheckBox(i18n("Application crashes"), this);
    m_driverCheckBox = new QCheckBox(i18n("Proprietary Driver availability"), this);
    m_hookCheckBox = new QCheckBox(i18n("Upgrade information"), this);
    m_installCheckBox = new QCheckBox(i18n("Restricted codec availability"), this);
    m_l10nCheckBox = new QCheckBox(i18n("Incomplete language support"), this);
    m_rebootCheckBox = new QCheckBox(i18n("Required reboots"), this);

    connect(m_apportCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_driverCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_hookCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_installCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_l10nCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_rebootCheckBox, SIGNAL(clicked()), this, SLOT(configChanged()));

    QWidget *spacer = new QWidget(this);

    QLabel *label2 = new QLabel(this);
    label2->setText(i18n("Notification type:"));

    QButtonGroup *notifyTypeGroup = new QButtonGroup(this);
    m_comboRadio = new QRadioButton(this);
    m_comboRadio->setText(i18n("Use both popups and tray icons"));
    m_disableKNotifyRadio = new QRadioButton(this);
    m_disableKNotifyRadio->setText(i18n("Tray icons only"));
    m_disableTrayIconRadio = new QRadioButton(this);
    m_disableTrayIconRadio->setText(i18n("Popup notifications only"));

    notifyTypeGroup->addButton(m_comboRadio);
    notifyTypeGroup->addButton(m_disableKNotifyRadio);
    notifyTypeGroup->addButton(m_disableTrayIconRadio);

    connect(m_comboRadio, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_disableKNotifyRadio, SIGNAL(clicked()), this, SLOT(configChanged()));
    connect(m_disableTrayIconRadio, SIGNAL(clicked()), this, SLOT(configChanged()));

    lay->addWidget(label);
    lay->addWidget(m_apportCheckBox);
    lay->addWidget(m_driverCheckBox);
    lay->addWidget(m_hookCheckBox);
    lay->addWidget(m_installCheckBox);
    lay->addWidget(m_l10nCheckBox);
    lay->addWidget(m_rebootCheckBox);
    lay->addWidget(spacer);
    lay->addWidget(label2);
    lay->addWidget(m_comboRadio);
    lay->addWidget(m_disableKNotifyRadio);
    lay->addWidget(m_disableTrayIconRadio);
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
    m_driverCheckBox->setChecked(!notifyGroup.readEntry("hideDriverNotifier", false));
    m_hookCheckBox->setChecked(!notifyGroup.readEntry("hideHookNotifier", false));
    m_installCheckBox->setChecked(!notifyGroup.readEntry("hideInstallNotifier", false));
    m_l10nCheckBox->setChecked(!notifyGroup.readEntry("hideL10nNotifier", false));
    m_rebootCheckBox->setChecked(!notifyGroup.readEntry("hideRestartNotifier", false));

    KConfigGroup notifyTypeGroup(&cfg, "NotificationType");
    QString notifyType = notifyTypeGroup.readEntry("NotifyType", "Combo");

    if (notifyType == "Combo") {
        m_comboRadio->setChecked(true);
    } else if (notifyType == "TrayOnly") {
        m_disableKNotifyRadio->setChecked(true);
    } else {
        m_disableTrayIconRadio->setChecked(true);
    }
}

void NotificationHelperConfigModule::save()
{
    KConfig cfg("notificationhelper", KConfig::NoGlobals);
    KConfigGroup notifyGroup(&cfg, "Event");

    notifyGroup.writeEntry("hideApportNotifier", !m_apportCheckBox->isChecked(), KConfig::Notify);
    notifyGroup.writeEntry("hideDriverNotifier", !m_driverCheckBox->isChecked(), KConfig::Notify);
    notifyGroup.writeEntry("hideHookNotifier", !m_hookCheckBox->isChecked(), KConfig::Notify);
    notifyGroup.writeEntry("hideInstallNotifier", !m_installCheckBox->isChecked(), KConfig::Notify);
    notifyGroup.writeEntry("hideL10nNotifier", !m_l10nCheckBox->isChecked(), KConfig::Notify);
    notifyGroup.writeEntry("hideRestartNotifier", !m_rebootCheckBox->isChecked(), KConfig::Notify);

    KConfigGroup notifyTypeGroup(&cfg, "NotificationType");
    notifyTypeGroup.writeEntry("NotifyType", m_comboRadio->isChecked() ? "Combo" :
                               m_disableKNotifyRadio->isChecked() ? "TrayOnly" :
                               m_disableTrayIconRadio->isChecked() ? "KNotifyOnly" :
                               "Combo", KConfig::Notify);

    cfg.sync();
    notifyGroup.sync();
}

void NotificationHelperConfigModule::defaults()
{
    m_apportCheckBox->setChecked(true);
    m_driverCheckBox->setChecked(true);
    m_hookCheckBox->setChecked(true);
    m_installCheckBox->setChecked(true);
    m_l10nCheckBox->setChecked(true);
    m_rebootCheckBox->setChecked(true);
}

void NotificationHelperConfigModule::configChanged()
{
  emit changed(true);
}

#include "notificationhelperconfigmodule.moc"
