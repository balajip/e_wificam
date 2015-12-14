var evtXhReq = 0;
var szConfigEvt = "/cgi-bin/admin/event_action.lua";
var xmlEvt = 0;


$(document).ready(function() {
    showAjaxImg();    
    initWidget();
    httpGet("xml", szConfigEvt, readEvt, clearAjaxImg);
});

function initWidget()
{
    // Set items for hour selection 
    setTimeOpt("scheHourS", 0, 24);
    setTimeOpt("scheHourE", 0, 24);

    // Set items for minute selection 
    setTimeOpt("scheMinS", 0, 60);
    setTimeOpt("scheMinE", 0, 60);

    //force to refresh the select in order to avoid the select widget from covering the text
    $("select .time").css("width", "45px");

    //TODO: 
    $(".cTODO").css("color", "#999999").attr("disabled", true); 
}

function setTimeOpt(id, first, last)
{
    var $obj = $("#"+id);
    for (var i = first; i<last; i++) {
        var opt = padLeft(i.toString(), 2, '0');
        $obj.append("<option value=\"" + i + "\">" + opt + "</option>"); 
    }    
}

function readEvt(data, textStatus, XMLHttpRequest)
{
    xmlEvt = data;

    //Get the schedule type
    var scheType = ($(data).find("enabled").text() == "true")?"always":"disable";
    if( scheType == "always")
        scheType = ($(data).find("schedule_enabled").text() == "true")?"schedule":scheType;
    setRadioValue("sche", scheType);
    scheChange();

    //Get Day
    var aDay = $(data).find("day").text().split(',');
    for (var i = 0; i < aDay.length; i++) {
        var iDay = parseInt(aDay[i]);
        if ((iDay < 7 ) && (iDay >= 0))
            $("#chkDay" + iDay).attr("checked", true);
    }

    //Get Time
    var aTimeS = $(data).find("start_time").text().split(':');
    var aTimeE = $(data).find("end_time").text().split(':');

    $("#scheHourS").val(parseInt(aTimeS[0]).toString());
    $("#scheMinS"). val(parseInt(aTimeS[1]).toString());
    $("#scheHourE").val(parseInt(aTimeE[0]).toString());
    $("#scheMinE"). val(parseInt(aTimeE[1]).toString());
  
    //Action
    $("#alarmInterval").val($(data).find("alarm_interval").text());
    $("#sendFtp").  attr("checked", ($(data).find("FTP").text() == "on"));
    $("#sendEmail").attr("checked", ($(data).find("Email").text() == "on"));    
}

function scheChange()
{
    var szSche = getRadioValue("sche");
    var bAct  = (szSche != "disable");
    var bSche = (szSche == "schedule");
    var objColor = "";

    objColor = (bSche)? "" : "#999999";
    $(".cSche").css("color", objColor).attr("disabled", !(bSche));
 
    objColor = (bAct)? "" : "#999999";
    $(".cAct").css("color", objColor).attr("disabled", !(bAct));

    //TODO: 
    $(".cTODO").css("color", "#999999").attr("disabled", true);
}

function updateEvt()
{
    var scheType = getRadioValue("sche");

    //Schedule type
    if (scheType == "disable")
    { 
        $(xmlEvt).find("enabled").text("false");
        return true;
    }
    else
    {
        $(xmlEvt).find("enabled").text("true");
        var sche = (scheType == "schedule")?"true":"false";
        $(xmlEvt).find("schedule_enabled").text(sche);
    }

    // Update schedule day
    var szDay = "";
    for (var i = 0; i < 7; i++) {
        if ($("#chkDay" + i)[0].checked)   
            szDay += ("," + i);
    }
    if (szDay.charAt(0) == ",") 
        szDay = szDay.substr(1, szDay.length-1);
    $(xmlEvt).find("day").text(szDay);
    
    //Update schedule time
    var hourS = $("#scheHourS").val();
    var hourE = $("#scheHourE").val();
    var minS  = $("#scheMinS").val(); 
    var minE  = $("#scheMinE").val(); 

    if ((parseInt(hourS) > parseInt(hourE)) ||
        ((parseInt(hourS) == parseInt(hourE)) && (parseInt(minS) > parseInt(minE)))){
        alert("End time must be larger than start time.");
        return false;
    }
    else 
    {
        $(xmlEvt).find("start_time").text(hourS + ":" + minS);
        $(xmlEvt).find("end_time").  text(hourE + ":" + minE);
    }

    //Update action
    $(xmlEvt).find("alarm_interval").text($("#alarmInterval").val());
    $(xmlEvt).find("FTP").  text(($("#sendFtp")[0].checked)? "on" : "off");
    $(xmlEvt).find("Email").text(($("#sendEmail")[0].checked)? "on" : "off");
    return true;    
}

function submitEvt()
{
    showAjaxImg();
    if(updateEvt())
        httpPost("xml", xmlEvt, szConfigEvt, null, clearAjaxImg);
    else 
        clearAjaxImg();   
}
