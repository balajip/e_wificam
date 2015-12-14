#!/usr/bin/lua
x = os.clock()
IPTABLE_DENYRULE = "/etc/iptables/iptables.deny"
--IPTABLE_DENYRULE = "iptables.deny"

REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")

if REQUEST_METHOD == "GET" then
	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
	print ("<root>")
	print ("    <access_list>")
	print ("        <deny>")
	Counter = 0
	for line in io.lines(IPTABLE_DENYRULE)
	do
		if string.find(line, "-A INPUT -p all -m iprange --src-range", 1, true) and string.find(line, "-j DROP") then
			StartIPStartIndex = string.find(line, "(%d+).(%d+).(%d+).(%d+)")
			StartIPEndIndex = string.find(line, "-", StartIPStartIndex+1)
			StartIP = string.sub(line, StartIPStartIndex, StartIPEndIndex-1)
			EndIPStartIndex = StartIPEndIndex+1
			EndIPEndIndex = string.find(line, " ", EndIPStartIndex)
			EndIP = string.sub(line, EndIPStartIndex , EndIPEndIndex-1)
			print ("            <rule id=\""..Counter.."\">")
			print ("                <start>"..StartIP.."</start>")
			print ("                <end>"..EndIP.."</end>")
			print ("            </rule>")
			Counter = Counter + 1
		end
	end
	print("            <total_num>"..Counter.."</total_num>")
	print("        </deny>")
	print("    </access_list>")
	print("</root>")
else
	math.randomseed( os.time() )
	randomnum = math.random(65536)
	-- write input to temp file
	f = io.open("/tmp/ipfilter."..randomnum, "w")
	if f then
		f:write(io.stdin:read("*all"))
		f:close()
	end

	-- write command to system manager
	io.output("/tmp/sys_mgr.fifo")
	assert(io.write("6 /tmp/ipfilter.", randomnum, " $\n"))
end

