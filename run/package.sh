#!/bin/sh

cp ../bin/wdctl bin
cp ../bin/wifidog bin
cp ../wifidog.conf etc
cp ../wifidog-msg.html etc
cp ../lib/libhttpd.so.0.0.0 lib

mips-openwrt-linux-strip bin/*
mips-openwrt-linux-strip lib/libhttpd.so.0.0.0

tar zcvf wifidog.tgz bin etc lib
cat install.sh wifidog.tgz > wifidog.run

chmod +x wifidog.run
cp wifidog.run wifidog-1.2.run
