#!/bin/sh

cp ../bin/wdctl .
cp ../bin/wifidog .
cp ../wifidog-msg.html .
cp ../wifidog.conf .
cp ../lib/libhttpd.so.0.0.0 .

mips-openwrt-linux-strip wdctl
mips-openwrt-linux-strip wifidog
mips-openwrt-linux-strip libhttpd.so.0.0.0

tar zcvf wifidog.tgz wdctl wifidog wifidog.conf wifidog-msg.html libhttpd.so.0.0.0
cat install.sh wifidog.tgz > wifidog.run

chmod +x wifidog.run
