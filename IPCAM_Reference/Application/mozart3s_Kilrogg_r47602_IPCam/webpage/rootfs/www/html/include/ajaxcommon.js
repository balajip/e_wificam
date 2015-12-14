/* -------------- AJAX function libraries ------------- */

//var xhReq = false; /* ajax HttpRequest instance */
/* request XMLHTPPRequest Objects*/
function getXMLHttpRequest()
{
    try
    {
        /* For FireFox, native object of javascript interpreter */
        request = new XMLHttpRequest();
        if (request.overrideMimeType) 
        {
		    request.overrideMimeType('text/xml');
        }
    }
    catch(err1)
    {
        try
        {
            /* some version IEs */
            request = new ActiveXObject("Msxml2.XMLHTTP");
        }   
        catch(err2)
        {
            try
            { 
                /* some version IEs */
                request = new ActiveXObject("Microsoft.XMLHTTP");
            }
            catch(err3)
            {
                request = false;
            }
        }
        
    }

    return request;
}

/* set XMLHTPPRequest Objects and do request */
function reqXMLHttp(method, url, sendMe, callback, basync)
{
//    alert(basync)
    var xhReqObj = false;
    xhReqObj = getXMLHttpRequest();
//    var randnum = parseInt(Math.random()*99999999);
    if (xhReqObj)
    {
        /* add rand number to confuse browser to avoid cach problem */
//        url += (url.match(/\?/) ? '&' : '?') + "rand=" + randnum;
        xhReqObj.open(method, url, basync);
        xhReqObj.setRequestHeader("If-Modified-Since","0");
        xhReqObj.onreadystatechange = function() {processReqChange(xhReqObj, callback);};
        xhReqObj.send(sendMe);
        if ((navigator.appName == "Netscape") && (basync == false))
        {
            callback(xhReqObj);
        }
        return xhReqObj;
    }
}

/* process readystatechange issue */
function processReqChange(xhReqObj, callback_func)
{
/*	var progress_status0 = document.getElementById("progress_0");
	var progress_status1 = document.getElementById("progress_1");
	var progress_status2 = document.getElementById("progress_2");
	var progress_status3 = document.getElementById("progress_3");
*/
    if (xhReqObj.readyState == 0)
    {}
    else if (xhReqObj.readyState == 1)
    {}
    else if (xhReqObj.readyState == 2)
    {}
    else if (xhReqObj.readyState == 3)
    {}
    else if (xhReqObj.readyState == 4)
    {
        if (xhReqObj.status == 200 && callback_func != null)
        {
            callback_func(xhReqObj);
        }
        else if (xhReqObj.status != 200)
        {
            alert("There was a problem retrieving the XML data:\n" + xhReqObj.status + ":" + xhReqObj.statusText);
        }
    }
}

function findChildNodeByName(thisNode, targetName)
{
	var thisChildNode = thisNode.childNodes;
	// alert(thisNode.nodeName + " " + thisChildNode.length)
	for (i = 0; i < thisChildNode.length; i ++)	{
		if (thisChildNode[i].nodeName == targetName) {
			return thisChildNode[i];
		}
	}

	return null;
}

function findNthChildNodeByName(thisNode, targetName, order)
{
	var counter = 0;
    var thisChildNode = thisNode.childNodes;
    // alert(thisNode.nodeName + " " + thisChildNode.length)
    for (i = 0; i < thisChildNode.length; i ++) {
        if (thisChildNode[i].nodeName == targetName) {
			counter++;
			if ((order+1) == counter) {
            	return thisChildNode[i];
			}
        }
    }
    return null;
}

function findChildNodeByNameId(thisNode, targetName, id)
{
	var thisChildNode = thisNode.childNodes;
	//alert(thisNode.nodeName + " " + thisChildNode.length)
	for (i = 0; i < thisChildNode.length; i ++)	{
		if (thisChildNode[i].nodeName == targetName) {
			if (thisChildNode[i].attributes.getNamedItem("id").nodeValue = id) {
				return thisChildNode[i];
			}
			//for (j = 0; j < candidateNode.attributes.)
		}
	}
	return null;
}

function getTextFromNode(thisNode)
{
	if (findChildNodeByName(thisNode, "#text") != null) {
		return findChildNodeByName(thisNode, "#text").nodeValue;
	} else {
		return "";
	}
}

