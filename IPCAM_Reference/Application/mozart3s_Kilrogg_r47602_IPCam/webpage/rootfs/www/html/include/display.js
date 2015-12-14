var VencXML=0;
var DisplayW=0;
var DisplayH=0;
var RecURL = "/cgi-bin/admin/vrecord.cgi?command=";
var SVCTLayer="3";
var SCVTAmount=1;
var SVCTEnable=false;

function clientSet()
{
    var mode = ($("#content").css("display") == "block")?"none":"block";
    var btnValue = (mode == "none")?"Show":"Hide";
    $("#content").css( "display", mode );
    $("#btnClient").val(btnValue + " Client Settings");
}

function OnSave()
{    
    showAjaxImg();   
    var expires = new Date();       
    expires.setTime (expires.getTime() + (10 * 365 * 24 * 60 * 60 * 1000));
    var strm = getRadioValue("stream");               
    setCookie("streamsource", strm, expires);
    setCookie("protocol", getRadioValue("ProtocolOpt"), expires);
    setCookie("bufferTime", $("#bufferTime").val(), expires);

    eval("var codec=videoin_c0_s"+strm+"_codectype");
    if(codec == "svc")
    {
        SVCTLayer = $(".svct:eq("+strm+")").val();
        setCookie("svcLayer0_" + strm, SVCTLayer, expires);
        
        SVCTEnable = $(".svcHeader:eq("+strm+")")[0].checked;
        var header = (SVCTEnable)?"on":"off";
        setCookie("svcHeader0_" + strm, header, expires);
    }
    
    //! set clear the AjaxImg in 1 seconds
    setTimeout("clearAjaxImg()", 1000); 
    document.getElementById("Dean").StopVideo();     
    setPulgin();
}

function testBuffer()
{
    var re=/^(\d+)$/g; 
    if(re.test($("#bufferTime").val()))
    {
        if( RegExp.$1 <= 3000 && RegExp.$1 >= 0 ) 
            return true;
    }           
    return false;   
}

function checkStream(data, textStatus, XMLHttpRequest)
{
    var content = "";
    var expires = new Date();       
    expires.setTime (expires.getTime() + (10 * 365 * 24 * 60 * 60 * 1000));    
    $(data).find("venc_encoder codec").each(function(i){
        $("#strmBlock input[name='stream']").val(i);
        $("#strmBlock .streamName").text("Stream " + (i+1).toString()); 
        var svctOpt = ($(this).text() == "svc")?"visible":"hidden";
        $("#strmBlock .svctOpt").css("visibility", svctOpt);
        var layerNum = parseInt($(data).find("venc_encoder svc_layer_num:eq("+i+")").text());
        $("#strmBlock .svct").html(fillOpt(layerNum));          
        $("#streams").append($("#strmBlock").html());       
        if(svctOpt == "visible")
        {
            if( isNaN(layerNum) )
                SVCTLayer = 3;
            else
                checkLayer(i, layerNum);
 
            setCookie("svcLayer0_" +i, SVCTLayer, expires);
            setCookie("svcAmount0_" + i, layerNum, expires);
            $(".svct:eq("+i+")").val(SVCTLayer);
            
            var preHeader = getCookie("svcHeader0_"+i);  
            var header = (preHeader != "on")?"off":preHeader;                                   
            setCookie("svcHeader0_" + i, header, expires);                         
            SVCTEnable = (header == "on");
            $(".svcHeader:eq("+i+")")[0].checked = SVCTEnable;            
        }              
    });   
    var stream = (getCookie("streamsource") == null)?0:getCookie("streamsource");
    setRadioValue("stream", stream);  
    setPulgin(); 
}

function checkLayer(stream, newAmt)
{
    var oldAmt = parseInt(getCookie("svcAmount0_"+stream));    
    var preLayer = parseInt(getCookie("svcLayer0_"+stream));
    if( isNaN(oldAmt) || isNaN(preLayer) ) 
        SVCTLayer = newAmt - 1;
    if(newAmt == oldAmt)
        SVCTLayer = preLayer;
    else if(newAmt > oldAmt)
    {
        SVCTLayer = preLayer + newAmt - oldAmt;
    } 
    else
    {
        SVCTLayer = preLayer - (oldAmt - newAmt);
        if( SVCTLayer < 0 )
            SVCTLayer = 0;
    }
}

