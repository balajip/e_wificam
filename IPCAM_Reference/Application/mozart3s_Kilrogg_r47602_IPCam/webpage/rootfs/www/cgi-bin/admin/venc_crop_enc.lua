#!/usr/bin/lua

x = os.clock()
CONF_FILE_PREFIX="/etc/conf.d/c0/"
--CONF_FILE_PREFIX="./"

local lfs = require"lfs"

VENCENCODER_0_CONF_FILE=CONF_FILE_PREFIX .. "vencencoder_conf.0.xml"


VENC_FILE=CONF_FILE_PREFIX .. "venc_conf.xml"



--REQUEST_METHOD = "GET"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")

--QUERY_STRING = "resolution=640x480"
QUERY_STRING = os.getenv("QUERY_STRING")

--top=value&left=value&width=value&height=value

print ("Content-type: text/html")
print ("Cache-Control: no-cache")
print ("")

print ("<html>")
print ("<body>")
if REQUEST_METHOD == "GET" then  -- GET scope
if QUERY_STRING~=nil then -- select case
  k1, v1, k2, v2, k3, v3, k4, v4 = string.match(QUERY_STRING, "(%w+)=(%w+)&(%w+)=(%w+)&(%w+)=(%w+)&(%w+)=(%w+)")
  
  top=v1
  left=v2
  width=v3
  height=v4
  
  if top ~= nil and left ~= nil and   width ~= nil and  height ~= nil then
    -- print("top ",top)
    -- print("left ",left)
    -- print("width ",width)
    --print("height ",height)
    tempfile = io.open(VENCENCODER_0_CONF_FILE, "r")
    if tempfile then
	  temp_buf = tempfile:read("*all")
	  tempfile:close()
    end
    _, _, head, crop_str, tail = string.find(temp_buf, "(.*)(<crop>.-</crop>)(.*)")
		   
    if top ~= nil then
	_, _, head_1, _, tail_1= string.find(crop_str, "(.*)(<top>.-</top>)(.*)")
	crop_top_str = head..head_1.."<top>"..top.."</top>"..tail_1..tail
	fp= io.open(VENCENCODER_0_CONF_FILE,"w")
	if fp then
		local ret, err = lfs.lock(fp, "w") -- exclusive lock!
	    fp:write(crop_top_str)
	    fp:close()
	end  
    end
    
    tempfile = io.open(VENCENCODER_0_CONF_FILE, "r")
    if tempfile then
	temp_buf = tempfile:read("*all")
	tempfile:close()
    end
    _, _, head, crop_str, tail = string.find(temp_buf, "(.*)(<crop>.-</crop>)(.*)")
		   
    
    if left ~= nil then
	_, _, head_1, _, tail_1= string.find(crop_str, "(.*)(<left>.-</left>)(.*)")
	  
	crop_left_str = head..head_1.."<left>"..left.."</left>"..tail_1..tail
		      --print("crop_left_str ",crop_left_str)
	fp= io.open(VENCENCODER_0_CONF_FILE,"w")
	if fp then
			  --print("hello!!")
			  local ret, err = lfs.lock(fp, "w") -- exclusive lock!
			  fp:write(crop_left_str)
			  fp:close()
	end  
    end

    tempfile = io.open(VENCENCODER_0_CONF_FILE, "r")
    if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
    end
    _, _, head, crop_str, tail = string.find(temp_buf, "(.*)(<crop>.-</crop>)(.*)")
		   
    if width ~= nil then
		      _, _, head_1, _, tail_1= string.find(crop_str, "(.*)(<width>.-</width>)(.*)")
	  
		      crop_width_str = head..head_1.."<width>"..width.."</width>"..tail_1..tail
		     

		      fp= io.open(VENCENCODER_0_CONF_FILE,"w")
		      if fp then
				 local ret, err = lfs.lock(fp, "w") -- exclusive lock! 
				 fp:write(crop_width_str)
				 fp:close()
		      end  
    end
    
    tempfile = io.open(VENCENCODER_0_CONF_FILE, "r")
    if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
    end
    _, _, head, crop_str, tail = string.find(temp_buf, "(.*)(<crop>.-</crop>)(.*)")
		   
    if height ~= nil then
		      _, _, head_1, _, tail_1= string.find(crop_str, "(.*)(<height>.-</height>)(.*)")
	  
		      crop_height_str = head..head_1.."<height>"..height.."</height>"..tail_1..tail
		     

		      fp= io.open(VENCENCODER_0_CONF_FILE,"w")
		      if fp then
				 local ret, err = lfs.lock(fp, "w") -- exclusive lock! 
				 fp:write(crop_height_str)
				 fp:close()
		      end  
      end

   
     
    if width ~= nil  and height ~=nil and top ~= nil and left ~= nil then
		    
		         
			tempfile = io.open(VENCENCODER_0_CONF_FILE, "r")
			if tempfile then
			      temp_buf = tempfile:read("*all")
			      tempfile:close()
		      end
    
		      _, _, head,_, tail = string.find(temp_buf, "(.*)(<resolution>.-</resolution>)(.*)")
		      encode_str = head.."<resolution>"..width.."x"..height.."</resolution>"..tail
		     

		      fp= io.open(VENCENCODER_0_CONF_FILE,"w")
		      if fp then
				local ret, err = lfs.lock(fp, "w") -- exclusive lock!	  
				fp:write(encode_str)
				fp:close()
		      end
		      
		         
		      tempfile = io.open(VENCENCODER_0_CONF_FILE, "r")
		      if tempfile then
			    temp_buf = tempfile:read("*all")
			      tempfile:close()
		    end
   
		      
		      
		      _, _, head,_, tail = string.find(temp_buf, "(.*)(<converting_method>.-</converting_method>)(.*)")
		      encode_str = head.."<converting_method>".."randomcropping".."</converting_method>"..tail
		     

		      fp= io.open(VENCENCODER_0_CONF_FILE,"w")
		      if fp then
				  local ret, err = lfs.lock(fp, "w") -- exclusive lock!
				  fp:write(encode_str)
			      fp:close()
		      end  
		      
		      
		      
    end
		
      -- reload venc-0, but it should change vencencoder switch value
	
   
	tempfile = io.open(VENC_FILE, "r")
	      if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
	      end
	 _, _, head, venc_encoder, tail = string.find(temp_buf, "(.*)(<venc_encoder>.-</venc_encoder>)(.*)")
	
	  _, _, encoder_head, switch0, interval_0, switch1, interval_1, switch2, interval_2, switch3 , encoder_tail = string.find(venc_encoder, "(.*)(<switch>.-</switch>)(.*)(<switch>.-</switch>)(.*)(<switch>.-</switch>)(.*)(<switch>.-</switch>)(.*)")

	  _, value0, _ =  string.match(switch0, "(<%w+>)(%w+)(</%w+>)")
	  value0 = (value0+1)%2
	  _, value1, _ =  string.match(switch1, "(<%w+>)(%w+)(</%w+>)")
	  --value1 = (value1+1)%2
	  _, value2, _ =  string.match(switch2, "(<%w+>)(%w+)(</%w+>)")
	  --value2 = (value2+1)%2
	  _, value3, _ =  string.match(switch3, "(<%w+>)(%w+)(</%w+>)")
	  --value3 = (value3+1)%2
	  venc_encoder = encoder_head.."<switch>"..value0.."</switch>"..interval_0.."<switch>"..value1.."</switch>"..interval_1.."<switch>"..value2.."</switch>"..interval_2.."<switch>"..value3.."</switch>"..encoder_tail

	venc_new_file = head..venc_encoder..tail
	
	-- write to venc file
        venc_file = io.open(VENC_FILE, "w")
        if venc_file then
				local ret, err = lfs.lock(venc_file, "w") -- exclusive lock!
                venc_file:write(venc_new_file)
                venc_file:close()
        end
	os.execute("/usr/bin/dos2unix.sh  "..VENCENCODER_0_CONF_FILE.."  >  /dev/null  2>&1")	
	os.execute("/usr/bin/dos2unix.sh  "..VENC_FILE.."  >  /dev/null  2>&1")
	os.execute("/etc/init.d/venc-0 reload")
  end
  
  
end
end
print ("</body>")
print ("</html>")