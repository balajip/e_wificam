var vencXML;
var ispXML;
var SensorNode;

var ispMasterNode;
var ispVideoinNode;
var initial=false;
var mouseKey=0;

/************************************************************************** 
 AE Window
 **************************************************************************/
var vencWidth = 0;  // venc master resolution
var vencHeight = 0; // venc master resolution
var MAX_AE_WIN = 6; // maximun AE window number

/************************************************************************** 
 Exposure Control
 **************************************************************************/
var AutoIris = false;
var GainData = new Array(new Array(), new Array());
var aeSelected=false; 

/************************************************************************** 
    White Balance
 **************************************************************************/
var wbChildNodes;
var gGainEnable;
var wbSelected=false; 

/************************************************************************** 
    Color Correction
 **************************************************************************/
var ccNode;
/************************************************************************** 
    WDR
 **************************************************************************/
var wdrEnable = false;

var asURL = "/cgi-bin/admin/isp_autoscene.lua?";
var autoScene = false;
var PreNR = false;

//-----------------------------------------------------------------------------
$(document).ready(function() {  
    //$("#div_options").css("margin",$("#div_temp").attr("offsetHeight")+"px 0px 0px 0px");
    //$("#div_temp").css("width",$("#div_options").attr("offsetWidth")+"px");  
    showAjaxImg();
    drawGridTable("ae");  
    drawGridTable("wb");
    httpGet("xml", "/cgi-bin/admin/fetch_xml.lua?sensor_dependent.xml",  parseSensorDep,  null);    
           
});

function drawGridTable(id)
{
    var content = "";
    for(var tr=0;tr<15;tr++)
    {
        content = content + "<tr>";
        var preTD = tr*16;
        for(var td=0;td<16;td++)
        {
            var tdID = (preTD + td).toString();
            var tdStr = "<td class=\""+id+"Block\" id=\""+ id + tdID + "\" width=\"15px\" height=\"15px\"" 
                      + " onclick=\"changeTD(this,'"+id+"')\" onmouseover=\"draw(this,'"+id+"')\"></td>";
            content = content + tdStr;
        }
        content = content + "</tr>";
    }
    $("#"+id+"Table").append(content);
}

function parseSensorDep(data, textStatus, XMLHttpRequest)
{
    var sensorType = $(data).find("sensor_type").text();
    SensorNode = $(data).find(sensorType)[0]; 
    if (SensorNode != null) 
    {
        if($(SensorNode).find("autoiris").text() == "off")
            $("#autoIris").css("display","none");
        else
            AutoIris = true;    
        if($(SensorNode).find("wdr").text() == "off")
            $("#wdrArea").css("display","none");
        else
            wdrEnable = true;    
        if($(SensorNode).find("prenr").text() == "off")
            $(".preNR").css("display","none");
   
        $(SensorNode).find("gain option").each(function(i){
            GainData[0][i] = parseInt($(this).text());    // config value
            GainData[1][i] = $(this).attr("text"); // display value       
        });
        //gGainEnable = ($(sensorNode).find("white_balance Green_Gain").text() == "on");
        if($(SensorNode).find("autoscene").text() == "off")
        {
            switchOpt("block","none");
            getManual();
            $("#ispa").css("display","none");             
        }
        else
        {
            autoScene = true;
            httpGet("xml", asURL+"command=status",  getISPMode,  null); 
        }
    }         
}

function getISPMode(data, textStatus, XMLHttpRequest) 
{
    //Check ISP mode
    var ispMode = $(data).find("enable").text();
    $("#asMode").val($(data).find("mode").text());
    if( ispMode == "1" )
    {
        switchOpt("none","block");
        clearAjaxImg();
    }
    else
    {
        switchOpt("block","none");
        getManual();     
    }
}  

function getManual()
{
    if(initial)
        return;
    else
        initial = true;    
    showAjaxImg();      
    //Image Adjustment
    setSlider("bright", -128, 127, 0, "brightValue");
    setSlider("sat", 0, 255, 0, "satValue");
    setSlider("con", -128, 127, 0, "conValue"); 
    //Noise Reduction
    setSlider("preNRStr", 0, 63, 0, "preNRStrValue");
    setSlider("deNoiseStr", 0, 168, 0, "deNoiseStrValue");
    setSlider("CurrWeight", 0, 9, 0, "CurrWeightValue");
    setSlider("deNoiseStr2", 0, 9, 0, "deNoiseStr2Value");
    
    setSlider("deImSen", 0, 255, 0, "deImSenValue");  
    //Edge Enhancement
    setSlider("eeSen", 0, 7, 0, "eeSenValue"); 
    setSlider("enSen", 0, 63, 0, "enSenValue");    
    setSlider("eeStr", 0, 31, 0, "eeStrValue");    
    setSlider("clip", 0, 31, 0, "clipValue"); 
    httpGet("xml", "/cgi-bin/admin/fetch_xml.lua?vencmaster_conf.xml",  parseVencInfo,  null);          
}

