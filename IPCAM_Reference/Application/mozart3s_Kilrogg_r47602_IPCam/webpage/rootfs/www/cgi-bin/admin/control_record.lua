#!/usr/bin/lua
VREC_CONF="/etc/conf.d/vrec_conf.xml"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")
local lfs = require"lfs"
print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
print ("<root>")
if  REQUEST_METHOD == "GET" then

file = io.open(VREC_CONF, "r")
if file then
            --tmp = string.gsub(file:read("*all"), "<%?.-\n", "")
            --tmp = string.gsub(tmp, "<.?root>.?", "")
	    tmp=file:read("*all")
            --print (tmp)
	    file:close()
end


_, _, vrec_head, switch, vrec_interval, record, vrec_tail = string.find(tmp, "(.*)(<switch>.-</switch>)(.*)(<record>.-</record>)(.*)")

_, switch_value, _ =  string.match(switch, "(<%w+>)(%w+)(</%w+>)")
switch_value = (switch_value+1)%2
_, record_value, _ =  string.match(record, "(<%w+>)(%w+)(</%w+>)")
--print(switch)
--print(switch_value)
--print(record)
--print(record_value)
if QUERY_STRING == "on" then
record_value=1
end
if QUERY_STRING == "off" then
record_value=0
end
new_vrec = vrec_head.."<switch>"..switch_value.."</switch>"..vrec_interval.."<record>"..record_value.."</record>".. vrec_tail
--print(new_vrec)
file = io.open(VREC_CONF, "w")
if file then
				local ret, err = lfs.lock(file, "w") -- exclusive lock!
                file:write(new_vrec)
                file:close()
end
os.execute("/usr/bin/dos2unix.sh  "..VREC_CONF.."  >  /dev/null  2>&1")
os.execute("/etc/init.d/vrec reload > /dev/null 2>&1")


end
print ("</root>")