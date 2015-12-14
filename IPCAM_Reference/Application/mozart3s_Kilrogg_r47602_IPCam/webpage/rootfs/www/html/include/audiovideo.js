/* AudioVideo JavaScript */
var VencXhReq = 0;
var sensorXhReq = 0;

var G711 = "<option value=\"64000\" >64 Kbps</option>";

var G726 = "<option value=\"16000\" >16 Kbps</option>" + 
           "<option value=\"24000\" >24 Kbps</option>" +
           "<option value=\"32000\" >32 Kbps</option>" +
           "<option value=\"40000\" >40 Kbps</option>";
//AAC4:8000
var AAC4 = "<option value=\"8000\">8 Kbps</option>" +
           "<option value=\"16000\">16 Kbps</option>" +
           "<option value=\"24000\">24 Kbps</option>" +
           "<option value=\"32000\">32 Kbps</option>";

var GAMR = "<option value=\"4750\">4.75 Kbps</option>" +
           "<option value=\"5150\">5.15 Kbps</option>" +
           "<option value=\"5900\">5.9 Kbps</option>" +
           "<option value=\"6700\">6.7 Kbps</option>" +
           "<option value=\"7400\">7.4 Kbps</option>" +
           "<option value=\"7950\">7.95 Kbps</option>" +
           "<option value=\"10200\">10.2 Kbps</option>" +
           "<option value=\"12200\">12.2 Kbps</option>";

var strTimeStampFormat = " %F %H:%M:%S%z";
var have_submit = false;
var maxFPS=0;
var FetchURL = "/cgi-bin/admin/fetch_xml.lua?";
var NeedUpdatePL=false;
var UpdatePLDone=false;
var SensorType="";

$(document).ready(function() {
    showAjaxImg();
    httpGet("text", "/cgi-bin/viewer/getparam.cgi", init, null);    
});

function init(data, textStatus, XMLHttpRequest)
{
    eval(data);
    httpGet("xml", FetchURL + "vencdisplay_conf.xml", parseVOC, null);
    httpGet("xml", FetchURL + "sensor_dependent.xml", parseSensor, null);
}

function parseVOC(data, textStatus, XMLHttpRequest)
{ 
    var output = "none";
    if( $(data).find("output").text() == "on" )
    {
        switch($(data).find("output_format").text())
        {
            case "NTSC":
                output = "ntsc";
                break;
            case "PAL":
                output = "pal";
                break;
            case "HDMI":
                if( $(data).find("output_resolution").text() == "1280x720" )
                    output = "720p60";
                else
                    output = "1080p" + $(data).find("output_freq").text();
                break;
            case "HDSDI":
                if( $(data).find("output_resolution").text() == "1280x720" )
                    output = "HDSDI_720p" + $(data).find("output_freq").text();
                else
                    output = "HDSDI_1080p" + $(data).find("output_freq").text();
                break;
        }
    }
    $("#voc").val(output);    
}

function parseSensor(data, textStatus, XMLHttpRequest)
{
    sensorXhReq = data;
    SensorType = $(data).find("sensor_type").text();
    maxFPS = parseInt($(sensorXhReq).find("max_framerate").text());
    $(data).find("max_resolution support_resolution").each( function(i){
        var opt = "<option value=\"" + this.getAttribute('number') + "\">" + 
                  this.getAttribute('text') + "</option>";
        $("#max_resolution").append(opt);                
    });
    if(typeof(videoin_c0_streamnum) == "undefined")
    {
        alert("Get stream number failed!");
    }
    else
    {
        var cgiUrl = FetchURL + "vencmaster_conf.xml";
        var streamCount = parseInt(videoin_c0_streamnum);
        for(var i=0;i<streamCount;i++)
        {
            cgiUrl = cgiUrl + "&vencencoder_conf." + i + ".xml";
        }
        cgiUrl = cgiUrl + "&../aenc_conf.xml";
        httpGet("xml", cgiUrl, parseVenc, null);               
    }    
}

