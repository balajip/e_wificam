var secXhReq = 0;
var xmlLocalInst;
var szConfigRoot = "/cgi-bin/admin/security.lua";
var xmlRoot;

$(document).ready(init);

function init()
{
    /* Show the processing mask */
    showAjaxImg();

    /* Get configures */
    httpGet("xml", szConfigRoot, readRoot, null);
}

function readRoot(data, textStatus, XMLHttpRequest)
{
    xmlRoot = data;  

    var szRootPass = $(data).find("root_pass").text();

    if (szRootPass == 0)
    {
        $("#security_user_i0_pass").val("");
        $("#confirm").val("");
    }
    else
    {
        $("#security_user_i0_pass").val("**************");
        $("#confirm").val("??????????????");    
    }

    clearAjaxImg();
}   

function getNewPw(data, textStatus, XMLHttpRequest)
{
    var szChangePw = $(data).find("change_pass").text();

        
    if (szChangePw != "0")
    {
        alert("Set password fail.");        
    }
    else
    {
        alert("Set password success.");
        httpGet("xml", szConfigRoot, readRoot, null);
    }   

    clearAjaxImg();
}   

function saveRoot()
{
    //Get value
    var szPw1 = $("#security_user_i0_pass").val();
    var szPw2 = $("#confirm").val();

    //Check value
    if (($.trim(szPw1) == "") && ($.trim(szPw2) == ""))
    {
        szPw1 = "";
        szPw2 = "";
    }
    else if (!chkPw(szPw1, szPw2))  
    {
        return;
    }
    else ;


    /* Show the processing mask */
    showAjaxImg();

    //Post and reload list
    var szParams = "security_userpass_0=" + szPw1 +
                   "&confirm=" + szPw2;
    
    httpPost("xml", szParams, szConfigRoot, getNewPw, null);
    
    return;
}
