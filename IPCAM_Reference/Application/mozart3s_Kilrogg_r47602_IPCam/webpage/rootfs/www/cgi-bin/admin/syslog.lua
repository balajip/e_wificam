#!/usr/bin/lua

REMOTE_LOG_XML_CONFIG="/etc/conf.d/remotesyslog_conf.xml"
SYSLOG_CONF="/etc/syslog.conf"
SYSSERVICES="/etc/services"
g_szCurElem=""

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")

if  os.getenv("REQUEST_METHOD") == "GET" then
	io.input(REMOTE_LOG_XML_CONFIG)
	io.stdout:write(io.read("*all"))
else
	math.randomseed ( os.time() )
		randomnum = math.random(65536)

	-- write input to temp file
	f = io.open("/tmp/syslog."..randomnum, "w")
	if f then
		f:write(io.stdin:read("*all"))
		f:close()
	end
	-- write command to system manager
	io.output("/tmp/sys_mgr.fifo")
	io.write("2 /tmp/syslog.", randomnum, " $\n")
	print ("Success")
--    end
--    os.execute("/etc/init.d/sysklogd restart")
end
