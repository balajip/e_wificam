#!/usr/bin/lua

TITLE="System Log"
LOGFILES="/var/log/messages.4 /var/log/messages.3 /var/log/messages.2 /var/log/messages.1 /var/log/messages"

print ("Content-type: text/html")
print ("Cache-Control: no-cache")
print ("")

print ("<html><head>")
print ("<title>"..TITLE.."</title>")
print ("<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/default.css\" media=\"all\">")
print ("</head><body>")
print ("<p class=\"confElement\">")


for filename in string.gfind(LOGFILES, "[%S]+") do
    local f = io.open(filename, "r")
    if f then
        io.stdout:write(string.gsub( f:read("*all"), "\n", "<br \>"))
        f:close()
    end
end


--os.execute("sed 's/$/<br \\/>/' "..LOGFILES)
print ("</p>")
print ("</body></html>")
