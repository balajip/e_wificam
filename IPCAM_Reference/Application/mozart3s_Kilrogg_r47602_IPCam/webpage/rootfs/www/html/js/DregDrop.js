// iMouseDown represents the current mouse button state: up or down
/*
lMouseState represents the previous mouse button state so that we can
check for button clicks and button releases:

if(iMouseDown && !lMouseState) // button just clicked!
if(!iMouseDown && lMouseState) // button just released!
*/
var mouseOffset = null;
var iMouseDown  = false;
var lMouseState = false;
var lLeftClick  = false;
var dragObject  = null;

var DragDrops   = [];
var lastTarget  = null;
var beginSlot   = null;
var endSlot     = null;
var beginClearSlot = null;
var endClearSlot   = null;

function CreateDragContainer(){
    /*
    Create a new "Container Instance" so that items from one "Set" can not
    be dragged into items from another "Set"
    */
    var cDrag        = DragDrops.length;
    DragDrops[cDrag] = [];

    /*
    Each item passed to this function should be a "container".  Store each
    of these items in our current container
    */
    for(var i=0; i<arguments.length; i++){
        var cObj = arguments[i];
        DragDrops[cDrag].push(cObj);
        cObj.setAttribute('DropObj', cDrag);
        
        /*
        Every top level item in these containers should be draggable.  Do this
        by setting the DragObj attribute on each item and then later checking
        this attribute in the mouseMove function
        */
        for(var j=0; j<cObj.childNodes.length; j++){

            // Firefox puts in lots of #text nodes...skip these
            if(cObj.childNodes[j].nodeName=='#text') continue;

            cObj.childNodes[j].setAttribute('DragObj', cDrag);
        }
    }
    
    for(var i=0; i<DragDrops[0][0].childNodes.length; i++){
        // Firefox puts in lots of #text nodes...skip these
        if (DragDrops[0][0].childNodes[i].nodeName=='#text') continue;
        
        DragDrops[0][0].childNodes[i].style.backgroundColor = '#eeeeee';
    }
    var thr;
    var tmm;
    var tinput;
    
    tinput = document.getElementById('begintime').value;
    thr = tinput.substr(0,2);
    tmm = tinput.substr(3,2);

    //alert(thr + ":" + tmm);
    beginSlot = thr * 12 + parseInt(tmm/5); 
    
    tinput = document.getElementById('endtime').value;
    thr = tinput.substr(0,2);
    tmm = tinput.substr(3,2);
    
    endSlot = thr * 12 + parseInt((tmm)/5);
    endSlot = (endSlot > 287) ? 287 : endSlot;
    
    for(var i=0; i<DragDrops[0][0].childNodes.length; i++){
        if (DragDrops[0][0].childNodes[i].id == 'TimeSlot'+(beginSlot+1)) {
            tStart = i;
        }
        if (DragDrops[0][0].childNodes[i].id == 'TimeSlot'+(endSlot+1)) {
            tEnd = i;
            break;
        }
    }
    for(var i=tStart; i<=tEnd; i++){
        // Firefox puts in lots of #text nodes...skip these
        if(DragDrops[0][0].childNodes[i].nodeName=='#text') continue;
        DragDrops[0][0].childNodes[i].style.backgroundColor = '#6495ed';
    }
    
}

function writeHistory(object, message){
    if(!object || !object.parentNode || !object.parentNode.getAttribute) return;
    var historyDiv = object.parentNode.getAttribute('history');
    if(historyDiv){
        historyDiv = document.getElementById(historyDiv);
        historyDiv.appendChild(document.createTextNode(object.id+': '+message));
        historyDiv.appendChild(document.createElement('BR'));

        historyDiv.scrollTop += 50;
    }
}

//若input的長度小於length，則在其左邊補0
function padLeft(input){ 
    if(input >= 10)  return input; 
    else    return '0'+input; 
} 

