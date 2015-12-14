#/bin/sh

echo "Now just support AAC4 bistream dump. You can check /etc/conf.d/aenc_conf.xml for codec."

/etc/init.d/rtsps  stop
/etc/init.d/aenc  stop

echo "Start /usr/sbin/aenc_receiver as daemon..."
/usr/sbin/aenc_receiver &

sleep 1

echo "Start /etc/init.d/aenc"
/etc/init.d/aenc  start

echo "The bistream dump file is /tmp/test.aac"
