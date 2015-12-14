/*
function showlangxml(xhReq)
{
    alert(xhReq.responseText);
}

langXhReq = reqXMLHttp("GET", "../include/translater.xml", null, showlangxml)
*/

var XMLHttpRequestTranslator = false;
if (window.XMLHttpRequest)
	XMLHttpRequestTranslator = new XMLHttpRequest();
else if (window.ActiveXObject)
	XMLHttpRequestTranslator = new ActiveXObject("Microsoft.XMLHTTP");
XMLHttpRequestTranslator.open("GET", "/include/translator.xml", false);
XMLHttpRequestTranslator.send(null);
xmlDoc=XMLHttpRequestTranslator.responseXML;
var lan=0;//getCookie("lan")

function loadlanguage()
{
	var tran=document.getElementsByTagName("span");
	for (var i = 0; i < tran.length; i++)
	{	
		if (tran[i].title == "symbol")
		{
			tran[i].innerHTML=xmlDoc.getElementsByTagName(tran[i].innerHTML)[lan].childNodes[0].nodeValue;
			tran[i].title="";
		}
	}

	var opt=document.getElementsByTagName("option");
	for (var i = 0; i < opt.length; i++)
	{	
		if (opt[i].title == "symbol")
		{
			opt[i].text=xmlDoc.getElementsByTagName(opt[i].text)[lan].childNodes[0].nodeValue;
			opt[i].title="";
		}
	}
	
	var input=document.getElementsByTagName("input");
	for (var i = 0; i < input.length; i++)
	{	
		 if((input[i].type=="button" || input[i].type=="submit") && input[i].title == "symbol")
		 {
			input[i].value=xmlDoc.getElementsByTagName(input[i].value)[lan].childNodes[0].nodeValue;
			input[i].title="";
		 }
	}	
}

