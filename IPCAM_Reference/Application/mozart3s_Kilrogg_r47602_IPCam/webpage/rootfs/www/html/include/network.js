var szConfigNet = "/cgi-bin/admin/network.lua";
var szConfigHttp = "/cgi-bin/admin/http.lua";
var szConfigRtsp = "/cgi-bin/admin/rtsp.lua";

var xmlNet = 0;
var xmlHttp = 0;
var xmlRtsp = 0;

var rtspMulticastIP = "239.128.1.100";
var rtspMulticastPort = 5560;
var rtspMulticastTTL = 15;

var bHttpDone = false;
var bRtspDonw = false;

$(document).ready(init);

function init()
{    
    httpGet("xml", szConfigNet,  readNet,  null);
    httpGet("xml", szConfigHttp, readHttp, null);
    httpGet("xml", szConfigRtsp, readRtsp, null);
}

function readNet(data, textStatus, XMLHttpRequest)
{
    xmlNet = data; 
    
    var szNetType = $(data).find("net_type").text();
    setRadioValue("network_type", szNetType);

    $("#network_ipaddress").val($(data).find("ip_addr").text());
    $("#network_subnet").val($(data).find("subnet_mask").text());
    $("#network_router").val($(data).find("gateway").text());
    $("#network_dns1").val($(data).find("pri_nameserver").text());
    $("#network_dns2").val($(data).find("sec_nameserver").text());

    $("#pppoeUser").val($(data).find("username").text());
    $("#pppoePW").val($(data).find("password").text());

    typeChange();
}


function readHttp(data, textStatus, XMLHttpRequest)
{
    xmlHttp = data;
    $("#httpPort").val($(data).find("HTTP_port").text());
    var streamNum = parseInt($(data).find("total_num").text());
    for (var i = 0; i < streamNum; i++)
        $("#httpStrmArea").append($("#httpStrm").html()); 
    setTimeout("fillHTTP()",100);
}

function fillHTTP()
{
    $("#httpStrmArea .httpStrmNo").each(function(i){
        $(this).text(i+1);
    });
    $("#httpStrmArea .httpStrmName").each(function(i){
        $(this).val($(xmlHttp).find("accessname:eq("+i+")").text());
    });
}

function changeMulticastPort(textbox)
{
    rtspMulticastPort = parseInt(textbox.value);
    setTimeout("fillRtsp()",100);
}

function readRtsp(data, textStatus, XMLHttpRequest)
{
    xmlRtsp = data;
    $("#rtspPort").val($(data).find("rtsp port").text());
    if ($(data).find("multicast ipaddress:eq(0)").text().length > 0)
        rtspMulticastIP = $(data).find("multicast ipaddress:eq(0)").text();
    $("#rtspMulticastIP").val(rtspMulticastIP);

    if ($(data).find("multicast videoport:eq(0)").text().length > 0)
        rtspMulticastPort = parseInt($(data).find("multicast videoport:eq(0)").text());
    $("#rtspMulticastPort").val(rtspMulticastPort);

    if ($(data).find("multicast ttleq(0)").text().length > 0)
        rtspMulticastTTL = parseInt($(data).find("multicast ttl:eq(0)").text());
    $("#rtspMulticastTTL").val(rtspMulticastTTL);

    var streamNum = parseInt($(data).find("live_stream total_num").text());
    for (var i = 0; i < streamNum; i++)
        $("#rtspStrmArea").append($("#rtspStrm").html()); 
    setTimeout("fillRtsp()",100);
}

function fillRtsp()
{
    $("#rtspStrmArea .rtspStrmNo").each(function(i){
        $(this).text(i+1);
    });
    $("#rtspStrmArea .rtspStrmName").each(function(i){
        $(this).val($(xmlRtsp).find("live_stream accessname:eq("+i+")").text());
    });
    $("#rtspStrmArea .rtspMulticastEnable").each(function(i){
		$(this).attr("checked", ($(xmlRtsp).find("multicast enable:eq("+i+")").text() == "true"));
    });

    $("#rtspStrmArea .rtspMulticastVideoPort").each(function(i){
        $(this).val(rtspMulticastPort + i*6);
    });
    $("#rtspStrmArea .rtspMulticastAudioPort").each(function(i){
        $(this).val(rtspMulticastPort + 2 + i*6);
    });
    $("#rtspStrmArea .rtspMulticastDataPort").each(function(i){
        $(this).val(rtspMulticastPort + 4 + i*6);
    });
}

function typeChange()
{
    var szNetType = getRadioValue("network_type");
    $(".subOpt").attr("disabled", true);
    if( szNetType != "dhcp" )
        $("." + szNetType).attr("disabled", false); 
}