function legalLayer(layer, amount)
{
    var smallest = 4 - parseInt(amount);
    if( layer < smallest )
        return true;
    else
        return false;
}

function fillOpt(layer)
{
    var content = "";
    var opt = "";
    var optName = "";
    for(var i=layer;i>0;i--)
    {
        optName = (i == layer)?"Full":"1/" + Math.pow(2,layer-i);
        opt = "<option value=\"" + (i-1) + "\">" + optName + " Frame Rate</option>";
        content = content + opt;
    }
    return content;
}
/*
          <option value="3">Full Frame Rate</option>
          <option value="2">1/2 Frame Rate</option>
          <option value="1">1/4 Frame Rate</option>
          <option value="0">1/8 Frame Rate</option>
*/
function strmChange()
{
}

$(document).ready(function() {
    showAjaxImg();  
    httpGet("text", "/cgi-bin/viewer/getparam.cgi", initStream, null);    

    var s_protocol=getCookie("protocol");
    if( s_protocol != "UDP" && s_protocol != "TCP" && 
        s_protocol != "HTTP" && s_protocol != "HTTPS")
        s_protocol = "UDP";
    if(location.protocol == "https:")
    {
        document.getElementsByName("ProtocolOpt")[2].disabled = "disabled";
        document.getElementsByName("ProtocolOpt")[3].disabled = "";
        if( s_protocol=="HTTP" )
            s_protocol="HTTPS";
    }
    else
    {
        document.getElementsByName("ProtocolOpt")[2].disabled = "";
        document.getElementsByName("ProtocolOpt")[3].disabled = "disabled";
        if( s_protocol=="HTTPS" )
            s_protocol="HTTP";
    }
    setRadioValue("ProtocolOpt", s_protocol);          
    setCookieDecade("protocol", s_protocol);

    var bt = getCookie("bufferTime");
    bt = isNaN(parseInt(bt))?"0":bt;
    $("#bufferTime").val(bt);

    //var dxva = getCookie("DXVA");
    //dxva = (dxva == "on")?"on":"off";
    //$("input[name='DXVAOpt'][value='"+dxva+"']")[0].checked = true;

    var cgiUrl = "/cgi-bin/admin/fetch_xml.lua?vencmaster_conf.xml";
    httpGet("xml", cgiUrl, checkIRCut, null);  
});

function initStream(data, textStatus, XMLHttpRequest)
{
    eval(data);
    var cgiUrl = "/cgi-bin/admin/fetch_xml.lua?vencencoder_conf.0.xml";
    var streamCount = parseInt(videoin_c0_streamnum);
    for(var i=1;i<streamCount;i++)
    {
        cgiUrl = cgiUrl + "&vencencoder_conf." + i + ".xml";
    }
    httpGet("xml", cgiUrl, checkStream, null);

    // Set plug-in title
    var pluginTitle = system_hostname;
    if( typeof(system_info_modelname)!="undefined" ) 
        pluginTitle = pluginTitle + "_" + system_info_modelname;   
    $("#page_title").text(pluginTitle);
}

function checkIRCut(data, textStatus, XMLHttpRequest)
{
    VencXML = data;
    //IR-Cut         
    var ir = ($(VencXML).find("ircut").text() == "on")?"IR-Cut:OFF":"IR-Cut:ON";  
    $("#irCut").val(ir);
    var cgiUrl = "/cgi-bin/admin/fetch_xml.lua?sensor_dependent.xml";
    httpGet("xml", cgiUrl, checkOpenFunction, null);                     
}

function checkOpenFunction(data, textStatus, XMLHttpRequest)
{
    var sensor = $(data).find("sensor_type").text(); 

    if($(data).find(sensor + " wdr").text() == "on") 
    {
        $("#wdr_on").css("display","inline");
        $("#wdr_off").css("display","inline");
    }  
    if($(data).find(sensor + " record").text() == "on")
    {
        $("#record").css("display","inline").attr("disabled",true); 
        checkSDAndRecord();
    }   
    if($(data).find(sensor + " autofocus").text() == "on") 
    {               
        $("#dock").css("display","inline");
        setAF();
    }      
}

