#!/usr/bin/lua

x = os.clock()
CONF_FILE_PREFIX="/etc/conf.d/c0/"
ISP_FILE_PREFIX=CONF_FILE_PREFIX.."isp_template/"
--CONF_FILE_PREFIX=""
local lfs = require"lfs"
--REQUEST_METHOD = "GET"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")

VENC_FILE = CONF_FILE_PREFIX.."venc_conf.xml"
MASTER_FILE = CONF_FILE_PREFIX.."vencmaster_conf.xml"
VIDEOIN_FILE = CONF_FILE_PREFIX.."vencvideoin_conf.xml"

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("") 

if REQUEST_METHOD == "GET" then  -- GET scope
	if QUERY_STRING == nil then -- onload case
		input = io.open(VENC_FILE, "r")
		k = input:read("*all")
		_, _, template = string.find(k, ".*(<template>.-</template>).*")
		input:close()
		 _, template, _ = string.match(template, "(<%w+>)(%w+)(</%w+>)")
		filename = ISP_FILE_PREFIX .. "isp_" ..template.. ".xml"
		f = io.open(filename, "r")
		if f then
			io.stdout:write(f:read("*all"))
			f:close()
		end
	end
else  -- POST scope
        -- step 1 - get the template index
------------ test code, if read from isp_indoor.xml, not from stdin --------------
--        tempfile = io.open("isp_indoor.xml", "r")
--        k = tempfile:read("*all")
--        _, _, template = string.find(k, ".*(<template>.-</template>).*")
--        tempfile:close()
----------------------------------------------------------------------------------
        _, _, template = string.find(io.stdin:read("*all"), ".*(<template>.-</template>).*")
        _, template, _ = string.match(template, "(<%w+>)(%w+)(</%w+>)")
        -- step 2 - open template file
        filename = ISP_FILE_PREFIX .. "isp_" ..template.. ".xml"
        tmp_file = io.open(filename, "r")
        if tmp_file then
                buf = tmp_file:read("*all")
                tmp_file:close()
                -- step 3 - find "<venc_master><isp>...</isp></venc_master>" scope
                _, _, venc_master_isp = string.find(buf, ".*(<venc_master>.-</venc_master>).*")
                _, _, venc_master_isp = string.find(venc_master_isp, ".*(<isp>.-</isp>).*")
                -- step 4 - find "<venc_videoin><isp>...</isp></venc_videoin>" scope
                _, _, venc_videoin_isp = string.find(buf, ".*(<venc_videoin>.-</venc_videoin>).*")
                _, _, venc_videoin_isp = string.find(venc_videoin_isp, ".*(<isp>.-</isp>).*")
        end
        -- step 5 - combine the new vencmaster config
        if venc_master_isp ~= nil then
                master_file = io.open(MASTER_FILE, "r")
                if master_file then
                        master_buf = master_file:read("*all")
                        master_file:close()
                end
                _, _, master_head, _, master_tail = string.find(master_buf, "(.*)(<isp>.-</isp>)(.*)")
                master_new_file = master_head..venc_master_isp..master_tail

                -- step 6 - write to master file
                master_file = io.open(MASTER_FILE, "w")
                if master_file then
						local ret, err = lfs.lock(master_file, "w") -- exclusive lock!
                        master_file:write(master_new_file)
                        master_file:close()
                end
        end
        -- step 7 - combine the new vencvideoin config
        if venc_videoin_isp ~= nil then
                videoin_file = io.open(VIDEOIN_FILE, "r")
                if videoin_file then
                        videoin_buf = videoin_file:read("*all")
                        videoin_file:close()
                end
                _, _, head, _, tail = string.find(videoin_buf, "(.*)(<isp>.-</isp>)(.*)")
                videoin_new_file = head..venc_videoin_isp..tail

                -- step 8 - write to vencvideoin file
                videoin_file = io.open(VIDEOIN_FILE, "w")
                if videoin_file then
						local ret, err = lfs.lock(videoin_file, "w") -- exclusive lock!
                        videoin_file:write(videoin_new_file)
                        videoin_file:close()
                end
        end
        -- step 9 - replace template field at venc_conf file
        tempfile = io.open(VENC_FILE, "r")
        if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
        end
        _, _, head, _, path_tag, venc_master, venc_videoin, tail = string.find(temp_buf, "(.*)(<template>.-</template>)(.*)(<venc_master>.-</venc_master>).*(<venc_videoin>.-</venc_videoin>)(.*)")

        -- new template field
        template = "<template>"..template.."</template>"

        -- new venc_master field
        _, _, master_head, switch, master_tail = string.find(venc_master, "(.*)(<switch>.-</switch>)(.*)")
        _, value, _ =  string.match(switch, "(<%w+>)(%w+)(</%w+>)")
        value = (value+1)%2
        venc_master = master_head.."<switch>"..value.."</switch>"..master_tail

        -- new venc_videoin field
        _, _, videoin_head, switch, videoin_tail = string.find(venc_videoin, "(.*)(<switch>.-</switch>)(.*)")
        _, value, _ =  string.match(switch, "(<%w+>)(%w+)(</%w+>)")
        value = (value+1)%2
        venc_videoin = videoin_head.."<switch>"..value.."</switch>"..videoin_tail

        venc_new_file = head..template..path_tag..venc_master.."\n    "..venc_videoin..tail
        -- step 10 - write to venc file
        venc_file = io.open(VENC_FILE, "w")
        if venc_file then
				local ret, err = lfs.lock(venc_file, "w") -- exclusive lock!
                venc_file:write(venc_new_file)
                venc_file:close()
        end
        -- step 11 - reload
		os.execute("/usr/bin/dos2unix.sh  "..MASTER_FILE.."  >  /dev/null  2>&1")
		os.execute("/usr/bin/dos2unix.sh  "..VIDEOIN_FILE.."  >  /dev/null  2>&1")
		os.execute("/usr/bin/dos2unix.sh  "..VENC_FILE.."  >  /dev/null  2>&1")
		os.execute("/etc/init.d/venc-0 reload")
end
