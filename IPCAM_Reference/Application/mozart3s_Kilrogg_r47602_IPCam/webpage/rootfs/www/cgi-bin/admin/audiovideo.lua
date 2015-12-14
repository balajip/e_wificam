#!/usr/bin/lua
x = os.clock()
CONF_FILE_PREFIX="/etc/conf.d/c0/"
VENC_CONF_PREFIX=CONF_FILE_PREFIX .. "vencencoder_conf."
VENC_MASTER_CONF=CONF_FILE_PREFIX .. "vencmaster_conf.xml"
VENC_FILE = CONF_FILE_PREFIX.."venc_conf.xml"
AENC_CONF="/etc/conf.d/aenc_conf.xml"
ADEC_CONF="/etc/conf.d/adec_conf.xml"
AUTOSCENE_CONF="/etc/conf.d/autoscene_conf.xml"
local lfs = require"lfs"

print ("Content-type: text/xml")
print ("Cache-Control: no-cache")
print ("")
if  os.getenv("REQUEST_METHOD") == "POST" then
	print ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
	print ("<root>")
	-- get 
	input = io.stdin:read("*all")
	
	_, _, venc_master_config, venc_enc_config, aenc_config = string.find(input, ".*(<venc_master>.-</venc_master>)(.*)(<audio_encoder>.-</audio_encoder>).*")
	
	-- write back to master conf
    fpXml = io.open(VENC_MASTER_CONF, "w")
	local ret, err = lfs.lock(fpXml , "w")
	fpXml:write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
    fpXml:write("<root>\n    ")
	fpXml:write(venc_master_config)
    fpXml:write("\n</root>\n")
	fpXml:close()
	--os.execute("/usr/bin/dos2unix.sh  "..VENC_MASTER_CONF.."  >  /dev/null  2>&1")
	-- for autoscene	
	-- When user change the power line frquency through UI, notify the autoscene process. 
	_, _, head, new_pline_freq_tag, tail =string.find(venc_master_config, "(.*)(<pline_freq>.-</pline_freq>)(.*)")
	_, new_pline_freq, _ =  string.match(new_pline_freq_tag, "(<pline_freq>)(%w+)(</pline_freq>)")
	
	
	tempfile = io.open(AUTOSCENE_CONF, "r")
    if tempfile then
		temp_buf = tempfile:read("*all")
        tempfile:close()
		_, _, head, pline_freq_tag, tail = string.find(temp_buf, "(.*)(<pline_freq>.-</pline_freq>)(.*)")
		new_autoscene_conf = head.."<pline_freq>"..new_pline_freq.."</pline_freq>"..tail
		autoscene_conf_file = io.open(AUTOSCENE_CONF, "w")
		if autoscene_conf_file then
	        autoscene_conf_file:write(new_autoscene_conf)
	        autoscene_conf_file:close()
		end
		os.execute("/usr/bin/dos2unix.sh  "..AUTOSCENE_CONF.."  >  /dev/null  2>&1")
		os.execute("/etc/init.d/autoscene reload")
		
	end
	 
	
	
-- end of autoscene 


	-- write back to encoder conf
	enc_counter = 0
	for enc_conf in string.gmatch(venc_enc_config, "<venc_encoder>.-</venc_encoder>")
	do
		VENC_CONF = VENC_CONF_PREFIX..enc_counter..".xml"
    	fpXml = io.open(VENC_CONF, "w")
		local ret, err = lfs.lock(fpXml , "w")
	    fpXml:write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
    	fpXml:write("<root>\n    ")
	    fpXml:write(enc_conf)
    	fpXml:write("\n</root>\n")
	    fpXml:close()
		enc_counter = enc_counter + 1
	end

    

--  reload venc-0, but it should change vencmaster and vencencoder switch value
        tempfile = io.open(VENC_FILE, "r")
        if tempfile then
                temp_buf = tempfile:read("*all")
                tempfile:close()
        end

        _, _, head, venc_master, venc_videoin, venc_encoder, tail = string.find(temp_buf, "(.*)(<venc_master>.-</venc_master>)(.*)(<venc_encoder>.-</venc_encoder>)(.*)")
        -- new venc_master field
        _, _, master_head, switch, master_tail = string.find(venc_master, "(.*)(<switch>.-</switch>)(.*)")
        _, value, _ =  string.match(switch, "(<%w+>)(%w+)(</%w+>)")
        value = (value+1)%2
        venc_master = master_head.."<switch>"..value.."</switch>"..master_tail


	start = 0
	tail_index = 0
	new_str = ""
	start, tail_index = string.find(venc_encoder, "<switch>.-</switch>", 0)
  	new_str = new_str..string.sub(venc_encoder, 0, start-1)
    str_temp = string.sub(venc_encoder, start, tail_index)
    _, value, _ =  string.match(str_temp, "(<%w+>)(%w+)(</%w+>)")
    value = (value+1)%2
    str_temp = "<switch>"..value.."</switch>"
    new_str = new_str..str_temp

	repeat
  		tail_temp = tail_index
  		start, tail_index = string.find(venc_encoder, "<switch>.-</switch>", tail_index)
  		if tail_index ~= NIL then
    		new_str = new_str..string.sub(venc_encoder, tail_temp+1, start-1)
    		str_temp = string.sub(venc_encoder, start, tail_index)
    		_, value, _ =  string.match(str_temp, "(<%w+>)(%w+)(</%w+>)")
    		value = (value+1)%2
    		str_temp = "<switch>"..value.."</switch>"
    		new_str = new_str..str_temp
  		elseif tail_index == NUL then
    		new_str = new_str..string.sub(venc_encoder, tail_temp+1, string.len(venc_encoder))
  		end
	until tail_index == NIL

--	print (new_str)

	 venc_new_file = head..venc_master..venc_videoin..new_str..tail
        -- step 10 - write to venc file
        venc_file = io.open(VENC_FILE, "w")
        if venc_file then
				local ret, err = lfs.lock(venc_file , "w")
                venc_file:write(venc_new_file)
                venc_file:close()
        end
	
	switch_file = io.open("/usr/share/www/cgi-bin/admin/ResSwitch.sh", "r")
	if switch_file  then
	  switch_file:close()
	  os.execute("/usr/share/www/cgi-bin/admin/ResSwitch.sh")
	end
	
	os.execute("/usr/bin/dos2unix.sh  "..VENC_FILE.."  >  /dev/null  2>&1")
	os.execute("/usr/bin/dos2unix.sh  "..VENC_MASTER_CONF.."  >  /dev/null  2>&1")
	os.execute("/etc/init.d/venc-0 reload")
-- write back to audio enc conf
    fpXml = io.open(AENC_CONF, "w")
	local ret, err = lfs.lock(fpXml , "w")
	fpXml:write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
    fpXml:write("<root>\n    ")
	fpXml:write(aenc_config)
    fpXml:write("\n</root>\n")
	fpXml:close()
	os.execute("/usr/bin/dos2unix.sh  "..AENC_CONF.."  >  /dev/null  2>&1")
	--os.execute("/etc/init.d/aenc reload")
	os.execute("/etc/init.d/aenc stop")
	os.execute("/etc/init.d/aenc start")

	print(string.format("<time>%.2f (sec)</time>", os.clock() - x))
	print ("</root>")
end
