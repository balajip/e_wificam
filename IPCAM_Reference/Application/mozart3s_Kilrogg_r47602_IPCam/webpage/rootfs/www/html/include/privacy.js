var PrivacyXML = 0;
var PrivacyNode = 0;

function openVideo()
{
    var player = new pluginPlayer("Dean");
    player.displayRefStream();  
    var stream = player.getStream();  
    eval("var res = videoin_c0_s"+stream+"_resolution.split('x')");
    var wantW = parseInt(res[0]) + 80;
    var wantH = parseInt(res[1]) + 160;
    window.resizeTo(wantW,wantH);
    setTimeout("openPrivacy()", 500);
}

function openPrivacy()
{
    var png = "http://" + location.hostname + "/pmaskpic/pmask.png";
    $("#Dean")[0].openPrivacyMask(png);
}

function saveSetting()
{
    $("#Dean")[0].savePrivacyMask();
    var change = ($(PrivacyNode).find("switch").text() == "0")?"1":"0";
    $(PrivacyNode).find("switch").text(change);
    sendXML();
}

function sendXML()
{
    showAjaxImg(); 
    httpPost("xml", PrivacyXML, "/cgi-bin/admin/videoin.lua", null, clearAjaxImg);  
}

$(document).ready(function() {
    showAjaxImg();    
    $.ajax({
        type: "GET",
        url: "/cgi-bin/admin/videoin.lua",
        dataType: "xml",
        cache: false, 
        complete: function(XMLHttpRequest){
            clearAjaxImg();
        }, 
        success: function(data, textStatus, XMLHttpRequest) {
            PrivacyXML = data;            
            PrivacyNode = $(data).find("venc_videoin pmask")[0];
     
            if ( $(PrivacyNode).find("enable").text() == "on" )
            {
                $("#Privacy_switch")[0].checked = true;
                openVideo(); 
            }
            else
                $(".content").css("visibility","hidden");
        }    
    });     
});

function changePrivacyState()
{
    if ( $("#Privacy_switch")[0].checked )
    {
        $(PrivacyNode).find("enable").text("on");
        sendXML();
        $(".content").css("visibility","visible");  
        openVideo();
    }
    else
    {
        $("#Dean")[0].StopVideo();     
        $(".content").css("visibility","hidden");
        $(PrivacyNode).find("enable").text("off");
        sendXML();
    }
}
