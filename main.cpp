#include "updatehelpernotifier.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>

static const char description[] =
    I18N_NOOP("Notification daemon for Kubuntu update helpers");

static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("updatehelpernotifier", 0, ki18n("Update Helper Notifier"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2009 Jonathan Thomas"), KLocalizedString(), 0, "echidnaman@kubuntu.org");
    about.addAuthor( ki18n("Jonathan Thomas"), KLocalizedString(), "echidnaman@kubuntu.org" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    UpdateHelperNotifier *notifier = new UpdateHelperNotifier;

    return app.exec();
}
