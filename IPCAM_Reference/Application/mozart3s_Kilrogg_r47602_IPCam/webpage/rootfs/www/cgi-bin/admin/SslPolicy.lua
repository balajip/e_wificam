#!/usr/bin/lua

--[[
function DropListNum ()
    local LocalTempFile = "/etc/iptemp.ip"
    os.execute(IPTABLES.." -L INPUT > "..LocalTempFile)
    local LocalFile = io.open(LocalTempFile)
    local LocalString
    local LocalNum = 0
    for LocalString in string.gmatch(LocalFile:read("*all"), "dpt:http") do
        --print("LocalString in DropListNum: "..LocalString)
        LocalNum = LocalNum + 1
    end
    --print("total number of drop http list: "..LocalNum.."\n")
    --print(LocalNum)
    LocalFile:close()
    os.execute("rm "..LocalTempFile)
    return LocalNum
end
]]--

REQUEST_METHOD = os.getenv("REQUEST_METHOD")
DAEMON_SCRIPT = "/etc/init.d/stunnel"
PEM = "/etc/conf.d/ssl/certificate.pem"
PID_FILE = "/var/run/stunnel.pid"
IPTABLES = "/usr/sbin/iptables"
IP_FLAG = " INPUT 1 -i eth0 -p tcp --dport 80 -j DROP"
IP_SFLAG = " INPUT 1 -i eth0 -p tcp --dport 443 -j DROP"
SWITCH_FILE = "/etc/conf.d/ssl/switch"
SWITCH = ""
INPUT = ""

local SFile = io.open(SWITCH_FILE)
SWITCH = string.gsub(SFile:read("*all"), "%c?", "")
SFile:close()

--[[
if os.execute("[ -f "..PID_FILE.." ]")==0 then  --chech if stunnel is executing
    SWITCH = "1"
else
    SWITCH = "0"
end
]]--
 

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache")
print ("")

if REQUEST_METHOD=="GET" then
	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
    print ("<root>")
    print ("    <sslpolicy>")
    print ("        <switch>"..SWITCH.."</switch>")
    print ("    </sslpolicy>")
    print ("</root>")
else
    local LocalString 
	local WFlag = 0
    for LocalString in io.stdin:lines() do
        if string.find(LocalString, "switch") then
            INPUT = LocalString:gsub("%s*<.?switch>%c?", "")  --INPUT is "        #"
        end
    end
	SFile = io.open(SWITCH_FILE, "w")
	if os.execute("[ -f "..PEM.." ]")==0 then  --check certificate exist
		if SWITCH~=INPUT then  --check switch
			if SWITCH=="1" then  --turn off
				if os.execute("[ -f "..PID_FILE.." ]")==0 then  --stunnel executing
					os.execute(IPTABLES.." -R"..IP_SFLAG)
					WFlag = 1
				end
			elseif SWITCH=="0" then  --turn on
				if os.execute("[ -f "..PID_FILE.." ]")==0 then  --stunnel executing
					os.execute(IPTABLES.." -R"..IP_FLAG)
					WFlag = 1
				end
			else
				--print("Error!")
			end
		end
	else
		--print("Certificate does not exist, please generate first")
	end

	if WFlag == 0 then
		SFile:write(SWITCH.."\n")
	else
		SFile:write(INPUT.."\n")
	end

	SFile:close()
	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
	print ("<root>")
	print ("    <redirect>"..WFlag.."</redirect>")
	if WFlag == 0 then
		print ("    <https>"..SWITCH.."</https>")
	else
		print ("    <https>"..INPUT.."</https>")
	end
	print ("</root>")
end

