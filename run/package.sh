#!/bin/sh

cp ../bin/wdctl .
cp ../bin/wdrst .
cp ../bin/wifidog .
cp ../wifidog.conf .
tar zcvf wifidog.tgz wdctl wdrst wifidog wifidog.conf
cat install.sh wifidog.tgz > wifidog.run
chmod +x wifidog.run
