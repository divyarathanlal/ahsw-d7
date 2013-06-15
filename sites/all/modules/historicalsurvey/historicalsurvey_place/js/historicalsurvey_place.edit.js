(function ($, Drupal, window, document, undefined) {
Drupal.behaviors.ahswEditPage = {attach: function (context, settings) {

  /*edit features */
  $("body").addClass("edit-mode");
  
  
  //overrides for core js
  $(".tabledrag-toggle-weight-wrapper").remove();

  
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
  $("#reviewed-fields .field-wrapper").click(function(){
    var column = $(this).parent("td").attr("column");
    var row = $(this).parent("td").attr("row");  
    openCell(column,row);
    window.location.hash='';
  });

  
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
  
  
  /*top block fields*/
  $("#top-block-wrapper .field-wrapper").mouseover(function () {
    $(this).find(".field-name").css("text-shadow", "0 0 0.2em #ccc").css("cursor", "pointer");
    $(this).find(".main-value").css("text-shadow", "0 0 0.2em #ccc").css("cursor", "pointer");
  });
  $("#top-block-wrapper .field-wrapper").mouseout(function () {
    $(this).find(".field-name").css("text-shadow","none");
    $(this).find(".main-value").css("text-shadow","none");
  });
  
  
  /*table block fields*/            
  $("#reviewed-fields .field-wrapper").mouseenter(function () {
    if(!$(this).hasClass("open")){
      $(this).parents("td").css("outline", "1px solid red").css("cursor", "pointer");
      $(this).find(".empty").show();
    }
  }); 
  $("#reviewed-fields .field-wrapper").mouseleave(function () {
    $(this).parents("td").css("outline", "none");
    if(!$(this).hasClass("open")){
      $(this).find(".empty").hide();
    }
  });
  
   //open the url hash-specified field on page load
  if(window.location.hash){   
    editReviewed($(window.location.hash));
    window.location.hash='';
  }  
  
  
  //edit-button action on reviewed fields
  $(".edit-button").hover(function(){
    $(this).css("cursor","pointer");
  });
  $(".edit-button").click(function(e){
    editReviewed(this);    
    e.stopPropagation();
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
  
  // moves field's value from reviewed field to an open unreviewed field
  function editReviewed(element){
    var row = $(element).parents("td").attr("row");
    var column = $(element).parents("td").attr("column");
    if(column != 'unreviewed'){
      alert("To edit Preservation Office or Professional level data, you must submit it at the Unreviewed level");
      var column = 'unreviewed';
    }
    
    openCell(column,row);
    
    var fieldvalue = $(element).find("textarea").val();
    var notesid = $(element).siblings(".current-version").find("span.footnote-link a").attr('note');
    
    //get notes if of unreviewed field here
    
    //var notesvalue = $("notesid").find("td.note-value .view-item").html();
    
    
    if(fieldvalue != 'none'){
      $(".row-open.column-open").find("label:contains('"+fieldvalue+"')").siblings("input").attr("checked","checked");
      $(".row-open.column-open").find("textarea").val(fieldvalue);
    }
    
    //if((typeof notesvalue != 'undefined') && (notesvalue.length!=0)){
      
      //put notesvalue into notes box here, whether there are existing notes or not
      
    //}
  }
  
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
  
  // opens cells in the reviewed fields table
  function openCell(column,row){
    var element = $("td[column='"+column+"'][row='"+row+"'] .field-wrapper");
    var isopen = $(element).parent("td").is(".row-open.column-open");
    
    if (!isopen){
      $(element).parents("td").css("outline", "none").css("cursor","default");
      
      /*first, close all cells*/
      closeAll(column);
      
      /* open the selected row and column */      
      $("#reviewed-fields td[column='"+column+"']").addClass("column-open");
      $("#reviewed-fields td[column='"+column+"']").removeClass("column-closed");
      $("#reviewed-fields td[row='"+row+"']").addClass("row-open");
      $("#reviewed-fields td[row='"+row+"']").removeClass("row-closed");  
      
      $("#reviewed-fields td.row-closed .field-wrapper").height("30px");       
      $("#reviewed-fields td.row-open .field-wrapper").height("auto");
      
      /*then open up the selected cell itself, and its meta-data*/    
      $(element).parent("td").attr("class","field-value row-open column-open");
      $(element).addClass("open");
      if(column=='unreviewed'){
        $(element).find(".current-version").hide();
        $(element).find(".edit-instruction").show();
        $(element).find(".form-wrapper").show();
              
        /*open the notes*/
        var note = $(element).find(".footnote-link a").attr("note");
        if((typeof note != 'undefined') && (note.length!=0)){
          $("#"+note).addClass("note-selected");
          $("#notes-wrapper tr#"+note+" .form-item").show();
          $("#notes-wrapper tr#"+note+" .view-item").hide();
        }else{      
          var id = $(element).find(".form-wrapper").attr("id");
          var newnoteid="#"+id.replace("edit-","note-");
          $(newnoteid).addClass("note-selected");
          if((typeof $(newnoteid) != 'undefined') && ($(newnoteid).length!=0)){
            $(newnoteid).show();
            $(newnoteid).find(".form-item").show();
          }
        }
      }else{        
        $(element).find(".revision-info").show();
        $(element).find(".empty").show();
        $(element).find(".edit-button").show();
      }
      

      /* animation experiments
      $("td.field-value").toggleClass("field-value row-closed column-closed",100,"easeOutExpo");  
      $("td[column='"+column+"']").toggleClass("row-closed column-open",100,"easeOutExpo");
      $("td[row='"+row+"']").toggleClass("row-open column-closed",100,"easeOutExpo");         
      $(element).parent("td").toggleClass("row-open row-closed",100);
      */
 
      /* make sure the open row's divs are full height, so we can click on them */ 
      if($(element).find(window.location.hash).length==0){
        var rowheight = $("#reviewed-fields td.row-open.column-open .field-wrapper").height();
        if(rowheight != 0){
          $("#reviewed-fields td.row-open .field-wrapper").not(".open").height(rowheight);
        }
      }
      
      //populate the field with the default value, in case it was changed
    }
  }
  
}};
})(jQuery, Drupal, this, this.document);