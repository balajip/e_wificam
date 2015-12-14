#!/usr/bin/lua

os.execute("/bin/cp /etc/conf.d/event_test.jpg /tmp/event")
os.execute("/usr/sbin/ftp_app.lua /tmp/event/event_test.jpg")
