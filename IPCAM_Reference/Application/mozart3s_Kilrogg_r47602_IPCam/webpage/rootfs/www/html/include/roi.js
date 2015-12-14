var CurrentWindows = 0;
var Max_Windows = 8;
var workingWindow = 0;
var VideoWidth=640;
var VideoHeight=480;
var ROINodes;
var ROIWindow;
var ROIURL = "/cgi-bin/admin/videoenc_roi.lua?id=";
var StreamQ;
var StreamFPS;
var MouseX=0;
var MouseY=0;  
var WRatio = VideoWidth/640;//(VideoWidth/2)/320
var HRatio = VideoHeight/480;

$(document).ready(function() {
    showAjaxImg();
    httpGet("text", "/cgi-bin/viewer/getparam.cgi", init, null); 
});

function init(data, textStatus, XMLHttpRequest)
{
    eval(data);
    if(typeof(videoin_c0_streamnum) != "undefined")
    { 
        var streamCount = parseInt(videoin_c0_streamnum);
        ROINodes = new Array(streamCount);                        
        for(var i=1;i<streamCount;i++)
        {
            getOneROI(i); 
        }   
        var opt = "";
        var cgiUrl = "/cgi-bin/admin/fetch_xml.lua?sensor_dependent.xml";
        for(var i=0;i<streamCount;i++)
        {
            opt = opt + "<option value=\"" + i + "\">Stream " + (i+1).toString() + "</option>";
            cgiUrl = cgiUrl + "&vencencoder_conf." + i + ".xml";
        }
        $("#stream").append(opt);          
        httpGet("xml", cgiUrl,  parseSensorDep,  null);          
    } 
     $(document).mousemove(function(e){
         MouseX = e.pageX;
         MouseY = e.pageY;
         //$('#currM').text("(" + e.pageX +", "+ e.pageY + ")");
     });  
}

function getOneROI(id)
{
    $.ajax({
        type: "GET", 
        url: ROIURL+id, 
        dataType: "xml", 
        cache: false, 
        success: function(data, textStatus, XMLHttpRequest) {                        
            ROINodes[id] = data;
        }
    });    
}

function parseSensorDep(data, textStatus, XMLHttpRequest)
{
    var sensorType = $(data).find("sensor_type").text();
    var sensorNode = $(data).find(sensorType)[0];     
    var maxRes = $(sensorNode).find("max_resolution support_resolution").text().split('x');
    if(!isNaN(parseInt(maxRes[0])) && !isNaN(parseInt(maxRes[1])))
    {
        SensorW = parseInt(maxRes[0]);
        SensorH = parseInt(maxRes[1]);
    }
    var streamCount = parseInt(videoin_c0_streamnum);
    StreamQ = new Array(streamCount); 
    StreamFPS = new Array(streamCount); 
    for(var i=0;i<streamCount;i++)
    {
        StreamQ[i] = parseInt($(data).find("h264_quant_value:eq("+i+")").text());
        StreamFPS[i] = parseInt($(data).find("frame_rate:eq("+i+")").text());
    }    
    httpGet("xml", ROIURL+"0", setROI0, clearAjaxImgSlowly);     
}

function setROI0(data, textStatus, XMLHttpRequest)
{ 
    ROINodes[0] = data;
    loadROI(0);
}

function loadROI(stream)
{
    var roiNode = $(ROINodes[stream]).find("roi")[0];
    CurrentWindows = 0;
    $("#new_win").attr("disabled", false);
    //window setting
    var roiCount = $(roiNode).find("win").length;
    var first = false;
    ROIWindow = new Array(roiCount);
    for( var i=0; i<roiCount; i++ )
    {
        ROIWindow[i] = $(roiNode).find("win:eq("+i+")")[0];
        if( i != 8 && $(ROIWindow[i]).find("active").text() == "on" )
        {
            CurrentWindows++;
            if(!first)
            {
                workingWindow = i;
                first = true;
            }
        }
    }
    if( CurrentWindows == Max_Windows )
    {
        $("#new_win").attr("disabled", true);
    } 
  
    var roi = ($(roiNode).find("enable").text() == "on");
    $("#roiSwitch")[0].checked = roi;            
    dqChange(roi);
    if (roi)     
        openVideo(stream); 
}

