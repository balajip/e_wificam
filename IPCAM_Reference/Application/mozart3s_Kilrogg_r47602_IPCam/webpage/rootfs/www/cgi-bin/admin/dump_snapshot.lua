#!/usr/bin/lua

io.write("Content-type: image/jpeg\r\n\r\n")

local file = assert(io.popen('/usr/share/www/cgi-bin/admin/gen_snapshot_get_filename.sh', 'r'))
local output = file:read('*all')
file:close()

if output ~= "" then
	local pic = assert(io.open(output),"rb")
	local pic_out = pic:read('*a')
	pic:close()
	io.write(pic_out)
end

