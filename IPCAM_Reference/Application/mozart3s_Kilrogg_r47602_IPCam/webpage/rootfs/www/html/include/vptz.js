var originWidth,
    originHeight,
    globalWidth,
    globalHeight,
    scale = 0;
var oldPTZString;
var minResizeWidth,minResizeHeight;

$(document).ready(function() {
    if( typeof(videoin_c0_cap_width) == "undefined" )
    {
        alert("Can't get capture size. Please check getparam.cgi.");
        return;
    }

    var tempglobal = videoin_c0_s2_resolution.split('x');
    var tempTarget = videoin_c0_s0_resolution.split('x');
        
    originWidth = parseInt(videoin_c0_cap_width);
    originHeight = parseInt(videoin_c0_cap_height);
    globalWidth = parseInt(tempglobal[0]);
    globalHeight = parseInt(tempglobal[1]);
    var tarW = parseInt(tempTarget[0]);//parseInt(videoin_c0_s0_crop_width);//
    var tarH = parseInt(tempTarget[1]);//parseInt(videoin_c0_s0_crop_height);//
    /*var diagG = Math.sqrt(globalWidth*globalWidth + globalHeight*globalHeight);
    var diagT = Math.sqrt(tarW*tarW + tarH*tarH);
    var diagO = Math.sqrt(originWidth*originWidth + originHeight*originHeight); */
    //scale = (diagT*diagG/diagO)/Math.sqrt(2);  

    var ogXRatio = originWidth/globalWidth;
    var ogYRatio = originHeight/globalHeight;
    var ogRatio = ( ogXRatio > ogYRatio )?ogYRatio:ogXRatio;

    minResizeWidth = Math.round((tarW/ogRatio)/8);//smaller:4
    minResizeHeight = Math.round((tarH/ogRatio)/8);
  
    //minResizeWidth = Math.round(tarW/32);
    //minResizeHeight = Math.round(tarH/32);

    if( navigator.appName == "Microsoft Internet Explorer" )
    { 
        var resolution = videoin_c0_s2_resolution.split('x');
        var wantW = parseInt(resolution[0]) + 50;
        var wantH = parseInt(resolution[1]) + 140;
        $("#Dean").css("visibility", "visible");
        window.resizeTo(wantW,wantH);        

        var player = new pluginPlayer("Dean");
        player.displayRefStream();    
        //left,right,top,bottom
        var maskRW = (globalWidth/2)/originWidth;
        var maskRH = (globalHeight/2)/originHeight;
        var cropLeft = Math.round(parseInt(videoin_c0_s0_crop_left)*maskRW);
        var cropTop = Math.round(parseInt(videoin_c0_s0_crop_top)*maskRH);
        var cropW = Math.round(parseInt(videoin_c0_s0_crop_width)*maskRW);
        var cropH = Math.round(parseInt(videoin_c0_s0_crop_height)*maskRH);
        scale = Math.sqrt(cropW*cropW + cropH*cropH);
        var cropRight = cropLeft + cropW;
        var cropBottom = cropTop + cropH;
        var rectangle = cropLeft + "," + cropRight + "," + cropTop + "," + cropBottom;            
        setTimeout("openPTZ('"+rectangle+"')", 300);    
    }  
});

function openPTZ(rect)
{
    $("#Dean")[0].openPTZ(rect,minResizeWidth,minResizeHeight);
    setLocation();     
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
    var tmp = $("#Dean")[0].getPTZString(scale);
    var ptz = tmp.split(',');
    var postData = "pan=" + ptz[0] + "&tilt=" + ptz[1] + "&zoom=" + ptz[2];
    if( oldPTZString == null || postData != oldPTZString )
        SendCtrl(postData);
    else
        setTimeout("setLocation()", 300);    
    oldPTZString = postData;
}    

function SendCtrl(strA)
{
    $.ajax({
        type: "POST",
        url: "/cgi-bin/vmotor.lua",
        dataType: "html",
        data: strA,
        cache: false, 
        success: function(data, textStatus, XMLHttpRequest) {
            onRequest = 0;
            setTimeout("setLocation()", 100); 
        }    
    });     
}

function updateWorkingWindow(i)
{
    //callback function for plug-in
    //Do not remove this
}