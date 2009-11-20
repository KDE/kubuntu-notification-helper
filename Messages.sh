#! /usr/bin/env bash
$XGETTEXT `find src/daemon/apportevent -name '*.cpp'` `find src/daemon/hookevent -name '*.cpp'` `find src/daemon/installevent -name '*.cpp'` `find src/daemon/rebootevent -name '*.cpp'` -o $podir/notificationhelper.pot