function parseVenc(data, textStatus, XMLHttpRequest)
{ 
    VencXhReq = data;
    //Color mode
    var color_mode = $(VencXhReq).find("venc_master color_mode").text();
    $("input[name='color'][value='" + color_mode + "']").attr("checked", true);

    //Power line frequency
    var line_freq = $(VencXhReq).find("venc_master pline_freq").text();
    $("input[name='line_freq'][value='" + line_freq + "']").attr("checked", true);

    //Video orientation
    var flip = ($(VencXhReq).find("venc_master flip_enable").text() == "1");
    $("#flip").attr("checked", flip);

    var mirror = ($(VencXhReq).find("venc_master mirror_enable").text() == "1");
    $("#mirror").attr("checked", mirror);

    //Maximum visible size
    var maxR = $(VencXhReq).find("venc_master resolution").text();
    var maxNum = parseInt( $(sensorXhReq).find("max_resolution support_resolution:contains('" + maxR + "')").attr("number"));
    $("#max_resolution").val(maxNum);
    //Dynamic streams
    var streamCount = parseInt(videoin_c0_streamnum);
    for(var i=0;i<streamCount;i++)
    {
        $("#videoSetting").append($("#sModel").html());
    }

    /**** audio ****/
    if( $(VencXhReq).find("audio_encoder device").size() == 1 )
        $("#audio2").css("display","none");
    $(VencXhReq).find("audio_encoder device").each(function(index) {
        if(SensorType == "nvp1114a")
        {
            //No mic
            $(".mic"+index).css("display","none");
            var mute = ($(this).find("mute").text() == "1");
            $("#mute"+index).attr("checked",mute);   
            var volume = parseInt($(this).find("volume").text()); 
            $("#vol"+index).slider({
                min: 1,
                max: 15,
                values: volume,
                slide: function(event, ui) {
                    $(".volValue"+index).text( ui.value );
                }    
            }); 
            $("#vol"+index).slider( "option", "value", volume );
            $(".volValue"+index).text( volume );
            $("input[name='ain"+index+"'][value='1']")[0].checked = true;                    
        }
        else if(SensorType == "it6604")
        {
            $(".mic"+index).css("display","none");
            $(".mute"+index).css("display","none");
            $(".audioGain"+index).css("display", "none");
            $("input[name='ain"+index+"'][value='1']")[0].checked = true; 
        } 
        else if(SensorType == "tw2866")
        {
            //No mic and mute
            $(".mic"+index).css("display","none");
            $(".mute"+index).css("display","none"); 
            var volume = parseInt($(this).find("volume").text()); 
            $("#vol"+index).slider({
                min: 0,
                max: 15,
                values: volume,
                slide: function(event, ui) {
                    $(".volValue"+index).text( ui.value );
                }    
            }); 
            $("#vol"+index).slider( "option", "value", volume );
            $(".volValue"+index).text( volume );
            $("input[name='ain"+index+"'][value='1']")[0].checked = true;            
        }       
        else
        {
            var mute = ($(this).find("mute").text() == "1");
            $("#mute"+index).attr("checked",mute);
            /************************************************************************** 
                Audio: Input Gain (Normal)
                    Display value: -34.5 ~ 33 dB (step: 1.5)
                    Configure value:   1 ~ 46    (step: 1)
             **************************************************************************/
            $("#vol"+index).slider({
                min: 0,
                max: 45,
                values: 0,
                slide: function(event, ui) {
                    var displayV = (parseInt(ui.value)*1.5 - 34.5).toString() + " dB";
                    $(".volValue"+index).text( displayV );
                }    
            }); 
            /* set Slider value */
            var volume = parseInt($(this).find("volume").text()) -1;
            $("#vol"+index).slider( "option", "value", volume );
            $(".volValue"+index).text( (volume*1.5 - 34.5).toString() + " dB" );
            var selection = $(this).find("input_selection").text();
            $("input[name='ain"+index+"'][value='" + selection + "']")[0].checked = true;
            switchGain( parseInt(selection) , index);
        }     
        
        
        var aCodec = $(this).find("codec type").text();      
        if (aCodec == "G711") 
        {
            aCodec = aCodec+"_"+$(this).find("codec mode").text();
        } 
        $("#audioCodec"+index).val(aCodec);
        audioCodecChange(aCodec,index); 
    });      
    setTimeout("fillVSetting()",100);
}

function optionStr(val, txt, selected)
{
    var opt = "<option value=\"" + val + "\"";
    var backStr = (selected)?" selected=\"selected\">":">";
    opt = opt + backStr + txt + "</option>";
    return opt;
}

