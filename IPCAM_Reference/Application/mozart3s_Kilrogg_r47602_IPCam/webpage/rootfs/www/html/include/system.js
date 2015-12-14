var SysXhReq = 0;
var xmlData;
normal = "normal";
triggered = "triggered";

$(document).ready(function() {
    $("#manualPanel").css("display","none");
    $.ajax({
        type: "GET",
        url: "/cgi-bin/admin/system.lua",
        dataType: "xml",
        cache: false, 
        success: function(data, textStatus, XMLHttpRequest) {                        
            xmlData = data;
            //alert(XMLHttpRequest.responseText);
            parseSysInfo();
        }    
    });     
    starttimer();
});

function parseSysInfo()
{
    $("#sysName").val($(xmlData).find("host").text());
    $("#timezone").val($(xmlData).find("timezone").text());
    /* get ledoff */ //Hide Turn On/Off LED Indicator by jesi 2009/01/09

    /* set method */  //keep,manual,ntp 
    var mode = $(xmlData).find("time_mode").text(); 
    if(mode == "manual")
        mode = "keep";
    $("input[name='method'][value='"+mode+"']").attr("checked" , true);

    var systime = $(xmlData).find("set_time").text();
    var sysmonth = systime.substr(0,2);
    var sysday = systime.substr(2,2);
    var syshours = systime.substr(4,2);
    var sysmin = systime.substr(6,2);
    var sysyear = systime.substr(8,4);
    var syssec = systime.substr(13,2);
    $("#assignDate").val(sysyear + "/" + sysmonth + "/" + sysday);
    $("#assignTime").val(syshours + ":" + sysmin + ":" + syssec);

    /* get ntp server name*/
    $("#ntpSever").val($(xmlData).find("ntp_server").text());
    
    /* get ntp update interval */
    $("#updateInterval").val($(xmlData).find("update_interval").text());

    loadlanguage();
    $("#content").css("visibility","visible");
}

function formatDateTime(digital)
{
    if (digital.length < 2)
    {
        digital = "0" + digital;
    }
    return digital;
}

function openPanel()
{
    $("#manualPanel").css("display","block");
    $("input[name='method'][value='pc']")[0].checked = true;
}

function submitform()
{
    $(xmlData).find("host").text($("#sysName").val());
    /* write back led status */ //Hide Turn On/Off LED Indicator by jesi 2009/01/09
    $(xmlData).find("timezone").text($("#timezone").val());
    var timeMode = $("input[name='method']:checked").val();
    if( timeMode == "pc" || timeMode == "assign" )
    { 
        var pcdate = $("#"+timeMode+"Date").val().split("/");
        var pctime = $("#"+timeMode+"Time").val().split(":");
        var timeStr = pcdate[1] + pcdate[2] + pctime[0] + pctime[1] + pcdate[0] + "." + pctime[2];
        $(xmlData).find("set_time").text(timeStr);
        timeMode = "manual";
    }
    $(xmlData).find("time_mode").text(timeMode);    
    $(xmlData).find("ntp_server").text($("#ntpSever").val());
    $(xmlData).find("update_interval").text($("#updateInterval").val());

    showAjaxImg();
    $.ajax({
        type: "POST",
        url: "/cgi-bin/admin/system.lua",
        dataType: "xml",
        processData: false,
        data: xmlData,
        cache: false, 
        complete: function(XMLHttpRequest){
            setTimeout("clearAjaxImg()", 100);
        },
        success:  function(data, textStatus, XMLHttpRequest) {
            $("#manualPanel").css("display","none");
            $("input[name='method'][value='keep']")[0].checked = true;  
        } 
    });  

    return;
}
/* ============ */
/*   PC Timer   */
/* ============ */
var timerID = null
var timerRunning = false
function display()
{
  var PCdate = new Date();
  var CurrentTime = keepTwoDigi(PCdate.getHours()) + ":" + keepTwoDigi(PCdate.getMinutes()) 
                    + ":" + keepTwoDigi(PCdate.getSeconds());
  var CurrentDate = PCdate.getFullYear() + "/" + keepTwoDigi(PCdate.getMonth()+1) 
                    + "/" + keepTwoDigi(PCdate.getDate());

  $("#pcTime").val(CurrentTime);
  $("#pcDate").val(CurrentDate);

  timerID = setTimeout("display()",1000);
  timerRunning = true;
}

function keepTwoDigi(val)
{
    if (parseInt(val) < 10) 
        return "0"+val;
    else
        return val;                              
}

function stoptimer() {
  if(timerRunning) {
    clearTimeout(timerID);
  }
  timerRunning = false;
}

function starttimer() {
  stoptimer();
  display();
}
