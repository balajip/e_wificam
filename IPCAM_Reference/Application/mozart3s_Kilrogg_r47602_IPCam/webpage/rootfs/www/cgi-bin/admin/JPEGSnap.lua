#!/usr/bin/lua
print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")

if  os.getenv("REQUEST_METHOD") == "POST" then
	
	os.execute("sh /bin/JPEGSnap.sh")

end


