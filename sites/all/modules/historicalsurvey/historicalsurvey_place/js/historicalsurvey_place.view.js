(function ($, Drupal, window, document, undefined) {
Drupal.behaviors.ahswFields = {attach: function (context, settings) {
  
  /* non-reviewed field actions */ 
  $("#top-block-wrapper .main-value,#top-block-wrapper .field-name").click(function () {
    var height = $(this).parents(".field-wrapper").css("height");
    if (height == "35px"){
      $(this).siblings(".revision-info").show();
      $(this).parent("div").siblings(".revision-count").show();
      $(this).parent("div").siblings(".revision").find("div").show();
      $(this).parents(".field-wrapper").css('height', 'auto');
      var autoHeight = $(this).parents(".field-wrapper").height();
      $(this).parents(".field-wrapper").height(height).animate({"height":autoHeight},50);
    }else{
      $(this).parents(".field-wrapper").animate({"height":"35px"},50);
    }
  });
  
  /* reviewed field accordion table trigger */  
  $("#reviewed-fields .field-wrapper").click(function () { 
    var column = $(this).parent("td").attr("column");
    var row = $(this).parent("td").attr("row");  
    openCell(column,row);
  });
  
  //close all when clicking outside table
  $(document).mouseup(function (e){
    var container = $(".open");
    if (container.length !== 0 && container.has(e.target).length === 0) {
      var column = $(container).parents("td.column-open").attr("column");
      //alert(column);
      closeAll(column);
    }
  });
  
  //toggle the column when clicking on the header
  $('td.review-level').hover(function() {
    $(this).css("cursor","pointer");
  });
  $('td.review-level').click(function() {
    var column = $(this).attr("column");
    closeAll(column);
    if($(this).hasClass("column-closed")) { //is closed
      $("td[column='"+column+"']").removeClass("column-closed");
      $("td[column='"+column+"']").addClass("column-open");
    }
  });
  
  /*close all cells and hide their meta-data*/
  function closeAll(column) {
    
    $("#reviewed-fields td.field-value").attr("class","field-value row-closed column-closed");
    $("#reviewed-fields td.field-name").attr("class","field-name row-closed");
    $("#reviewed-fields td.review-level").attr("class","review-level column-closed");

    $("#reviewed-fields td[column='"+column+"']").addClass("column-open");    
    $("#reviewed-fields td[column='"+column+"']").removeClass("column-closed");    
    
    $("#reviewed-fields .empty").hide();
    $("#reviewed-fields .edit-button").hide();
    $("#reviewed-fields .edit-instruction").hide();
    $("#reviewed-fields .revision-info").hide();
    $("#reviewed-fields .revision-count").hide();
    $("#reviewed-fields .revision").hide();
    $("#reviewed-fields .field-wrapper").height("30px");
    $("#reviewed-fields .field-wrapper").removeClass("open");
    $("#reviewed-fields .current-version").show();
    $("#reviewed-fields .form-wrapper").hide();
    
    $("tr").removeClass("note-selected");
    $("#notes-wrapper .new-note").hide();
    $("#notes-wrapper .form-item").hide();
    $("#notes-wrapper .view-item").show();
  }
  
  function openCell(column,row){      
    var element = $("td[column='"+column+"'][row='"+row+"'] .field-wrapper")
    var isopen = $(element).parent("td").is(".row-open.column-open");
    if (!isopen){
      $(element).parents("td").css("outline", "none").css("cursor","default");

      /*first, close all cells */
      closeAll(column);
      /* $("td.field-value").attr("class","field-value row-closed column-closed");
      $("td.field-name").attr("class","field-name row-closed");
      $("td.review-level").attr("class","review-level column-closed");
      $(".footnote-link").hide();
      $(".revision-info").hide();
      $(".revision-count").hide();
      $(".revision").hide();
      $(".empty").hide();
      $(".edit-button").hide();
      $(".field-wrapper").removeClass("open");
      $("tr").removeClass("note-selected");
      if(window.location.hash){window.location.hash='#!';} */
      
      /* open the selected row and column */      
      $("td[column='"+column+"']").addClass("column-open");
      $("td[column='"+column+"']").removeClass("column-closed");
      $("td[row='"+row+"']").addClass("row-open");
      $("td[row='"+row+"']").removeClass("row-closed");  
      
      $("#reviewed-fields td.row-closed .field-wrapper").height("30px");       
      $("#reviewed-fields td.row-open .field-wrapper").height("auto");
      
      /*then open up the selected cell itself, and its meta-data*/    
      $(element).parent("td").attr("class","field-value row-open column-open");
      $(element).find(".footnote-link").show();
      $(element).find(".revision-info").show();
      $(element).find(".revision-count").show();
      $(element).find(".empty").show();
      $(element).find(".edit-button").show();
      $(element).addClass("open");
      
      var note = $(element).find(".footnote-link a").attr("note");
      $("#notes-wrapper tr#"+note).addClass("note-selected");

      /* animation experiments
      $("td.field-value").toggleClass("field-value row-closed column-closed",100,"easeOutExpo");  
      $("td[column='"+column+"']").toggleClass("row-closed column-open",100,"easeOutExpo");
      $("td[row='"+row+"']").toggleClass("row-open column-closed",100,"easeOutExpo");         
      $(element).parent("td").toggleClass("row-open row-closed",100);
      */
 
      /* make sure the open row's divs are full height, so we can click on them */ 
      var rowheight = $("#reviewed-fields td.row-open.column-open .field-wrapper").height();
      if(rowheight != 0){
        $("#reviewed-fields td.row-open .field-wrapper").not(".open").height(rowheight);
      }
    } 
  }
  
  /* if there is an open row, clicking outside table closes the cells */
/*   $("html").click(function() {
    if($("td.row-open").length != 0){
      var column = $("td.row-open.column-open").attr("column");
      if(typeof column == 'undefined'){
        var column = $("td.review-level.column-open").attr("column");
      }
      // first, close all cells and hide their meta-data
      $("td.field-value").attr("class","field-value row-closed column-closed");
      $("td.field-name").attr("class","field-name row-closed");
      $("td.review-level").attr("class","review-level column-closed");
      $(".footnote-link").hide();
      $(".revision-info").hide();
      $(".revision-count").hide();
      $(".revision").hide();
      $(".empty").hide();
      $(".field-wrapper").removeClass("open");
      
      // open the most recent column 
      $("td.field-value[column='"+column+"']").attr("class","field-value row-closed column-open");
      $("td.review-level[column='"+column+"']").attr("class","review-level column-open");
      $("#reviewed-fields td.row-closed .field-wrapper").height("30px"); 
    }
  });

  $("table#reviewed-fields").click(function(event){
     event.stopPropagation();
  }); */
  
  
  /* field revision box toggler */
  $(".revision-count").hover(
    function(){
      $(this).css("cursor","pointer");
    },
    function(){
      $(this).css("cursor","default");
    }
  );
  $(".revision-count").click(function(){
    if($(this).siblings(".revision").is(":visible")){
      $(this).siblings(".revision").hide();
    }else{
      $(this).siblings(".revision").show();
    }
    $(this).parent(".field-wrapper").height("auto"); 
  });
  

  /* tab swticher */
/*   $("#reviewed-fields-tabs .tab").mouseover(function() {
    $(this).css("cursor", "pointer");
  });
  $("#tab-identification").click(function() {  
    $("#reviewed-fields-tabs .tab").removeClass("active");  
    $(this).addClass("active");
    $("#identification").show();    
    $("#history").hide();   
    $("#architectural-description").hide();    
    $("#references").hide();    
  });
  $("#tab-history").click(function() {  
    $("#reviewed-fields-tabs .tab").removeClass("active");
    $(this).addClass("active");
    $("#identification").hide();   
    $("#history").show(); 
    $("#architectural-description").hide();    
    $("#references").hide();      
  });
  $("#tab-architectural-description").click(function() {  
    $("#reviewed-fields-tabs .tab").removeClass("active");
    $(this).addClass("active");
    $("#identification").hide();   
    $("#history").hide();    
    $("#architectural-description").show();    
    $("#references").hide();   
  });
  $("#tab-references").click(function() {  
    $("#reviewed-fields-tabs .tab").removeClass("active");
    $(this).addClass("active");
    $("#identification").hide();   
    $("#history").hide();    
    $("#architectural-description").hide();    
    $("#references").show();    
  }); */
    
  
  /*top block fields*/
  $("#top-block-wrapper .field-wrapper").mouseover(function () {
    $(this).find(".field-name").css("text-shadow", "0 0 0.2em #ccc").css("cursor", "pointer");
    $(this).find(".main-value").css("text-shadow", "0 0 0.2em #ccc").css("cursor", "pointer");
  });
  $("#top-block-wrapper .field-wrapper").mouseout(function () {
    $(this).find(".field-name").css("text-shadow","none");
    $(this).find(".main-value").css("text-shadow","none");
  });
  
  //show edit button on hover for top blocks
  $("#fields-1-wrapper").hover(function(){
    $(this).find(".edit-button").show();
  });
  $("#fields-1-wrapper").mouseleave(function(){
    $(this).find(".edit-button").hide();
  });
  $("#fields-2-wrapper").hover(function(){
    $(this).find(".edit-button").show();
  });  
  $("#fields-2-wrapper").mouseleave(function(){
    $(this).find(".edit-button").hide();
  });
  
  /*table block fields*/            
  $("#reviewed-fields .field-wrapper").mouseenter(function () {
    if(!$(this).hasClass("open")){
      $(this).parents("td").css("outline", "1px solid blue").css("cursor", "pointer");
      $(this).find(".empty").show();
    }
  }); 
  $("#reviewed-fields .field-wrapper").mouseleave(function () {
    $(this).parents("td").css("outline", "none");
    if(!$(this).hasClass("open")){
      $(this).find(".empty").hide();
    }
  });
  
  /*footnote hover highlighting */
  $(".footnote-link a").parents(".field-wrapper").hover(
    function() {
      //$(this).css("cursor","pointer");
      var note = $(this).find(".footnote-link a").attr("note");
      $("#notes-wrapper tr#"+note).addClass("note-selected");
    },
    function() {
      var note = $(this).find(".footnote-link a").attr("note");
      if('#'+note != window.location.hash){
        $("#notes-wrapper tr#"+note).removeClass("note-selected");
      }
    }
  );  
  $("#notes-wrapper tr").hover(
    function() {      
      var note = $(this).attr("id");            
      $(this).addClass("note-selected");
      $(".footnote-link a[note='"+note+"']").parents("td").css("outline", "1px solid black");
    },
    function() {
      var note = $(this).attr("id");
      if('#'+note != window.location.hash){
        $(this).removeClass("note-selected");
      }
      $(".footnote-link a[note='"+note+"']").parents("td").css("outline", "none");      
    }
  );
  
  $(".footnote-link a").click(function(){
    var note = $(this).attr("note");
    $("#notes-wrapper tr#"+note).addClass("note-selected");
    window.location.hash='#'+note;
  });
  $("#notes-wrapper tr").click(function(){
    var note = $(this).attr("id");
    var column = $(".footnote-link a[note='"+note+"']").parents("td").attr("column");
    var row = $(".footnote-link a[note='"+note+"']").parents("td").attr("row");
    openCell(column,row);
    $("tr").removeClass("note-selected");
    $("#notes-wrapper tr#"+note).addClass("note-selected");
    window.location.hash='#'+note;
  });
  
  /*check for note hash on page load*/
  if(window.location.hash) {
    $(window.location.hash).addClass("note-selected");
  }
  
  /* edit page swticher */
  $(".edit-button").click(function(){
/*     var hash=$(this).find("a").attr("id");
    window.location.hash='#'+hash; */
    $(".page-node #fade").show();
  });
   

}};
})(jQuery, Drupal, this, this.document);