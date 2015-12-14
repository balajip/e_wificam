#!/usr/bin/lua

x = os.clock()

CONFIG_FILE_PATH = "/etc/conf.d/c0/"
--QUERY_STRING = "vdec_conf.xml&rtspc.0.xml&rtspc.1.xml&rtspc.2.xml&rtspc.3.xml"
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("") 
print ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>") 
print ("<root>") 

if QUERY_STRING~=nil then
    for filename in string.gfind(QUERY_STRING, "[^&]+") do
        local file = io.open(CONFIG_FILE_PATH..filename, "r")
        if file then
            tmp = string.gsub(file:read("*all"), "<%?.-\n", "")
            tmp = string.gsub(tmp, "<.?root>.?", "")
            print (tmp)
        end
    end
end

--print(string.format("<time>%.2f (sec)</time>", os.clock() - x))

print ("</root>") 
