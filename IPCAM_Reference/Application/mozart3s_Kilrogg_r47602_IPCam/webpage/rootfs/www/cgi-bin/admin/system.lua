#!/usr/bin/lua

SYSTEM_XML_CONF = "/etc/conf.d/system.xml"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("")

if REQUEST_METHOD == "GET" then
    settime= "<set_time>" .. os.date("%m%d%H%M%Y.%S") .. "</set_time>"
    for line in io.lines(SYSTEM_XML_CONF) do
        if (string.match(line, "<set_time>") ~= nil) then
            io.stdout:write(settime .. "\n")
        else
            io.stdout:write(line.."\n")
        end         
    end
--      io.input(SYSTEM_XML_CONF)
--      io.stdout:write(io.read("*all"))
else
    print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
    print ("<root>")

    math.randomseed( os.time() )
	randomnum = math.random(65536)

    -- write input to temp file
    f = io.open("/tmp/system."..randomnum, "w")
    if f then
        f:write(io.stdin:read("*all"))
        f:close()
    end
    
    -- write command to system manager 
    io.output("/tmp/sys_mgr.fifo")
    ret = assert(io.write("4 /tmp/system.", randomnum, " $\n"))
	
    if ret == true then
	    ret = 0
	else
	    ret = 1
	end
	
    print ("    <reset_config>"..ret.."</reset_config>")
	print ("</root>")
end
