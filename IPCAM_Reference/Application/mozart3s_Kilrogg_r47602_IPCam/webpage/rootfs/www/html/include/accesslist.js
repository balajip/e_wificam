var xmlList = 0;
var iListNum = 0;
var iListMax = 20;
var szConfigList = "/cgi-bin/admin/ipfilter.lua";

$(document).ready(init);

function init()
{
    showAjaxImg();

    /* Get configures */
    loadList();
}

function loadList()
{
    httpGet("xml", szConfigList, readList, null);
}

function readList(data, textStatus, XMLHttpRequest)
{
    xmlList = data;  

    //clear the select widget
    var $list = $("#list").empty();

    //Insert the list to select widget
    $(xmlList).find("deny rule").each(function(){
            var val  = parseInt($(this).attr("id"));
            var text = $(this).children("start").text() + " ~ " + $(this).children("end").text();
            $list.append("<option value=\"" + val + "\">" + text + "</option>");
        }
    );

    //Get the list length
    iListNum = parseInt($(xmlList).find("deny total_num").text());

    /* Hide the processing mask */
    clearAjaxImg(); 
}

function addDenyList(){

    //Check list number
    if (iListNum >= iListMax) {
        alert("Reach max deny list(20).");
        return;
    }

    //Check IP address
    var szIpS = $("#ipS").val();
    var szIpE = $("#ipE").val();
    
    if (!chkIP(szIpS))  return;
    if (!chkIP(szIpE))  return;

    $("#ipS").val("");
    $("#ipE").val("");

    showAjaxImg();

    var content = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?><root><access_list><deny>";

    $(xmlList).find("deny rule").each(function(){
        var ips = $(this).children("start").text();
        var ipe = $(this).children("end").text();
        var id =  $(this).attr("id");                            
        content = content + getOneRule(id, ips, ipe);
    });
  
    content = content + getOneRule(iListNum, szIpS, szIpE);
    iListNum++; 
    content = content + "<total_num>"+iListNum+"</total_num></deny></access_list></root>"; 

    //Post and reload list
    httpPost("text", content, szConfigList, loadListLater, null);
}

function loadListLater()
{
    setTimeout("loadList()", 300);
}

function getOneRule(id, ips, ipe)
{
    return  "<rule id=\""+id+"\"><start>"+ips+"</start><end>"+ipe+"</end></rule>";   
}

function deleteDenyList()
{
    showAjaxImg();
    if (iListNum <= 0)  
        return;

    var idx = $("#list").val();
    $(xmlList).find("deny rule[id='" + idx + "']").remove();
    $(xmlList).find("deny total_num").text(--iListNum);

    //Update the list id in xml
    var i = 0;
    $(xmlList).find("deny rule").each(function(){
        $(this).attr("id", i++);
    })

    //Post and reload
    httpPost("text", xmlList, szConfigList, loadList, null);
}
