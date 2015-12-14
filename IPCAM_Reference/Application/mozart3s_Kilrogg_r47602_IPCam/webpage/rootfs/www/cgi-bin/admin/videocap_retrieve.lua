#!/usr/bin/lua
os.clock()
AWB_TMP_FILE = "/tmp/vencmaster_videocap_retrieve.xml"
MSG_TOOL = "/usr/bin/msgctrl"
FIFO_PATH = "/tmp/venc/c0/command.fifo"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
if REQUEST_METHOD == "GET" then
	os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h master -c RetVideoCapStatus")
	f = assert(io.open(AWB_TMP_FILE, "r"))
	if f then
		io.stdout:write(f:read("*all"))
		f:close()
	end
end
