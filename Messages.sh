#! /usr/bin/env bash
$XGETTEXT `find src/daemon -name '*.cpp'` -o $podir/notificationhelper.pot
$XGETTEXT `find src/kcmodule -name '*.cpp'` -o $podir/kcm_notificationhelper.pot
