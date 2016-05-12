#!/bin/sh

cp ../bin/wdctl .
cp ../bin/wdrst .
cp ../bin/wifidog .
cp ../wifidog-msg.html .
cp ../wifidog.conf .
cp ../lib/libhttpd.so.0.0.0 .

mips-openwrt-linux-strip wdctl
mips-openwrt-linux-strip wdrst
mips-openwrt-linux-strip wifidog

tar zcvf wifidog.tgz wdctl wdrst wifidog wifidog.conf wifidog-msg.html libhttpd.so.0.0.0
cat install.sh wifidog.tgz > wifidog.run

chmod +x wifidog.run
