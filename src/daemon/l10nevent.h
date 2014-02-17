#ifndef L10NEVENT_H
#define L10NEVENT_H

#include "event.h"

class L10nEvent : public Event
{
    Q_OBJECT
public:
    explicit L10nEvent(QObject *parent = 0);
    virtual ~L10nEvent();

public slots:
    void show();

private slots:
    void run();
};

#endif // L10NEVENT_H
