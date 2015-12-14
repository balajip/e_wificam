#!/usr/bin/lua

vencmaster_conf = "/etc/conf.d/c0/vencmaster_conf.xml"
vencencoder0_conf = "/etc/conf.d/c0/vencencoder_conf.0.xml"
vencencoder1_conf = "/etc/conf.d/c0/vencencoder_conf.1.xml"
SensorType = "MT9P031"

dumb_resolution_querier = function (input_file)
	for line in io.lines(input_file) do
		_, _, res = string.find(line, "<resolution>(.*)</resolution>")
		if res ~= nil then
			break
		end
	end
	return res
end


dumb_roi_resolution_querier = function (input_file)
    tempfile = io.open(input_file, "r")
    if tempfile then
		    temp_buf = tempfile:read("*all")
		    tempfile:close()
    end
    _, _, head, crop_str, tail = string.find(temp_buf, "(.*)(<crop>.-</crop>)(.*)")
    _, _, roiwidth= string.find(crop_str, "<width>(.*)</width>")
    _, _, roiheight= string.find(crop_str, "<height>(.*)</height>")
   
    res=roiwidth.."x"..roiheight
  
  return res
end

resVisible = dumb_resolution_querier(vencmaster_conf) 
resROI     = dumb_roi_resolution_querier(vencencoder0_conf) 
resProj    = dumb_roi_resolution_querier(vencencoder1_conf) 

print ("Content-type: text/plain\r")
print ("Cache-Control: no-cache")
print ("")

print ("resVisible=\"" .. resVisible .. "\";")
print ("resROI=\"" .. resROI .. "\";")
print ("resProj=\"" .. resProj .. "\";")

function switch(selector, t)
	assert (type(t) == "table", "Table not supplied to switch")
	local f = t [selector]

	if f then
		assert (type(f) == "function", "Not a function")
		f ()
	else
		print ("Resolution=\1280x1024\";") -- default setting
	end -- if
end -- switch

action = {
  ["OV5653"] = function () 
			print("Resolution=\"2560x1920,1600x1200,1280x1024,1280x720\";") 
		end,
  ["MT9D131"] = function () 
			print("Resolution=\"1600x1200\";") 
		end,
  ["MT9M011"] = function () 
			print("Resolution=\"1280x1024\";") 
		end,
  ["MT9M111"] = function () 
			print("Resolution=\"1280x1024\";") 
		end,
  ["MT9P031"] = function () 
			print("Resolution=\"2560x1920,1600x1200,1280x1024,1280x720\";") 
		end,
  ["MT9V111"] = function () 
			print("Resolution=\"640x480\";") 
		end,
  ["NULLSENSOR"] = function () 
			print("Resolution=\"704x480\";") 
		end,
  ["OV9630"] = 	function () 
			print("Resolution=\"1280x1024\";") 
		end,
  ["OV9710"] = 	function () 
			print("Resolution=\"1280x800\";") 
		end,
  ["PAS5101PE"] = 	function () 
			print("Resolution=\"1280x1024\";") 
		end,
  ["IMX035"] = 	function () 
			print("Resolution=\"1280x1024\";") 
		end,
  ["ICX098"] = 	function () 
			print("Resolution=\"640x480\";") 
		end,
  ["MN34030PL"] = 	function () 
			print("Resolution=\"1280x1024\";") 
		end,
}

switch(SensorType, action)
