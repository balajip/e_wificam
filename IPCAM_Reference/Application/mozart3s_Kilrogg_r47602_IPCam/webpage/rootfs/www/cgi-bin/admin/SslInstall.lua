#!/usr/bin/lua

REQUEST_METHOD = os.getenv("REQUEST_METHOD")
CONTENT_TYPE = os.getenv("CONTENT_TYPE")
SSL_PARSER = "/usr/share/www/cgi-bin/admin/SslParser.lua"
CONFIG_FILE_PATH="/etc/conf.d/ssl"
CERT=CONFIG_FILE_PATH.."/host.cert"
KEY=CONFIG_FILE_PATH.."/host.key"
REQ=CONFIG_FILE_PATH.."/host.req"
PEM=CONFIG_FILE_PATH.."/certificate.pem"
TMP=CONFIG_FILE_PATH.."/tmp"
PID_FILE="/var/run/stunnel.pid"
DAEMON_SCRIPT="/etc/init.d/stunnel"
EXE="/sbin/openssl"
E_FLAG="-noout -modulus -in "
IPTABLES = "/usr/sbin/iptables"
IP_SFLAG = " INPUT 1 -i eth0 -p tcp --dport 443 -j DROP"
-- three state: Start, FoundNewBoundary, OutputFile
CURRENT_STATE = "Start"

print ("Content-type: text/html")
print ("Cache-Control: no-cache")
print ("")
print("<html><body>")

if os.execute("[ -f "..PEM.." ]")==0 or os.execute("[ -f "..CERT.." ]")==0 then  --check PEM or CERT exist
	print("Certificate exist, please remove at first</br></body></html>")
elseif os.execute("[ -f "..REQ.." ]")~=0 then       --check REQ exist
	print("Request dose not exist, please generate request at first</br></body></html>")
else
    --if os.execute("[ -f "..CERT.." ]")~=0 then  --check CERT does not exist
        --os.execute("touch /root/host.cert")
    --end

	local LocalFile = io.open(CERT, "w+")
	if LocalFile then
		--print ("open local file ok")
	else
		print ("Error!")
        os.exit()
	end

	local BoundaryStartIndex = string.find(CONTENT_TYPE, "boundary=")
	local Boundary = string.sub(CONTENT_TYPE, BoundaryStartIndex+9)
    local line

	for line in io.lines()
	do
		if CURRENT_STATE == "Start" then
			if string.find(line, Boundary) then -- find the boundary
				CURRENT_STATE = "FoundNewBoundary"
			end	
		elseif	CURRENT_STATE == "FoundNewBoundary" then
			if string.sub(line, 1) == "\r" then 
				CURRENT_STATE = "OutputFile"
				break
			end
		end
	end		

	local Buffer = io.read("*all")  -- get original file
	BoundaryEndIndex = string.find(Buffer, Boundary)
	Buffer = string.sub(Buffer, 1, BoundaryEndIndex-3)
	Buffer = Buffer:gsub("\r", "")
	local PemSIdx = string.find(Buffer, "----BEGIN CERTIFICATE----")  -- get PEM
	if PemSIdx == nil then
		os.execute("rm "..CERT)
		print ("Install Fail!!<br>Wrong file format!!<br></body></html>")
		os.exit()
	end
	local PemEIdx = string.find(Buffer, "----END CERTIFICATE----")
	if PemEIdx == nil then
		os.execute("rm "..CERT)
		print ("Install Fail!!<br>Wrong file format!!<br></body></html>")
		os.exit()
	else
		PemEIdx = PemEIdx + 24
	end
	local Pem = string.sub(Buffer, PemSIdx, PemEIdx)

	LocalFile:write(Pem)
	LocalFile:flush()
	LocalFile:close()
	
	-- check public key
	os.execute("("..EXE.." req "..E_FLAG..REQ..";"..EXE.." x509 "..E_FLAG..CERT..") | uniq -u > "..TMP)
	LocalFile = io.open(TMP)
	local Tmp = LocalFile:read("*all")
	LocalFile:flush()
	LocalFile:close()
	--os.execute("rm "..TMP)

	if Tmp == "" then -- public key is the same
		os.execute("cat "..KEY.." "..CERT.." > "..PEM)
    	os.execute("lua "..SSL_PARSER)  --generate xml file
		print ("Upload Certificate Success!<br></body></html>")
		
		--if os.execute("[ -f "..PID_FILE.." ]")==0 then  --check stunnel is executing
			os.execute(IPTABLES.." -R"..IP_SFLAG)
			os.execute(DAEMON_SCRIPT.." restart")
		--end
	else
		os.execute("rm "..CERT)
		print ("Invalid public key! Upload Certificate Fail!<br></body></html>")
	end
end