function fillVSetting()
{
    //Stream resolution
    streamResolution($("#max_resolution").val()); 
    var streamCount = parseInt(videoin_c0_streamnum);
    for(var i=0;i<streamCount;i++)
    {    
        $(".sTitle:eq("+i+")").append((i+1).toString());
        //Codec
        var temp;
        eval("temp = videoin_c0_s"+i+"_support_codectype.split(\",\");");
        var vencEncNode = $(VencXhReq).find("venc_encoder:eq("+i+")")[0];
        var codec = $(vencEncNode).find("codec").text();              
        var j=0;  
        var content = "";            
        while( temp[j] != null && temp[j] != "" )
        {
            var codecSelect = (codec == temp[j]);
            var displayTxt = (temp[j] == "svc")?"SVC-T":temp[j].toUpperCase();   
            content = content + optionStr(temp[j], displayTxt, codecSelect);        
            j++;
        }
        $(".codec:eq("+i+")").append(content);        
        content = "";        
       
        //Max frame rate
        var line_freq = $(VencXhReq).find("venc_master pline_freq").text();
        var s_MaxFPS = maxFPS;
        if(line_freq == "50")
        {
            if(maxFPS == 60)
                s_MaxFPS = 50;
            else if(maxFPS == 30)
                s_MaxFPS = 25;    
        }
        var selector = $(".frameRate:eq("+i+")")[0];
        for(var j=0;j<selector.length;j++)
        {
            var opt = parseInt(selector.options[j].value);
            if( opt > s_MaxFPS )
            {
                selector.remove(j);
                j--;
            }
        }
        var frameRate = $(vencEncNode).find("frame_rate").text();
        $(".frameRate:eq("+i+")").val(frameRate);
        //Intra frame period & Video quality
        if( codec == "mjpeg" )
        {
            //Video quality (mjpeg)
            $(".qSettings:eq("+i+")").html($("#mjpgQ").html());
            eval("setTimeout(\"setMjpgQuility('"+i+"')\",100);");
        } 
        else
        {
            $(".period:eq("+i+")").append($("#commonP").html());             
            $(".qSettings:eq("+i+")").append($("#commonQ").html());
            if(codec == "mpeg4")
            {
                eval("setTimeout(\"setMpeg4Quility('"+i+"')\",100);");
            }        
            else if(codec == "h264")
            {
                eval("setTimeout(\"setH264Quility('"+i+"')\",100);");
            }
            else 
            {
                $(".qSettings:eq("+i+")").append($("#svct").html());
                eval("setTimeout(\"setSVCQuility('"+i+"')\",100);");
            }
        }                         
        //Text Overlay:Font
        var font = $(vencEncNode).find("text_overlay font").text(); 
        $(".txtFont:eq("+i+")").val(font);
        //Text Overlay:Text Field & Timestampe
        var cusText = $(vencEncNode).find("text_overlay custom_text").text();
        
        var timestampS0 = cusText.indexOf(strTimeStampFormat);
        if (timestampS0 == -1) 
        {
            //No timestampe in custom text
            $(".txtOverlay:eq("+i+") .txtField").val(cusText); 
            $(".txtOverlay:eq("+i+") .t_switch").attr("checked",false);
            $(".timestamp_name:eq("+i+")").css("color","#999999");
        } 
        else 
        {
            $(".txtOverlay:eq("+i+") .txtField").val(cusText.substring(0, timestampS0)); 
            $(".txtOverlay:eq("+i+") .t_switch").attr("checked",true);
            $(".timestamp_name:eq("+i+")").css("color","#000000");
        } 
        $(".txtOverlay:eq("+i+") .t_switch").attr("name",i.toString());
        //Text Overlay:switch
        var txtSwitch = $(vencEncNode).find("text_overlay enable").text();
        if(txtSwitch == "off")
        {
            $(".txtOverlay_switch:eq("+i+")").attr("checked",false);
            $(".txtOverlay:eq("+i+")").css("display","none");
        }
        else
        {
            $(".txtOverlay_switch:eq("+i+")").attr("checked",true);
            $(".txtOverlay:eq("+i+")").css("display","block");
        }        
    }
    display();
    clearAjaxImg();
}

