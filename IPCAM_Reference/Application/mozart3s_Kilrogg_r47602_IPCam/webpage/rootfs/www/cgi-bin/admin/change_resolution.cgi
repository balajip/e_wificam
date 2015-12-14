#!/usr/bin/lua

x = os.clock()
CONF_FILE_PREFIX="/etc/conf.d/c0/"
VENCENCODER_CONF_PREFIX=CONF_FILE_PREFIX .. "vencencoder_conf."
VENC_FILE = CONF_FILE_PREFIX.."venc_conf.xml"
VENCENCODER_CONF_0=VENCENCODER_CONF_PREFIX .. "0.xml"
local lfs = require"lfs"
--REQUEST_METHOD = "GET"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")

--QUERY_STRING = "resolution=640x480"
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/html")
print ("Cache-Control: no-cache")
print ("")

print ("<html>")
print ("<body>")


if REQUEST_METHOD == "GET" then  -- GET scope
	if QUERY_STRING~=nil then -- select case
		k, v = string.match(QUERY_STRING, "(%w+)=(%w+)")
		if k == "resolution" then
			resolution = v
		end

        tempfile = io.open(VENCENCODER_CONF_0, "r")
        if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
        end

        _, _, head, _, tail = string.find(temp_buf, "(.*)(<resolution>.-</resolution>)(.*)")
		venc_encoder_0 = head.."<resolution>"..resolution.."</resolution>"..tail

        -- write to venc_encoder_0 file
        venc_encoder_0_file = io.open(VENCENCODER_CONF_0, "w")
        if venc_encoder_0_file then
				local ret, err = lfs.lock(venc_encoder_0_file, "w") -- exclusive lock!
                venc_encoder_0_file:write(venc_encoder_0)
                venc_encoder_0_file:close()
        end

    	-- reload venc-0, but it should change vencencoder switch value
		tempfile = io.open(VENC_FILE, "r")
        if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
        end

        _, _, head, venc_encoder, tail = string.find(temp_buf, "(.*)(<venc_encoder>.-</venc_encoder>)(.*)")

        -- new venc_encoder field
        _, _, encoder_head, switch0, interval_01, switch1, interval_12, switch2, encoder_tail = string.find(venc_encoder, "(.*)(<switch>.-</switch>)(.*)(<switch>.-</switch>)(.*)(<switch>.-</switch>)(.*)")

        _, value0, _ =  string.match(switch0, "(<%w+>)(%w+)(</%w+>)")
        value0 = (value0+1)%2

        _, value1, _ =  string.match(switch1, "(<%w+>)(%w+)(</%w+>)")
--        value1 = (value1+1)%2

        _, value2, _ =  string.match(switch2, "(<%w+>)(%w+)(</%w+>)")
--        value2 = (value2+1)%2

        venc_encoder = encoder_head.."<switch>"..value0.."</switch>"..interval_01.."<switch>"..value1.."</switch>"..interval_12.."<switch>"..value2.."</switch>"..encoder_tail
        venc_new_file = head..venc_encoder..tail

        -- write to venc file
        venc_file = io.open(VENC_FILE, "w")
        if venc_file then
				local ret, err = lfs.lock(venc_file, "w") -- exclusive lock!
                venc_file:write(venc_new_file)
                venc_file:close()
        end
		os.execute("/usr/bin/dos2unix.sh  "..VENCENCODER_CONF_0.."  >  /dev/null  2>&1")
		os.execute("/usr/bin/dos2unix.sh  "..VENC_FILE.."  >  /dev/null  2>&1")
        os.execute("/etc/init.d/venc-0 reload")
        print ("done")
        end
print ("</body>")
print ("</html>")
end