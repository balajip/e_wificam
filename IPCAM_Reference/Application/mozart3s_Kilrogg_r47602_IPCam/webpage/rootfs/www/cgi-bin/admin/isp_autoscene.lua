#!/usr/bin/lua

x = os.clock()
CONF_FILE_PREFIX="/etc/conf.d/"
AUTOSCENE_CONF_FILE=CONF_FILE_PREFIX .. "autoscene_conf.xml"
AUTOSCENE_STATUS_FILE=CONF_FILE_PREFIX ..".autoscene_status"
--REQUEST_METHOD = "GET"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
local lfs = require"lfs"

--command=enable&mode=wdr [outdoor/indoor/wdr]
--command=changemode&mode=wdr [outdoor/indoor/wdr]
--command=disable 
--command=status
QUERY_STRING = os.getenv("QUERY_STRING")

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>")
print ("<root>")


if REQUEST_METHOD == "GET" then  -- GET scope
if QUERY_STRING~=nil then -- select case
		current_cmd=nil
		mode="indoor"
		enable="0"
		
		k1,v1= string.match(QUERY_STRING, "(%w+)=(%w+)")
		
		
		if v1 ~=nil then
		  current_cmd=v1
		 -- print ("command=",current_cmd)  
		end 

		
		
		
		  tempfile = io.open(AUTOSCENE_CONF_FILE, "r")
		  if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		  end
		
		  _, _, _,mode, _ = string.find(temp_buf, "(<mode>)(.*)(</mode>)")   
		
		
		
		
	       if current_cmd == "enable" then
		    
		    k1,v1,k2,v2= string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)")
		 
		    mode=v2
		    
		    
		    tempfile = io.open(AUTOSCENE_CONF_FILE, "r")
		    if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		    end
		    _, _, head, _, tail = string.find(temp_buf, "(.*)(<mode>.-</mode>)(.*)")
		    scene_str = head.."<mode>"..mode.."</mode>"..tail
		    fp= io.open(AUTOSCENE_CONF_FILE,"w")
		    if fp then
			local ret, err = lfs.lock(fp, "w") 	
			fp:write(scene_str)
			fp:close()
		    end
		    os.execute("/usr/bin/dos2unix.sh  "..AUTOSCENE_CONF_FILE.."  >  /dev/null  2>&1")
		    os.execute("/etc/init.d/autoscene  reload  >  /dev/null  2>&1")

		    --print("enable")
		    os.execute("/usr/bin/msgctrl  -f /tmp/venc/c0/command.fifo  -h videoin -c ISPEnable  >  /dev/null  2>&1")
		    --os.execute("/usr/bin/msgctrl  -f /tmp/autoscenecmd.fifo  -h autoscene -c initial >  /dev/null  2>&1")
		    os.execute("/usr/bin/msgctrl -f /tmp/autoscenecmd.fifo  -h autoscene -c initial_and_start  >  /dev/null  2>&1")
		    os.execute("/usr/bin/msgctrl -f  /tmp/venc/c0/command.fifo  -h  master -c AutoSceneIsRun >  /dev/null  2>&1")
		    enable="1"
		    os.execute("rm -f "..AUTOSCENE_STATUS_FILE)
		    os.execute("touch  "..AUTOSCENE_STATUS_FILE)
		    os.execute("echo "..enable .."  >  "..AUTOSCENE_STATUS_FILE)
	      elseif current_cmd == "disable" then  
		    
		    --print("disable")
		    os.execute("/usr/bin/msgctrl -f /tmp/autoscenecmd.fifo  -h autoscene -c stop  >  /dev/null  2>&1 ")
		    os.execute("/usr/bin/msgctrl -f  /tmp/venc/c0/command.fifo  -h  master -c AutoSceneIsNotRun >  /dev/null  2>&1")
		    os.execute("/usr/bin/msgctrl  -f /tmp/venc/c0/command.fifo  -h videoin -c ISPDisable  >  /dev/null  2>&1")		   
		     enable="0"
		    os.execute("rm -f "..AUTOSCENE_STATUS_FILE)
		    os.execute("touch  "..AUTOSCENE_STATUS_FILE)
		    os.execute("echo "..enable .."  >  "..AUTOSCENE_STATUS_FILE)
	       elseif current_cmd == "changemode" then
		  k1,v1,k2,v2= string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)")
		 
		  mode=v2

		   tempfile = io.open(AUTOSCENE_CONF_FILE, "r")
		    if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		    end
		    _, _, head, _, tail = string.find(temp_buf, "(.*)(<mode>.-</mode>)(.*)")
		    scene_str = head.."<mode>"..mode.."</mode>"..tail
		    fp= io.open(AUTOSCENE_CONF_FILE,"w")
		    if fp then
			local ret, err = lfs.lock(fp, "w") 	
			fp:write(scene_str)
			fp:close()
		    end
			os.execute("/usr/bin/dos2unix.sh  "..AUTOSCENE_CONF_FILE.."  >  /dev/null  2>&1")
		    os.execute("/etc/init.d/autoscene  reload  >  /dev/null  2>&1")
  
	        elseif current_cmd == "status" then
		  --print ("status")
		  
	
		  --print ("mode:",mode)
		
	  
		  tempfile = io.open(AUTOSCENE_STATUS_FILE, "r")
		  if tempfile then
		      temp_buf = tempfile:read("*all")
		      tempfile:close()
		       _,_,enable, _ = string.find(temp_buf, "(.*)(\n)") 
		      --print ("enable:",  enable)
		  else
		    enable="0"
		
		  end
		  
		    
		

		end
  
 
	print("<enable>"..enable.."</enable>")
	print("<mode>"..mode.."</mode>")


end
end
print ("</root>")
