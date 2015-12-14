var MotionXML = 0;
var MotionNode = 0;
var MotionWindow;
var CurrentWindows = 0;
var Max_Windows = 16;
var workingWindow = 0;
var motionSetting = "";
var VideoWidth=640;
var VideoHeight=480;

function initSlider(id, valID)
{
    $("#"+id).slider({
        min: 0,
        max: 100,
        values: 0,
        slide: function(event, ui) {       
            $("#"+valID).text(ui.value);
        }    
    });       
}

$(document).ready(function() {
    showAjaxImg();
    initSlider("objSize", "objSizeValue"); 
    initSlider("sensi", "sensiValue"); 
    httpGet("text", "/cgi-bin/viewer/getparam.cgi", init, null); 
});

function init(data, textStatus, XMLHttpRequest)
{
    eval(data);
    httpGet("xml", "/cgi-bin/admin/videoin.lua", parseMotion, clearAjaxImgSlowly);   
}

function parseMotion(data, textStatus, XMLHttpRequest)
{
    MotionXML = data;            
    MotionNode = $(data).find("venc_videoin motion")[0];
    if ( $(MotionNode).find("enable").text() == "on" )
    {
        $("#motion_switch")[0].checked = true;
        findWindow();
        if( CurrentWindows > 0 )
            changeWindow();  
        openVideo(); 
    }
    else
    {
        $("#Dean").css("visibility", "hidden");
        $("#content").css("display", "none");
    }    
}

function findWindow()
{
    var MW_count = $(MotionNode).find("win").length;
    MotionWindow = new Array(MW_count);
    var first = false;
    CurrentWindows = 0;
    for( var i = 0; i < MW_count; i++ )
    {
        MotionWindow[i] = $(MotionNode).find("win:eq("+i+")")[0];
        if( $(MotionWindow[i]).find("active").text() == "on" )
        {                
            CurrentWindows++;
            if(!first)
            {
                workingWindow = i;
                first = true;
            }
        }
    }
    if( CurrentWindows >= Max_Windows )
    {
        $("#new_win").attr("disabled", true);
    }
}

function changeWindow()
{
    $("#w_name").val($(MotionWindow[workingWindow]).find("name").text());
    var obj_size = parseInt($(MotionWindow[workingWindow]).find("objsize").text());
    var sensitivity = parseInt($(MotionWindow[workingWindow]).find("sensitivity").text());
    setSliderValue("#objSize", "#objSizeValue", obj_size);
    setSliderValue("#sensi", "#sensiValue", sensitivity);      
}

function setSliderValue(id,vid,val)
{
    $(id).slider( "option", "value", val ); 
    $(vid).text(val);    
}

function openVideo()
{  
    if( CurrentWindows > 0 )
        changeWindow();  
    var player = new pluginPlayer("Dean");
    //player.setOpt("Width=640,Height=480");
    player.setRefSDP();
    player.displayStream();  
    var stream = player.getStream();  
    eval("var res = videoin_c0_s"+stream+"_resolution.split('x')");  
    VideoWidth = parseInt(res[0]);
    VideoHeight = parseInt(res[1]);
    $("#Dean").css("width", VideoWidth +"px").css("height", VideoHeight +"px");     
    setTimeout("setupMotion()", 800);  
}

function setupMotion()
{
    motionstring();
    $("#Dean")[0].openMotionSetup(motionSetting);
    clearAjaxImg();
}

function backupWindow(w_Window)
{
    if(CurrentWindows > 0)
    {
        if( document.getElementById('w_name').value == "")
        {
            alert("Window name can't be empty.");
            return false;
        }
        $(MotionWindow[w_Window]).find("name").text($("#w_name").val());
        $(MotionWindow[w_Window]).find("objsize").text($("#objSizeValue").text());
        $(MotionWindow[w_Window]).find("sensitivity").text($("#sensiValue").text());
    }
    return true;
}

function saveSetting()
{
    showAjaxImg();
    updateWindows(MotionWindow);
    if( backupWindow(workingWindow) )
    {
        sendSetting();
    }
}

function sendSetting()
{
    httpPost("text", MotionXML, "/cgi-bin/admin/videoin.lua", clearAjaxImg, null);    
}