function setPulgin()
{
    var stream = getCookie("streamsource");
    stream = isNaN(stream)?0:stream;
    eval("var res=videoin_c0_s"+stream+"_resolution.split('x')"); 
    DisplayW = res[0];
    DisplayH = res[1];    
    var borderW = parseInt(DisplayW) + 60;    
    $(".divBorder").css("width", borderW); 
    var titleH = $("#display_image")[0].offsetTop - $("#page_title")[0].offsetTop;
    var borderH = parseInt(DisplayH) + titleH + 30;
    $(".divBorder").css("height", borderH);     
    var test = $("#Dean")[0];
    if( test && test != "undefined" )
    {
        var player = new pluginPlayer("Dean");
        player.setStream(stream);
        var bufTime = $("#bufferTime").val();
        if( bufTime != "" && parseInt(bufTime) != 0 )
            player.setOpt("BufferTime=" + $("#bufferTime").val());  
        eval("var codec=videoin_c0_s"+stream+"_codectype");
        if(codec == "svc")
        {
            var amount = parseInt(getCookie("svcAmount0_"+stream));
            if( amount > 1 )
            {
                SVCTLayer = $(".svct:eq("+stream+")").val();
                SVCTEnable = $(".svcHeader:eq("+stream+")")[0].checked;       
                player.setSVCT(SVCTEnable, SVCTLayer);
            }
        }                                       
        player.displayStream(); 
    }
    clearAjaxImg();
}

function checkSDAndRecord()
{     
    $.ajax({
        type: "GET",
        url: RecURL + "sd_status",
        dataType: "xml",
        cache: false, 
        error: function(jqXHR, textStatus, errorThrown) {
            alert("Error on vrecord.cgi check SD status.");
            clearAjaxImg();
        },
        success: function(data, textStatus, XMLHttpRequest) {
            if($(data).find("status").text() == "OK")
                checkRecordStatus();    
        }    
    });     
}

function checkRecordStatus()
{    
    $.ajax({
        type: "GET",
        url: RecURL + "vrec_status",
        dataType: "xml",
        cache: false, 
        complete: function(XMLHttpRequest){
            setTimeout("clearAjaxImg()", 100);
        }, 
        error: function(jqXHR, textStatus, errorThrown) {
            alert("Error on vrecord.cgi check Record status.");
        },
        success: function(data, textStatus, XMLHttpRequest) {
            if($(data).find("status").text() == "OK") 
            {  
                var rec = $(data).find("message").text();
                var ctrlType = ( rec == "VREC_RECORDING")?"Stop Recording":"Record"; 
                $("#record").val(ctrlType).attr("disabled",false);                             
            } 
            else
                alert("Info: " + $(data).find("message").text());   
        }    
    });     
}

function recControl()
{    
    $("#record").attr("disabled",true);
    showAjaxImg();
    var mode = ($("#record").val() == "Record")?"on":"off";
    httpGet("xml", RecURL + mode, null, checkSDAndRecord);         
}

function sendIRCut()
{
    showAjaxImg();
    $("#irCut").attr("disabled",true); 
    var ir = ($("#irCut").val() == "IR-Cut:ON")?"on":"off";
    $(VencXML).find("ircut").text(ir);
    httpPost("text", VencXML, "/cgi-bin/admin/vencmaster.lua", parseIR, clearAjaxImg);     
}

function parseIR(data, textStatus, XMLHttpRequest)
{
    var txt = ($("#irCut").val() == "IR-Cut:ON")?"IR-Cut:OFF":"IR-Cut:ON"; 
    $("#irCut").val(txt).attr("disabled",false);      
}

function takeSnapshot()
{
    $("#Dean")[0].takeSnapshot();
}

function takeJPEGSnap()
{

	var req;
	var doesNotSupport = true; 

	if (window.XMLHttpRequest) { 
	 	req = new XMLHttpRequest; 
	} else if (window.ActiveXObject) { 
	  	req = new ActiveXObject("Microsoft.XMLHTTP"); 
	} 
	if (req)
	{
		req.open("POST", "/cgi-bin/admin/JPEGSnap.lua", true);
		req.setRequestHeader("If-Modified-Since","0");
		req.onreadystatechange = function() {processReqChange(req, null);};
		req.send(null);
		alert("The captured image will be available at your hostpc tftp path and in /tmp folder!!"); 
	}
	else
	{
		alert("Your browser does not support XMLHttpRequest technology!"); 
		doesNotSupport = false; 
	}
}

function openPTZ()
{
    if (navigator.appName == "Microsoft Internet Explorer")
    {
        window.open("display_new_ptz.html","PTZ","height=480,width=640");
    }
    else
    {
        alert("Please use IE to see the PTZ window.");
    }
}

