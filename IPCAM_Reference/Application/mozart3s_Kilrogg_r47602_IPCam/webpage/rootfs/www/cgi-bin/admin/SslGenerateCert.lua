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
SSL_PARSER = "/usr/share/www/cgi-bin/admin/SslParser.lua"
DAEMON_SCRIPT = "/etc/init.d/stunnel"
CONFIG_FILE_PATH = "/etc/conf.d/ssl"
CONF = CONFIG_FILE_PATH.."/openssl.cnf"
CERT=CONFIG_FILE_PATH.."/host.cert"
KEY=CONFIG_FILE_PATH.."/host.key"
REQ=CONFIG_FILE_PATH.."/host.req"
PEM=CONFIG_FILE_PATH.."/certificate.pem"
IPTABLES = "/usr/sbin/iptables"
IP_SFLAG = " INPUT 1 -i eth0 -p tcp --dport 443 -j DROP"
EXE="/sbin/openssl req"
FLAG_P1=" -out "..CERT.." -newkey rsa:1024 -nodes -keyout "..KEY.." -sha1 -config "..CONF.." -subj "
FLAG_P2=" -x509 -days "
PROP={"C", "ST", "L", "O", "OU", "CN", "emailAddress"}
ELEMENTS={"country", "state", "location", "organization", "unit", "name", "email", "days"}
INPUT={}
SUBJECT=""


print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
if REQUEST_METHOD == "GET" then
    print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
    print ("<root>")
    print ("    <sslgeneratecert>")
    print ("        <country>.</country>")
    print ("        <state>.</state>")
    print ("        <location>.</location>")
    print ("        <organization>.</organization>")
    print ("        <unit>.</unit>")
    print ("        <name>.</name>")
    print ("        <email>.</email>")
    print ("        <days>365</days>")
    print ("    </sslgeneratecert>")
    print ("</root>")
else
    print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
    print ("<root>")
	print ("    <reqout>")
	print ("        <text>")
    local LocalString
    local LocalIndex = 1
    for LocalString in io.stdin:lines() do  --get subject parameter
        if LocalIndex <= tonumber(table.getn(ELEMENTS)) and LocalString:match(ELEMENTS[LocalIndex]) == ELEMENTS[LocalIndex] then
            INPUT[LocalIndex] = LocalString:gsub("%s*<.?"..ELEMENTS[LocalIndex]..">%c?", "")  --IE will send strings end with a \r
            --print(INPUT[LocalIndex].." "..LocalIndex)
            LocalIndex = LocalIndex + 1
        end
    end
    if os.execute("[ -f "..PEM.." ]")==0 or os.execute("[ -f "..CERT.." ]")==0 then  --certificate exist
        print("Certificate exist, please remove at first!")
	--elseif os.execute("[ -f "..REQ.." ]")==0 then
		--print("Request exist, please remove at first!")
    else
		if os.execute("[ -f "..REQ.." ]")==0 then
			print("Request exist, remove!")
			os.execute("rm -f "..REQ)
		end
	    GenSubject()  --generate subject
	    os.execute(EXE..FLAG_P1.."'"..SUBJECT.."'"..FLAG_P2..INPUT[8])
        --print(SUBJECT)
		if os.execute("[ -f "..CERT.." ]")==0 then
			os.execute("cat "..KEY.." "..CERT.." > "..PEM)
        	os.execute("lua "..SSL_PARSER)  --generate xml file
			os.execute(IPTABLES.." -R"..IP_SFLAG)  --filter https
--			os.execute(DAEMON_SCRIPT.." restart")  --restart stunnel
        	print("Generate certificate OK!")
		else
			print("Generate certificate FAIL!")
			--print("Generate certificate FAIL!Please check if 'COUNTRY' is EXACTLY 2 letters, and 'VALIDITY' is not NULL!!")
		end
    end
	print ("        </text>")
	print ("    </reqout>")
	print ("</root>")
end