function openVideo(no)
{
    //Boundary: 1M 1280 x 800 
    var opt = "NoIntelligent";
    eval("var res = videoin_c0_s"+no+"_resolution.split('x')"); 
    if(!isNaN(parseInt(res[0])) && !isNaN(parseInt(res[1])))
    { 
        VideoWidth = parseInt(res[0]);
        VideoHeight = parseInt(res[1]);
        if( VideoWidth > 1280 || VideoHeight > 800 )
        {
            VideoWidth = VideoWidth/2;
            VideoHeight = VideoHeight/2;            
        }
        WRatio = VideoWidth/640;//(VideoWidth/2)/320
        HRatio = VideoHeight/480;
        $("#Dean").css("width", VideoWidth +"px").css("height", VideoHeight +"px");
        opt = opt + ",Width="+VideoWidth+",Height="+VideoHeight; 
    }
                  
    var player = new pluginPlayer("Dean");
    player.setOpt(opt);
    player.setStream(no);
    player.displayStream();   

    setTimeout("setupMotion()", 900);  
}

function setupMotion()
{
    $("#Dean")[0].openMotionSetup(roiString());
    $("#Dean")[0].playerControl("ROI=pop;");
    clearAjaxImg();
}

function addROI()
{
    var str = "";
    for(var i=0;i<Max_Windows;i++)
    {
        if($(ROIWindow[i]).find("active").text() == "off")
        {
            str = i.toString() + ",";
            str = str + (VideoWidth/4-10).toString() + ","+(VideoWidth/4+10).toString()+ ",";
            str = str + (VideoHeight/4-10).toString() + ","+(VideoHeight/4+10).toString()+ ",";
            $(ROIWindow[i]).find("active").text("on");
            workingWindow = i;
            $("#Dean")[0].AddMotionRectangle(str);
            CurrentWindows++;
            break;
        }
    }    
    if( CurrentWindows == Max_Windows )
    {
        $("#new_win").attr("disabled", true);
    }
}

function updateWorkingWindow(m_index)
{
    updateWindows();
    if( m_index != -1 && $(ROIWindow[m_index]).find("active").text() == "off" )
    {
        //del this window, find an actived one.
        CurrentWindows--;
        $("#new_win").attr("disabled", false);  
        apply();     
    }
    else
        workingWindow = m_index;
}

function saveSetting()
{
    showAjaxImg();
    var winName = $("#winName").val();
    if( winName == "" )
        winName = "Window" + workingWindow;
    $(ROIWindow[workingWindow]).find("name").text(winName);  
    if(chkSetting()) 
    { 
        updateWindows();
        $(ROIWindow[workingWindow]).find("name").text(winName);
        var no = parseInt($("#stream").val());
        var qp = parseInt($("#quality").val()) - StreamQ[no];
        $(ROIWindow[workingWindow]).find("QP").text(qp);
        $(ROIWindow[workingWindow]).find("interval").text($("#period").val());
        httpPost("xml", ROINodes[no], ROIURL+no, null, clearAjaxImg);
        closeDiag();
    }
    else
        clearAjaxImg();
}

function apply()
{
    showAjaxImg(); 
    updateWindows();
    var no = parseInt($("#stream").val());
    httpPost("xml", ROINodes[no], ROIURL+no, null, clearAjaxImg);
}

