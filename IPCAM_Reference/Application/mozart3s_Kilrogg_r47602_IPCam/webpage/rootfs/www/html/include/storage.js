var SDStatus=0;
var SearchResult=0;
var FileDir = "";

$(document).ready(function() { 
    sendControl("/cgi-bin/admin/vrecord.cgi?command=sd_status");
    $("#dateFrom").datepicker(); 
    $("#dateFrom").datepicker( "option", "dateFormat", 'yy-mm-dd' ); 
    $("#dateTo").datepicker();     
    $("#dateTo").datepicker( "option", "dateFormat", 'yy-mm-dd' );
    //Remove blank appearing on the bottom of page (datepicker)
    $(".ui-helper-clearfix").css("display","none");
    // add parser through the tablesorter addParser method 
    $.tablesorter.addParser({ 
        // set a unique id 
        id: 'mediaType', 
        is: function(s) { 
            // return false so this parser is not auto detected 
            return false; 
        }, 
        format: function(s) { 
            // format your data for normalization
            return s.toLowerCase().replace(/h264/,0).replace(/mp4v/,1).replace(/mjpeg/,2);  
        }, 
        // set type, either numeric or text 
        type: 'numeric' 
    });
    $("#playDialog:ui-dialog").dialog( "destroy" );  
    $("#playDialog").dialog({ 
        autoOpen: false,
        height: 535,
        width: 670,
        modal: true
    });
    $("#pager").css("display", "none"); 
});  

function sendControl(url)
{
    showAjaxImg();  
    $.ajax({
        type: "GET",
        url: url,
        dataType: "xml",
        cache: false, 
        complete: function(XMLHttpRequest){
            clearAjaxImg();
        }, 
        success: function(data, textStatus, XMLHttpRequest) {
            SDStatus = data;            
            var msg = ($(data).find("status").text() == "OK")?"OK":$(data).find("message").text();
            $("#status").text(msg);
            $("#total").text($(data).find("total_size").text());
            $("#free").text($(data).find("free_size").text());
            $("#used").text($(data).find("used_size").text());
            $("#percent").text($(data).find("used_percentage").text());
            var cyclic = ($(data).find("enable_cyclic_storage").text() == "1");
            $("#cyclic")[0].checked = cyclic;
        }    
    });        
}

function save()
{
    var command = ($("#cyclic")[0].checked)?"enable":"disable";
    var url = "/cgi-bin/admin/vrecord.cgi?command=" + command + "_cyclic_storage";
    sendControl(url);
}   
 
function format()
{
    sendControl("/cgi-bin/admin/vrecord.cgi?command=format");    
}

function testDate(str,objName)
{
    var re=/^(\d+)\-(\d+)\-(\d+)$/g; 
    if(re.test(str))
    {
        if( RegExp.$2<13 && RegExp.$3<32 ) 
            return true;   
    }
    alert(objName + ": Date format error!");           
    return false;   
}

function testTime(str,objName)
{
    var re=/^(\d+)\:(\d+)\:(\d+)$/g; 
    if(re.test(str))
    {
        if( RegExp.$1<24 && RegExp.$2<60 && RegExp.$3<60 ) 
            return true; 
    } 
    alert(objName + ": Time format error!");          
    return false;   
}

function searchClips()
{
    //Check format
    if( !testDate( $("#dateFrom").val(), "From" ) ||
        !testTime( $("#timeFrom").val(), "From" ) ||
        !testDate( $("#dateTo").val(), "To" ) ||
        !testTime( $("#timeTo").val(), "To" ) )
        return;

    //Input the searching condition ("From" trigger time and "To" trigger time)
    showAjaxImg();
    var fromDate = $("#dateFrom").val().replace(/-/g, "");  
    var fromTime = $("#timeFrom").val().replace(/:/g, "");                                                      
    var startTime = fromDate + fromTime;
    var toDate = $("#dateTo").val().replace(/-/g, "");  
    var toTime = $("#timeTo").val().replace(/:/g, "");                                                      
    var endTime = toDate + toTime;
    $.ajax({
        type: "GET",
        url: "/cgi-bin/admin/vrecord.cgi?command=search&start=" + startTime + "&end=" + endTime,
        dataType: "xml",
        cache: false, 
        complete: function(XMLHttpRequest){
            clearAjaxImg();
        },    
        success: function(data, textStatus, XMLHttpRequest) {
            if( $(data).find("status").text() == "OK" )
            {
                SearchResult = data;
                FileDir = "http://" + window.location.hostname + "/" 
                          + $(data).find("dir_url").text();
                $("#result tbody").empty();              
                var total = $(data).find("total_num").text();                
                if( total > 0 )
                {
                    $("#pager").css("display", "block");                    
                    $(data).find("videoclip").each(function(i){
                        var time = $(this).find("trigger_time").text();
                        var codec = $(this).find("video_codec").text();
                        insertRow(time, codec);                        
                    });
                    $("#result").tablesorter({  
                        headers: { 
                            0:{sorter: false},
                            2:{sorter: 'mediaType' }
                        },
                        sortList: [[1,1]],
                        widthFixed: true, 
                        widgets: ['zebra']  
                    }).tablesorterPager({container: $("#pager")});                 
                }
                else
                    $("#pager").css("display", "none");
                alert("Total: " + total );                                       
            }
            else
                alert("Error: " + $(data).find("message").text());
        }    
    }); 

}