function setMjpgQuility(i)
{    
    var mjegq = $(VencXhReq).find("venc_encoder:eq("+i+") mjpeg_quant_value").text();
    $(".qSettings:eq("+i+") select.quality").val(mjegq);    
}

function setMpeg4Quility(i)
{
    var vencEncNode = $(VencXhReq).find("venc_encoder:eq("+i+")")[0];
    //Intra frame period 
    var period = $(vencEncNode).find("mpeg4_key_interval").text();  
    $(".period:eq("+i+") .periodSelect").val(period);
    //Video quality:Rate Control Mode 
    var r_control = $(vencEncNode).find("mpeg4_quality_type").text();    
    $(".qSettings:eq("+i+") input[name='rate_control']").each(function(){
        $(this).attr("name","rate_control"+i.toString());
    }); 
    $("input[name='rate_control"+i+"'][value='"+r_control +"']").attr("checked",true);
    //Video quality:Bit Rate
    var bitR = $(vencEncNode).find("mpeg4_bitrate").text();
    $(".qSettings:eq("+i+") select.bitRate").val(bitR);
    //Video quality:Quality
    var mpegq = $(vencEncNode).find("mpeg4_quant_value").text();
    $(".qSettings:eq("+i+") .selectQ").val(mpegq);                
    $(".qSettings:eq("+i+") .inputQ").css("display","none");      
}

function setH264Quility(i)
{
    var vencEncNode = $(VencXhReq).find("venc_encoder:eq("+i+")")[0];    
    //Intra frame period (h264)
    var period = $(vencEncNode).find("h264_key_interval").text();    
    $(".period:eq("+i+") .periodSelect").val(period);
    //Video quality:Rate Control Mode 
    var r_control = $(vencEncNode).find("h264_quality_type").text();          
    $(".qSettings:eq("+i+") input[name='rate_control']").each(function(){
        $(this).attr("name","rate_control"+i.toString());
    });     
    $("input[name='rate_control"+i+"'][value='"+r_control +"']").attr("checked",true);
    //Video quality:Bit Rate
    var bitR = $(vencEncNode).find("h264_bitrate").text();
    $(".qSettings:eq("+i+") select.bitRate").val(bitR);
    //Video quality:Quality
    var h264q = $(vencEncNode).find("h264_quant_value").text();
    $(".qSettings:eq("+i+") .inputQ").val(h264q);  
    $(".qSettings:eq("+i+") .selectQ").css("display","none");     
}

function setSVCQuility(i)
{
    var vencEncNode = $(VencXhReq).find("venc_encoder:eq("+i+")")[0];    
    //Intra frame period (h264)
    var period = $(vencEncNode).find("svc_key_interval").text();    
    $(".period:eq("+i+") .periodSelect").val(period);
    //Video quality:Rate Control Mode 
    var r_control = $(vencEncNode).find("svc_quality_type").text();          
    $(".qSettings:eq("+i+") input[name='rate_control']").each(function(){
        $(this).attr("name","rate_control"+i.toString());
    });     
    $("input[name='rate_control"+i+"'][value='"+r_control +"']").attr("checked",true);
    //Video quality:Bit Rate
    var bitR = $(vencEncNode).find("svc_bitrate").text();
    $(".qSettings:eq("+i+") select.bitRate").val(bitR);
    //Video quality:Quality
    var h264q = $(vencEncNode).find("svc_quant_value").text();
    $(".qSettings:eq("+i+") .inputQ").val(h264q);  
    $(".qSettings:eq("+i+") .selectQ").css("display","none"); 
	var selector = $(".qSettings:eq("+i+") .svcLayer")[0];
	var s_max_svc_layer_num = $(vencEncNode).find("svc_max_layer_num").text();
    for(var j=0;j<selector.length;j++)
    {
		var opt = parseInt(selector.options[j].value);
		if( opt > s_max_svc_layer_num )
		{
			selector.remove(j);
			j--;
		}
     }
     var layer = $(vencEncNode).find("svc_layer_num").text();
     $(".qSettings:eq("+i+") .svcLayer").val(layer);     
}

function switchGain( selectedValue, id )
{
    var mode = ( selectedValue == 1 )?"block":"none";
    $(".audioGain" + id).css("display", mode);
}

