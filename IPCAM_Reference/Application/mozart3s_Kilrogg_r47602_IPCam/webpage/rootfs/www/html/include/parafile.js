$(document).ready(init);

function init()
{
    /* Adjust the size of text box for current log */
    adjustSize();
}

function adjustSize()
{
    var iHeight = $(window).height();
    var iOffset = $("#paraList").parent().offset().top;

    $("#paraList").css("height", (iHeight - iOffset - 80) + "px");
};