function insertRow(time, codec)
{
    var content = "<tr><td><input type=\"checkbox\" /></td><td>"+ timeForRead(time)
        + "</td><td>" + codec + "</td></tr>";
    $("#result tbody").append(content);
}

function timeForRead(time)
{
    if(time.length < 14)
        return time;
    var result = time.substring(0,4) + "-" + time.substring(4,6) + "-" + 
                 time.substring(6,8) + " " + time.substring(8,10) + ":" + 
                 time.substring(10,12) + ":" + time.substring(12);
    return result;              
}

function timeForFind(time)
{
    if(time.length < 19)
        return time;
    var result = time.substring(0,4) +  time.substring(5,7) + 
                 time.substring(8,10) +  time.substring(11,13) + 
                 time.substring(14,16) + time.substring(17);
    return result;              
}

function uncheckAll()
{
    $("#result tbody input").each(function(i){
        this.checked = false;
    });
}

function remove()
{
    showAjaxImg();
    var data = "<?xml version=\"1.0\" standalone=\"yes\"?><root>";
    var trCount = $("#result tbody tr").length;
    var trIndex = 0;
    var clipCount = 0;    
    var first = false;
    var temp = "";
    while(trIndex < trCount)
    {
        var tr = $("#result tbody tr:eq("+trIndex+")")[0];
        if($(tr).find("input")[0].checked)
        {
            var time = timeForFind($(tr).find("td:eq(1)").text());
            var clip = $(SearchResult).find("videoclip:contains('"+time+"')")[0];
            var fileName = $(clip).find("file_name").text();
            temp = temp + "<videoclip id=\""+ clipCount + "\"><file_name>" 
                   + fileName + "</file_name></videoclip>";  
            clipCount++;               
        }        
        trIndex++;               
    }
    data = data + "<total_num>" + clipCount + "</total_num>" + temp + "</root>";
    $.ajax({
        type: "POST",
        url: "/cgi-bin/admin/vrecord.cgi?command=remove",
        dataType: "text",
        processData: false,
        data: data,
        cache: false, 
        complete: function(XMLHttpRequest){
            clearAjaxImg();
        }, 
        success: function(data, textStatus, XMLHttpRequest) {
            alert("Remove success");
        }    
    });       
}

function view()
{
    trCount = $("#result tbody tr").length;
    trIndex = 0;    
    var first = false;
    while(trIndex < trCount)
    {
        var tr = $("#result tbody tr:eq("+trIndex+")")[0];
        if($(tr).find("input")[0].checked)
        {
            var time = timeForFind($(tr).find("td:eq(1)").text());
            var clip = $(SearchResult).find("videoclip:contains('"+time+"')")[0];
            var fileName = $(clip).find("file_name").text();
            var url = FileDir + "/" + fileName; 
            if(!first)
            {
                first = true;
                $("#playDialog").dialog("open");   
                Player.URL = url;            
            }
            else
            {
                var list = Player.currentPlaylist;
                list.appendItem(Player.newMedia(url));
            }            
        }        
        trIndex++;               
    }
    Player.controls.play();          
}

var trIndex = 0;
var trCount = 0;
var downloadOK = true;
function downloadChecked()
{
    trCount = $("#result tbody tr").length;
    trIndex = 0;
    downloadOne();   
}

function downloadOne()
{
    var iframe = $("#video")[0];
    while(trIndex < trCount)
    {
        var tr = $("#result tbody tr:eq("+trIndex+")")[0];
        if($(tr).find("input")[0].checked)
        {
            if( downloadOK )
            {
                var time = timeForFind($(tr).find("td:eq(1)").text());
                var clip = $(SearchResult).find("videoclip:contains('"+time+"')")[0];
                var fileName = $(clip).find("file_name").text();
                var url = FileDir + "/" + fileName; 
                //getOneVideo(url);                                                 
                window.open(url);
            }
            else
            {
                setTimeout("downloadOne()",200);
                return;
            }    
        }        
        trIndex++;               
    }    
}

var timer;
function  getOneVideo(url)   
{
    downloadOK = false;    
    frames("video").location = url;
    timer = setInterval(checkload,100);
}
function  checkload()
{
    if( document.all("video").readyState == "complete" )
    {
        clearInterval(timer);
        downloadOK = true;
    }
}
