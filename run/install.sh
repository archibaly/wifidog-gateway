#!/bin/sh

LINES=24
TAR_EXE=/tmp/wifidog.tgz

ok=`cat /etc/rc.local | grep wifidog`
if [ -z "$ok" ]; then
	sed -i '/exit 0/i\wifidog' /etc/rc.local
fi

tail -n +$LINES $0 > $TAR_EXE

tar -zxvf $TAR_EXE -C /bin > /dev/null
mv /bin/wifidog.conf /etc
mv /bin/wifidog-msg.html /etc
rm -f $TAR_EXE

mv /bin/libhttpd.so.0.0.0 /lib
cd /lib
ln -sf libhttpd.so.0.0.0 libhttpd.so.0
ln -sf libhttpd.so.0 libhttpd.so

exit 0