function changeCodec(obj)
{
    var codec = $(obj).find("option:selected").val();
    var stitle = $(obj).closest("dl").find(".sTitle").text();
    var i = ( parseInt(stitle.charAt(stitle.length-1)) - 1 ).toString();
    if(codec == "mjpeg")
    {
        $(".period:eq("+i+")").empty();
        $(".qSettings:eq("+i+")").html($("#mjpgQ").html());
        eval("setTimeout(\"setMjpgQuility('"+i+"')\",100);");
    }
    else
    {
        $(".period:eq("+i+")").html($("#commonP").html());             
        $(".qSettings:eq("+i+")").html($("#commonQ").html());
        if(codec == "mpeg4")
        {
            eval("setTimeout(\"setMpeg4Quility('"+i+"')\",100);");
        }        
        else if(codec == "h264")
        {
            eval("setTimeout(\"setH264Quility('"+i+"')\",100);");
        }
        else 
        {
            $(".qSettings:eq("+i+")").append($("#svct").html());
            eval("setTimeout(\"setSVCQuility('"+i+"')\",100);");
        }
    }        
}

function switchTextOverlay(obj)
{
    var stitle = $(obj).closest("dl").find(".sTitle").text();
    var i = ( parseInt(stitle.charAt(stitle.length-1)) - 1 ).toString();
    $(".qSettings:eq("+i+") .t_switch").attr("name",i.toString());
    var txtMode = (obj.checked)?"block":"none";
    $(".txtOverlay:eq("+i+")").css("display",txtMode);
}

function switchTimeStamp(obj)
{
    var t_color = (obj.checked)?"#000000":"#999999";
    $(".timestamp_name:eq("+obj.name+")").css("color",t_color);
}

function submitform()
{
    showAjaxImg();
    setTimeout("updateVOC()",50);
    updateMaster();
    updateVenc();
    updateAenc();
    $.ajax({
      type: "POST",
      dataType: "text",
      processData: false,
      data: VencXhReq,
      url: "/cgi-bin/admin/audiovideo.lua",
      success: function(data, textStatus, XMLHttpRequest) {
          clearAjaxImg(); 
          //alert(XMLHttpRequest.responseText);
      },
      complete: function(XMLHttpRequest){
          if (XMLHttpRequest.status != 200) { 
              alert("Sever: " + XMLHttpRequest.statusText); 
          }
      }    
    }); 
}

function getVOCURL(en, mat, res, freq)
{
    var tmpURL = "/cgi-bin/admin/display.lua?output=" + en +
                 "&format=" + mat + "&resolution=" + res +
                 "&freq=" + freq;
    return tmpURL;
}

function updateVOC()
{
    var m_url = "";
    switch($("#voc").val())
    {
        case "none":
            m_url = getVOCURL("off", "0", "0x0", "0");
            break;
        case "ntsc":
            m_url = getVOCURL("on", "NTSC", "720x480", "60");
            break;
        case "pal":
            m_url = getVOCURL("on", "PAL", "720x576", "60");
            break;
        case "720p60":
            m_url = getVOCURL("on", "HDMI", "1280x720", "60"); 
            break;
        case "1080p24":
            m_url = getVOCURL("on", "HDMI", "1920x1080", "24"); 
            break;
        case "1080p30":
            m_url = getVOCURL("on", "HDMI", "1920x1080", "30");  
            break;
        case "1080p60":
            m_url = getVOCURL("on", "HDMI", "1920x1080", "60"); 
            break;
        case "HDSDI_720p30":
            m_url = getVOCURL("on", "HDSDI", "1280x720", "30"); 
            break;
        case "HDSDI_720p60":
            m_url =  getVOCURL("on", "HDSDI", "1280x720", "60"); 
            break;
        case "HDSDI_1080p24":
            m_url =  getVOCURL("on", "HDSDI", "1920x1080", "24"); 
            break;
        case "HDSDI_1080p30":
            m_url = getVOCURL("on", "HDSDI", "1920x1080", "30");
            break;
        case "HDSDI_1080p60":
            m_url = getVOCURL("on", "HDSDI", "1920x1080", "60"); 
            break;
        default:
            break;    
    }
    if(m_url != "")   
        httpGet("xml", m_url, null, null);         
}

function alterSettings(xhReq)
{
    clearAjaxImg();
    have_submit = false;
}

