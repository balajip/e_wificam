#!/usr/bin/lua

--parse xml file
require "lxp"

XML_CONFIG="/etc/conf.d/smtp_config.xml"

X = ""

IMAGE1 = arg[1]
--IMAGE2 = arg[2]
SERVER = ""
from   = ""
rcpt   = ""
SUBJECT  = ""
PORT   = ""
user = ""
password = ""
CC = ""
plain_text_body = ""

local decoded_subject
local decoded_text_body

function decode(xmltext)
    decode_apos = string.gsub(xmltext, "&apos;", "\'")
    decode_quot  = string.gsub(decode_apos, "&quot;", "\"")
    decode_gt =  string.gsub(decode_quot, "&gt;", ">")
    decode_lt =  string.gsub(decode_gt, "&lt;", "<")
    decode_amp =  string.gsub(decode_lt, "&amp;", "&")
	return decode_amp
end



function getParam(T, S)
	if (T == "SERVER") then
	SERVER = S
	elseif (T == "USER") then
	user = S
	elseif (T == "PASSWORD") then
	password = S
	elseif (T == "from") then
	from = S
	elseif (T == "rcpt") then
	rcpt = S
	elseif (T == "SUBJECT") then
	SUBJECT = S
	decoded_subject = decode(SUBJECT)
	elseif (T == "PORT") then
	PORT = S
	elseif (T == "CC") then
	CC = S
	elseif (T == "CONTENT") then
	plain_text_body = S
	decoded_text_body = decode(plain_text_body)
	else
	
	end	
end


callbacks = {
	StartElement = function (parser, name, attrs)
	X = name
--        if name == "IMAGE1" then
            --X= "IMAGE1"
            --print(X)
        --end
    end,
    EndElement = function (parser, name)
    end,
    CharacterData = function (parser, name)
	    string=string.gsub(name,"\t","")	-- remove tab
	    string=string.gsub(string," ","")   -- remove space
        if string ~= "\n" and string ~="" then        	
        getParam(X,name)
        	--print("X="..X)
        	--print("string =["..string.."]")        	
        end
    end
}

p = lxp.new(callbacks)
for l in io.lines(XML_CONFIG) do
    p:parse(l)    
    p:parse("\n")
end

-- end of parsing
----------------------------------
local smtp = require("smtp")
--print (smtp)

-- jpg file
mesgt1 = {
  headers = {
        ["content-type"] = 'image/jpeg; name="event.jpg"',
        ["content-disposition"] = 'attachment; filename="event.jpg"',
        ["content-description"] = 'an event image',
        ["content-transfer-encoding"] = "BASE64"
      },
      body = ltn12.source.chain(
        ltn12.source.file(io.open(IMAGE1, "rb")),
        ltn12.filter.chain(
          mime.encode("base64"),
          mime.wrap()
        )
      )
}
--plain text
if (decoded_text_body) then	
plain_text = {
 --	body = "One event occurs."
 	body = decoded_text_body

}
else
plain_text = {
	body = "One event occurs."
}
end

multipart_mesgt = {	
		headers = {
		  	from = from,
	     	to = rcpt,
			cc = CC,
     		subject = decoded_subject
		},
		body ={
				[1] = plain_text,
				[2] = mesgt1,
			}
}

if not (PORT) then PORT = 25 end

if (CC ~= "") then
	all_rcpt = {rcpt, CC}
else
	all_rcpt = rcpt
end

if (user ~= "") and (password ~= "") then
r, e = smtp.send{
  from = from,
  rcpt = all_rcpt,
server = SERVER,
  user = user,
  password = password,
  port = PORT,
  source = smtp.message(multipart_mesgt)
}
else 
r, e = smtp.send{
  from = from,
  rcpt = all_rcpt,
server = SERVER,
  port = PORT,
  source = smtp.message(multipart_mesgt)
}
end

--print (e)
if (e) then
os.execute("logger -p mail.notice ".."[smtp]"..e)
else
e = "ok"
end
--os.exit(0);

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
print ("<root>")
print ("<smtp_return>")
print (e)
print ("</smtp_return>")
print ("</root>")

