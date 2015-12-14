var szVersionInfo = "/cgi-bin/admin/versioninfo.lua";

var bHaveSubmit = false;


$(document).ready(init);


function init()
{
	/* Show the processing mask */
    showAjaxImg();

	/* Get configures */
	httpGet("xml", szVersionInfo, readVERInfo, null);
}
function checkUpdate()
{
	if (bHaveSubmit)	return;
	
  	if ($("#sysfrmwareFile").val() == "") {
    	alert("No firmware specified");
    	return;
  	}
	
  	bHaveSubmit = true;
	$("#formUpdate").submit();
}

function checkUpgrade()
{
	if (bHaveSubmit)	return;
	
  	if ($("#firmwareFile").val() == "") {
    	alert("No firmware specified");
    	return;
  	}
	
  	bHaveSubmit = true;
	$("#formUpgrade").submit();
}

function readVERInfo(data, textStatus, XMLHttpRequest)
{
    var szVersion = $(data).find("version").text();
	var szDate    = $(data).find("create_date").text();
    
    if (szVersion)
		$("#firmwareCurrVer").html(szVersion);
		$("#firmware_current_vrsn").html(szVersion);
    if (szDate)
		$("#firmwareCreateDate").html(szDate);
		$("#firmware_create_dte").html(szDate);

}

// --------------------------------------------------------------------------------------
function restore()
{
	var data;
	
	httpPost("", data, "/cgi-bin/admin/restore.lua", null, null);
	alert("Server is restarting now.");	
}

function reboot()
{	
	var data;
	
	httpPost("", data, "/cgi-bin/admin/reboot.lua", null, null);
	alert("Server is restarting now.");
}