function changeShutter(line,original)
{
    if(line == "50")
    {
        switch(original)
        {
            case "30":
                return "25";
            case "60":
                return "50";
            case "120":
                return "100";
            default:
                return original;
        }
    }
    else
    {
        switch(original)
        {
            case "25":
                return "30";
            case "50":
                return "60";
            case "100":
                return "120";
            default:
                return original;
        }
    }    
}

function updateMaster()
{
    var s_color = $("input[name='color']:checked").val();
    var s_line = $("input[name='line_freq']:checked").val();
    var s_flip = $("#flip")[0].checked? "1" : "0";
    var s_mirror = $("#mirror")[0].checked? "1" : "0";
    var maxResNum = $("#max_resolution").val();
    var maxRes =  $(sensorXhReq).find("max_resolution support_resolution[number='"+maxResNum+"']").text();
    var masterNode = $(VencXhReq).find("venc_master")[0];
    var ori_line = $(masterNode).find("pline_freq").text();
    if( ori_line != s_line )
    {
        //line frequency is changed, need to check shutter  
        var newMinShutter = changeShutter(s_line, $(masterNode).find("min_shutter").text());
        var newMaxShutter = changeShutter(s_line, $(masterNode).find("max_shutter").text());
        $(masterNode).find("min_shutter").text(newMinShutter);
        $(masterNode).find("max_shutter").text(newMaxShutter);
        //Need to update current ISP template   
        $.ajax({
            type: "GET",
            url: "/cgi-bin/admin/isp_venc.lua",
            dataType: "xml",
            cache: false, 
            success: function(data) {                
                var ispvencMaster = $(data).find("venc_master")[0];
                $(ispvencMaster).find("min_shutter").text(newMinShutter);
                $(ispvencMaster).find("max_shutter").text(newMaxShutter);
                //Save to template 
                httpPost("xml", data, "/cgi-bin/admin/isp_template.lua", null, null); 
                //saveShutter(data);                  
            }
        });          
    } 

    $(masterNode).find("resolution").text(maxRes);
    $(masterNode).find("pline_freq").text(s_line);
    $(masterNode).find("flip_enable").text(s_flip);
    $(masterNode).find("mirror_enable").text(s_mirror);
    $(masterNode).find("color_mode").text(s_color);
}

function updateVenc()
{
    var streamCount = parseInt(videoin_c0_streamnum);
    for(var i=0;i<streamCount;i++)
    {        
        var vencEncNode = $(VencXhReq).find("venc_encoder:eq("+i+")")[0];

        var s_mode = $(".codec:eq("+i+")").val();
        $(vencEncNode).find("codec").text(s_mode);
        $(vencEncNode).find("resolution").text($(".frameSize:eq("+i+")").val());
        $(vencEncNode).find("frame_rate").text($(".frameRate:eq("+i+")").val());        
        if( s_mode == "mjpeg" )
        {
            var s_quality = $(".qSettings:eq("+i+") .quality").val();
            $(vencEncNode).find("mjpeg_quant_value").text(s_quality);
            //alert(s_mode+","+s_size+","+s_maxfps+","+s_quality);
        }
        else
        {
            var s_intra = $(".period:eq("+i+") .periodSelect").val();                    
            var s_rateCtrl = $(".qSettings:eq("+i+") input:checked").val();
            //s_rateCtrl: Maybe wrong when more than one group of radio button.
            var s_bitRate = $(".qSettings:eq("+i+") .bitRate").val();
            var s_quality;
            if( s_mode == "mpeg4" )
            {
                s_quality = $(".qSettings:eq("+i+") .selectQ").val();
                $(vencEncNode).find("mpeg4_key_interval").text(s_intra);
                $(vencEncNode).find("mpeg4_quality_type").text(s_rateCtrl);
                $(vencEncNode).find("mpeg4_bitrate").text(s_bitRate);
                $(vencEncNode).find("mpeg4_quant_value").text(s_quality);                
            }    
            else if( s_mode == "h264" )
            {
                s_quality = $(".qSettings:eq("+i+") .inputQ").val();
                $(vencEncNode).find("h264_key_interval").text(s_intra);
                $(vencEncNode).find("h264_quality_type").text(s_rateCtrl);
                $(vencEncNode).find("h264_bitrate").text(s_bitRate);
                $(vencEncNode).find("h264_quant_value").text(s_quality);               
            }    
            else
            {
                s_quality = $(".qSettings:eq("+i+") .inputQ").val();
                $(vencEncNode).find("svc_key_interval").text(s_intra);
                $(vencEncNode).find("svc_quality_type").text(s_rateCtrl);
                $(vencEncNode).find("svc_bitrate").text(s_bitRate);
                $(vencEncNode).find("svc_quant_value").text(s_quality);
                var s_layer = $(".qSettings:eq("+i+") .svcLayer").val();
                $(vencEncNode).find("svc_layer_num").text(s_layer);                  
            }   
        }
        var s_textSwitch = $(".txtOverlay_switch:eq("+i+")")[0].checked? "on" : "off"; 
        $(vencEncNode).find("text_overlay enable").text(s_textSwitch);
         //alert(s_textSwitch);
        if( s_textSwitch == "on" )
        {
            var s_font = $(".txtFont:eq("+i+")").val();
            var s_cusText = $(".txtField:eq("+i+")").val();
            if($(".t_switch:eq("+i+")")[0].checked)
                s_cusText = s_cusText + strTimeStampFormat;
            $(vencEncNode).find("text_overlay font").text(s_font);
            $(vencEncNode).find("text_overlay custom_text").text(s_cusText);    
             //alert(s_font+","+s_cusText);
        }
    }    
}

