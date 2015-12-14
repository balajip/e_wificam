#!/usr/bin/lua

x = os.clock()
CONF_FILE_PREFIX="/etc/conf.d/c0/"
VENCDISP_CONF_FILE=CONF_FILE_PREFIX .. "vencdisplay_conf.xml"
VENC_FILE=CONF_FILE_PREFIX .. "venc_conf.xml"
local lfs = require"lfs"
--none:/cgi-bin/admin/display.lua?output=off&format=0&resolution=0x0&freq=0 
--NTSC:/cgi-bin/admin/display.lua?output=on&format=NTSC&resolution=720x480&freq=60
--PAL:/cgi-bin/admin/display.lua?output=on&format=PAL&resolution=720x576&freq=60
--720p@60:/cgi-bin/admin/display.lua?output=on&format=HDMI&resolution=1280x720&freq=60
--1080p@24:/cgi-bin/admin/display.lua?output=on&format=HDMI&resolution=1920x1080&freq=24
--1080p@30:/cgi-bin/admin/display.lua?output=on&format=HDMI&resolution=1920x1080&freq=30
--1080p@60:/cgi-bin/admin/display.lua?output=on&format=HDMI&resolution=1920x1080&freq=60
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
		--out_format = nil
		--resolution = nil
		--freq =nil
		k1, v1, k2, v2, k3, v3,k4,v4= string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)&(%w+)=(%w+)&(%w+)=(%w+)")
		if k1 == "output" then
		  output= v1
		end
	      
		if k2 == "format" then
			out_format = v2
		end
		if k3 == "resolution" then
		      resolution=v3
		end
		if k4 == "freq" then
		      freq=v4
		end
	  
	     tempfile = io.open(VENCDISP_CONF_FILE, "r")
	      if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
	      end

	      _, _, head, _, tail = string.find(temp_buf, "(.*)(<output>.-</output>)(.*)")
	      venc_display = head.."<output>"..output.."</output>"..tail
	      fp= io.open(VENCDISP_CONF_FILE,"w")
	      if fp then
				local ret, err = lfs.lock(fp, "w") -- exclusive lock!
                fp:write(venc_display)
                fp:close()
	      end


	    if output == "on" then
		  tempfile = io.open(VENCDISP_CONF_FILE, "r")
		  if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
		  end

		  _, _, head, _, tail = string.find(temp_buf, "(.*)(<output_format>.-</output_format>)(.*)")
		  venc_display = head.."<output_format>"..out_format.."</output_format>"..tail
		  fp= io.open(VENCDISP_CONF_FILE, "w")
		  if fp then
			local ret, err = lfs.lock(fp, "w") -- exclusive lock!  
		    fp:write(venc_display)
		    fp:close()
		  end



		  tempfile = io.open(VENCDISP_CONF_FILE, "r")
		  if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
		  end




		_, _, head, _, tail = string.find(temp_buf, "(.*)(<output_resolution>.-</output_resolution>)(.*)")
		  venc_display = head.."<output_resolution>"..resolution.."</output_resolution>"..tail
		  fp= io.open(VENCDISP_CONF_FILE,"w")
		  if fp then
			local ret, err = lfs.lock(fp, "w") -- exclusive lock!  
		    fp:write(venc_display)
		    fp:close()
		  end


		  tempfile = io.open(VENCDISP_CONF_FILE, "r")
		  if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
		  end




		_, _, head, _, tail = string.find(temp_buf, "(.*)(<output_freq>.-</output_freq>)(.*)")
		  venc_display = head.."<output_freq>"..freq.."</output_freq>"..tail
		  fp= io.open(VENCDISP_CONF_FILE, "w")
		  if fp then
			local ret, err = lfs.lock(fp, "w") -- exclusive lock!  
		    fp:write(venc_display)
		    fp:close()
		  end
	  end
	-- reload venc-0, but it should change vencencoder switch value
		tempfile = io.open(VENC_FILE, "r")
        if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
        end




        _, _, head, venc_display, tail = string.find(temp_buf, "(.*)(<venc_display>.-</venc_display>)(.*)")

        -- new venc_encoder field
        _, _, venc_display_head,  switch, venc_display_tail = string.find(venc_display, "(.*)(<switch>.-</switch>)(.*)")

        _, value, _ =  string.match(switch, "(<%w+>)(%w+)(</%w+>)")
        value = (value+1)%2


        venc_display = venc_display_head.."<switch>"..value.."</switch>"..venc_display_tail
        venc_new_file = head..venc_display..tail

        -- write to venc file
        venc_file = io.open(VENC_FILE, "w")
        if venc_file then
			    local ret, err = lfs.lock(venc_file, "w") -- exclusive lock!
                venc_file:write(venc_new_file)
                venc_file:close()
        end
		os.execute("/usr/bin/dos2unix.sh  "..VENCDISP_CONF_FILE.."  >  /dev/null  2>&1")
		os.execute("/usr/bin/dos2unix.sh  "..VENC_FILE.."  >  /dev/null  2>&1")
        os.execute("/etc/init.d/venc-0 reload > /dev/null 2>&1")
        print ("done")



end
end
print ("</body>")
print ("</html>")