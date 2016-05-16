#!/bin/sh

LINES=29
TAR=/tmp/wifidog.tgz

wifidog=`cat /etc/rc.local | grep wifidog`
if [ -z "$wifidog" ]; then
	sed -i '/exit 0/i\wifidog' /etc/rc.local
fi

prodaemon=`cat /etc/rc.local | grep prodaemon`
if [ -z "$prodaemon" ]; then
	sed -i '/exit 0/i\prodaemon' /etc/rc.local
fi

tail -n +$LINES $0 > $TAR

tar -zxvf $TAR -C /bin > /dev/null
mv /bin/wifidog.conf /etc
mv /bin/wifidog-msg.html /etc
rm -f $TAR

mv /bin/libhttpd.so.0.0.0 /lib
cd /lib
ln -sf libhttpd.so.0.0.0 libhttpd.so.0
ln -sf libhttpd.so.0 libhttpd.so

exit 0
