/***************************************************************************
 *   Copyright © 2009 by Jonathan Thomas <echidnaman@kubuntu.org>          *
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

#include "installgui.h"

// Qt includes
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

// // KDE includes
#include <KDialog>
#include <KLocalizedString>
#include <KNotification>
#include <KToolInvocation>

InstallGui::InstallGui(QObject* parent, const QString application, const QMap<QString, QString> packageList)
        : QObject(parent)
        , m_dialog(0)
        , m_applicationName(0)
        , m_toInstallList(0)
{
    m_toInstallList.clear();
    m_applicationName = application;

    m_dialog = new KDialog;
    m_dialog->setWindowIcon(KIcon("download"));
    m_dialog->setCaption(i18n("Install Packages"));
    m_dialog->setButtons(KDialog::Ok | KDialog::Cancel);
    m_dialog->setButtonText(KDialog::Ok, i18n("Install Selected"));
    connect(m_dialog, SIGNAL(okClicked()), SLOT(runPackageInstall()));
    connect(m_dialog, SIGNAL(okClicked()), SLOT(cleanUpDialog()));

    QWidget* widget = new QWidget(m_dialog);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    widget->setLayout(layout);

    QLabel* label = new QLabel(widget);
    label->setWordWrap(true);
    label->setText(i18n("For extra multimedia functionality, select packages to be installed."
                        " These packages are not installed by default due to either patent"
                        " issues or restrictive licensing."));
    layout->addWidget(label);

    QListWidget* listWidget = new QListWidget(widget);
    connect(listWidget, SIGNAL(itemChanged(QListWidgetItem *)), SLOT(packageToggled(QListWidgetItem *)));
    layout->addWidget(listWidget);

    QMap<QString, QString>::const_iterator nameIter = packageList.constBegin();
    while (nameIter != packageList.end()) {
        QListWidgetItem* item = new QListWidgetItem(nameIter.value());
        item->setToolTip(nameIter.key());
        m_toInstallList << nameIter.key();
        item->setCheckState(Qt::Checked);
        listWidget->addItem(item);
        ++nameIter;
    }

    m_dialog->setMainWidget(widget);
    m_dialog->show();
}

InstallGui::~InstallGui()
{
    delete m_dialog;
}

void InstallGui::packageToggled(QListWidgetItem *item)
{
    if (item->checkState() == Qt::Checked) {
        m_toInstallList << item->toolTip();
    } else {
        m_toInstallList.removeAt(m_toInstallList.indexOf(item->toolTip()));
    }
}

void InstallGui::runPackageInstall()
{
    int returnValue = KToolInvocation::kdeinitExec("/usr/lib/kde4/libexec/kdesu",
                                                   QStringList() << "install-package --install" << m_toInstallList);
    if (returnValue == 0) {
        KNotification *notify = new KNotification("Install", 0);
        notify->setComponentData(KComponentData("notificationhelper"));

        notify->setPixmap(KIcon("download").pixmap(48,48));
        notify->setText(i18n("Once the install is finished, you will need to restart %1 to use the new functionality", m_applicationName));

        notify->sendEvent();
    }
}

void InstallGui::cleanUpDialog()
{
    m_dialog->deleteLater();
    m_dialog = 0;
}

#include "installgui.moc"