function setSlider(id, sMin, sMax, val, displayID)
{
    $("#"+id).slider({
        min: sMin,
        max: sMax,
        values: val,
        slide: function(event, ui) {       
            $("#"+displayID).text(ui.value);
        }    
    });     
}

function parseVencInfo(data, textStatus, XMLHttpRequest)
{
    vencXML = data;
    var tmpResolution = $(vencXML).find("venc_master setting resolution").text().split('x');
    setExposureSlider();
    vencWidth = parseInt(tmpResolution[0]);
    vencHeight = parseInt(tmpResolution[1]);

    /* fill AE window x,y,width and height range */
    $(".ae_x_range").text("(0~"+(vencWidth-1)+")");
    $(".ae_y_range").text("(0~"+(vencHeight-1)+")");

    //! need vencWidth and vencHeight information, so have to call after they are ready.
    httpGet("xml", "/cgi-bin/admin/isp_venc.lua",  parseISPInfo,  null);
}

function setExposureSlider()
{
    setSlider("lumi", 0, 255, 0, "lumiValue");
    setSlider("offset", 0, 255, 0, "offsetValue");      
    $("#gain").slider({
        range: true,
        min: 0,
        max: GainData[0].length-1,
        values: [0, 3],
        slide: function(event, ui) {       
            var low = parseInt(ui.values[0]);
            var high = parseInt(ui.values[1]);
            $("#gainValue").text(GainData[1][low] + " ~ " + GainData[1][high]);
            chkDigiEE(high,low);
        },
        change: function(event, ui) {   
            var high = parseInt(ui.values[1]);
            if( high == 0 )
            {
                alert("Max gain can NOT be 1x");
                $("#gain").slider("values", 1, 1); 
                $("#gainValue").text("1x ~ " + GainData[1][1]);
            }
        }    
    });    
    var tmp = "<div class=\"ui-slider-range\" id=\"digiG\" style=\"background-color:#436eee;\"/>";
    $("#gain").append(tmp);
}

function setExposureGain()
{
    var maxGain = $(ispMasterNode).find("max_gain").text();
    var minGain = $(ispMasterNode).find("min_gain").text();   

    for (var i = 0; i < GainData[0].length; i++) {
        if (maxGain == GainData[0][i])
        {
            $("#gain").slider("values", 1, i); 
        }    
        if (minGain == GainData[0][i]) 
        {
            $("#gain").slider("values", 0, i); 
        }
    } 
    var values = $("#gain").slider( "option", "values" );
    var low = parseInt(values[0]);
    var high = parseInt(values[1]);
    $("#gainValue").text(GainData[1][low] + " ~ " + GainData[1][high]); 
    $("#gainValue").css("width","100px");  
    chkDigiEE(high,low);
}

function chkDigiEE(high,low)
{
    var digiBound = 5;
    if(high > digiBound)
    {                
        var one = Math.round(100/(GainData[0].length-1));                
        var divL = ( low > digiBound )?low*one:digiBound*one;
        var divW = ( low > digiBound )?(high-low)*one:(high-digiBound)*one;
        $("#digiG").css({width:divW+"%",left:divL+"%"});
    }
    else
        $("#digiG").css("width","0%");    
}

function setTableEvent(selector, id)
{
    $(selector).mousedown(function(event) {
        mouseKey = parseInt(event.which);
        if(this.id != id + "Table")
        {
            tdSelected = ($(this).hasClass(id+"Selected"));
            draw(this, id);
        }  
        return false;
    });
    $(selector).mouseup(function(event) {
        mouseKey = 0;
        return false;
    });
    $(selector).bind("contextmenu",function(e){
       return false;
    });     
}