function mouseUp(ev){
    var tStart;
    var tEnd;
    var i;
    
    ev         = ev || window.event;
    var target = ev.target || ev.srcElement;

    /*
    dragObj is the grouping our item is in (set from the createDragContainer function).
    if the item is not in a grouping we ignore it since it can't be dragged with this
    script.
    */
    var dragObj = target.getAttribute('DragObj');

    // if the mouse was moved over an element that is draggable
    if(dragObj!=null) {
    
        // Check color region
        if (lLeftClick || ((beginSlot < beginClearSlot) && (endClearSlot < endSlot))) 
        {
            for(i=0; i<DragDrops[0][0].childNodes.length; i++){
                if (DragDrops[0][0].childNodes[i].id == 'TimeSlot'+(beginSlot+1)) {
                    tStart = i;
                }
                if (DragDrops[0][0].childNodes[i].id == 'TimeSlot'+(endSlot+1)) {
                    tEnd = i;
                    break;
                }
            }
            for(i=tStart; i<=tEnd; i++){
                // Firefox puts in lots of #text nodes...skip these
                if(DragDrops[0][0].childNodes[i].nodeName=='#text') continue;
                DragDrops[0][0].childNodes[i].style.backgroundColor = '#6495ed';
            }
            
        }
        else {
            for(i=0; i<DragDrops[0][0].childNodes.length; i++){
                if (DragDrops[0][0].childNodes[i].id == 'TimeSlot'+(beginClearSlot+1)) {
                    tStart = i;
                }
                if (DragDrops[0][0].childNodes[i].id == 'TimeSlot'+(endClearSlot+1)) {
                    tEnd = i;
                    break;
                }
            }
            for(i=tStart; i<=tEnd; i++){
                // Firefox puts in lots of #text nodes...skip these
                if(DragDrops[0][0].childNodes[i].nodeName=='#text') continue;
                DragDrops[0][0].childNodes[i].style.backgroundColor = '#eeeeee';
            }
        }
    }
    
    dragObject = null;
    iMouseDown = false;
}




function mouseDown(ev){
    ev         = ev || window.event;
    var target = ev.target || ev.srcElement;

    iMouseDown = true;

    /*
    Left or Right Button
        # event.which in Netscape Navigator
        # event.button in Internet Explorer
    */
    if (parseInt(navigator.appVersion)>3) {
        var clickType=1;
        if (navigator.appName=="Netscape") {
            clickType = ev.which;
        }
        else {
            clickType = event.button;
        }
        lLeftClick = (clickType == 1);
    }

/*    
    if(lastTarget){
        writeHistory(lastTarget, 'Mouse Down Fired');
    }
*/    
    if(target.onmousedown || (target.getAttribute('DragObj')!=null)){
        //writeHistory(target, 'DragObj');
        
        // Reset color to white
        if (lLeftClick) {
            // fill in the input
            endSlot = beginSlot = (target.id.substring(8, target.id.length)-1);
            document.getElementById('begintime').value = padLeft(parseInt(beginSlot/12))+':' + padLeft((beginSlot%12)*5);
            document.getElementById('endtime').value = padLeft(parseInt(beginSlot/12))+':' + padLeft((beginSlot%12)*5+4);
            
            // reset color
            for(var i=0; i<DragDrops[0][0].childNodes.length; i++){
                // Firefox puts in lots of #text nodes...skip these
                if(DragDrops[0][0].childNodes[i].nodeName=='#text') continue;
                DragDrops[0][0].childNodes[i].style.backgroundColor = '#eeeeee';
            }
        }
        else {
            endClearSlot = beginClearSlot = (target.id.substring(8, target.id.length)-1);
        }
        
        // update Radio button

	    document.getElementsByName("method")[1].checked = 1;
		document.getElementById("begintime").disabled="";
		document.getElementById("endtime").disabled="";
    
        return false;
    }
}

