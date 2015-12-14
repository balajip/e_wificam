#!/usr/bin/lua
x = os.clock()
WEB_SERVER_CONF = "/etc/conf.d/lighttpd/lighttpd.conf"
SERVERPUSH_CONF = "/etc/conf.d/serverpush_conf.xml"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache")
print ("")

if REQUEST_METHOD == "GET" then
	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
	print ("<root>")
	Counter = 0
    for line in io.lines(WEB_SERVER_CONF)
    do
		if string.sub(line, 1, 1) ~= "#" and string.find(line, "server.port") then
			TotalLen = string.len(line)
			Index = string.find(string.reverse(line)," ")-1
			Port = string.sub(line, TotalLen-Index+1)
			print ("    <HTTP_port>"..Port.."</HTTP_port>")
		end
	end
    for line in io.lines(SERVERPUSH_CONF)
    do
		_, _, _, name, _ = string.find(line, "(.*)(<accessname>.-</accessname>)(.*)")
		if name ~= nil then
			_, value, _ = string.match(name, "(<accessname>)(.*)(</accessname>)")
			print ("    <video TrackNo=\""..Counter.."\">\n        <accessname>"..value.."</accessname>")
			print ("    </video>")
			Counter = Counter + 1
		end
	end
	print ("<total_num>"..Counter.."</total_num>")
	print ("</root>")
else
	math.randomseed( os.time() )
	randomnum = math.random(65536)
	-- write input to temp file  
	f = io.open("/tmp/http."..randomnum, "w")
	if f then
		f:write(io.stdin:read("*all"))
	   	f:close()
	end                                                 
                                                    
	-- write command to system manager                      
	io.output("/tmp/sys_mgr.fifo")                         
	assert(io.write("5 /tmp/http.", randomnum, " $\n")) 
end