function parseISPInfo(data, textStatus, XMLHttpRequest)
{    
    ispXML = data;
    ispMasterNode = $(ispXML).find("venc_master isp")[0];
    ispVideoinNode = $(ispXML).find("venc_videoin isp")[0];
    var master = new xmlParser($(ispMasterNode));
    var videoin = new xmlParser($(ispVideoinNode)); 

    $("#custom_template").val($(ispXML).find("template").text());

    /************************************************************************** 
        Image Grids Setting
     **************************************************************************/
     
    var $aeGrid = new xmlParser($(ispMasterNode).find("auto_exposure grid_range")); 
    $("#aeWin_x").val( $aeGrid.getV("startX") );  
    $("#aeWin_y").val( $aeGrid.getV("startY") );
    $("#aeWinWidth").val( $aeGrid.getV("width") );
    $("#aeWinHeight").val( $aeGrid.getV("height") ); 
    imgGridChange('x'); 
    imgGridChange('y');

    /************************************************************************** 
        Exposure
     **************************************************************************/
    var aeMode = master.getV("auto_exposure mode"); 
    setRadioValue("ae", aeMode); 
    $(ispMasterNode).find("auto_exposure grid").each(function(i){
        var tdID = $(this).attr("id");
        var tdEnable = ($(this).find("selected").text() == "true")?"#bcd2e6":"";
        $("#ae" + tdID).css("background-color",tdEnable);
        if(tdEnable == "#bcd2e6")
            $("#ae" + tdID).addClass("aeSelected");
    });
    setTableEvent("#aeTable td", "ae");  
    setTableEvent("#aeTable", "ae"); 
    aeModeChange(aeMode);

    $(ispMasterNode).find("auto_exposure grid").each(function(i){
        var tdID = $(this).attr("id");
        var tdEnable = ($(this).find("selected").text() == "true")?"#bcd2e6":"";
        $("#ae" + tdID).css("background-color",tdEnable);
        if(tdEnable == "#bcd2e6")
            $("#ae" + tdID).addClass("aeSelected");
    });
    // Exposure Control
    if(AutoIris)
    {
        var m_iris = master.getV("autoiris");
        setRadioValue("iris", m_iris);
    }    
    setSliderValue("#lumi", "#lumiValue", master.getInt("target_luminance"));
    setSliderValue("#offset", "#offsetValue", master.getInt("target_offset"));  
    $("#shutterMin").val( master.getV("min_shutter") ); 
    $("#shutterMax").val( master.getV("max_shutter") ); 
    setExposureGain();    

    // WDR
    if(wdrEnable)        
    {
        var wdrNode = new xmlParser($(ispMasterNode).find("wdr"));
        var wdrMode = (wdrNode.getV("enable") == "off")?"disable":wdrNode.getV("level");
        $("#wdr").val(wdrMode);
        $("#wdrVal").val(wdrNode.getV("levelvalue"));
        wdrChange();        
    }

    /************************************************************************** 
        White Balance
     **************************************************************************/
    setTableEvent("#wbTable td", "wb");  
    setTableEvent("#wbTable", "wb"); 
    var frameMode = master.getV("white_balance full_frame_grid");
    setRadioValue("wb", frameMode);
    if(frameMode == "true")
        $(".wbT").css("display","none");

    $(ispMasterNode).find("white_balance grid").each(function(i){
        var tdID = $(this).attr("id");
        var tdEnable = ($(this).find("selected").text() == "true")?"#bcd2e6":"";
        $("#wb" + tdID).css("background-color",tdEnable);
        if(tdEnable == "#bcd2e6")
            $("#wb" + tdID).addClass("wbSelected");
    });

    var whiteBalanceMode = master.getV("white_balance select_mode");
    $("#wbMode").val(whiteBalanceMode);
    wbChildNodes = $(ispMasterNode).find("white_balance")[0];
    //if(!gGainEnable)
    //    $("#wbgGain").css("display","none");
    wbModeChange(whiteBalanceMode);

    /************************************************************************** 
        Color Correction
     **************************************************************************/    
    ccNode = new xmlParser($(ispMasterNode).find("color_correction"));
    $(".coeff").each(function(){
        this.value = ccNode.getV(this.id);
    });

    /************************************************************************** 
        Image Adjustment
     **************************************************************************/

    var imgAdjNode =  new xmlParser($(ispMasterNode).find("image_adjustment"));
    setRadioValue("imgAdjMode", imgAdjNode.getV("select_mode"));    

    setSliderValue("#bright", "#brightValue", imgAdjNode.getInt("bright"));
    setSliderValue("#sat", "#satValue", imgAdjNode.getInt("saturation"));
    setSliderValue("#con", "#conValue", imgAdjNode.getInt("contrast"));

    PreNR = ($(ispVideoinNode).find("pre_nr enable").text() == "on"); 

    //Noise Reduction
    loadNoiseReduction();
    
    /************************************************************************** 
        Edge Enhancement
     **************************************************************************/
    var eeNode = new xmlParser($(ispVideoinNode).find("edge_enhancement post"));     
    var edgeSensi = eeNode.getInt("edge_sensitivity");
    var edgeNoise = eeNode.getInt("noise_sensitivity");
    var edgeStr = eeNode.getInt("strength");
    var edgeClip = eeNode.getInt("clip");
    var edgeEn = (eeNode.getV("enable") == "on");
    setSliderValue("#eeSen", "#eeSenValue", edgeSensi);
    setSliderValue("#enSen", "#enSenValue", edgeNoise);
    setSliderValue("#eeStr", "#eeStrValue", edgeStr);
    setSliderValue("#clip", "#clipValue", edgeClip);        
    $("#edgeEnhanceEable")[0].checked = edgeEn;
    edgeEnhanceEnableChange(edgeEn);    

    /************************************************************************** 
        Lens Correction
     **************************************************************************/
    var geoMode = videoin.getV("geometric select_mode");
    $("#geoMode").val(geoMode);
    geoChange(geoMode);

    var photoMode = videoin.getV("photometric mode");
    $("#photoMode").val(photoMode);
    var photoR = videoin.getV("photometric lsc_ratio");
    $("#lsc").val(photoR);
    photoChange(photoMode);

    /************************************************************************** 
        De-interlacing
     **************************************************************************/
    var diNode = new xmlParser($(ispVideoinNode).find("deinterlacer"));
    var diEnable = (diNode.getV("enable") == "on");
    $("#diEnable")[0].checked = diEnable;
    setRadioValue("deMode", diNode.getV("mode"));    
    diEnableChange(diEnable);

    setTimeout("hideBlock()",100);
}