function chkSetting()
{
    //Check Quality
    var no = parseInt($("#stream").val());
    var up = StreamQ[no] + 10;
    var low = StreamQ[no] - 10;
    if(up > 51)
        up = 51;
    if(low < 0)
        low = 0;    
    if(!verifyNum("quality",up,low,"quality"))
        return false;
    //Check interval 
    if(!verifyNum("period",300,1,"frame period"))
        return false;   
    return true;
}

function verifyNum(id, upBound, lowBound, itemName)
{
    var itemValue = parseInt($("#"+id).val());
    
    if(isNaN(itemValue))
    {
        alert("Please keyin number in " + itemName);
        return false;
    }
    if(itemValue < lowBound || itemValue > upBound)
    {
        alert("Please keyin number between " + lowBound + " and " + upBound + " in " + itemName);
        return false;
    }
    return true;
}

function saveBack()
{
    showAjaxImg();
    if( verifyNum("periodBack",300,1,"frame period") )
    {
        $(ROIWindow[8]).find("interval").text($("#periodBack").val());
        var no = parseInt($("#stream").val());
        httpPost("xml", ROINodes[no], ROIURL+no, null, clearAjaxImg);
        closeBackDiag();        
    }
    else
        clearAjaxImg();
}

function updateWindows()
{
    var str = $("#Dean")[0].currMotionWindow;
    var data = str.split(',');
    var count = parseInt(data[0]);

    var index_m = 1, m_left = 0, m_top = 0, m_width = 0, m_height = 0;
    for(var i=0;i<count;i++)
    {
        var active = (data[index_m] == "1")?"on":"off";
        $(ROIWindow[i]).find("active").text(active);
        index_m++;

        m_left = Math.round(parseInt(data[index_m])/WRatio);
        $(ROIWindow[i]).find("left").text(m_left.toString());
        index_m++;

        m_width = Math.round(parseInt(data[index_m])/WRatio) - m_left;
        var diff = m_left + m_width - 320;
        if( diff > 0 )
            m_width = m_width - diff;
        $(ROIWindow[i]).find("width").text(m_width.toString());
        index_m++;

        m_top = Math.round(parseInt(data[index_m])/HRatio);
        $(ROIWindow[i]).find("top").text(m_top.toString());
        index_m++;

        m_height = Math.round(parseInt(data[index_m])/HRatio) - m_top;
        diff = m_top + m_height - 240;
        if( diff > 0 )
            m_height = m_height - diff;
        $(ROIWindow[i]).find("height").text(m_height.toString());
        index_m++;
    }
}

function roiString()
{
    var s = Max_Windows + ","; 
    for(var i=0;i<Max_Windows;i++)
    {
        var act = ( $(ROIWindow[i]).find("active").text() == "on" )?"1,":"0,";         
        var w_top = Math.round( intFromNode(ROIWindow[i],"top")*HRatio );
        var w_left = Math.round( intFromNode(ROIWindow[i],"left")*WRatio );
        var w_right = w_left + Math.round( intFromNode(ROIWindow[i],"width")*WRatio );
        var w_bottom = w_top + Math.round( intFromNode(ROIWindow[i],"height")*HRatio );
        s = s + act + w_left + "," + w_right + "," + w_top + "," + w_bottom + ",";
    }
    return s;
}

function switchROI()
{
    showAjaxImg();
    var streamNo = parseInt($("#stream").val());
    var roi = $("#roiSwitch")[0].checked;
    var en = (roi)?"on":"off";
    $(ROINodes[streamNo]).find("roi enable").text(en);
    httpPost("xml", ROINodes[streamNo], ROIURL+streamNo, null, clearAjaxImg);
    dqChange(roi);
    if ( roi )                 
        setTimeout("openVideo('"+streamNo+"')", 200); 
    else
        $("#Dean")[0].StopVideo();
}

function dqChange(isOpened)
{
    var roi = (isOpened)?"block":"none";
    $("#Dean").css("display",roi);
}

