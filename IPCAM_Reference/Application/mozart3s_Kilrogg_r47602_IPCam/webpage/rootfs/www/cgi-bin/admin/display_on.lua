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
	os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h display -c EnableDisplay")
end

print ("<root>send successfully")
print ("</root>")
