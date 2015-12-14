#!/usr/bin/lua
x = os.clock()
CONF_FILE_PREFIX="/etc/conf.d/c0/"
VENC_FILE = CONF_FILE_PREFIX.."venc_conf.xml"
VIDEOIN_CONFIG = CONF_FILE_PREFIX .."vencvideoin_conf.xml"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml")
print ("Cache-control: no-cache")
print ("")

if REQUEST_METHOD == "GET" then
	io.input(VIDEOIN_CONFIG)
      	io.stdout:write(io.read("*all"))
else
    	print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
    	print ("<root>")
    	-- write input to temp file
    	f = io.open(VIDEOIN_CONFIG, "w")
    	if f then
        	f:write(io.stdin:read("*all"))
        	f:close()
    	end
    	-- reload venc-0, but it should change vencvideoin switch value
	tempfile = io.open(VENC_FILE, "r")
        if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
        end

        _, _, head, venc_videoin, tail = string.find(temp_buf, "(.*)(<venc_videoin>.-</venc_videoin>)(.*)")
        -- new venc_videoin field
        _, _, videoin_head, switch, videoin_tail = string.find(venc_videoin, "(.*)(<switch>.-</switch>)(.*)")
        _, value, _ =  string.match(switch, "(<%w+>)(%w+)(</%w+>)")
        value = (value+1)%2
        venc_videoin = videoin_head.."<switch>"..value.."</switch>"..videoin_tail
	-- combine to new file
	venc_new_file = head..venc_videoin..tail
--print (venc_new_file)
        -- step 10 - write to venc file
        venc_file = io.open(VENC_FILE, "w")
        if venc_file then
                venc_file:write(venc_new_file)
                venc_file:close()
        end
		os.execute("/usr/bin/dos2unix.sh  "..VIDEOIN_CONFIG.."  >  /dev/null  2>&1")
		os.execute("/usr/bin/dos2unix.sh  "..VENC_FILE.."  >  /dev/null  2>&1")
        os.execute("/etc/init.d/venc-0 reload")
    	print(string.format("<time>%.2f (sec)</time>", os.clock() - x))
        print ("</root>")
end

