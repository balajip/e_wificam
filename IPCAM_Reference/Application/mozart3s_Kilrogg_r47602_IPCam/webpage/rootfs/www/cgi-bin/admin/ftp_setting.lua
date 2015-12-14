#!/usr/bin/lua

FTP_CONF = "/etc/conf.d/ftp_config.xml"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("") 
if REQUEST_METHOD == "GET" then
	io.input(FTP_CONF)
	io.stdout:write(io.read("*all"))
else
	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
	print ("<root>")
    -- write input to temp file
    f = io.open(FTP_CONF, "w")
    if f then
	    f:write(io.stdin:read("*all"))
	    f:close()
	end
	os.execute("/usr/bin/dos2unix.sh  "..FTP_CONF.."  >  /dev/null  2>&1")
	print ("</root>")
end