function updateWindows(winNode)
{
    var str = $("#Dean")[0].currMotionWindow;
    var data = str.split(',');
    var count = parseInt(data[0]);
    var w_ratio = VideoWidth/640;//(VideoWidth/2)/320
    var h_ratio = VideoHeight/480;
    var index_m = 1, m_left = 0, m_top = 0, m_width = 0, m_height = 0;
    for(var i=0;i<count;i++)
    {
        var active = (data[index_m] == "1")?"on":"off";
        $(winNode[i]).find("active").text(active);
        index_m++;

        m_left = Math.round(parseInt(data[index_m])/w_ratio);
        $(winNode[i]).find("left").text(m_left.toString());
        index_m++;

        m_width = Math.round(parseInt(data[index_m])/w_ratio) - m_left;
        var diff = m_left + m_width - 320;
        if( diff > 0 )
            m_width = m_width - diff;
        $(winNode[i]).find("width").text(m_width.toString());
        index_m++;

        m_top = Math.round(parseInt(data[index_m])/h_ratio);
        $(winNode[i]).find("top").text(m_top.toString());
        index_m++;

        m_height = Math.round(parseInt(data[index_m])/h_ratio) - m_top;
        diff = m_top + m_height - 240;
        if( diff > 0 )
            m_height = m_height - diff;
        $(winNode[i]).find("height").text(m_height.toString());
        index_m++;
    }
}

function motionstring()
{
    var s = MotionWindow.length.toString() + ",";
    var w_ratio = VideoWidth/640;//(VideoWidth/2)/320
    var h_ratio = VideoHeight/480;
    for(var i=0;i<MotionWindow.length;i++)
    {
        var act = ( $(MotionWindow[i]).find("active").text() == "on" )?"1,":"0,";         
        var w_top = CalculateInt(MotionWindow[i], "top", h_ratio);
        var w_left = CalculateInt(MotionWindow[i], "left", w_ratio);
        var w_right = w_left + CalculateInt(MotionWindow[i], "width", w_ratio);
        var w_bottom = w_top + CalculateInt(MotionWindow[i], "height", h_ratio);
        s = s + act + w_left + "," + w_right + "," + w_top + "," + w_bottom + ",";
    }
    motionSetting = s;
}

function CalculateInt(winNode, varName, ratio)
{
    return Math.round(parseInt($(winNode).find(varName).text())*ratio);
}

function addmotion()
{
    var str = "";
    for(var i=0;i<MotionWindow.length;i++)
    {
        if($(MotionWindow[i]).find("active").text() == "off")
        {
            str = i.toString() + ",";
            str = str + (VideoWidth/4-10).toString() + ","+(VideoWidth/4+10).toString()+ ",";
            str = str + (VideoHeight/4-10).toString() + ","+(VideoHeight/4+10).toString()+ ",";
            $("#w_name").val("Window" + (i+1).toString());
            $(MotionWindow[i]).find("active").text("on");
            $(MotionWindow[i]).find("objsize").text("0");
            $(MotionWindow[i]).find("sensitivity").text("0");
            setSliderValue("#objSize", "#objSizeValue", 0);
            setSliderValue("#sensi", "#sensiValue", 0);  
            workingWindow = i;
            var Dean = document.getElementById('Dean');
            Dean.AddMotionRectangle(str);
            CurrentWindows++;
            break;
        }
    }    
    if( CurrentWindows == Max_Windows )
    {
        $("#new_win").attr("disabled", true);
    }
}

function changeMotionState()
{
    showAjaxImg();
    if ( $("#motion_switch")[0].checked )
    {
        $(MotionNode).find("enable").text("on");
        sendSetting();
        $("#Dean").css("visibility", "visible");
        $("#content").css("display", "block");
        findWindow();
        if( CurrentWindows > 0 )
            changeWindow();   
        $(document).ready(function() {                      
            setTimeout("openVideo()", 200); 
        });     
    }
    else
    {
        $("#Dean")[0].StopVideo();
        $("#Dean").css("visibility", "hidden");
        $("#content").css("display", "none");        
        $(MotionNode).find("enable").text("off");
        sendSetting();
    }
}

function updateWorkingWindow(m_index)
{
    updateWindows(MotionWindow);
    if( m_index > -1 )
    {
        if($(MotionWindow[m_index]).find("active").text() == "off")
        {
            //window deleted, find an actived one.
            CurrentWindows--;
            if(CurrentWindows > 0)
            {            
                for(var i=0;i<MotionWindow.length;i++)
                {
                    if($(MotionWindow[i]).find("active").text() == "on")
                    {
                        workingWindow = i;
                        changeWindow();            
                        break;
                    }
                }
            }
            else
            {
                $("#w_name").val("");
                setSliderValue("#objSize", "#objSizeValue", 0);
                setSliderValue("#sensi", "#sensiValue", 0);                   
            }
            $("#new_win").attr("disabled", false);
        }
        else if(  workingWindow != m_index )
        {                     
            if( backupWindow(workingWindow) )
            {
                workingWindow = m_index;
                changeWindow();
            }        
        }
    }
}