function loadNoiseReduction()
{
    $("#preNREable")[0].checked = PreNR;
    var preStr = $(ispVideoinNode).find("denoise pre normal_mode_strength").text();
    var preStrV = parseInt(preStr) - 105;
    setSliderValue("#preNRStr", "#preNRStrValue", preStrV);  
    preNREnableChange(PreNR);  

    var dnNode = new xmlParser($(ispVideoinNode).find("denoise post"));
    var deNoiseEn = ( dnNode.getV("enable") == "on");        
    var deNoiseMode = dnNode.getV("mode");
    var normal = dnNode.getInt("normal_mode_strength");       
    var weight = dnNode.getInt("strong_curr_weight");
    var strong = dnNode.getInt("strong_ref_strength");
    var adpEn = dnNode.getV("strong_motion_adaptive_enable");
    
    $("#deNoiseEable")[0].checked = deNoiseEn;
    deNoiseEnableChange(deNoiseEn);
    $("#deNoiseMode").val(deNoiseMode);
    deNoiseChange(deNoiseMode);
    setSliderValue("#deNoiseStr", "#deNoiseStrValue", normal);
    setSliderValue("#CurrWeight", "#CurrWeightValue", weight);
    setSliderValue("#deNoiseStr2", "#deNoiseStr2Value", strong);
    $("input[name='motion'][value='"+adpEn+"']")[0].checked = true; 

    //De-impulse
    var dimNode = new xmlParser($(ispMasterNode).find("deimpulse"));
    var deImpulseEn = ( dimNode.getV("enable") == "on");    
    $("#deImpulseEable")[0].checked = deImpulseEn;
    var oriDeIm = dimNode.getInt("dc_bot_threshold"); 
    var deImpulse = Math.round(oriDeIm/257);
    setSliderValue("#deImSen", "#deImSenValue", deImpulse);
    deImpulseChange(deImpulseEn);    
}

function xmlParser(jNode)
{
    var node = jNode;
    this.getV = function(nodeName){
        return node.find(nodeName).text();
    }
    this.getInt = function(nodeName){
        return parseInt(node.find(nodeName).text());
    }
    this.setV = function(nodeName, val){
        node.find(nodeName).text(val);
    } 
    this.getN = function(nodeName){
        return node.find(nodeName);
    }   
}

function hideBlock()
{
    $(".div_drawer").css("display","none");
    clearAjaxImg();
}

function setSliderValue(id,vid,val)
{
    $(id).slider( "option", "value", val ); 
    $(vid).text(val);    
}

function aeModeChange(openAE)
{
    var mode = (parseInt(openAE) == 2)?"inline":"none";
    $("#aeT").css("display", mode);
}

function changeTD(obj, id)
{
    if(tdSelected)
    {
        $(obj).css("background-color","").removeClass(id+"Selected");        
    }    
    else 
    {
        $(obj).css("background-color","#bcd2e6").addClass(id+"Selected");     
    }
}

function imgGridChange(axis)
{
     var val = parseInt($("#aeWin_"+axis).val());
     var upperBound = (axis == "x")?vencWidth:vencHeight;
     var maxSize = (!isNaN(val) && (0 <= val) && (val < upperBound))?upperBound-val:"Error!";
     $("#aeWinMax_"+axis).text(maxSize);        
}

function wdrChange()
{
    var mode = ($("#wdr").val() == "4")?"block":"none";
    $("#wdrVal").css("display", mode);
}

function wbWinChange(mode)
{
    $(".wbT").css("display",mode);
}
 
function draw(obj, id)
{
    switch (mouseKey) {  //2:Middle mouse button
        case 1:
            //Left
            $(obj).css("background-color","#bcd2e6").addClass(id+"Selected");
            break;
        case 3:
            //Right
            $(obj).css("background-color","").removeClass(id+"Selected");
            break;
    }    
}

function displayWBGain(val)
{
    return Math.round((val/1024)*1000)/1000;
}

function wbModeChange(whiteBalanceMode)
{
    var selectedNodes = $(wbChildNodes).find("mode[id='"+whiteBalanceMode+"']")[0];
    var rg = parseInt($(selectedNodes).find("Red_Gain").text());
    var bg = parseInt($(selectedNodes).find("Blue_Gain").text());

    $("#redGain").val(displayWBGain(rg));
    $("#blueGain").val(displayWBGain(bg));

    // Enable when white balance mode is Manual 
    var s_disabled = (whiteBalanceMode == "Manual"); 
    $("#keepWB").attr("disabled", s_disabled );

    var mode = (whiteBalanceMode == "Manual")?"block":"none"; 
    $("#wbGain").css("display", mode);    
}

