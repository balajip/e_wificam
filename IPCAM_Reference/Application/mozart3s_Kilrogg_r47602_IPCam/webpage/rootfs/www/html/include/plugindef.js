/* RTSP plug-in defines */
var Channel= 0;
var StrmID = 0;
var AccessName,Codectype;

function pluginPlayer(playerID)
{
    var id = playerID;
    var mjpgID = "";
    var ch = 0;
    var stream = 0;
    var svctLayer = 3;
    var svctEnable = false;
    var sdp = "";
    var option = "NoSync";
    this.setCH = function(x){
        if(!isNaN(parseInt(x)))
            ch = x;
    }    
    this.setStream = function(x){
        if(!isNaN(parseInt(x)))
            stream = x;
    }
    this.setSVCT = function(enable, val){
        if(enable)
            svctEnable = true;
        if(!isNaN(parseInt(val)))
            svctLayer = val;
    }    
    this.getStream = function(){
        return stream;
    }    
    this.setOpt = function(x){
        addOpt(x);    
    }  
    function addOpt(opt)
    {
        if(option != "")
            option = opt + "," + option;
        else
            option = opt;   
    }  
    this.displayStream = function (imgID){
        if( isIE() )
        {
            //Normal
            if( sdp == "" )
                setSDP();
            checkSVC();
            var img = document.getElementById(imgID);
            if(img != null)
                img.style.visibility = "hidden";
            var player = this;    
            $.ajax({
                type: "GET",
                url: "/cgi-bin/admin/videoin.lua",
                dataType: "xml",
                cache: false, 
                success: function(data, textStatus, XMLHttpRequest) {
                    if($(data).find("venc_videoin motion enable").text() != "on")
                    {
                        if( option.match("NoIntelligent") == null )
                            addOpt("NoIntelligent");
                    }
                    else
                    {
                        //Motion is open, check if crop
                        eval("var capW=videoin_c0_cap_width;"); 
                        eval("var capH=videoin_c0_cap_height;"); 
                        eval("var cropT=videoin_c0_s"+stream+"_crop_top;"); 
                        eval("var cropL=videoin_c0_s"+stream+"_crop_left;"); 
                        eval("var cropW=videoin_c0_s"+stream+"_crop_width;"); 
                        eval("var cropH=videoin_c0_s"+stream+"_crop_height;");   
                        if( capW != cropW || capH != cropH )
                        {
                            //Crop exist
                            var capOpt = "Cap=" + capW + "x" + capH;
                            var cropOpt = "Crop=" + cropL + "x" + cropT + "x" +
                                          cropW + "x" + cropH;
                            addOpt(capOpt);   
                            addOpt(cropOpt);           
                        }
                    }           
                    getProtocolNPlay();
                }    
            }); 
        }
        else
        {
            //Support MJPG, but need to see if stream codec is MJPG
            eval("var codec=videoin_c"+ch+"_s"+stream+"_codectype");
            if(codec == "mjpg")
            {
                var img = document.getElementById(imgID);
                if(img != null)
                {
                    eval("sdp=network_http_s"+stream+"_accessname");
                    img.src = "/" + sdp;
                    img.style.visibility = "visible";
                }
                $("#"+playerID).css("visibility", "hidden");                             
            }
            else
            {
                alert("Please use IE or choose MJPG codec to see the streaming.");
            }            
        }        
    } 
    this.displayRefStream = function (){
        if( isIE() )
        {
            //Normal
            this.setRefSDP();
            checkSVC();
            addOpt("NoIntelligent");
            getProtocolNPlay();
        }  
        else 
            alert("Please use IE to display streaming.");                    
    }   
    function isIE()
    {
        if( window.ActiveXObject !== undefined )
            return true;
        else 
            return false;
    }
    function setSDP()
    {
        eval("sdp=network_rtsp_s"+stream+"_accessname");
    } 
    this.setRefSDP = function (){
        eval("var refCodec=typeof(videoin_c"+ch+"_s2_codectype);");
        stream = ( refCodec != "undefined" )?2:0;
        setSDP();
    }                        
    function checkSVC()
    {
        eval("var codec=videoin_c"+ch+"_s"+stream+"_codectype");
        if(codec == "svc")
        {        
            var header = (svctEnable)?"&svc-header=enable":"";
            sdp = sdp + "?svc-t_fps_setting=" + svctLayer + header;        
        }    
    } 
    function getProtocolNPlay()
    {
        //IE 9 can not use VMR7 to view motion detection
        //Use VMR9 instead
        if( navigator.appVersion.indexOf("MSIE 9") != -1 )
        {
            //It's IE9
            addOpt("Render=VMR9"); 
        } 
        else
        {
            eval("var res=videoin_c0_s"+stream+"_resolution.split('x')");
            var dispW = parseInt(res[0]);
            if( dispW > 1920 && option.indexOf("Render=VMR9") == -1 )
                addOpt("Render=VMR9");
        }     
        var p=getCookie("protocol"); 
        switch(p)
        {
            case "HTTPS":
              if(location.protocol == "http:")
                  parent.location.replace("https://"+location.hostname);
              addOpt("SSL");  //HTTPS/SSL   
              playNormal(); 
              break;
            case "HTTP":
              if(location.protocol == "https:")
                  parent.location.replace("http://"+location.hostname); 
              var currPort = (location.port == "")?"80":location.port; 
              addOpt("HTTP-Port=" + currPort);  //HTTP     
              playNormal();  
              break;
            case "TCP":
              addOpt("TCP");
              playStream();
              break;  
            default:   
              playStream();
              break;         
        }    
    }
    
    function playNormal()
    {
        var url = "rtsp://" + location.hostname + "/" + sdp;
        $("#"+id)[0].openRTSPEx(url, option);    
    }
    
    function playStream()
    {
        $.ajax({
            type: "GET",
            url: "/cgi-bin/admin/rtsp.lua",
            dataType: "xml",
            cache: false, 
            success: function(data, textStatus, XMLHttpRequest) {
                var portVar = ":" + $(data).find("rtsp port").text();
                var url = "rtsp://" + location.hostname + portVar + "/" + sdp;
                $("#"+id)[0].openRTSPEx(url, option);     
            }    
        });     
      
    }   
}

