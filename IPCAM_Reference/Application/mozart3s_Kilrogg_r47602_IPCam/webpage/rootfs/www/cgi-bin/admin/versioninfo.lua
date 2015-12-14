#!/usr/bin/lua

VERSION_FILE="/usr/local/firmware/.firmware-release"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")

if REQUEST_METHOD == "GET" then
        print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
        print ("<root>")
	print ("    <firmware>")
	for line in io.lines(VERSION_FILE)
	do
		if string.find(line, "Firmware Version:") ~= nil then
			Index = string.find(line, ":")+2
			print ("        <version>"..string.sub(line, Index).."</version>")
		end

		if string.find(line, "Create on:") ~= nil then
			Index = string.find(line, ":")+2
			print ("        <create_date>"..string.sub(line, Index).."</create_date>")
		end
	end
	print ("    </firmware>")
	print ("</root>")
end
