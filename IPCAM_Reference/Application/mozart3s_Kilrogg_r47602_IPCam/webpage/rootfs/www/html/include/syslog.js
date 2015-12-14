/* remote syslog JavaScript */

var SysLogXhReq = 0;
var xmlLocalInst;

var szConfigLog = "/cgi-bin/admin/syslog.lua";
var xmlLog = 0;

$(document).ready(init);

function init()
{
    /* Show the processing mask */
    showAjaxImg();

    /* Adjust the size of text box for current log */
    var iHeight = $(window).height();
    var iOffset = $("#currLog").parent().offset().top;

    $("#currLog").css("height", (iHeight - iOffset - 80) + "px");
    
    /* Get configures */
    httpGet("xml", szConfigLog, readLog, null);

    clearAjaxImg();
}
 
function readLog(data, textStatus, XMLHttpRequest)
{
    xmlLog = data;  

    /* get syslog status*/
    var enable = ($(data).find("enable_remote_syslog").text() == "1");    
    $("#enableRemoteLog").attr("checked", enable);    

    /* get remote remote server ip */
    $("#logIp").val($(data).find("server_ip").text());
    /* get remote remot server port */
    $("#logPort").val($(data).find("server_port").text());
}


function submit()
{
    /* Check value first */
    var enable = ($("#enableRemoteLog")[0].checked)?"1":"0";
    var ip = "";
    var port = "";

    if (enable == "1") 
    {
        ip   = $("#logIp").val();
        port = $("#logPort").val()
        
        if (!chkIP(ip))             
            return;
        if (!chkPort(port))     
            return;
    }
    else 
    {
        $("#logIp").val("");
        $("#logPort").val("");
    }
        
    showAjaxImg();
    /* Update xml configure content */  
    $(xmlLog).find("enable_remote_syslog").text(enable);
    $(xmlLog).find("server_ip").text(ip);
    $(xmlLog).find("server_port").text(port);   

    httpPost("text", xmlLog, szConfigLog, null, clearAjaxImg);
}
