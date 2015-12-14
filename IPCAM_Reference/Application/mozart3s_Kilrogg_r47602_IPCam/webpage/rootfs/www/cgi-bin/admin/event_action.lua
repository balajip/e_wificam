#!/usr/bin/lua

EVENT_ACTION_CONF = "/etc/conf.d/event_action_conf.xml"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
local lfs = require"lfs"
print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("") 
if REQUEST_METHOD == "GET" then
	io.input(EVENT_ACTION_CONF)
	io.stdout:write(io.read("*all"))
else
	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
	print ("<root>")
    -- write input to temp file
    f = io.open(EVENT_ACTION_CONF, "w")
    if f then
		local ret, err = lfs.lock(f, "w") -- exclusive lock!
	    f:write(io.stdin:read("*all"))
	    f:close()
	end
	print ("</root>")
	os.execute("/usr/bin/dos2unix.sh  "..EVENT_ACTION_CONF.."  >  /dev/null  2>&1")
	os.execute("/etc/init.d/event reload")
end

