#!/bin/sh

RUN=wifidog-20160607.1.run

cp ../bin/wdctl bin
cp ../bin/wifidog bin
cp ../wifidog.conf etc
cp ../wifidog-msg.html etc
cp ../lib/libhttpd.so.0.0.0 lib

mipsel-openwrt-linux-strip bin/*
mipsel-openwrt-linux-strip lib/libhttpd.so.0.0.0

tar zcvf wifidog.tgz bin etc lib
cat install.sh wifidog.tgz > $RUN

chmod +x $RUN
