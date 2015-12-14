#!/usr/bin/lua
os.clock()
 
MSG_TOOL = "/usr/bin/msgctrl"
FIFO_PATH = "/tmp/venc/c0/command.fifo"
AF_STATUS_PATH = "/tmp/af_status.log"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
print ("<root>")
if REQUEST_METHOD == "GET" then
	os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h videoin -c dumpAFStatus")
	os.execute("usleep 100000")
	tempfile = io.open(AF_STATUS_PATH, "r")
	if tempfile then
	      temp_buf = tempfile:read("*all")
	      tempfile:close()
	      print(temp_buf)
	    
	end
	
end
print ("</root>")
