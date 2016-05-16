#!/bin/sh

cp ../bin/wdctl .
cp ../bin/wifidog .
cp ../bin/prodaemon .
cp ../lib/libhttpd.so.0.0.0 .

mips-openwrt-linux-strip wdctl
mips-openwrt-linux-strip wifidog
mips-openwrt-linux-strip prodaemon
mips-openwrt-linux-strip libhttpd.so.0.0.0

tar zcvf wifidog.tgz wdctl wifidog prodaemon wifidog.conf wifidog-msg.html libhttpd.so.0.0.0
cat install.sh wifidog.tgz > wifidog.run

chmod +x wifidog.run