function preNREnableChange(isChecked) 
{
    $(".preNRStr").prop("disabled", !isChecked);
    $("#preNRStr").slider( "option", "disabled", !isChecked );
    swColor("preNRStr", isChecked);
}

function deNoiseEnableChange(isChecked) 
{
    $(".deNoise").prop("disabled", !isChecked);
    if(isChecked)
        deNoiseChange($("#deNoiseMode").val());
}

function deNoiseChange(mode)
{
    var no = parseInt(mode); 
    var norDisable = ( no == 3 );
    var threeDDisable = ( no < 3 );
    $("#deNoiseStr").slider( "option", "disabled", norDisable );
    $("#CurrWeight").slider( "option", "disabled", threeDDisable );
    $("#deNoiseStr2").slider( "option", "disabled", threeDDisable );
    $("input[name='motion']").attr("disabled", threeDDisable);
    swColor("deNoiseStr", !norDisable);
    swColor("threeD", !threeDDisable);
}

function swColor(area, enable)
{
    var optcolor = (enable)? "" : "#999999";
    $("."+area).css("color",optcolor);    
}

function deImpulseChange(isChecked) 
{
    $("#deImSen").slider( "option", "disabled", !isChecked );
    swColor("deImpulse", isChecked);
}

function edgeEnhanceEnableChange(isChecked) 
{
    $("#eeSen").slider( "option", "disabled", !isChecked ); 
    $("#enSen").slider( "option", "disabled", !isChecked );     
    $("#eeStr").slider( "option", "disabled", !isChecked ); 
    $("#clip").slider( "option", "disabled", !isChecked ); 
    swColor("eeArea", isChecked);
}

function diEnableChange(isChecked) 
{
    $("input[name='deMode']").attr("disabled", !isChecked );
    swColor("deInterlace", isChecked);
}

function geoChange(geoMode) 
{
    if ((geoMode!="disable") && (geoMode!="userdefine")) 
    {
        var isrc = "../pic/Geo_LDC/"+vencWidth+"x"+vencHeight+"_"+geoMode+".jpg"; 
        var geoImg = "<img src=\""+isrc+"\" style=\"width:320px;height:240px\" />";
        $("#geoLdc").html(geoImg);        
    }
    else
        $("#geoLdc").empty();
}

function photoChange(photoMode) 
{
    var btnMode = ((photoMode=="userdefine1")||(photoMode=="userdefine2"))?"inline":"none";
    $("#photoUD").css("display",btnMode);
}

function templateChange(newTemp) 
{
    var url = "/cgi-bin/admin/isp_template.lua?template="+newTemp;
    showAjaxImg();
    httpGet("xml", url,  parseISPInfo,  null);
}

function onASMode(optMode)
{
    if(optMode == "enable")
    {
        //auto
        switchOpt("none","block");
    }
    else
    {
        switchOpt("block","none");
        getManual();       
    }        
}

function switchOpt(maunal,auto)
{
    $("#asOptions").css("display",auto);
    $("#div_options").css("display",maunal);    
}

function verifyAEWinParam()
{
    var passed = true;
    if(checkIntRange("#aeWin_x",0,vencWidth-1))
    {
        var xValue = parseInt($("#aeWin_x").val());
        if(!checkIntRange("#aeWinWidth",1,vencWidth-xValue))
            passed = false;                
    }
    else
        passed = false;
    if(checkIntRange("#aeWin_y",0,vencHeight-1))
    {
        var yValue = parseInt($("#aeWin_y").val());
        if(!checkIntRange("#aeWinHeight",1,vencHeight-yValue))
            passed = false;                
    }
    else
        passed = false;

    return passed;
}

function checkIntRange(id,minV,maxV)
{
    var val = parseInt($(id).val());
    var result = (!isNaN(val) && (minV <= val) && (val <= maxV));
    var objColor = (result)?"":"red";
    $(id).css("color",objColor);
    return result;
}

function updateImgGrid()
{        
    if (!verifyAEWinParam()) {
        alert("Error: Invalid AE Window Value!!!!");
        return false;
    }
    var $aeGrid = new xmlParser($(ispMasterNode).find("auto_exposure grid_range")); 
    $aeGrid.setV( "startX", $("#aeWin_x").val() );
    $aeGrid.setV( "startY", $("#aeWin_y").val() );
    $aeGrid.setV( "width", $("#aeWinWidth").val() );
    $aeGrid.setV( "height", $("#aeWinHeight").val() );  
    return true;
}

