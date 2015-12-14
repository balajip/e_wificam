#!/usr/sbin/lua

XML_TAG_NAME={"certificate", "validity", "spkey", "rsapk", "signature", "pem"}
CERT_TAG={"version", "serial", "salgo", "issuer"}
VALI_TAG={"before", "after", "subject"}
SPK_TAG={"spkalgo"}
RSA_TAG={"modulus", "exponent"}
SIGN_TAG={"algo", "sign"}
PEM_TAG={"text"}
XML_TAG={CERT_TAG, VALI_TAG, SPK_TAG, RSA_TAG, SIGN_TAG, PEM_TAG}
OUTPUT={}
CONFIG_FILE_PATH="/etc/conf.d/ssl"
CONF=CONFIG_FILE_PATH.."/openssl.cnf"
PEM=CONFIG_FILE_PATH.."/certificate.pem"
SSL_TEXT=CONFIG_FILE_PATH.."/ssltext"
SSL_XML=CONFIG_FILE_PATH.."/ssl.xml"
EXE="/sbin/openssl x509 "

os.execute(EXE.." -in "..PEM.." -noout -vn -out "..SSL_TEXT)  --generate a temp file

if os.execute("[ -f "..SSL_XML.." ]")~=0 then  --check output file exist
    os.execute("touch "..SSL_XML)
end

local FileText = io.open(SSL_TEXT) --("file.in")
local FileXml = io.open(SSL_XML, "w")
local i = 0
local j = 0
local k = 0
local line = ""

i=0
for line in FileText:lines() do
	if string.find(line, "*vn*") then
		i = i + 1
		OUTPUT[i] = ""
	else
		OUTPUT[i] = OUTPUT[i]..line
		if i == 9 or i == 12 or i == 13 then
		OUTPUT[i] = OUTPUT[i].."\n"
		end
	end
end

k=1
for i=1, table.getn(XML_TAG), 1 do
    FileXml:write(string.format("%8s<%s>", " ", XML_TAG_NAME[i]).."\n")
    for j=1, table.getn(XML_TAG[i]), 1 do
		--File:write(i..j..k)
        FileXml:write(string.format("%12s<%s>%s</%s>", " ", XML_TAG[i][j], OUTPUT[k], XML_TAG[i][j]).."\n")
        k = k + 1
    end
    FileXml:write(string.format("%8s</%s>", " ", XML_TAG_NAME[i]).."\n")
end
FileText:close()
FileXml:close()
os.execute("/usr/bin/dos2unix.sh  "..SSL_XML.."  >  /dev/null  2>&1")
os.execute("rm "..SSL_TEXT)
