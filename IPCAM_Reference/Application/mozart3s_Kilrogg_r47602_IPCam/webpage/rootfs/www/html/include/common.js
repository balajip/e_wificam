//transform the xml string to a xmlDocument
function getXmlDoc(szXml)
{
    var xmlDoc;

    if (window.DOMParser)
    {
        var parser = new DOMParser();
        xmlDoc = parser.parseFromString(szXml,"text/xml");
    }
    else // Internet Explorer
    {
        xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
        xmlDoc.async="false";
        xmlDoc.loadXML(szXml);
    }

    return xmlDoc;
}


//url: remote configure file
//callback: function(data, textStatus, HttpRequest)
//(data is the response text; textStatus is the request response; HttpRequest is the request object)
function httpGet(dType, url, cbSuccess, cbComplete)
{

    $.ajax({
        type: "GET",
        url: url,
        dataType: dType,
        cache: false, 

        error: function(data, textStatus) {
//            alert("Error: " + textStatus);
        },

        complete: cbComplete,
        
        success: cbSuccess
    });  

}


//url: remote configure file
//callback: function(data, textStatus, HttpRequest)
//(data is the response text; textStatus is the request response; HttpRequest is the request object)
function httpPost(dataType, data, url, cbSuccess, cbComplete)
{
    $.ajax({
        type: "POST",
        url: url,
        dataType: dataType,
        processData: false,
        data: data,
        cache: false, 

        error: function(data, textStatus) {
//            alert("Error: " + textStatus);
        },
        
        complete: cbComplete,
        
        success: cbSuccess
    });     
}

//Get the value of the selected radio button by name attribute.
function getRadioValue(group)
{
    return $("input[name='"+group+"']:checked").val();
}

//Toogle the selected radio button by name attribute and value.
function setRadioValue(group, val)
{
    $("input[name='"+group+"'][value='"+val+"']")[0].checked = true;
}

function getCookie(Name) 
{
  var search = Name + "="
  if (document.cookie.length > 0) { 
      // if there are any cookies
      offset = document.cookie.indexOf(search)
      if (offset != -1) { 
          // if cookie exists
          offset += search.length

          // set index of beginning of value
          end = document.cookie.indexOf(";", offset)

          // set index of end of cookie value
          if (end == -1)
              end = document.cookie.length

          return unescape(document.cookie.substring(offset, end))
      }
  }
  return 0;
}

function setCookie(name, value, expire) 
{
    strCookie = name + "=" + escape(value);
    if (expire)
        strCookie += "; expires=" + expire.toGMTString();
    
  document.cookie = strCookie;
}

function setCookieDecade(name, val)
{
    var expires = new Date();       
    expires.setTime (expires.getTime() + (10 * 365 * 24 * 60 * 60 * 1000));
    var str = name + "=" + escape(val) + "; expires=" + expires.toGMTString();
    document.cookie = str;
}

function translator(str)
{
    return xmlDoc.getElementsByTagName(str)[lan].childNodes[0].nodeValue;
}


function openurl(url)
{
    var subWindow = window.open(url, "","width=800, height=600, scrollbars=yes, status=yes");
    subWindow.focus();
}


function trim(stringToTrim)
{
    return stringToTrim.replace(/^\s+|\s+$/g,"");
}


//If length of "str" smaller then "length", add "sign" at left side of "str".
function padLeft(str,length,sign) 
{
    if(str.length>=length)  return str;
    else                    return padLeft(sign+str, length, sign);
}


/*
//The following codes do not be used currently, but still called by some unused codes.

function writeTextToNode(xmlobj, thisNode, value)
{
    if (thisNode.firstChild)
    {
        thisNode.firstChild.nodeValue=value;
    }
    else
    {
        thisNode.appendChild(xmlobj.createTextNode(value));
    }
}

function updatecheck(inputName, checkbox)
{
    if (checkbox.checked)
        inputName.value = "1";
    else
        inputName.value = "0";
}

function updatecheckById(Id, checkbox)
{
    if (checkbox.checked)
        document.getElementById(Id).value = "1";
    else
        document.getElementById(Id).value = "0";
}

function checkIPaddr(input)
{
    var filter=/\d+\.\d+\.\d+\.\d+/;
    if (!filter.test(input.value)){
        alert(translator("please_enter_a_valid_ip_address"));
        input.focus();
        input.select();
        return -1;
    }
    input.value = input.value.match(filter)[0];
    subip = input.value.split(".");
    for (i = 0; i < subip.length; i ++)
    {
        if ((parseInt(subip[i]) > 255) || (parseInt(subip[i]) < 0 ))
        {
            alert(translator("please_enter_a_valid_ip_address"));
            input.focus();
            input.select();
            return -1;
        }
    }    
    if ((parseInt(subip[0]) == 0) && (parseInt(subip[1]) == 0) && (parseInt(subip[2]) == 0) && (parseInt(subip[3]) == 0))
    {
        alert(translator("please_enter_a_valid_ip_address"));
        input.focus();
        input.select();
        return -1;
    }
    
    return 0;
}
*/




// - Processing mask ---------------------------------------------------------------