var SIPCommand = "";
function SIPControl()
{
    var b_sip = $("#sip")[0];
    b_sip.disabled = true;
    SIPCommand = ( b_sip.value == "Talk" )?"open":"close";
    detectSIPStatus();     
}

function detectSIPStatus()
{
    //0: closed
    //1: connecting
    //2: connected
    //3: closing
    var dean = document.getElementById("Dean");
    var b_sip = document.getElementById("sip");
    var statusNow = dean.sipStatus;
    if( statusNow == 0 )
    {
        if(SIPCommand == "open")
        {
            dean.openAudioUpload("sip:alice@"+ location.hostname +":5060");
            SIPCommand = "";
            b_sip.value = "Connecting...";
            setTimeout("detectSIPStatus()", 200);
            return;
        }
        b_sip.value = "Talk";
        b_sip.disabled = "";
        b_sip.focus();  
    }
    else if( statusNow == 1 )
    {
        b_sip.value = "Connecting...";
        setTimeout("detectSIPStatus()", 200);
    }
    else if( statusNow == 2 )
    {
        if(SIPCommand == "close")
        {
            dean.closeAudioUpload();
            SIPCommand = "";
            b_sip.value = "Closing...";
            setTimeout("detectSIPStatus()", 200);
            return;
        }        
        b_sip.value = "Stop Talking";
        b_sip.disabled = "";
        b_sip.focus();
    } 
    else if( statusNow == 3 )
    {
        b_sip.value = "Closing...";
        setTimeout("detectSIPStatus()", 200);
    }
    else
        alert("SIP Status Error: Status is " + statusNow);        
}

function WDRCtrl(sw)
{
    showAjaxImg();
    var url = "/cgi-bin/admin/widedynamicrange_" + sw + ".lua";      
    httpGet("xml", url, WDRStatus, clearAjaxImg);      
}

function WDRStatus(data, textStatus, XMLHttpRequest)
{
    alert($(data).find("root").text());
}

var AFSwitch = false;
var MouseLoc = "0,0";
function setAF()
{
    var docked = 0;
    $(".afPanel").height($(window).height());
    $("#dock .dock").click(function(){
        $(this).parent().parent().addClass("docked").removeClass("free");
        
        docked += 1;
        var dockH = ($(window).height()) / docked;
        var dockT = 0;               
        
        $("#dock li ul.docked").each(function(){
            $(this).height(dockH).css("top", dockT + "px");
            dockT += dockH;
        });
        $(this).parent().find(".undock").show();
        $(this).hide();
        
        if (docked > 0)
            $("#contentAll").css("margin-left","240px");
        else
            $("#contentAll").css("margin-left", "0px"); 
        $("#Dean")[0].playerControl("MonitorMouse");    
        AFSwitch = true;
        checkMouseLocation();    
    });
    
    $("#dock .undock").click(function(){
        $(this).parent().parent().addClass("free").removeClass("docked")
            .animate({left:"-180px"}, 200).height($(window).height()).css("top", "0px");
        
        docked = docked - 1;
        var dockH = ($(window).height()) / docked;
        var dockT = 0;               
        
        $("#dock li ul.docked").each(function(){
            $(this).height(dockH).css("top", dockT + "px");
            dockT += dockH;
        });
        $(this).parent().find(".dock").show();
        $(this).hide();
        
        if (docked > 0)
            $("#contentAll").css("margin-left", "240px");
        else
            $("#contentAll").css("margin-left", "0px");   
        AFSwitch = false;    
        $("#Dean")[0].playerControl("StopMonitorMouse");
    });        
    $("#dock > li").hover(function(){
          $(this).find("ul").animate({left:"40px"}, 200);
          $("#contentAll").css("margin-left","240px");
      }, function(){
          $(this).find("ul.free").animate({left:"-180px"}, 200);
          if ( $(".afPanel").hasClass("free") )
          $("#contentAll").css("margin-left", "0px"); 
    });
    $("#advArea").css("display","none");  
 
    var fsv = parseInt($(VencXML).find("auto_focus speed").text());
    var zsv = parseInt($(VencXML).find("auto_focus zoomspeed").text());
    var nsv = parseInt($(VencXML).find("auto_focus noisethreshold").text());
    $("#zoomSpeed").slider({
        min: 0, max: 6, value: zsv    
    });     
    $("#focusSpeed").slider({
        min: 0, max: 6, value: fsv
    }); 
    $("#afSearch").slider({
        min: 0, max: 7, value: nsv    
    }); 
    setRadioValue("reFocus", $(VencXML).find("auto_focus refocus").text());     
}

