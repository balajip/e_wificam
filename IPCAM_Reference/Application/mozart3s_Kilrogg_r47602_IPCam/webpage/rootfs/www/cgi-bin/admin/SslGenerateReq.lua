#!/usr/bin/lua

function GenSubject ()  --generate SUBJECT
	local LocalIndex=0
	local LocalString
	for LocalIndex,LocalString in ipairs(INPUT) do
		if LocalIndex==table.getn(PROP) then
			break
		end
		if LocalString~="" then
			SUBJECT=SUBJECT.."/"..PROP[LocalIndex].."="..LocalString
		end
	end
end

REQUEST_METHOD = os.getenv("REQUEST_METHOD")
CONFIG_FILE_PATH = "/etc/conf.d/ssl"
CONF=CONFIG_FILE_PATH.."/openssl.cnf"
CERT=CONFIG_FILE_PATH.."/host.cert"
REQ=CONFIG_FILE_PATH.."/host.req"
KEY=CONFIG_FILE_PATH.."/host.key"
PEM=CONFIG_FILE_PATH.."/certificate.pem"
EXE="/sbin/openssl req"
FLAG=" -out "..REQ.." -newkey rsa:1024 -nodes -keyout "..KEY.." -sha1 -config "..CONF.." -subj "
PROP={"C", "ST", "L", "O", "OU", "CN", "emailAddress"}
ELEMENTS={"country", "state", "location", "organization", "unit", "name", "email"}
INPUT={}
SUBJECT=""

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")

if REQUEST_METHOD == "GET" then
    print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
    print ("<root>")
    print ("    <sslgeneratereq>")
    print ("        <country>.</country>")
    print ("        <state>.</state>")
    print ("        <location>.</location>")
    print ("        <organization>.</organization>")
    print ("        <unit>.</unit>")
    print ("        <name>.</name>")
    print ("        <email>.</email>")
    print ("    </sslgeneratereq>")
    print ("</root>")
else
    local LocalString
    local LocalIndex = 1
	local Hint = 1
    for LocalString in io.stdin:lines() do  --get subject parameter
        if LocalIndex <= tonumber(table.getn(ELEMENTS)) and LocalString:match(ELEMENTS[LocalIndex]) == ELEMENTS[LocalIndex] then
            INPUT[LocalIndex] = LocalString:gsub("%s*<.?"..ELEMENTS[LocalIndex]..">%c?", "")  --IE will send strings end with a \r
            LocalIndex = LocalIndex + 1
        end
    end
	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
	print ("<root>")
	print ("    <reqout>")
	print ("        <text>")
    if (os.execute("[ -f "..PEM.." ]")==0 or os.execute("[ -f "..CERT.." ]")==0) then  --certificate exist
        print("Certificate exist, please remove at first!")
		Hint = 0
    else
		if os.execute("[ -f "..REQ.." ]")==0 then  --request exist
			--print("Request exist, overwrite!!")
		end
	    GenSubject()  --generate subject
	    os.execute(EXE..FLAG.."'"..SUBJECT.."'")
		--print(SUBJECT)
        local LocalFile = io.open(REQ)
		if LocalFile then
        	print (LocalFile:read("*all"))
        	LocalFile:flush()
        	LocalFile:close()
		else
			print ("Generate request FAIL!")
			Hint = 0
			--print ("Generate request FAIL!Please check if 'COUNTRY' is EXACTLY 2 letters")
		end
    end
	print ("        </text>")
	print ("        <hint>"..Hint.."</hint>")
	print ("    </reqout>")
	print ("</root>")
end

