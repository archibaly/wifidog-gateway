#!/bin/sh

LINES=17
TAR=/tmp/wifidog.tgz

tail -n +$LINES $0 > $TAR

tar -zxvf $TAR -C / > /dev/null

rm -f $0
rm -f $TAR

killall -9 wifidog
wifidog

exit 0
