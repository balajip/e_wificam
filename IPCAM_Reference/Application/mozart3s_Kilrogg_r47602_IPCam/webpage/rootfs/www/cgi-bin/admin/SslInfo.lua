#!/usr/bin/lua

REQUEST_METHOD = os.getenv("REQUEST_METHOD")
DAEMON_SCRIPT = "/etc/init.d/stunnel"
CONFIG_FILE_PATH = "/etc/conf.d/ssl"
PEM = CONFIG_FILE_PATH.."/certificate.pem"
CERT=CONFIG_FILE_PATH.."/host.cert"
KEY=CONFIG_FILE_PATH.."/host.key"
REQ=CONFIG_FILE_PATH.."/host.req"
SSL_XML=CONFIG_FILE_PATH.."/ssl.xml"
PID_FILE = "/var/run/stunnel.pid"
IPTABLES = "/usr/sbin/iptables"
IP_SFLAG = " INPUT 1 -i eth0 -p tcp --dport 443 -j DROP"
SWITCH = ""
INPUT = ""

if os.execute("[ -f "..PEM.." ]")==0 then  --PEM exist
    SWITCH = "1"
elseif os.execute("[ -f "..REQ.." ]")==0 then  --REQ exist
    SWITCH = "2"
else
    SWITCH = "0"
end
 
print ("Content-type: text/xml") 
print ("Cache-Control: no-cache")
print ("")

if REQUEST_METHOD=="GET" then  --send certificate info to client
	local SslXmlExist=os.execute("[ -f "..SSL_XML.." ]")
    print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
    print ("<root>")
    print ("    <sslinfo>")
    print ("		<active>"..SWITCH.."</active>")
	if SslXmlExist==0 then  --if exist
    	local File = io.open(SSL_XML)
        print (File:read("*all"))
    end
    print ("    </sslinfo>")
    print ("</root>")
else                           --remove certificate
    --if os.execute("[ -f "..PID_FILE.." ]")==0 then  --stunnel is executing
        os.execute(DAEMON_SCRIPT.." stop")
        os.execute(IPTABLES.." -R"..IP_SFLAG)
    --end
    os.execute("rm -f "..CERT)
    os.execute("rm -f "..KEY)
    os.execute("rm -f "..PEM)
    os.execute("rm -f "..REQ)
    os.execute("rm -f "..SSL_XML)
	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
	print ("<root>")
	print ("    <text>")
    print ("Remove OK")
	print ("    <text>")
	print ("<root>")
end

