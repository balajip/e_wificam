var szConfigSmtp = "/cgi-bin/admin/smtp_setting.lua";
var szConfigFtp  = "/cgi-bin/admin/ftp_setting.lua";

var bSmtpReqDone = false;
var bFtpReqDone = false;

var xmlFtp  = 0;
var xmlSmtp = 0;


$(document).ready(function() {
    /* Show the processing mask */
    showAjaxImg();
    bSmtpReqDone = false;
    bFtpReqDone  = false;
    
    /* Get configures */
    httpGet("xml", szConfigFtp,  readFtp,  null);
    httpGet("xml", szConfigSmtp, readSmtp, null);
});

function readFtp(data, textStatus, XMLHttpRequest)
{
    xmlFtp = data;

    $("#ftpHost").val($(data).find("HOST").text());
    $("#ftpPort").val($(data).find("PORT").text());
    $("#ftpUserName").val($(data).find("USER_NAME").text());
    $("#ftpPW").val($(data).find("PASSWD").text());
    $("#ftpRemoteFloder").val($(data).find("REMOTE_FOLDER").text());

    setRadioValue("ftpMode", $(data).find("FTP_MODE").text());

    /* check if ftp and smtp are ready, then remove the processing mask */
    bFtpReqDone = true;    
    if (bSmtpReqDone && bFtpReqDone) clearAjaxImg();    
}


function readSmtp(data, textStatus, XMLHttpRequest)
{
    smtpXhReq = XMLHttpRequest;
    xmlSmtp   = data;

    $("#mailUser").val($(data).find("USER").text());
    $("#mailPw").val($(data).find("PASSWORD").text());    
    $("#mailSendAddr").val($(data).find("from").text());
    $("#mailServAddr").val($(data).find("SERVER").text());
    $("#mailRcvAddr").val($(data).find("rcpt").text());
    $("#mailCCAddr").val($(data).find("CC").text());
    $("#mailSubject").val($(data).find("SUBJECT").text());
    $("#mailContent").val($(data).find("CONTENT").text());

    /* check account mode */
    if ($("#mailUser").val() == "") {
        $("#mailAuthMode").val(0);
        $("#mailPw").val("");
    } else {
        $("#mailAuthMode").val(1);      
    }
    authModeChange();

    /* check if ftp and smtp are ready, then remove the processing mask */
    bSmtpReqDone = true;    
    if (bSmtpReqDone && bFtpReqDone) clearAjaxImg(); 
}



function authModeChange()
{
    var mode = $("#mailAuthMode").val();
    
    if (parseInt(mode) == 0) {
        
        $("#mailUser").attr("disabled", true).css("color","#999999");
        $("#mailPw").attr("disabled", true).css("color","#999999");

        $("#mailUserTr").css("color","#999999");
        $("#mailPwTr").css("color","#999999");
        
    } else {
    
        $("#mailUser").removeAttr("disabled").css("color","");
        $("#mailPw").removeAttr("disabled").css("color","");
    
        $("#mailUserTr").css("color","");
        $("#mailPwTr").css("color","");
    }
    
}

function updateFtp()
{
    $(xmlFtp).find("HOST").text($("#ftpHost").val());   
    $(xmlFtp).find("USER_NAME").text($("#ftpUserName").val());
    $(xmlFtp).find("PASSWD").text($("#ftpPW").val());
    $(xmlFtp).find("REMOTE_FOLDER").text($("#ftpRemoteFloder").val());
    $(xmlFtp).find("PORT").text($("#ftpPort").val());

    var ftpMode = getRadioValue("ftpMode");
    $(xmlFtp).find("FTP_MODE").text(ftpMode);
}

function submitFtp(test)
{
    showAjaxImg();  /* show processing icon and mask */    
    updateFtp();
    if(test == "on")    
        httpPost("xml", xmlFtp, szConfigFtp, reqFtpThenTest, clearAjaxImgSlowly);  
    else
        httpPost("xml", xmlFtp, szConfigFtp, null, clearAjaxImg);  
}

function reqFtpThenTest(data, textStatus, XMLHttpRequest)
{
    alert("FTP test");
    httpPost("xml", data, "/cgi-bin/admin/ftp_test.lua", reqFtpTestReturn, clearAjaxImgSlowly);   
}


function reqFtpTestReturn(data, textStatus, XMLHttpRequest)
{
    clearAjaxImg();
    alert($(data).find("ftp_return").text());   
}

function updateSmtp()
{   
    /* check account mode */
    if ($("#mailAuthMode").val() == "0") {
        $("#mailUser").val("");
        $("#mailPw").val("");
    }

    /* update xml content */
    $(xmlSmtp).find("from").    text($("#mailSendAddr").val());
    $(xmlSmtp).find("SERVER").  text($("#mailServAddr").val());
    $(xmlSmtp).find("USER").    text($("#mailUser").val());
    $(xmlSmtp).find("PASSWORD").text($("#mailPw").val());
    $(xmlSmtp).find("rcpt").    text($("#mailRcvAddr").val());
    $(xmlSmtp).find("CC").      text($("#mailCCAddr").val());
    $(xmlSmtp).find("SUBJECT").text($("#mailSubject").val());
    $(xmlSmtp).find("CONTENT").text($("#mailContent").val());

}

function submitSmtp(test)
{
    showAjaxImg();  /* show processing icon and mask */
    updateSmtp();
    if(test == "on")
        httpPost("xml", xmlSmtp, szConfigSmtp, reqSmtpThenTest, clearAjaxImgSlowly);
    else    
        httpPost("xml", xmlSmtp, szConfigSmtp, null, clearAjaxImg);
}

function reqSmtpThenTest(data, textStatus, XMLHttpRequest)
{
    alert("SMTP test");
    httpPost("xml", data, "/cgi-bin/admin/smtp_test.lua", reqSmtpTestReturn, clearAjaxImgSlowly);
}

function reqSmtpTestReturn(data, textStatus, XMLHttpRequest)
{
    clearAjaxImg();
    alert($(data).find("smtp_return").text());
}