function showAjaxImg() 
{
    var ajaxImg = document.getElementById("ajaxImg");
    var ajaxMask = document.getElementById("ajaxMask"); 
    if (ajaxMask) {
//      alert("scroll: "+document.body.scrollHeight+" offset: "+document.body.offsetHeight+" client: "+ document.body.clientHeight+" document.documentElement.clientHeight: "+ this.innerHeight   + 
//            "\ndocument.documentElement.clientHeight: "+document.documentElement.clientHeight+" offset: "+ajaxMask.offsetHeight+" client: "+ ajaxMask.clientHeight+" style: "+ajaxMask.style.height);
        ajaxMask.style.height = Math.max(document.documentElement.scrollHeight, document.documentElement.clientHeight) +"px";
        ajaxMask.style.position = "fixed";
        ajaxMask.style.top = "0 px";
        ajaxMask.style.zIndex = "100"; // the mask have to cover all input elements
        ajaxMask.style.display="block";
    }
    if (ajaxImg) {
//  alert("document.documentElement.scrollTop: " + document.documentElement.scrollTop + " document.documentElement.clientHeight/2: "+ (document.documentElement.clientHeight/2) + " document.documentElement.clientHeight/2|0 " + (document.documentElement.clientHeight/2|0) +
//        "\n ajaxImg.style.top: "+ajaxImg.style.top + " a/2+b: "+ ((document.documentElement.clientHeight/2) + document.documentElement.scrollTop));
        ajaxImg.style.top = ((document.documentElement.clientHeight/2|0) + document.documentElement.scrollTop) + "px";
    
        var show_img = document.getElementById("show_img");
        if (! show_img) {
            var img = document.createElement("img");
            img.id = 'show_img';
            img.src = '../pic/ajax-loader.gif';
            ajaxImg.appendChild(img);
        }
    }
}

function clearAjaxImg() 
{
    var ajaxImg = document.getElementById("ajaxImg");
    var ajaxMask = document.getElementById("ajaxMask"); 
    if (ajaxMask) {
        ajaxMask.style.display="none";
    }
    if (ajaxImg) {
//        show_img = document.getElementById("show_img");
//        ajaxImg.removeChild(show_img);
    }
}

function clearAjaxImgSlowly() 
{
    setTimeout("clearAjaxImg()", 1000);
}

// - Check value ------------------------------------------------------------------

//Check if val is between min and max.
//Return:   true - between min and max
//          false - out of range  
function chkRange(val, min, max)
{
    if ((val >= min) && (val <=max))    return true;
    else                                return false;
}

//Check if email is a valid email address.
//Return:   true - valid
//          false - not valid
function chkEmail(email)
{
    var filter=/^([\w-]+(?:\.[\w-]+)*)@((?:[\w-]+\.)*\w[\w-]{0,66})\.([a-z]{2,6}(?:\.[a-z]{2})?)$/;

    if (!filter.test(email)) {
        alert("Please input a valid email address.");
        return false;
    }

    return true;        
}
    
/*
function checkemail(input)
{
    var filter=/^([\w-]+(?:\.[\w-]+)*)@((?:[\w-]+\.)*\w[\w-]{0,66})\.([a-z]{2,6}(?:\.[a-z]{2})?)$/i
    if (filter.test(input.value))
        return 0;
    else{
        alert(translator("please_input_a_valid_email_address"));
        input.focus();
        input.select();
        return -1;
    }
}
*/

//Check if ip is legal
//Return:   true: legal
//          false: illegal
function chkIP(ip)
{
    var bOK = true;
    var aSubIp;
    var ip;

    var filter=/^\d{1,}\.\d{1,}\.\d{1,}\.\d{1,}$/;


    if (!filter.test(ip)) {
        bOK = false;
    }
    else {

        aSubIp = ip.split(".");

        for (i = 0; i < 4; i ++)
        {
            if (!chkRange(parseInt(aSubIp[i]), 0, 255)) 
                bOK = false;
        }  
    }


    if (!bOK) {
        alert("Please enter a valid IP address");
    }

    return bOK;
    
}

//check if the port is legal.
//Return:   true: legal
//          false: illegal
function chkPort(port)
{
    var filter =/^\d{1,}$/;
    var inputVal;


    if (filter.test(port)) {
        inputVal = parseInt(port);

        if ((inputVal != "NaN") && chkRange(inputVal, 0, 65535)) {
            return true;
        }
    }

    /* valid port */
    alert("Port must between 0 and 65535");
    return false;
    
}

//Check pasword(szPw1) and the confirmed password(szPw2);   
//          1. password is composite by legal characters(without space): 
//             legal characters: 0-9, a-z, A-Z, !, @, $, %, -, ., ^, ~, '  
//          2. password and the confirmed-password are the same.
//Return:   true: legal, and szPw1 and szPw1 are the same
//          false: illegal characters or the two password(szPw1 and szPw2) are different.
function chkPw(szPw1, szPw2)
{
    var filter =/^[\w!@$%\-\.\^~']+$/;

    if (!filter.test(szPw1))
    {
        alert("You have used invalid characters password.");
        return false;
    }


    if (szPw1 != szPw2){
        alert("The confirm password differs from the password");
        return false;
    }

    return true;
}
