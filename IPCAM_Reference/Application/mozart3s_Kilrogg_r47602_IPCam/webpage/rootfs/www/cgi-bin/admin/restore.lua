#!/usr/bin/lua

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("") 
    print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
    print ("<root>") 
	-- invoke sysmgr to execute reboot
	io.output("/tmp/sys_mgr.fifo")
	ret = assert(io.write("7\n"))
		
	if ret == true then 
		ret = 0 
	else 
		ret = 1 
	end 
	
	print ("    <reset_config>"..ret.."</reset_config>") 
	print '</root>'