function updateExposureCtrl()
{ 
    var aeNode = new xmlParser($(ispMasterNode).find("auto_exposure"));
    var aeMode = getRadioValue("ae");
    aeNode.setV("mode", aeMode);
    $(ispMasterNode).find("auto_exposure grid").each(function(i){
        var tdID = $(this).attr("id");
        var tdEnable = ($("#ae" + tdID).hasClass("aeSelected"))?"true":"false";
        $(this).find("selected").text(tdEnable);
        var weight = (tdEnable == "true")?"1":"0"; 
        $(this).find("weight").text(weight);
    }); 
    if(AutoIris)
        aeNode.setV( "autoiris", getRadioValue("iris") );

    var gains = $("#gain").slider( "option", "values" );
    aeNode.setV( "max_gain", GainData[0][parseInt(gains[1])] );
    aeNode.setV( "min_gain", GainData[0][parseInt(gains[0])] );
    
    aeNode.setV( "target_luminance", $("#lumiValue").text() );
    aeNode.setV( "target_offset", $("#offsetValue").text() );
    var low = parseInt($("#shutterMin").val());
    var high = parseInt($("#shutterMax").val());  
      
    if( !verifyNum("shutterMin", 10000000, 1, 0) || !verifyNum("shutterMax", 10000000, 1, 0) )
        return false;
    if( low < high )    
    {
        alert("Min shutter should small then max shutter.");
        return false;
    }
    aeNode.setV( "max_shutter", high );
    aeNode.setV( "min_shutter", low );
    return true;
}

function updateWhiteBalance()
{
    var wbNode = new xmlParser($(ispMasterNode).find("white_balance"));
    var fullMode =getRadioValue("wb");
    wbNode.setV( "full_frame_grid", fullMode );
    if(fullMode == "false")
    {
        wbNode.getN("grid").each(function(i){
            var tdID = $(this).attr("id");
            var tdEnable = ($("#wb" + tdID).hasClass("wbSelected"))?"true":"false";
            $(this).find("selected").text(tdEnable);
            var weight = (tdEnable == "true")?"1":"0"; 
            $(this).find("weight").text(weight);
        });        
    }       

    var whiteBalanceMode = $("#wbMode").val();    
    wbNode.setV( "select_mode", whiteBalanceMode );
    //! Update value in if manual mode
    if (whiteBalanceMode == "Manual") 
    {
        if( !verifyNum("redGain", 7.999, 0, 3) || !verifyNum("blueGain", 7.999, 0, 3) )
            return false;        
        var rg = wbGainForSave($("#redGain").val());
        var bg = wbGainForSave($("#blueGain").val());
        var manual = wbNode.getN("mode[id='Manual']");
        manual.find("Red_Gain").text(rg);
        manual.find("Blue_Gain").text(bg);
    }
    return true;
}

function wbGainForSave(val)
{
    return  Math.round(parseFloat(val)*1024);
}

function updateColorCorrection()
{
    /*  RR, GG, BB: -4.000 ~ +4.000
         otherwise: -2.000 ~ +2.000
            offset: 0~15
    */
    if ( (!verifyNum("coeff_RR", 4, -4, 3)) ||
         (!verifyNum("coeff_GG", 4, -4, 3)) ||
         (!verifyNum("coeff_BB", 4, -4, 3)) ||
            
         (!verifyNum("coeff_RG", 2, -2, 3)) ||
         (!verifyNum("coeff_RB", 2, -2, 3)) ||
            
         (!verifyNum("coeff_BR", 2, -2, 3)) ||
         (!verifyNum("coeff_BG", 2, -2, 3)) ||
            
         (!verifyNum("coeff_GR", 2, -2, 3)) ||
         (!verifyNum("coeff_GB", 2, -2, 3)) ||

         (!verifyNum("offset_R", 15, 0, 0)) ||
         (!verifyNum("offset_G", 15, 0, 0)) || 
         (!verifyNum("offset_B", 15, 0, 0))

       )   return false;
         
    setNum("coeff_RR", 3);
    setNum("coeff_RG", 3);
    setNum("coeff_RB", 3);
    setNum("coeff_GR", 3);
    setNum("coeff_GG", 3);
    setNum("coeff_GB", 3);
    setNum("coeff_BR", 3);
    setNum("coeff_BG", 3);
    setNum("coeff_BB", 3);
    setNum("offset_R", 0);
    setNum("offset_B", 0);
    setNum("offset_G", 0);
    return true;
}

function verifyNum(id, upBound, lowBound, precision)
{
    var itemValue = parseFloat($("#"+id).val()).toFixed(precision);
    
    if(isNaN(itemValue))
    {
        alert("Please keyin number in " + id);
        return false;
    }
    if(itemValue < lowBound || itemValue > upBound)
    {
        alert("Please keyin number between " + lowBound.toFixed(precision) + " and " + upBound.toFixed(precision) + " in " + id);
        return false;
    }
    return true;
}

function setNum(ElementId, precision)
{
    var rounded = parseFloat($("#"+ElementId).val()).toFixed(precision);
    $("#"+ElementId).val(rounded);
    ccNode.setV(ElementId, rounded);
}

