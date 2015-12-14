/* ibpeset java script */

var bGetPolicyModel = false;

var szConfigPolicy = "/cgi-bin/admin/SslPolicy.lua";
var xmlPolicy = 0;

var szConfigSslInfo = "/cgi-bin/admin/SslInfo.lua";
var xmlSslInfo = 0;

var szSslGenerateCert = "/cgi-bin/admin/SslGenerateCert.lua";
var xmlSelfCert = 0;

var szSslGenerateReq  = "/cgi-bin/admin/SslGenerateReq.lua";
var xmlCert = 0;

$(document).ready(function (){
    showAjaxImg();
    $(".cert").attr("disabled", true);
    $(".noCert").attr("disabled", true);
    /* Get configures */
    httpGet("xml", szConfigPolicy,    readPolicy,          null);    
    httpGet("xml", szSslGenerateCert, getSelfCertTreeDone, null);
    httpGet("xml", szSslGenerateReq,  getCertTreeDone,     null);
});

function readPolicy(data, textStatus, XMLHttpRequest)
{
    xmlPolicy = data;  

    var bSwitch = ($(data).find("switch").text() == "1");
    $("#https_enable_checkbox")[0].checked = bSwitch;
    
    if (bSwitch)
        $("#spanRemove").css("display", "block");

   httpGet("xml", szConfigSslInfo, readSslInfo, null);
}   

function readSslInfo(data, textStatus, XMLHttpRequest)
{
    xmlSslInfo = data;

    var iAct = parseInt($(data).find("sslinfo active").text());
    if (iAct != 1)
    {       
        if (iAct == 2)  
            $("#certState").html("Request has been created but NOT installed.");
        else            
            $("#certState").html("No Certificate Installed.");

        $("#trSubject").css("display", "none");

        $(".cert").attr("disabled", true);
        $(".noCert").attr("disabled", false);        
    }
    else
    {
        $("#certState").html("Active");
        $("#trSubject").css("display", "");
        $(".cert").attr("disabled", false);
        $(".noCert").attr("disabled", true);             

        //Show certificate info
        $("#certSubject").html($(data).find("certificate issuer").text());
        $("#certVer").html($(data).find("certificate version").text());
        $("#certSN").html($(data).find("certificate serial").text());
        $("#certSigAlgo").html($(data).find("certificate salgo").text());
        $("#certIssuer").html($(data).find("certificate issuer").text());
        $("#certNValidBefore").html($(data).find("validity before").text());
        $("#certNValidAfter").html($(data).find("validity after").text());
        $("#certValidSubject").html($(data).find("validity subject").text());
        $("#certPubKey").html($(data).find("spkey spkalgo").text());

        replaceBR($("#certModules").get(0), $(data).find("spkey modulus").text());
        $("#certExp").html($(data).find("spkey exponent").text());
        $("#certAlgo").html($(data).find("signature algo").text());
        replaceBR($("#certSig").get(0), $(data).find("signature sign").text());
        replaceBR($("#certPEM").get(0), $(data).find("pem text").text());
    }
    clearAjaxImg();
}


function getSelfCertTreeDone(data, textStatus, XMLHttpRequest)
{
    xmlSelfCert = data;
}

function getCertTreeDone(data, textStatus, XMLHttpRequest)
{
    xmlCert = data;
}

function reloadSslInfo()
{  
    httpGet("xml", szConfigSslInfo, readSslInfo, clearAjaxImgSlowly);
}

function removeCert()
{
    showAjaxImg();
    httpPost("text", xmlSslInfo, szConfigSslInfo, reloadSslInfo, null);
}

function applyPolicy()
{
    showAjaxImg();

    var szEnable = ($("#https_enable_checkbox")[0].checked)?"1":"0";
    $(xmlPolicy).find("sslpolicy switch").text(szEnable);
    
    httpPost("xml", xmlPolicy, szConfigPolicy, policyReturn, clearAjaxImg);
}

function policyReturn()
{
    clearAjaxImg();

    var expires = new Date();       
    expires.setTime (expires.getTime() + (10 * 365 * 24 * 60 * 60 * 1000));
    if ($("#https_enable_checkbox")[0].checked) {
        $("#btnRemove").attr("disabled", true).css("color", "#aaaaaa");
        $("#spanRemove").css("display", "block");
        setCookie("protocol", 3, expires);
        parent.location.replace("https://"+location.hostname);   
    } else {
        $("#btnRemove").attr("disabled", false).css("color", "");
        $("#spanRemove").css("display", "none");
        var protocal=getCookie("protocol"); 
        if(protocal == "3")
            setCookie("protocol", 1, expires);
        parent.location.replace("http://"+location.hostname);    
    }
}

function submitCertReq()
{
    //Check value
    var szCountry = $("#certCountry").val();
    if (szCountry.length != 2) {
        alert("Country error.");
        return;
    }    

    showAjaxImg();

    $("#certBtnSubmit").attr("disabled", true);
    //Update
    $(xmlCert).find("country").text($("#certCountry").val());
    $(xmlCert).find("state").text($("#certState").val());
    $(xmlCert).find("location").text($("#certLocation").val());
    $(xmlCert).find("organization").text($("#certOrganization").val());
    $(xmlCert).find("unit").text($("#certUnit").val());
    $(xmlCert).find("name").text($("#certName").val());
    $(xmlCert).find("email").text($("#certEmail").val());

    //Post
    httpPost("xml", xmlCert, szSslGenerateReq, certReqReturn, clearAjaxImgSlowly);
    return;
}

