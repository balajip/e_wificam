#!/usr/bin/lua

function getpwd (id)
    io.input("/etc/passwd")
    local s = io.read("*all")
    _, _, pwd = string.find(s, id..":(.-):")
    return pwd
end

print ("Content-type: text/xml") 
print ("Cache-Control: no-cache") 
print ("") 
print ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") 
print ("<root>")

if  os.getenv("REQUEST_METHOD") == "GET" then
    print ("    <req_method>GET</req_method>") 
    -- get root password from /etc/passwd
    rootpwd = getpwd ("root")
    
    -- test password length
    _, len = string.find(rootpwd, rootpwd)
    if len == 0 then
        roothaspwd="0"
    else
        roothaspwd="1"
    end    

    print ("    <root_pass>"..roothaspwd.."</root_pass>") 
else
    query_string = io.stdin:read("*all")
    _, _, newpass =  string.find(query_string, ".-=(.-)&.+")

    io.output("/tmp/sys_mgr.fifo")
    ret = assert(io.write("0 root ", newpass, " $\n"))    
    
    -- ret 0 for success
    if ret == true then
        ret = 0
    else
        ret = 1
    end
    print ("    <change_pass>"..ret.."</change_pass>")    
end

print ("</root>") 

