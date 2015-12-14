var originWidth,
    originHeight,
    globalWidth,
    globalHeight,
    maskRW,
    maskRH,
    scale = 0;
var oldPTZString;
var minResizeWidth,minResizeHeight;
var VencXhReq = 0;
var sensorXhReq = 0;

$(document).ready(function() {
    httpGet("text", "/cgi-bin/viewer/getparam.cgi",  initial,  null);
});

function initial(data, textStatus, XMLHttpRequest)
{
    eval(data);
    $.ajax({
        type: "GET",
        url: "/cgi-bin/admin/fetch_xml.lua?sensor_dependent.xml",
        dataType: "xml",
        cache: false, 
        success: function(data) {
            sensorXhReq = data;
            if(typeof(videoin_c0_streamnum) == "undefined")
            {
                alert("Get stream number failed!");
            }
            else
            {
                var cgiUrl = "/cgi-bin/admin/fetch_xml.lua?vencmaster_conf.xml";
                var streamCount = parseInt(videoin_c0_streamnum);
                for(var i=0;i<streamCount;i++)
                {
                    cgiUrl = cgiUrl + "&vencencoder_conf." + i + ".xml";
                }
                cgiUrl = cgiUrl + "&../aenc_conf.xml";
                $.ajax({
                    type: "GET",
                    url: cgiUrl,
                    dataType: "xml",
                    cache: false, 
                    success: function(data, textStatus, XMLHttpRequest) {                        
                        VencXhReq = data;
                        //alert(XMLHttpRequest.responseText);
                        streamResolution();
                    }    
                });                    
            }
        }
    }); 
        
    originWidth = parseInt(videoin_c0_cap_width);
    originHeight = parseInt(videoin_c0_cap_height);
    globalWidth = 960;//parseInt(tempglobal[0]);
    globalHeight = 540;//parseInt(tempglobal[1]);
    maskRW = (globalWidth/2)/originWidth;
    maskRH = (globalHeight/2)/originHeight;

    $("#Dean").css("width", "960px").css("height", "540px");    
    window.resizeTo(1020,740);     
    var player = new pluginPlayer("Dean");
    player.setOpt("Width=960,Height=540");
    player.displayRefStream();
    //left,right,top,bottom
    setTimeout("setupROI()",500);     
}

function setupROI()
{
    $("#currentSize").text(videoin_c0_s0_crop_width + "x" + videoin_c0_s0_crop_height);
    var cropLeft = Math.round(parseInt(videoin_c0_s0_crop_left)*maskRW);
    var cropTop = Math.round(parseInt(videoin_c0_s0_crop_top)*maskRH);
    var cropW = Math.round(parseInt(videoin_c0_s0_crop_width)*maskRW);
    var cropH = Math.round(parseInt(videoin_c0_s0_crop_height)*maskRH);
    var cropRight = cropLeft + cropW;
    var cropBottom = cropTop + cropH;
    var rectangle = cropLeft + "," + cropRight + "," + cropTop + "," + cropBottom;            
    $("#Dean")[0].openPTZ(rectangle,8,8);
    setLocation();     
}

function streamResolution()
{
    var content="";   
    $("#frameSize").empty();
    var maxR = $(VencXhReq).find("venc_master resolution").text();
    var maxNum = parseInt( $(sensorXhReq).find("max_resolution support_resolution:contains('" + maxR + "')").attr("number"));
    var vencEncNode = $(VencXhReq).find("venc_encoder:eq(0)")[0];
    var resolution = $(vencEncNode).find("resolution").text();
    var resolutionNum = parseInt($(sensorXhReq).find("stream_resolution support_resolution:contains('" + resolution + "')").attr("number"));                           
    var supReCount = $(sensorXhReq).find("stream_resolution support_resolution").length; 
    for(var j=0;j<supReCount;j++)
    {      
        var supResolution = $(sensorXhReq).find("stream_resolution support_resolution:eq("+j+")");   
        var optNum = parseInt(supResolution.attr("number"));
        
        if(optNum == resolutionNum)
        {
            content = content + optionStr(supResolution.text(), supResolution.attr("text"), true);                                          
        }
        else if(optNum <= maxNum)
        {
            content = content + optionStr(supResolution.text(), supResolution.attr("text"), false);                                            
        }            
    }
    $("#frameSize").append(content);
}

function optionStr(val, txt, selected)
{
    var opt = "<option value=\"" + val + "\"";
    var backStr = (selected)?" selected=\"selected\">":">";
    opt = opt + backStr + txt + "</option>";
    return opt;
}

function setRefSize()
{
    var ref = $("#frameSize").val();
    var tmp = ref.split('x');
    var cropW = Math.round((parseInt(tmp[0])-1)*maskRW); //0-1919
    var cropH = Math.round((parseInt(tmp[1])-1)*maskRH);
    var cmd = "changePTZ=0," + cropW + ",0," + cropH;
    $("#currentSize").text(ref);
    $("#Dean")[0].playerControl(cmd);
}

var now = new Date();
var timeStamp = now.getTime();
var onRequest = 0;
function RefreshDrag()
{
    var drag = new Date(),
        nowTime = drag.getTime();
    if((nowTime - timeStamp) > 100 && onRequest == 0)
    {
        onRequest = 1;
        setLocation();
        timeStamp = nowTime;
    }
}

function setLocation()
{
    var str = $("#Dean")[0].currMotionWindow;
    var data = str.split(',');
    var m_left = Math.round(parseInt(data[2])/maskRW);
    var m_top = Math.round(parseInt(data[4])/maskRH);
    var m_width = Math.round((parseInt(data[3])-parseInt(data[2]))/maskRW);
    var m_height = Math.round((parseInt(data[5])-parseInt(data[4]))/maskRH);
    var loc = "(" + m_left + "," + m_top + ") ";
    $("#currentSize").text(loc + m_width + "x" + m_height);
    setTimeout("setLocation()", 300);    
}    

function updateROI()
{
    var str = $("#Dean")[0].currMotionWindow;   
    var data = str.split(',');
    var m_left = Math.round( parseInt(data[2])/maskRW );
    var m_top = Math.round( parseInt(data[4])/maskRH );
    var m_width = Math.round((parseInt(data[3])-parseInt(data[2]))/maskRW);
    var m_height = Math.round((parseInt(data[5])-parseInt(data[4]))/maskRH);
    var diff = m_left + m_width - originWidth;
    if( diff > 0 )
        m_width = m_width - diff;
    diff = m_top + m_height - originHeight;
    if( diff > 0 )
        m_height = m_height - diff;
    var $venc0 = $(VencXhReq).find("venc_encoder").eq(0);
    $venc0.find("converting_method").text("randomcropping");
    $venc0.find("crop left").text(m_left);
    $venc0.find("crop top").text(m_top);
    $venc0.find("crop width").text(m_width);
    $venc0.find("crop height").text(m_height);
    $venc0.find("resolution").text(m_width + "x" + m_height);
}

function sendROI()
{
    showAjaxImg();
    updateROI();
    $.ajax({
      type: "POST",
      dataType: "xml",
      processData: false,
      data: VencXhReq,
      url: "/cgi-bin/admin/audiovideo.lua",
      complete: function(XMLHttpRequest){
          clearAjaxImg();      
          if (XMLHttpRequest.status != 200) { 
              alert("Sever: " + XMLHttpRequest.statusText); 
          }
      }    
    });    
}

function closePage()
{
    window.close();
}

function updateWorkingWindow(i)
{
    //callback function for plug-in
    //Do not remove this
}