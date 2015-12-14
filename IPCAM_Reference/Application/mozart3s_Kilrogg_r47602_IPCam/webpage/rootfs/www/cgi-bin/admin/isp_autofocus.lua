#!/usr/bin/lua

x = os.clock()
CONF_FILE_PREFIX="/etc/conf.d/c0/"
VENCMASTER_CONF_FILE=CONF_FILE_PREFIX .. "vencmaster_conf.xml"
VENC_FILE=CONF_FILE_PREFIX .. "venc_conf.xml"
MSG_TOOL = "/usr/bin/msgctrl"
FIFO_PATH = "/tmp/venc/c0/command.fifo"

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
--command=changezoom&zoom=1 [0,1]
--command=changevalue&value=1 [0,1]
--command=changespeed&speed=4 [0,1,2,3,4,5,6]
--command=changewindow&startX=1&startY=1&width=100&height=100
--command=nocommands
--command=changezoomspeed&zoomspeed=4 [0,1,2,3,4,5,6]
--command=changenoisethreshold&noisethreshold=3 [0,1,2,3,4,5,6,7]
--command=changerefocus&refocus=0 [on,off]
if REQUEST_METHOD == "GET" then  -- GET scope
if QUERY_STRING~=nil then -- select case
		 current_cmd=nil
		 value=nil
		 speed=nil
		 startX=nil
		 startY=nil
		 width=nil
		 height=nil
		 zoom=nil
		 zoomspeed=nil
		 noisethreshold=nil
		 refocus=nil
		 
		 k1,v1= string.match(QUERY_STRING, "(%w+)=(%w+)&")
		 --print ("command=",v1)
		 --print("query str=",QUERY_STRING)
		 if v1 == "changevalue" then
		  k1,v1,k2,v2= string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)")
		  value = v2
		 elseif v1 == "changespeed" then
		  k1,v1,k2,v2= string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)")
		  speed = v2
		 elseif v1 == "changewindow" then				--command=chanegwindow&startX=0&startY=0.5&width=100&height=100
		    --print("query str=",QUERY_STRING)
		    k1, v1, k2, v2, k3, v3, k4, v4,k5,v5 = string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)&(%w+)=(%w+)&(%w+)=(%w+)&(%w+)=(%w+)")
		    --print("get: ",k1, v1, k2, v2, k3, v3, k4, v4,k5,v5)
		    startX = v2
		    startY = v3
		    width= v4
		    height = v5
		 elseif v1 == "changezoom" then
		  k1,v1,k2,v2= string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)")
		  zoom = v2
		 elseif v1 == "changezoomspeed" then
		  k1,v1,k2,v2= string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)")
		  zoomspeed=v2
		 elseif v1 == "changenoisethreshold" then
		  k1,v1,k2,v2= string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)")
		  noisethreshold=v2
		elseif v1 == "changerefocus" then
		  k1,v1,k2,v2= string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)")
		  refocus=v2
		 end
		
		if v1 ~=nil then
		    --print("value=",value)
		    current_cmd=v1
		    tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		    if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		    end
		    _, _, head, _, tail = string.find(temp_buf, "(.*)(<current_cmd>.-</current_cmd>)(.*)")
		    focus_cmd_str = head.."<current_cmd>"..v1.."</current_cmd>"..tail
		    fp= io.open(VENCMASTER_CONF_FILE,"w")
		    if fp then
				local ret, err = lfs.lock(fp, "w") -- exclusive lock!
				fp:write(focus_cmd_str)
				fp:close()
		    end    
		end

  

		if zoomspeed ~= nil then
		    tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		    if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		    end
		    _, _, head, _, tail = string.find(temp_buf, "(.*)(<zoomspeed>.-</zoomspeed>)(.*)")
		    focus_zoomspeed_str = head.."<zoomspeed>"..zoomspeed.."</zoomspeed>"..tail
		    fp= io.open(VENCMASTER_CONF_FILE,"w")
		    if fp then
			local ret, err = lfs.lock(fp, "w") -- exclusive lock!
			fp:write(focus_zoomspeed_str)
			fp:close()
		    end 
		end
		  
		if  noisethreshold ~= nil then
		    tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		    if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		    end
		    _, _, head, _, tail = string.find(temp_buf, "(.*)(<noisethreshold>.-</noisethreshold>)(.*)")
		    focus_noisethreshold_str = head.."<noisethreshold>".. noisethreshold.."</noisethreshold>"..tail
		    fp= io.open(VENCMASTER_CONF_FILE,"w")
		    if fp then
			fp:write(focus_noisethreshold_str)
			fp:close()
		    end 
		end
		  
		if refocus ~= nil then
		      tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		    if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		    end
		    _, _, head, _, tail = string.find(temp_buf, "(.*)(<refocus>.-</refocus>)(.*)")
		    focus_refocus_str = head.."<refocus>".. refocus.."</refocus>"..tail
		    fp= io.open(VENCMASTER_CONF_FILE,"w")
		    if fp then
			local ret, err = lfs.lock(fp, "w") -- exclusive lock!	
			fp:write(focus_refocus_str)
			fp:close()
		    end 

		end



		if zoom ~= nil then
		    --print("zoom=",zoom)
		    tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		    if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		    end
		    _, _, head, _, tail = string.find(temp_buf, "(.*)(<zoom>.-</zoom>)(.*)")
		    focus_zoom_str = head.."<zoom>"..zoom.."</zoom>"..tail
		    fp= io.open(VENCMASTER_CONF_FILE,"w")
		    if fp then
			local ret, err = lfs.lock(fp, "w") -- exclusive lock!
			fp:write(focus_zoom_str)
			fp:close()
		    end 
		    if zoom == "0" then
		      os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h master -c AF_ZoomIn")
		    elseif zoom == "1" then
		      os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h master -c AF_ZoomOut")
		    end
		 end



		 if value ~= nil then
		    --print("value=",value)
		    tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		    if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		    end
		    _, _, head, _, tail = string.find(temp_buf, "(.*)(<value>.-</value>)(.*)")
		    focus_value_str = head.."<value>"..value.."</value>"..tail
		    fp= io.open(VENCMASTER_CONF_FILE,"w")
		    if fp then
			local ret, err = lfs.lock(fp, "w") -- exclusive lock!
			fp:write(focus_value_str)
			fp:close()
		    end
		    if value == "0" then
		      os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h master -c AF_FocusNear")
		    elseif value == "1" then
		      os.execute(MSG_TOOL.." -f "..FIFO_PATH.." -h master -c AF_FocusFar")
		    end
		 end
		 if speed ~= nil then
		  --print("speed=",speed)
		    tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		    if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		    end
		    _, _, head, _, tail = string.find(temp_buf, "(.*)(<speed>.-</speed>)(.*)")
		    focus_speed_str = head.."<speed>"..speed.."</speed>"..tail
		    fp= io.open(VENCMASTER_CONF_FILE,"w")
		    if fp then
			local ret, err = lfs.lock(fp, "w") -- exclusive lock!
			fp:write(focus_speed_str)
			fp:close()
		    end    
		 end

		 if startX ~= nil or startY ~= nil or   width ~= nil or  height ~= nil then
		 -- print("startX ",startX)
		 -- print("startY ",startY)
		 -- print("width ",width)
		  --print("height ",height)
		  tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		  if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
		  end
		  _, _, head, focus_str, tail = string.find(temp_buf, "(.*)(<auto_focus>.-</auto_focus>)(.*)")
		   
		  if startX ~= nil then
		      _, _, head_1, _, tail_1= string.find(focus_str, "(.*)(<startX>.-</startX>)(.*)")
	  
		      focus_startX_str = head..head_1.."<startX>"..startX.."</startX>"..tail_1..tail
		    

		      fp= io.open(VENCMASTER_CONF_FILE,"w")
		      if fp then
			  local ret, err = lfs.lock(fp, "w") -- exclusive lock!
			  fp:write(focus_startX_str)
			  fp:close()
		      end  
		  end
		  tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		  if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
		  end
		  _, _, head, focus_str, tail = string.find(temp_buf, "(.*)(<auto_focus>.-</auto_focus>)(.*)")
		   
		
		  if startY ~= nil then
		      _, _, head_1, _, tail_1= string.find(focus_str, "(.*)(<startY>.-</startY>)(.*)")
	  
		      focus_startY_str = head..head_1.."<startY>"..startY.."</startY>"..tail_1..tail
		      --print("focus_startY_str ",focus_startY_str)
			  
		      fp= io.open(VENCMASTER_CONF_FILE,"w")
		      if fp then
			  --print("hello!!")
			  local ret, err = lfs.lock(fp, "w") -- exclusive lock!
			  fp:write(focus_startY_str)
			  fp:close()
		      end  
		  end
		   tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		  if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
		  end
		  _, _, head, focus_str, tail = string.find(temp_buf, "(.*)(<auto_focus>.-</auto_focus>)(.*)")
		   
		  if width ~= nil then
		      _, _, head_1, _, tail_1= string.find(focus_str, "(.*)(<width>.-</width>)(.*)")
	  
		      focus_width_str = head..head_1.."<width>"..width.."</width>"..tail_1..tail
		     

		      fp= io.open(VENCMASTER_CONF_FILE,"w")
		      if fp then
			  local ret, err = lfs.lock(fp, "w") -- exclusive lock!
			  fp:write(focus_width_str)
			  fp:close()
		      end  
		  end
		  tempfile = io.open(VENCMASTER_CONF_FILE, "r")
		  if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
		  end
		  _, _, head, focus_str, tail = string.find(temp_buf, "(.*)(<auto_focus>.-</auto_focus>)(.*)")
		   
		  if height ~= nil then
		      _, _, head_1, _, tail_1= string.find(focus_str, "(.*)(<height>.-</height>)(.*)")
	  
		      focus_height_str = head..head_1.."<height>"..height.."</height>"..tail_1..tail
		     

		      fp= io.open(VENCMASTER_CONF_FILE,"w")
		      if fp then
			  local ret, err = lfs.lock(fp, "w") -- exclusive lock!
			  fp:write(focus_height_str)
			  fp:close()
		      end  
		  end

		



 
	      end
		  
	    -- reload venc-0, but it should change vencencoder switch value
	      tempfile = io.open(VENC_FILE, "r")
	      if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
	      end

	      _, _, head, venc_master, tail = string.find(temp_buf, "(.*)(<venc_master>.-</venc_master>)(.*)")

	      -- new venc_encoder field
	      _, _, venc_master_head,  switch, venc_master_tail = string.find(venc_master, "(.*)(<switch>.-</switch>)(.*)")
	      _, value, _ =  string.match(switch, "(<%w+>)(%w+)(</%w+>)")
	      value = (value+1)%2

	      venc_master = venc_master_head.."<switch>"..value.."</switch>"..venc_master_tail
	      venc_new_file = head..venc_master..tail

        -- write to venc file
        venc_file = io.open(VENC_FILE, "w")
        if venc_file then
			    local ret, err = lfs.lock(venc_file, "w") -- exclusive lock!
                venc_file:write(venc_new_file)
                venc_file:close()
        end
		os.execute("/usr/bin/dos2unix.sh  "..VENC_FILE.."  >  /dev/null  2>&1")
		os.execute("/usr/bin/dos2unix.sh  "..VENCMASTER_CONF_FILE.."  >  /dev/null  2>&1")
	if ( current_cmd ~= "changezoom" ) and ( current_cmd ~= "changevalue" ) then	
	  os.execute("/etc/init.d/venc-0 reload > /dev/null 2>&1")
	end 
	print ("done")


end
end
print ("</body>")
print ("</html>")