function updateImgAdj()
{
    var imgAdjNode =  new xmlParser($(ispMasterNode).find("image_adjustment"));
    imgAdjNode.setV("select_mode", getRadioValue("imgAdjMode"));
    imgAdjNode.setV("contrast", $("#conValue").text());
    imgAdjNode.setV("bright", $("#brightValue").text());
    imgAdjNode.setV("saturation", $("#satValue").text());

    return true;
}

function  updateDeNoise()
{
    updatePreNR();
    var deNoiseEn = ($("#deNoiseEable")[0].checked)?"on":"off";    
    var dnNode = new xmlParser($(ispVideoinNode).find("denoise post"));
    dnNode.setV("enable", deNoiseEn);
    if( deNoiseEn == "on" )
    {
        var mode = $("#deNoiseMode").val();
        dnNode.setV("mode", mode);
        var modeVal = parseInt(mode);
        if( modeVal != 3 )
        {
            dnNode.setV("normal_mode_strength", $("#deNoiseStrValue").text());        
        }
        if( modeVal > 2 )
        {
            var motion = $("input[name='motion']:checked").val();
            dnNode.setV("strong_motion_adaptive_enable", motion); 
            dnNode.setV("strong_curr_weight", $("#CurrWeightValue").text());           
            dnNode.setV("strong_ref_strength", $("#deNoiseStr2Value").text()); 
        }
    } 

    var deImpulse = ($("#deImpulseEable")[0].checked)?"on":"off";  
    $(ispMasterNode).find("deimpulse enable").text(deImpulse);   
    if( deImpulse == "on" )
    {
        var thrVal = Math.round(parseInt($("#deImSenValue").text())*257);
        $(ispMasterNode).find("deimpulse dc_bot_threshold").text(thrVal);
    }    
    return true;
}

function updatePreNR()
{
    if( $(".preNR").css("display") != "none" )
    {
        PreNR = $("#preNREable")[0].checked; 
        var preNREn = (PreNR)?"on":"off"; 
        $(ispVideoinNode).find("pre_nr enable").text(preNREn);
        var preNode = new xmlParser($(ispVideoinNode).find("denoise pre"));
        preNode.setV("enable", preNREn);
        if(PreNR)
        {    
            preNode.setV("mode", "2");    
            var preNRV = parseInt($("#preNRStrValue").text()) + 105;
            preNode.setV( "normal_mode_strength", preNRV.toString() );
        }
    }
}

function updateEdgeEnhance()
{
    var eeEnable = ($("#edgeEnhanceEable")[0].checked)?"on":"off";   
    var eeNode = new xmlParser($(ispVideoinNode).find("edge_enhancement post"));        
    eeNode.setV("enable", eeEnable);
    if( eeEnable == "on" ) 
    {
        eeNode.setV("edge_sensitivity", $("#eeSenValue").text());
        eeNode.setV("noise_sensitivity", $("#enSenValue").text());
        eeNode.setV("strength", $("#eeStrValue").text());
        eeNode.setV("clip", $("#clipValue").text());
    }
    return true;
}

function updateLensCorrect()
{
    $(ispVideoinNode).find("geometric select_mode").text($("#geoMode").val());
    $(ispVideoinNode).find("photometric mode").text($("#photoMode").val());
    if(!verifyNum("lsc", 100, 0, 0))
        return false;
    $(ispVideoinNode).find("photometric lsc_ratio").text($("#lsc").val());
    return true;
}

function updateDeInterlace()
{
    var diNode = new xmlParser($(ispVideoinNode).find("deinterlacer"));
    var diEnable = ($("#diEnable")[0].checked)?"on":"off";
    diNode.setV("enable", diEnable);
    if ( diEnable == "on" ) 
    {        
        diNode.setV("mode", getRadioValue("deMode"));
    }         
    return true;
}

//OV10630: 1->12dB, 2->30dB, 3->48dB (4)custmized 
function updateWDR()
{
    if(wdrEnable)
    {   
        var selectVal = $("#wdr").val();
        var wdrSW = (selectVal == "disable")?"off":"on";
        var wdrNode = new xmlParser($(ispMasterNode).find("wdr"));
        wdrNode.setV( "enable", wdrSW );
        if( wdrSW == "on" )
        {
            wdrNode.setV( "level", selectVal );
            var lvValue;
            switch(selectVal)
            {
                case "1":
                    lvValue = "12";
                    break;
                case "2":
                    lvValue = "30";
                    break;
                case "3":
                    lvValue = "48";
                    break;
                case "4":
                    lvValue = $("#wdrVal").val();
                    break;   
            }
            wdrNode.setV( "levelvalue", lvValue );
        }        
    }
    return true;
}

function keepWB()
{
    showAjaxImg();       
    $.ajax({
        type: "GET",
        url: "/cgi-bin/admin/keepcurrentwb.lua",
        cache: false, 
        complete: function(XMLHttpRequest){
            clearAjaxImg();
        }, 
        success:  function(data, textStatus, XMLHttpRequest) {
            //change to manual mode
            var wdrNode = new xmlParser($(data).find("root"));
            $("#wbMode").val("Manual");
            wbModeChange("Manual");
            $("#redGain").val(displayWBGain(wdrNode.getInt("RGain")));
            $("#blueGain").val(displayWBGain(wdrNode.getInt("BGain")));
        }
    }); 
}

