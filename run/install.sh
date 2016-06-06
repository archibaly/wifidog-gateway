#!/bin/sh

LINES=17
TAR=/tmp/wifidog.tgz

tail -n +$LINES $0 > $TAR

tar -zxf $TAR -C /

rm -f $0
rm -f $TAR

killall -9 wifidog 2> /dev/null
wifidog

exit 0
