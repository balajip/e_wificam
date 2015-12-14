/* ----- cookie handls libraries ----- */
function getCookieVal(offset)
{
    var endstr = document.cookie.indexOf(";", offset);
    if (endstr == -1)
        endstr = document.cookie.length;
    return unescape(document.cookie.substring(offset, endstr));   
}

function getCookie(name)
{
    var arg = escape(name) + "=";
    var arglen =arg.length;
    var cookielen = document.cookie.length;
    var i = 0;
    while(i<cookielen)
    {
        var j = i + arglen;
        if (document.cookie.substring(i, j) == arg)
            return getCookieVal(j);
        i = document.cookie.indexOf("", i) + 1;
        if (i == 0) break;
    }
    return null;
}

function setCookie(name, value)
{
    var argv = setCookie.arguments;
    var argc = setCookie.arguments.length;
    var expirehr = (argc>2)?argv[2]:null;
    var path = (argc>3)?argv[3]:null;
    var domain = (argc>4)?argv[4]:null;
    var secure = (argc>5)?argv[5]:false;
    var now = new Date();
    var expires = new Date();
    var exnow;
    if (expirehr == null)
    {
        expires = null;
    }
    else
    {
        expires.setTime(now.getTime() + expirehr * 60 * 60 * 1000);
    }
    document.cookie = name + "=" + escape(value) +
    ((expires == null) ? "" : (";expires=" + expires.toGMTString())) +
    ((path == null) ? "" : (";path=" + path)) +
    ((domain == null) ? "" : (";domain=" + domain)) +
    ((secure == false)? "" :(";secure"));
}

function delCookie(name)
{
    var exp = new Date();
    exp.setTime(exp.getTime() - 1);
    var cval = getCookie(name);
    document.cookie= escape(name)+ "=" + cval +"; expires=" + exp.toGMTString();
}

function listCookie() {
	document.writeln("<b>Cookie string : </b><br />" + document.cookie + "<br />");
	document.writeln("<b>List cookies :</b>");
	document.writeln("<table border=1>");
	document.writeln("<tr><th>Name<th>Value");
	cookieArray = document.cookie.split(";");
	for (var i=0; i<cookieArray.length; i++) {
		thisCookie = cookieArray[i].split("=");
		cookieName = unescape(thisCookie[0]);
		cookieValue = unescape(thisCookie[1]);
		document.writeln("<tr><td><font color=red>"+cookieName+"</font><td><font color=green>"+cookieValue+"</font>");
	}
	document.writeln("</table>");
}

