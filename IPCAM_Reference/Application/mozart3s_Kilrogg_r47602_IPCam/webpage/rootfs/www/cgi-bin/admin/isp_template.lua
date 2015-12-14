#!/usr/bin/lua

x = os.clock()
CONF_FILE_PREFIX="/etc/conf.d/c0/"
ISP_FILE_PREFIX=CONF_FILE_PREFIX.."isp_template/"
--CONF_FILE_PREFIX=""
local lfs = require"lfs"
--REQUEST_METHOD = "POST"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")

--QUERY_STRING = "template=outdoor"
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("") 

if REQUEST_METHOD == "GET" then  -- GET scope
	if QUERY_STRING~=nil then -- select case
		k, v = string.match(QUERY_STRING, "(%w+)=(%w+)")
		if k == "template" then
			filename = ISP_FILE_PREFIX .. "isp_" ..v.. ".xml"
			io.input(filename)
			io.stdout:write(io.read("*all"))
		end
	end
else  -- POST scope
	-- open a temp file
	math.randomseed(os.time())
	randomnum = math.random(65536)

	-- write input to temp file
	f = io.open("/tmp/isp_return_file."..randomnum, "w")
	if f then
		local ret, err = lfs.lock(f, "w")
		f:write(io.stdin:read("*all"))
        	f:close()
			os.execute("/usr/bin/dos2unix.sh  ".."/tmp/isp_return_file."..randomnum.."  >  /dev/null  2>&1")
    	end
	-- reopen the temp file and parse the template tag
	tempfile = io.open("/tmp/isp_return_file."..randomnum, "r")
	k = tempfile:read("*all")
	_, _, template = string.find(k, ".*(<template>.-</template>).*")
	tempfile:close()
	-- template will be <template>....</template>
	 _, template, _ = string.match(template, "(<%w+>)(%w+)(</%w+>)") 
		-- the relatvie file store in name
		filename = ISP_FILE_PREFIX .. "isp_" ..template.. ".xml"
		-- open the isp_name.xml and overwrite it
		isp_file = io.open(filename, "w")
		if isp_file then
			local ret, err = lfs.lock(isp_file, "w")
			isp_file:write(k)
			isp_file:close()
			os.execute("/usr/bin/dos2unix.sh  "..filename.."  >  /dev/null  2>&1")
		end
	os.execute("rm -f ".."/tmp/isp_return_file."..randomnum)
        print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
        print ("<root></root>")
end