function submitNet()
{
    var szIp, szSubnet, szRouter, szDns1, szDns2;
    var szUserName, szPassword1, szPassword2;    
    var szNetType = getRadioValue("network_type");
    
    if (szNetType == "pppoe")
    {
        szPassword1 = $("#pppoePW").val();
        szPassword2 = $("#pppoePWConfirm").val();
        if (szPassword1 != szPassword2)
        {
            alert("Password not match.");
            return;
        }
        szUserName = $("#pppoeUser").val();
    }
    else if (szNetType == "static")
    {
        szIp = $("#network_ipaddress").val();
        if (!chkIP(szIp))   return;
        
        szSubnet = $.trim($("#network_subnet").val());
        if ((szSubnet != "") && (!chkIP(szSubnet))) return;
        
        szRouter = $.trim($("#network_router").val());
        if ((szRouter != "") && (!chkIP(szRouter))) return;
        
        szDns1 = $.trim($("#network_dns1").val());
        if ((szDns1 != "") && (!chkIP(szDns1))) return;
        
        szDns2 = $.trim($("#network_dns2").val());
        if ((szDns2 != "") && (!chkIP(szDns2))) return;
    }

    showAjaxImg();
    //Update value in xml
    $(xmlNet).find("net_type").text(szNetType);

    if (szNetType == "pppoe")
    {
        $(xmlNet).find("username").text(szUserName);
        $(xmlNet).find("password").text(szPassword1);
    }
    else if (szNetType == "static")
    {
        $(xmlNet).find("ip_addr").text(szIp);
        $(xmlNet).find("subnet_mask").text(szSubnet);
        $(xmlNet).find("gateway").text(szRouter);
        $(xmlNet).find("pri_nameserver").text(szDns1);
        $(xmlNet).find("sec_nameserver").text(szDns2);
    }

    //Post
    httpPost("xml", xmlNet, szConfigNet, null, clearAjaxImgSlowly);
}

function submitHttpRtsp()
{
    //Get value
    var szHttpPort = $("#httpPort").val();
    var szRtspPort = $("#rtspPort").val();
    //Check
    if (!chkPort(szHttpPort))   return;
    if (!chkPort(szRtspPort))   return; 

    if (szHttpPort == szRtspPort) {
        alert("Http port can not be the same as Rtsp port.");
        return;
    }

    var streamNum = parseInt($(xmlRtsp).find("live_stream total_num").text());
    var rtspName = new Array();
    var multicastEnable = new Array();
    for (var i = 0; i < streamNum; i++)
    {
        rtspName[i] = $(".rtspStrmName:eq("+i+")").val();
        multicastEnable[i] = $(".rtspMulticastEnable:eq("+i+")").attr("checked")? "true":"false"
        //console.log(multicastEnable[i] + " " + multicastIP[i]+" " +multicastVideoPort[i]+" "+multicastAudioPort[i]+" "+multicastDataPort[i]+"" + multicastTTL[i]);
    }
    for (var i = 0; i < streamNum; i++)
    {
        for (var j = 0; j < streamNum; j++)
        {            
            if( i != j && rtspName[i] == rtspName[j])
            {
               alert("Access name can not be the same.");
               return;
            }
        }    
    }

    bHttpDone = false;
    bRtspDone = false;
    showAjaxImg();
    $(xmlHttp).find("HTTP_port").text(szHttpPort);
    $(xmlRtsp).find("rtsp port").text(szRtspPort);

    for (var i = 0; i < streamNum; i++)
    {
        $(xmlRtsp).find("live_stream accessname:eq("+i+")").text(rtspName[i]);

        var port = rtspMulticastPort + i*6;
        var multicast_node = $(xmlRtsp).find("multicast:eq("+i+")");//.text(multicastEnable[i]);
        if (multicast_node.length)
		{
			multicast_node.find("enable").text(multicastEnable[i]);
            multicast_node.find("ipaddress").text(rtspMulticastIP);
            multicast_node.find("ttl").text(rtspMulticastTTL);
            multicast_node.find("videoport").text(port);
            multicast_node.find("audioport").text(port+2);
            multicast_node.find("mdataport").text(port+4);
		}
        else
        {
			var new_nodes = "<multicast>" +
                "<enable>" + multicastEnable[i] + "</enable>" +
                "<ipaddress>" + rtspMulticastIP + "</ipaddress>" +
                "<ttl>" + rtspMulticastTTL + "</ttl>" +
                "<videoport>" + port + "</videoport>" +
                "<audioport>" + (port+2) + "</audioport>" +
                "<mdataport>" + (port+4) + "</mdataport>" +
                "</multicast>";
			//IE doesn't support append(). 
           $(xmlRtsp).find("stream:eq("+i+")").append($(new_nodes));
        }
    }

    var xmlstring;
    if (typeof XMLSerializer != "undefined")
    {
        xmlstring = (new XMLSerializer()).serializeToString(xmlRtsp);
    }
    else
    {
        xmlstring = xmlRtsp.xml;
    }
    xmlstring = xmlstring.replace(/ xmlns=.+?"/g, "");

    /* POST */
    httpPost("text", xmlHttp, szConfigHttp, null, httpDone);
    httpPost("xml",  xmlstring, szConfigRtsp, null, rtspDone);
}

function httpDone()
{
    if (bRtspDone)  
        setTimeout("chkHTTPPortNTransfer()", 1500);
    else            
        bHttpDone = true;
}

function rtspDone()
{
    if (bHttpDone)  
        setTimeout("chkHTTPPortNTransfer()", 1500);
    else            
        bRtspDone = true;
}

function chkHTTPPortNTransfer()
{
    clearAjaxImg();
    //If the port number is 80 (which is the default port number), 
    //nothing will be returned
    var newPort = $("#httpPort").val();
    var currPort = (location.port == "")?"80":location.port;
    if( currPort != newPort && location.protocol == "http:")
        parent.window.location.replace("http://"+location.hostname+":"+newPort+"/");
}