function mouseMove(ev){
    ev           = ev || window.event;
    var target   = ev.target || ev.srcElement;
    var mousePos = mouseCoords(ev);
    
    // mouseOut event - fires if the item the mouse is on has changed
    if(lastTarget && (target!==lastTarget)){
        //writeHistory(lastTarget, 'Mouse Out Fired');

        // reset the classname for the target element
        var origClass = lastTarget.getAttribute('origClass');
        if(origClass) lastTarget.className = origClass;
    }
    
    /*
    dragObj is the grouping our item is in (set from the createDragContainer function).
    if the item is not in a grouping we ignore it since it can't be dragged with this
    script.
    */
    var dragObj = target.getAttribute('DragObj');

    // if the mouse was moved over an element that is draggable
    if(dragObj!=null)
    {
        // mouseOver event - Change the item's class if necessary
        if(target!=lastTarget){
            //writeHistory(target, 'Mouse Over Fired');

            var oClass = target.getAttribute('overClass');
            if(oClass){
                target.setAttribute('origClass', target.className);
                target.className = oClass;
            }
        }
        
        // if the user is just starting to drag the element
        if(iMouseDown){
            
            var slot = (target.id.substring(8, target.id.length)-1);
            
            //writeHistory(target, 'Start Dragging');
            if (lLeftClick) {
                // fill in color
                target.style.backgroundColor = '#6495ed';
                // fill in the input
                if (slot > endSlot) {
                    document.getElementById('endtime').value = padLeft(parseInt(slot/12))+':' + padLeft((slot%12)*5+4);
                    endSlot = slot;
                }
                else if (slot < beginSlot) {
                    document.getElementById('begintime').value = padLeft(parseInt(slot/12))+':' + padLeft((slot%12)*5);
                    beginSlot = slot;
                }
            }
            else {
                // reset color
                target.style.backgroundColor = '#eeeeee';
                if (slot < beginClearSlot) {
                    beginClearSlot = slot;
                } 
                else if (slot > endClearSlot) {
                    endClearSlot = slot;
                }
                
                if ((beginClearSlot < beginSlot) && (beginSlot < endClearSlot) && (beginSlot < endSlot)) {
                    beginSlot = endClearSlot;
                    if (beginSlot == endSlot) 
                        document.getElementById('begintime').value = document.getElementById('endtime').value = '00:00';
                    else
                        document.getElementById('begintime').value = padLeft(parseInt(beginSlot/12))+':' + padLeft((beginSlot%12)*5);
                }
                else if ((beginClearSlot < endSlot) && (endSlot < endClearSlot) && (beginSlot < endSlot)) {
                    endSlot = beginClearSlot;
                    if (beginSlot == endSlot) 
                        document.getElementById('begintime').value = document.getElementById('endtime').value = '00:00';
                    else
                        document.getElementById('endtime').value = padLeft(parseInt(endSlot/12))+':' + padLeft((endSlot%12)*5+4);
                }
                
            }
                
            
            return false;
        }
    }
    // track the current mouse state so we can compare against it next time
    lMouseState = iMouseDown;

    // mouseMove target
    lastTarget  = target;

}

function contextMenu(ev) {
    ev           = ev || window.event;
    var target   = ev.target || ev.srcElement;
    var mousePos = mouseCoords(ev);
    
    // mouseOut event - fires if the item the mouse is on has changed
    if(lastTarget && (target!==lastTarget)){
        // reset the classname for the target element
        var origClass = lastTarget.getAttribute('origClass');
        if(origClass) lastTarget.className = origClass;
    }
    
    /*
    dragObj is the grouping our item is in (set from the createDragContainer function).
    if the item is not in a grouping we ignore it since it can't be dragged with this
    script.
    */
    var dragObj = target.getAttribute('DragObj');

    /* 
    if the mouse was moved over an element that is draggable
    we block the context menu
    */
    if(dragObj!=null)
    {
        //writeHistory(target, 'Block Menu');
        return false;
    }
}

function mouseCoords(ev){
    if(ev.pageX || ev.pageY){
        return {x:ev.pageX, y:ev.pageY};
    }
    return {
        x:ev.clientX + document.body.scrollLeft - document.body.clientLeft,
        y:ev.clientY + document.body.scrollTop  - document.body.clientTop
    };
}

document.onmousemove = mouseMove;
document.onmouseup   = mouseUp;
document.onmousedown = mouseDown;
document.oncontextmenu = contextMenu;
/*        
window.onload = function(){
    for(var i=0; i<nDemos; i++){
        Demos[i] = document.getElementById('Demo'+i);
    }
    
    CreateDragContainer(document.getElementById('DragContainer1'));
    
    if(Demos[3]) {
        makeClickable(document.getElementById('ClickImage1'));
        makeClickable(document.getElementById('ClickImage2'));
        makeClickable(document.getElementById('ClickImage3'));
        makeClickable(document.getElementById('ClickImage4'));
    }
    if(Demos[5]) {
        makeDraggable(document.getElementById('DragImage5'));
        makeDraggable(document.getElementById('DragImage6'));
        makeDraggable(document.getElementById('DragImage7'));
        makeDraggable(document.getElementById('DragImage8'));
    }
}
*/