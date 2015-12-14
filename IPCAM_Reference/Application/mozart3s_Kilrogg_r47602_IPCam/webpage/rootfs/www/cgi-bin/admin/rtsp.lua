#!/usr/bin/lua
x = os.clock()
RTSPS_XML_CONF = "/etc/conf.d/stream_server_config.xml"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")
local lfs = require"lfs"
print ("Content-type: text/xml") 
print ("Cache-Control: no-cache")
print ("")

if REQUEST_METHOD == "GET" then
    io.input(RTSPS_XML_CONF)
    io.stdout:write(io.read("*all"))
else

    -- write input to temp file
    f = io.open(RTSPS_XML_CONF, "w")
    if f then
		local ret, err = lfs.lock(f, "w") -- exclusive lock!
        f:write(io.stdin:read("*all"))
        f:close()
    end
	os.execute("/usr/bin/dos2unix.sh  "..RTSPS_XML_CONF.."  >  /dev/null  2>&1")
    os.execute("/etc/init.d/rtsps stop")
    os.execute("/etc/init.d/rtsps start")
    os.execute("/etc/init.d/aenc stop")
    os.execute("/etc/init.d/aenc start")
    os.execute("/usr/sbin/setmdnsconf")
    os.execute("/etc/init.d/mDNSResponderPosix reload")
    print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
    print ("<root>")
    print(string.format("<time>%.2f (sec)</time>", os.clock() - x))
	print ("</root>")
end