function checkMouseLocation()
{
    if(AFSwitch)
    {
        var player = $("#Dean")[0];
        var loc = player.getInfo("ClickXY");
        var finalX=0,finalY=0,finalW=160,finalH=120;
        if( loc != MouseLoc )
        {
            var xy = loc.split(",");
            var x = Math.round((640*parseInt(xy[0]))/DisplayW);
            var y = Math.round((480*parseInt(xy[1]))/DisplayH);
            if( x < 80 ) 
            {
                finalX=0;
                finalW=80+x;                
            }
            else if( x > 559 )
            {
                finalX=x-80;
                finalW=639-finalX;                 
            }
            else
            {
                finalX=x-80;
                finalW=160;                  
            }
            if( y < 60 )
            {
                finalY=0;
                finalH=60+y;     
            }
            else if( y > 419 )
            {
                finalY=y-60;
                finalH=479-finalY;                      
            }
            else
            {
                finalY=y-60;
                finalH=120;                  
            }
            MouseLoc = loc; 
            var command="changewindow&startX=" + finalX + "&startY=" + finalY + 
                        "&width=" + finalW + "&height=" + finalH;
            showAjaxImg();  
            $.ajax({
                type: "GET",
                url: "/cgi-bin/admin/isp_autofocus.lua?command="+command,
                cache: false, 
                complete: function(XMLHttpRequest){
                    clearAjaxImg();
                    setTimeout("checkMouseLocation()", 500);
                }, 
                success: function(data, textStatus, XMLHttpRequest) {                    
                }    
            });                          
        }
        else
            setTimeout("checkMouseLocation()", 500);
    }
}

function advChange() 
{    
    var displayMode = ($("#advArea").css("display") == "block")?"none":"block";
    $("#advArea").css("display",displayMode);
    var arrow = ( displayMode == "block" )?"&#9660;":"&#9658;";
    $("span.advArrow").html(arrow);
}

function osdSet()
{
    var displayMode = ($("#osd").css("display") == "block")?"none":"block";
    $("#osd").css( "display",displayMode );
    var btn = ($("#btnOSD").val() == "Show Magnification")?"Hide":"Show";
    $("#btnOSD").val( btn + " Magnification" );
    UpdateMag = (displayMode == "block");
    if(UpdateMag)
        getMag(); 
}

var UpdateMag = false;
function getMag()
{
    if(UpdateMag)
        httpGet("xml", "/cgi-bin/admin/af_status.lua", fillMag, null);
}

function fillMag(data, textStatus, XMLHttpRequest)
{
    var zoom = $(data).find("zoomRatio").text() + "X";
    $("#osd").text(zoom);    
    setTimeout("getMag()", 1000);
}

var AFBaseURL = "/cgi-bin/admin/isp_autofocus.lua?command=";
function saveAdvAF()
{
    showAjaxImg();     
    var ntv = $( "#afSearch" ).slider( "option", "value" );
    var ntURL = AFBaseURL + "changenoisethreshold&noisethreshold=" + ntv;
        
    httpGet("html", ntURL, function(data){
        var rfURL = AFBaseURL + "changerefocus&refocus=" + getRadioValue("reFocus");
        httpGet("html", rfURL, null, clearAjaxImg);
    }, null); 
}

function sendCommand(commandURL)
{
    showAjaxImg();
    httpGet("html", AFBaseURL+commandURL, null, clearAjaxImg);      
}

function near()
{        
    sendCommand("changevalue&value=0");         
}

function far()
{ 
    sendCommand("changevalue&value=1");  
}

function saveFocus()
{
    var val = $( "#focusSpeed" ).slider( "option", "value" );
    sendCommand("changespeed&speed="+val);  
}

function zoomout()
{
    sendCommand("changezoom&zoom=1");  
}

function zoomin()
{
    sendCommand("changezoom&zoom=0");  
}

function saveZoom()
{
    var val = $( "#zoomSpeed" ).slider( "option", "value" );
    sendCommand("changezoomspeed&zoomspeed="+val);  
}
