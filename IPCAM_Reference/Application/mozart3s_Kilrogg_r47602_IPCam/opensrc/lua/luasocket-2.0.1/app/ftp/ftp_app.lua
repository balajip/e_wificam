#!/usr/bin/lua
-- parse xml file
require "lxp"

XML_CONFIG="/etc/conf.d/ftp_config.xml"
SNAPSHOT_FOLDER= "/tmp/event/"
SNAPSHOT_PATH = arg[1]
_,j = string.find(SNAPSHOT_PATH, SNAPSHOT_FOLDER)
FILENAME = string.sub(SNAPSHOT_PATH, j+1)

X = ""

HOST = ""
USER_NAME = ""
PASSWD = ""
PORT   = 21
REMOTE_FOLDER = ""
IS_ACTIVE_MODE = "false"

function getParam(T, S)
	if (T == "HOST") then
	HOST = S
	elseif (T == "USER_NAME") then
	USER_NAME = S
	elseif (T == "PASSWD") then
	PASSWD = S
	elseif (T == "REMOTE_FOLDER") then
	REMOTE_FOLDER = S
	elseif (T == "PORT") then
	PORT = S
	elseif (T == "FTP_MODE") then
		if (S == "PORT") then
		IS_ACTIVE_MODE = "true"
		end
	else
	
	end	
end

callbacks = {
	StartElement = function (parser, name, attrs)
	X = name
    end,
    EndElement = function (parser, name)
    end,
    CharacterData = function (parser, name)
	    string=string.gsub(name,"\t","")	-- remove tab
	    string=string.gsub(string," ","")   -- remove space
        if string ~= "\n" and string ~="" then        	
        getParam(X,name)
--        	print("X="..X)
--        	print("string =["..string.."]")        	
        end
    end
}

p = lxp.new(callbacks)
for l in io.lines(XML_CONFIG) do
    p:parse(l)
    p:parse("\n")
end

-- end of parsing
------------------------

local ftp = require("ftp")
local ltn12 = require("ltn12")

if not (PORT) then PORT = 21 end
ssource =  ltn12.source.file( io.open(SNAPSHOT_FOLDER..FILENAME, "r") )
--ssource =ltn12.source.string("123")
f, err = ftp.put{
  host = HOST,
  user = USER_NAME,
  password = PASSWD,
--  command = "appe",
  type ="i",
  active_mode = IS_ACTIVE_MODE,
  port = PORT,
  source = ssource,
  argument = REMOTE_FOLDER..FILENAME
}
--[[
fff, eee = ftp.put("ftp://User1:User1@192.168.2.81/snp-20090325-133158.jpg", "123")
--]]

--print (ssource)
if (err)
--then	print (err)
then os.execute("logger -p daemon.warning ".."[ftp]"..err)
else
err = "ok"
end
--print (f)


print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
print ("<root>")
print ("<ftp_return>")
print (err)
print ("</ftp_return>")
print ("</root>")