function updateAenc()
{
    $(VencXhReq).find("audio_encoder device").each(function(index) {
        var aInput = $("input[name='ain"+index+"']:checked").val();
        $(this).find("input_selection").text(aInput); 
 
        if(SensorType != "it6604")
        {
            if(SensorType != "tw2866")
            {
                var aMute = $("#mute"+index)[0].checked? "1" : "0";
                $(this).find("mute").text(aMute);
            }            
            var aVol = parseInt($("#vol"+index).slider("option", "value"));
            if(SensorType != "nvp1114a" && SensorType != "tw2866") 
                aVol = aVol + 1;
            $(this).find("volume").text(aVol);            
        }   
    
        var aCodecMode = $("#audioCodec"+index).val();
        var aCodecModeSp = aCodecMode.split('_');        
        $(this).find("codec type").text(aCodecModeSp[0]);
        if (aCodecModeSp.length > 1)
            $(this).find("codec mode").text(aCodecModeSp[1]);
        else
            $(this).find("codec mode").text("N.A.");
            
        var aBRate = $("#audioBitrate"+index).val();
        $(this).find("codec bitrate").text(aBRate);
        $(this).find("codec sample_rate").text("8000");    
    });    
}

function changeFPS()
{        
    if($("input[name='line_freq']:checked").val() == "50")
    {
        //Remove 30 fps or 60 fps
        var streamCount = parseInt(videoin_c0_streamnum);
        for(var i=0;i<streamCount;i++)
        {
            var s_fps = $(".frameRate:eq("+i+")").val();
            if( maxFPS == 60 ) 
            {           
                $(".frameRate:eq("+i+") option[value='60']").remove();
                if( s_fps == 60 )
                    $(".frameRate:eq("+i+")").val("50");
            }    
            else if( maxFPS == 30 )  
            {
                $(".frameRate:eq("+i+") option[value='30']").remove();
                if( s_fps == 30 )
                    $(".frameRate:eq("+i+")").val("25");                    
            }    
        }    
    }
    else
    {
        //Add 30 fps or 60 fps
        var streamCount = parseInt(videoin_c0_streamnum);
        for(var i=0;i<streamCount;i++)
        {
            if(!$(".frameRate:eq("+i+")").children().is("option[value='"+maxFPS+"']"))
            {
                var opt = "<option value=\"" + maxFPS + "\">" + maxFPS + "</option>";
                $(".frameRate:eq("+i+")").append(opt); 
            }    
        }         
    }
}

