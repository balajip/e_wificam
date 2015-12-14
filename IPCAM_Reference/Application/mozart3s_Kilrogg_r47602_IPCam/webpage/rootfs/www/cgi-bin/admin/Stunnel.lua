#!/usr/bin/lua

REQUEST_METHOD = os.getenv("REQUEST_METHOD")

DAEMON_SCRIPT = "/etc/init.d/stunnel"

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
print ("<root>")
print ("<return>ok</return>")
print ("</root>")

os.execute(DAEMON_SCRIPT.." restart")  --restart stunnel