function submitSelfCertReq()
{   
    var szCountry = $("#selfCertCountry").val();
    var szDays    = $("#selfCertDays").val();

    //Check value
    if (szCountry.length != 2) {
        alert("Country error.");
        return;
    }

    if (szDays.length == 0) {
        alert("Valid Days Error");
        return;
    }
 
    showAjaxImg();

    $("#selfCertBtnSubmit").attr("disabled", true);

    $(xmlSelfCert).find("country").text(szCountry);
    $(xmlSelfCert).find("state").text($("#selfCertState").val());
    $(xmlSelfCert).find("location").text($("#selfCertLocation").val());
    $(xmlSelfCert).find("organization").text($("#selfCertOrganization").val());
    $(xmlSelfCert).find("unit").text($("#selfCertUnit").val());
    $(xmlSelfCert).find("name").text($("#selfCertName").val());
    $(xmlSelfCert).find("email").text($("#selfCertEmail").val());
    $(xmlSelfCert).find("days").text(szDays);


    /* Post */
    httpPost("xml", xmlSelfCert, szSslGenerateCert, certReqReturn, clearAjaxImgSlowly);
}


function certReqReturn(data, textStatus, XMLHttpRequest)
{
    httpGet("xml", "/cgi-bin/admin/Stunnel.lua", null, null);
    var szHint = $(data).find("hint").text();
    var szText = $(data).find("text").text();
    
    if (szHint == "1")
    {
        // dispaly hint note            
        $("#div_hint").css("display", "block");
        $("#reqout_text").css("display", "none");
        $("#reqout_code").val($.trim(szText));
    }
    else
    {
        // convert line feed characters ('\n' -> "<br \>") 
        replaceBR($("#reqout_text").get(0), szText);
        $("#reqout_text").css("display", "block");
    }


    $(".dialogDiv").css("display", "none"); 
    $("#div_response").css("display", "block");

    //Enable submit button
    $("#certBtnSubmit").attr("disabled", false);
    $("#selfCertBtnSubmit").attr("disabled", false);

    clearAjaxImg();
    reloadSslInfo();    
}



function uploadCert()
{
    if ($("#fileCert").val() == "") {
        alert("No file specified.");
        return;
    }

    $("#formUpload").submit();

    $("#div_install").css("display", "none");
    $("#upload_target").css("display", "block");
    $("#div_response").css("display", "block");

    //! update sslinfo in 2 seconds
    setTimeout("reloadSslInfo();", 5000);
}

function close_dialog()
{
    $(".dialogDiv").    css("display", "none");
    $("#div_mask").     css("display", "none");
    $("#upload_target").css("display", "none");
    $("#div_hint").     css("display", "none");

    $("#reqout_text").html("");   
}

function openDlg_selfCert()
{
    $("#div_mask").    css("display", "block");
    $("#div_selfCert").css("display", "block");
}

function openDlg_cert()
{
    $("#div_mask").css("display", "block");
    $("#div_cert").css("display", "block");
}

function openDlg_install()
{
    $("#div_mask").   css("display", "block");
    $("#div_install").css("display", "block");
}

function openDlg_property()
{
    $("#div_mask").    css("display", "block");
    $("#div_property").css("display", "block");
}

function copy_code()
{
    copyToClipboard($("#reqout_code").val());
}


// --------------------------------------------------------------------------

function replaceBR(ele, str)
{   /* convert line feed characters ('\n' -> "<br \>") */
    var row = str.split('\n');
    ele.innerHTML = row[0];

    for(var i=1; i<row.length; i++) {
        ele.innerHTML += "<br\>";
        ele.innerHTML += row[i];
    }
}

//Copy to clipboard: ref http://forum.moztw.org/viewtopic.php?p=131407
function copyToClipboard(txt) {
    var copied = false;
     if(window.clipboardData) {
        window.clipboardData.clearData();
        window.clipboardData.setData("Text", txt);
        copied = true;
     } else if (window.netscape) {
        try {
           netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
        } catch (e) {
           alert("The browser does not support. Please copy manually.");
        }
        var clip = Components.classes['@mozilla.org/widget/clipboard;1']
        .createInstance(Components.interfaces.nsIClipboard);
        if (!clip)
           return;
        var trans = Components.classes['@mozilla.org/widget/transferable;1']
        .createInstance(Components.interfaces.nsITransferable);
        if (!trans)
           return;
        trans.addDataFlavor('text/unicode');
        var str = new Object();
        var len = new Object();
        var str = Components.classes["@mozilla.org/supports-string;1"]
        .createInstance(Components.interfaces.nsISupportsString);
        var copytext = txt;
        str.data = copytext;
        trans.setTransferData("text/unicode",str,copytext.length*2);
        var clipid = Components.interfaces.nsIClipboard;
        if (!clip)
           return false;
        clip.setData(trans,null,clipid.kGlobalClipboard);
        copied = true;
     }
}    