function changeStream()
{
    $("#Dean")[0].StopVideo();    
    showAjaxImg();
    closeAllPop();
    var strm = parseInt($("#stream").val());
    loadROI(strm);
    clearAjaxImg();
}
var Pop = false;
var Wait = 255;
function pop(func,i)
{    
    if(func == "info" && !Pop)
    {
        var no = parseInt($("#stream").val());
        var interval = parseFloat($(ROIWindow[8]).find("interval").text());
        var winFPS = (parseFloat(StreamFPS[no])/interval).toFixed(2);
        var msg = "Background Window,<br />Q:"+StreamQ[no]+", fps:"+winFPS;
        Wait = Math.random();        
        //$('#currM').text("( " + MouseX +", "+ MouseY + " )");
        setTimeout("showInfo('"+msg+"','"+Wait+"')",50);
    }
    else if(func == "winin")
    {        
        Pop = true;
        workingWindow = i;
        updateWindows(ROIWindow);
        var ost = $("#Dean").offset();  
        var w_top = Math.round( intFromNode(ROIWindow[i],"top")*HRatio*2 );
        var w_left = Math.round( intFromNode(ROIWindow[i],"left")*WRatio*2 );
        var w_right = w_left + Math.round( intFromNode(ROIWindow[i],"width")*WRatio*2 );
        MouseX = ost.left + w_right + 2;
        MouseY = ost.top + w_top;
        var no = parseInt($("#stream").val());
        var interval = parseFloat($(ROIWindow[i]).find("interval").text());
        var winFPS = (parseFloat(StreamFPS[no])/interval).toFixed(2);
        var q = intFromNode(ROIWindow[i],"QP") + StreamQ[no];
        var msg = $(ROIWindow[i]).find("name").text() + ",<br />Q:"+q+", fps:"+winFPS;
        Wait = Math.random();        
        showInfo(msg,Wait);         
    }
    else if(func == "set")
    {
        Pop = true;
        $(".popInfo").css("display","none").css("z-index","1");
        var infoTop = MouseY + 5;
        var infoLeft = MouseX + 5;
        var strm = parseInt($("#stream").val());
        if(i == 8)
        {                              
            $("#qBack").text("Quality: " + StreamQ[strm]);
            $("#periodBack").val($(ROIWindow[i]).find("interval").text());
            $(".popSetBack").css("top",infoTop +"px").css("left",infoLeft+"px");
            $(".popSetBack").css("display","block").css("z-index","100");
        }
        else
        {                   
            $("#winName").val($(ROIWindow[i]).find("name").text());
            
            var q = intFromNode(ROIWindow[i],"QP") + StreamQ[strm];
            $("#quality").val(q);
            $("#period").val($(ROIWindow[i]).find("interval").text());
            $(".popSet").css("top",infoTop +"px").css("left",infoLeft+"px");
            $(".popSet").css("display","block").css("z-index","100");       
        }    
    }    
}

function closeAllPop()
{
    $(".popInfo").css("display","none").css("z-index","1");
    $(".popSet").css("display","none").css("z-index","1");
    $(".popSetBack").css("display","none").css("z-index","1");
}

function intFromNode(winNode, variable)
{
    return parseInt($(winNode).find(variable).text());
}

function showInfo(msg,wait)
{
    Pop = true;
    $(".popInfo").css("top",MouseY +"px").css("left",MouseX+"px");
    $("#info").html(msg);
    $(".popInfo").css("display","block").css("z-index","100");
    setTimeout("closePop('"+wait+"')",3000); 
}

function closePop(waitFor)
{
    if(Wait == waitFor)         
        $(".popInfo").css("display","none").css("z-index","1");
}

function resetPop()
{
    Pop = false;
}

function closeDiag()
{
    $(".popSet").css("display","none").css("z-index","1");
    Pop = true;
}

function closeBackDiag()
{
    $(".popSetBack").css("display","none").css("z-index","1");
    Pop = true;
}

function keepPop()
{
    Pop = true;
}