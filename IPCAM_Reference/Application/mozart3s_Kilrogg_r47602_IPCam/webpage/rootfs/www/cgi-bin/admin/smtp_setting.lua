#!/usr/bin/lua

SMTP_CONF = "/etc/conf.d/smtp_config.xml"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
local lfs = require"lfs"
print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("") 
if REQUEST_METHOD == "GET" then
	io.input(SMTP_CONF)
	io.stdout:write(io.read("*all"))
else
	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
	print ("<root>")
    -- write input to temp file
    f = io.open(SMTP_CONF, "w")
    if f then
		local ret, err = lfs.lock(f, "w") -- exclusive lock!
	    f:write(io.stdin:read("*all"))
	    f:close()
	end
	os.execute("/usr/bin/dos2unix.sh  "..SMTP_CONF.."  >  /dev/null  2>&1")
	print ("</root>")
end

