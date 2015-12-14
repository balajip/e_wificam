#!/usr/bin/lua
os.clock()
MSG_TOOL = "/usr/bin/msgctrl"
FIFO_PATH = "/tmp/venc/c0/command.fifo"
KEEP_CURRENT_WB_FILE_PATH = "/etc/conf.d/keep_current_wb"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
print ("<root>")
if REQUEST_METHOD == "GET" then
	os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h videoin -c keepCurrentWB")
	os.execute("usleep 100000")
	tempfile = io.open(KEEP_CURRENT_WB_FILE_PATH, "r")
	if tempfile then
	      temp_buf = tempfile:read("*all")
	      tempfile:close()
	      _, _, redgain, _, bluegain= string.find(temp_buf, "(.*)( )(.*)")
	  
	     -- print(temp_buf)
	     -- print(head_1)
	      print("<RGain>"..redgain.."</RGain>")
	      print("<BGain>"..bluegain.."</BGain>")
	      print("<GGain>".."64".."</GGain>")
	end
	
end
print ("</root>")
