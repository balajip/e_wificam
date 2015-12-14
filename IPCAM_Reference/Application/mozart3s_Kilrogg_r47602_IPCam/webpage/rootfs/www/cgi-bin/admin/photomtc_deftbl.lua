#!/usr/bin/lua
os.clock()
MSG_TOOL = "/usr/bin/msgctrl"
FIFO_PATH = "/tmp/venc/c0/command.fifo"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
if REQUEST_METHOD == "GET" then
	
	str =  string.match(QUERY_STRING, "user_defined_%d")
	
	if str == "user_defined_2" then
		os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h videoin -c photomtcdeftbl2")
	else
		os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h videoin -c photomtcdeftbl1")
	end	
	--for waiting venc finish writing the new LDC data to user defined table. 
	os.execute("sleep 6")
	
	print("str:"..str)
end
