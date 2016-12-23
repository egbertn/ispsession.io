// Write your Javascript code.

        
$( document ).ready(function() 
{
        
    function doAjaxPanel(refresh, path) {        
        $.get(path + "&ajaxRefresh=" + (refresh ? "1" : "0"),
               function( data ) { $( "div.default_content" ).html( data );}
        ); 
    };

    $("button.refresh").click(function(){ window.location.reload();});
    // how to pass the session to an Ajax panel.   
    $("button.ajax").click(function (e) {
        var me = $(this);     
        doAjaxPanel(true, me.attr("data-path")); return false;
    });
            
    if ($(".main").length > 0) //Default_Content.asp tag
    {        
        doAjaxPanel(false, $("button.ajax").attr("data-path"));//default call
    }
});
        
