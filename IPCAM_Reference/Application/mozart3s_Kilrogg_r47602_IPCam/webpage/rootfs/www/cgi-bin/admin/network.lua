#!/usr/bin/lua

NETWORK_XML_CONF = "/etc/network/network_conf.xml"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("")

if REQUEST_METHOD == "GET" then
	os.execute("/sbin/ifconfig eth0 | grep 'inet ' | sed 's/^.*addr://g' | sed 's/  Bcast.*$//g' > /tmp/ipaddr.txt")
	for line in io.lines(NETWORK_XML_CONF)
	do
		if string.find(line, "ip_addr") then
			f = io.open("/tmp/ipaddr.txt")
			if f then
				ipaddr = f:read("*all")
				f:close()
				os.remove("/tmp/ipaddr.txt")
			end
			Len = string.len(ipaddr)
			ipaddr = string.sub(ipaddr, 1, Len-1)
			io.stdout:write("                <ip_addr>"..ipaddr.."</ip_addr>")
		else
			io.stdout:write(line)
		end
	end
else
    print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
    print ("<root>")

    math.randomseed( os.time() )
	randomnum = math.random(65536)

    -- write input to temp file
    f = io.open("/tmp/network."..randomnum, "w")
    if f then
        f:write(io.stdin:read("*all"))
        f:close()
    end
    
    -- write command to system manager 
    io.output("/tmp/sys_mgr.fifo")
    ret = assert(io.write("1 /tmp/network.", randomnum, " $\n"))
	
    if ret == true then
	    ret = 0
	else
	    ret = 1
	end
	
    print ("    <reset_config>"..ret.."</reset_config>")
    print ("    <reboot>false</reboot>")
	print ("</root>")
	
end
