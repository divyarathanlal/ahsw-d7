(function ($, Drupal, window, document, undefined) {
Drupal.behaviors.ahswFields = {attach: function (context, settings) {



  $(document).ready(function() {
        
    //edit cells should follow height of content when manually expanding textarea boxes
    /* $(".form-wrapper").mousemove(function() {
      var height1 = $(".open > .form-wrapper > div").outerHeight();
      var height2 = $(".open .edit-instruction").outerHeight();
      $(".open").height(height1+height2);    
    }); */
    
  
    //Field Wizard    
    var wizardElement;
    
    $(".wizard-trigger").hover(function() {
      $(this).css('cursor','pointer');
    });
    
    $(".wizard-trigger").click(function() {openWizard(this);});    
    $(".field-wizard .cancel-button,.lightbox-shadow").click(function() {closeWizard();});    
    $(".field-wizard .insert-button").click(function() {insertWizardValue();});    
    $(".field-wizard select").click(function() {setWizardSelect();});
    
    function setWizardSelect() {    
      $(wizardElement).find(".select2-options").hide(); 
      var parentTID = $(wizardElement).find(".select1 select").val();
      $(wizardElement).find("#parent-"+parentTID).show();       
        
      $(wizardElement).find(".select3-options").hide(); 
      var subparentTID = $(wizardElement).find("#parent-"+parentTID+" select").val();
      $(wizardElement).find("#parent-"+subparentTID).show(); 
    }
    
    function insertWizardValue() {
      var select1ID = $(wizardElement).find(".select1").find("select").val();
      var select1Val = $(wizardElement).find(".select1").find("option[value="+select1ID+"]").text();
      
      var select2ID = $(wizardElement).find(".select2").find("#parent-"+select1ID+" select").val();      
      var select2Val = $(wizardElement).find(".select2").find("option[value="+select2ID+"]").text(); 
      
      var select3ID = $(wizardElement).find(".select3").find("#parent-"+select2ID+" select").val();      
      var select3Val = $(wizardElement).find(".select3").find("option[value="+select3ID+"]").text();  
      
      var fieldID = $(wizardElement).attr('id');  
      
      if(fieldID.indexOf('description') !== -1) { 
        if(select2Val == '') {     
          var insertVal = select1Val+": ";
        } 
        else if(select3Val == '') {
          var insertVal = select1Val+" "+select2Val+": ";
        }
        else {
          var insertVal = select1Val+" "+select2Val+": "+select3Val;
        }
      }
      else {
        if(select2Val == '') {     
          var insertVal = select1Val;
        } 
        else{
          var insertVal = select1Val+" - "+select2Val;         
        }
      }
      
      var textarea = $(wizardElement).siblings(".form-type-textarea").find("textarea");
      var currentVal=textarea.val();
      if(currentVal.length>0) {
        textarea.val(currentVal+"\n"+insertVal);
      }
      else {
        textarea.val(insertVal);
      }      
      closeWizard();
    }
    
    function openWizard(element) {
      wizardElement = $(element).parent("div").siblings(".field-wizard");    
      $(wizardElement).show();
      $('.lightbox-shadow').show();
      setWizardSelect();
    }
    
    function closeWizard() {      
      $(wizardElement).find(".select1 select").val(0);      
      $(wizardElement).find(".select2 select").val(0);
      $(wizardElement).find(".select3 select").val(0);
            
      $(wizardElement).find(".select1-options").hide(); 
      $(wizardElement).find(".select2-options").hide(); 
      $(wizardElement).find(".select3-options").hide(); 
      
      $(wizardElement).hide();
      $('.lightbox-shadow').hide();
    }
    
    
    //Construction Year Widget
    //inital state
    var mode;
    setYearForm(1);
    setYearForm(2);
    
    $("#edit-field-construction-year-1-und-0-est-act-0").change(function() {setYearForm(1);});  
    $("#edit-field-construction-year-1-und-0-est-act-1").change(function() {setYearForm(1);});  
    $("#edit-field-construction-year-1-und-0-sing-rang-0").change(function() {setYearForm(2);});  
    $("#edit-field-construction-year-1-und-0-sing-rang-1").change(function() {setYearForm(2);});  
    
    function setYearForm(mode) {
      if(mode===1){
        if($("#edit-field-construction-year-1-und-0-est-act-0").is(":checked")) {
          $(".construction-year-actual").hide();
          $(".construction-year-actual input").val("");
          $(".construction-year-estimated").show();
        }
        else {
          $(".construction-year-estimated").hide();
          $(".construction-year-estimated input").val("");
          $(".construction-year-actual").show();
        }
      }
      if(mode===2){
        if($("#edit-field-construction-year-1-und-0-sing-rang-0").is(":checked")) {
          $(".construction-year-actual-range").show();
          $(".construction-year-estimated-range").show();
        }
        else {   
          $(".construction-year-actual-range").hide();
          $(".construction-year-actual-range input").val("");
          $(".construction-year-estimated-range").hide();
          $(".construction-year-estimated-range input").val("");
        }
      }
    }
    
    
    //Designation group actions
    
    $(".designation-group-title").hover(function() {
      $(this).css("cursor","pointer");
    });
    $(".designation-group-title").click(function() {
      if($(this).siblings('div').is(':visible')) {
        $(this).siblings('div').hide();
        $(this).find('.expand').text('+');
      }
      else {
        $(".designation-group .form-radios").hide();
        $(".designation-group-title .expand").text('+');
        $(this).siblings('div').show();
        $(this).find('.expand').text('−');
      }
    });
    $(".designation-group input").click(function() {    
      var val = '';
      if($(this).val() != 0) {
        var val = " : "+$(this).siblings('label').text();
      }
      $(this).parents('.form-radios').siblings('div').find('span.value').text(val);
    });
    
  });
}};
})(jQuery, Drupal, this, this.document);