function saveOption(option)
{
    var res = false;
    showAjaxImg();    
    if (option == "imgGrid")                res = updateImgGrid();
    else if (option == "exposureCtrl")    res = updateExposureCtrl();
    else if (option == "whiteBalance")    res = updateWhiteBalance();
    else if (option == "colorCorrection") res = updateColorCorrection();
    else if (option == "imgAdj")          res = updateImgAdj();
    else if (option == "deNoise")         res = updateDeNoise();
    else if (option == "edgeEnhance")     res = updateEdgeEnhance();
    else if (option == "lensCorrect")     res = updateLensCorrect();            
    else if (option == "deInterlace")     res = updateDeInterlace();
    else if (option == "wdrCtrl")         res = updateWDR();

    if (res) 
        saveSceneNTemplate(); 
    else clearAjaxImg();
}

function saveSceneNTemplate()
{   
    if(autoScene)
    {
        var queryS = asURL + "command=disable";
        httpGet("xml", queryS, saveTemplate, null );
    }
    else
        saveTemplate(); 
}

function saveTemplate()
{
    //isp_template.lua will return html header
    var url = "/cgi-bin/admin/isp_template.lua";       
    httpPost("xml", ispXML, url, applyTemp, null); 
}

function applyTemp()
{
    var url = "/cgi-bin/admin/isp_venc.lua";
    //isp_venc.lua post will get nothing
    httpPost("text/xml", ispXML, url, null, clearAjaxImg);
}

function saveNApply()
{
    showAjaxImg();
    updateAll();
    saveSceneNTemplate(); 
}

function updateAll()
{
    return updateImgGrid() &&
           updateExposureCtrl() &&
           updateWhiteBalance() &&
           updateColorCorrection() &&
           updateImgAdj() &&
           updateDeNoise() &&
           updateEdgeEnhance() &&
           updateLensCorrect() &&
           updateDeInterlace()&&
           updateWDR();
}

function saveISPMode()
{
    showAjaxImg();
    var asmode = $("#asMode").val();
    //Check WDR
    if(asmode == "wdr")
    {        
        if (SensorNode != null) 
        {
            if($(SensorNode).find("wdr").text() == "off")
                alert("Your sensor does not support WDR.");
            else
            {
                //Open WDR first
                httpGet("xml", "/cgi-bin/admin/isp_venc.lua",  openWDR,  null);            
            }
        }
        else
            alert("Get sensor info fail");    
    }
    else
    {    
        var queryS = asURL + "command=enable&mode=" + asmode; 
        httpGet("xml", queryS, null, clearAjaxImg );
    }          
}

function openWDR(data, textStatus, XMLHttpRequest)
{
    ispXML = data;
    var wdrNode = new xmlParser($(ispXML).find("venc_master isp wdr"));
    wdrNode.setV( "enable", "on" );
    var url = "/cgi-bin/admin/isp_template.lua";       
    httpPost("xml", data, url, saveWDR, null);
}

function saveWDR()
{
    var url = "/cgi-bin/admin/isp_venc.lua";
    //isp_venc.lua post will get nothing
    httpPost("text/xml", ispXML, url,  null, changeASWDR);
}

function changeASWDR(data, textStatus, XMLHttpRequest)
{
    var queryS = asURL + "command=enable&mode=wdr"; 
    httpGet("xml", queryS, null, clearAjaxImg );
}

function definePhoto()
{
    showAjaxImg();    
    var tmp = ($("#photoMode").val() == "userdefine1")?"user_defined_1":"user_defined_2";
    httpGet("text", "/cgi-bin/admin/photomtc_deftbl.lua?"+tmp, null, clearAjaxImg);   
}

function selectAll(id)
{
    $("#"+id+"Table td").css("background-color","#bcd2e6").addClass(id+"Selected"); 
}

function center(id)
{
    for(var tr=4;tr<11;tr++)
    {
        var preTD = tr*16;
        for(var td=4;td<12;td++)
        {
            var tdID = (preTD + td).toString();
            $("#"+id+tdID).css("background-color","#bcd2e6").addClass(id+"Selected");
        }
    }
}

function clearAll(id)
{
    $("#"+id+"Table td").css("background-color","").removeClass(id+"Selected");    
}

function treeview(childID) 
{    
    var displayMode = ($("#"+childID).css("display") == "block")?"none":"block";
    $("#"+childID).css("display",displayMode);
    var arrow = ( displayMode == "block" )?"&#9660;":"&#9658;";
    $("span."+childID).html(arrow);
}

function openGridTable()
{
    var command = "width=520, height=500, scrollbars=yes, status=yes";
    var subWindow = window.open("/setup/grid.html", "",command);
    subWindow.focus();    
}
