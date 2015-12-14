#!/usr/bin/lua

-- ptzctrl -f /tmp/venc/c0/command.fifo -p 0.5 -t -0.5
redirect_to = "/changePTZ.asp"
REQUEST_METHOD = os.getenv("REQUEST_METHOD")
QUERY_STRING = os.getenv("QUERY_STRING")
input = io.read("*all")
_, _, pan, tilt, zoom = string.find(input, "pan=(.*)&tilt=(.*)&zoom=(.*)")
host = "encoder0"
-- we should verify the input, but tonumber doesn't support floating-point now 
--[[
pan = tonumber(pan)
if pan == nil then pan = 0 end
if pan > 1 then pan = 1 end
if pan < -1 then pan = -1 end
tilt = tonumber(tilt)
if tilt == nil then tilt = 0 end
if tilt > 1 then tilt = 1 end
if tilt < -1 then tilt = -1 end
zoom = tonumber(zoom)
if zoom == nil then zoom = 1 end
if zoom > 16 then zoom = 16 end
if zoom < 1/16 then zoom = 1/16 end
]]

os.execute("ptzctrl -f /tmp/venc/c0/command.fifo -p " .. pan .. " -t " ..tilt .. " -z " .. zoom.." -s ".."7".." -h "..host)

print ("Content-type: text/html")
print ("Cache-Control: no-cache")
print ("")
print ("<html><head></head>")
--[[
print ("<body>")
print ("p_pan= " .. pan)
print ("p_tilt = " .. tilt)
print ("p_zoom = " .. zoom)
print ("</body>")
]]
print ("<body onLoad=\"window.location = \'" .. redirect_to .. "\'\"></body>")
print ("</html>")

