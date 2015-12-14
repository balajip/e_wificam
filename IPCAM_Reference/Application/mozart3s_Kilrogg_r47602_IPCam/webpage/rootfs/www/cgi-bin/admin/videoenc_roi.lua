#!/usr/bin/lua
x = os.clock()
--videoenc_roi.lua?id=0
CONF_FILE_PREFIX="/etc/conf.d/c0/"
QUERY_STRING = os.getenv("QUERY_STRING")
local lfs = require"lfs"
STRAM_NUM=0
--print ("STRAM_NUM:",STRAM_NUM) 
VENC_FILE = CONF_FILE_PREFIX.."venc_conf.xml"
VENCENCODER_CONFIG =""
--print ("VENCENCODER_CONFIG:",VENCENCODER_CONFIG)
REQUEST_METHOD = os.getenv("REQUEST_METHOD")


print ("Content-type: text/xml")
print ("Cache-control: no-cache")
print ("")

k1,v1= string.match(QUERY_STRING, "(%w+)=(%w+)")
STRAM_NUM=v1
VENCENCODER_CONFIG = CONF_FILE_PREFIX.."vencencoder_conf."..STRAM_NUM..".xml"
if REQUEST_METHOD == "GET" then
	io.input(VENCENCODER_CONFIG)
      	io.stdout:write(io.read("*all"))
else
    	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
    	print ("<root>")
    	-- write input to temp file
    	f = io.open(VENCENCODER_CONFIG , "w")
    	if f then
			local ret, err = lfs.lock(f, "w") -- exclusive lock!
        	f:write(io.stdin:read("*all"))
        	f:close()
    	end
    	-- reload venc-0, but it should change vencvideoin switch value
	tempfile = io.open(VENC_FILE, "r")
        if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
        end

        _, _, head, venc_encoder, tail = string.find(temp_buf, "(.*)(<encoder id=\""..STRAM_NUM.."\">.-</encoder>)(.*)")
        -- new venc_videoin field
        _, _, encoder_head, switch, encoder_tail = string.find(venc_encoder, "(.*)(<switch>.-</switch>)(.*)")
        _, value, _ =  string.match(switch, "(<%w+>)(%w+)(</%w+>)")
        value = (value+1)%2
        venc_encoder = encoder_head.."<switch>"..value.."</switch>".. encoder_tail
	-- combine to new file
	venc_new_file = head.. venc_encoder..tail
--print (venc_new_file)
        -- step 10 - write to venc file
        venc_file = io.open(VENC_FILE, "w")
        if venc_file then
				local ret, err = lfs.lock(venc_file, "w") -- exclusive lock!
                venc_file:write(venc_new_file)
                venc_file:close()
        end
		os.execute("/usr/bin/dos2unix.sh  "..VENCENCODER_CONFIG.."  >  /dev/null  2>&1")
		os.execute("/usr/bin/dos2unix.sh  "..VENC_FILE.."  >  /dev/null  2>&1")
        os.execute("/etc/init.d/venc-0 reload")
    	print(string.format("<time>%.2f (sec)</time>", os.clock() - x))
        print ("</root>")
end

