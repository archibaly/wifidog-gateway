#!/bin/sh

LINES=19
TAR_EXE=/tmp/wifidog.tgz

ok=`cat /etc/rc.local | grep wifidog`
if [ -z "$ok" ]; then
	sed -i '/exit 0/i\wifidog' /etc/rc.local
fi

tail -n +$LINES $0 > $TAR_EXE

tar -zxvf $TAR_EXE -C /bin
mv /bin/wifidog.conf /etc
rm -f $TAR_EXE
wifidog

exit 0