function streamResolution(resNum)
{
    var content="";   
    var maxResNum = parseInt(resNum);
    $(".frameSize").each( function(i)
    {
        $(this).empty();
        var vencEncNode = $(VencXhReq).find("venc_encoder:eq("+i+")")[0]; 
        var resolution = $(vencEncNode).find("resolution").text();
        var resolutionNum = parseInt($(sensorXhReq).find("stream_resolution support_resolution:contains('" + resolution + "')").attr("number"));   
        if(resolutionNum > maxResNum)     
            resolutionNum = maxResNum;                          
        var supReCount = $(sensorXhReq).find("stream_resolution support_resolution").length; 
        var findRes = false;
        for(var j=0;j<supReCount;j++)
        {      
            var supResolution = $(sensorXhReq).find("stream_resolution support_resolution:eq("+j+")");   
            var optNum = parseInt(supResolution.attr("number"));
            
            if(optNum == resolutionNum)
            {
                content = content + optionStr(supResolution.text(), supResolution.attr("text"), true);
                findRes = true;                                          
            }
            else if(optNum <= resNum)
            {
                content = content + optionStr(supResolution.text(), supResolution.attr("text"), false);                                            
            }            
        }
        if(!findRes)
        {
            content = content + optionStr(resolution, resolution, true);
        }
        $(this).append(content);
        content=""; 
    });
}

function openPrivacy()
{
    var command = "width=800, height=600, scrollbars=yes, status=yes";
    var subWindow = window.open("/setup/privacy.html", "",command);
    subWindow.focus();
}

function rebootSystem()
{
    have_submit = true;
    //! set clear the AjaxImg in 20 seconds
    setTimeout("clearAjaxImg()", 20000);
    httpGet("text/xml", "/cgi-bin/admin/reboot.lua", null, null);
    have_submit = false;
}


/* ============ */
/*   PC Timer   */
/* ============ */
var timerID = null;
var timerRunning = false;
function display(){
  var PCdate = new Date();
  var CurrentTime = "";
  var CurrentDate = "";
  var TimeZone = "";

  if (PCdate.getHours() < 10) {
    CurrentTime = CurrentTime + "0";
  }
  CurrentTime = CurrentTime + PCdate.getHours() + ":";

  if (PCdate.getMinutes() < 10) {
    CurrentTime = CurrentTime + "0";
  }
  CurrentTime = CurrentTime + PCdate.getMinutes() + ":";

  if (PCdate.getSeconds() < 10) {
    CurrentTime = CurrentTime + "0";
  }
  CurrentTime = CurrentTime + PCdate.getSeconds();

  CurrentDate = CurrentDate + (PCdate.getFullYear()) + "-";

  if ((PCdate.getMonth()+1) < 10) {
    CurrentDate = CurrentDate + "0";
  }
  CurrentDate = CurrentDate + (PCdate.getMonth()+1) + "-";

  if (PCdate.getDate() < 10) {
    CurrentDate = CurrentDate + "0";
  }
  CurrentDate = CurrentDate + PCdate.getDate();

  var vTimeZone = ((PCdate.getTimezoneOffset() / 60) * 100);
  TimeZone = (vTimeZone > 0)? "-" : "+";
  TimeZone = TimeZone + padLeft(Math.abs(vTimeZone), 4, '0');
  $(".timestamp").val(CurrentDate + " "+CurrentTime + TimeZone);

  timerID = setTimeout("display()",1000);
  timerRunning = true;
}

function stoptimer() 
{
    if(timerRunning) {
        clearTimeout(timerID);
    }
    timerRunning = false;
}

function starttimer() 
{
    stoptimer();
    display();
}

function audioCodecChange(aCodec,ch)
{
    var no = parseInt(ch);
    if( aCodec == "G711_alaw" || aCodec == "G711_ulaw" )
        $("#audioBitrate"+ch).html(G711);
    else if(aCodec == "G726")
        $("#audioBitrate"+ch).html(G726);
    else if(aCodec == "AAC4")
        $("#audioBitrate"+ch).html(AAC4);
    else if(aCodec == "GAMR")
        $("#audioBitrate"+ch).html(GAMR);

    var aMode  = $(VencXhReq).find("audio_encoder codec mode").eq(no).text();
    var oriCodec = $(VencXhReq).find("audio_encoder codec type").eq(no).text();      
    if ((aMode != "N.A.") && (aMode.length != 0)) 
    {
        oriCodec = oriCodec+"_"+aMode
    } 
    if( oriCodec == aCodec )
        setTimeout("setAudioBitrate('"+ch+"')",100);
}

function setAudioBitrate(ch)
{
    var no = parseInt(ch);
    var aBRate = $(VencXhReq).find("audio_encoder codec bitrate").eq(no).text();
    $("#audioBitrate"+ch).val(aBRate);
}


