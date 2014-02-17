#include "l10nevent.h"

L10nEvent::L10nEvent(QObject *parent) :
    Event(parent)
{
}

void L10nEvent::show()
{
    // check if packages are missing for the current default language
    // inform user
    // if user wishes to install -> install

    Event::show(icon, text, actions);
}

void L10nEvent::run()
{
    Event::run();
}
