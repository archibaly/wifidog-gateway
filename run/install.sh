#!/bin/sh

LINES=12
TAR=/tmp/wifidog.tgz

tail -n +$LINES $0 > $TAR

tar -zxvf $TAR -C / > /dev/null
rm -f $TAR

exit 0
