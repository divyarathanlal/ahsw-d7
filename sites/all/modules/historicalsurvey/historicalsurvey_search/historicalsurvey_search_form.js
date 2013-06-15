(function ($, Drupal, window, document, undefined) {
    
  $(document).ready(function (){
    
   var blank_html;    
   $('select').focus(function() {
      blank_html = $(this).find("option[value=0]").html();
      $(this).css("color","#000");
      $(this).find("option[value=0]").html('');
   });
   $('select').blur(function() {    
      $(this).find("option[value=0]").html(blank_html);
      $(this).find("option:contains('Select')").filter(":selected").parent('select').css("color","#999");
      $(this).find("option:contains('none available')").filter(":selected").parent('select').css("color","#999");
   });
   
   $('input[type=text]').focus(function(){
     if($(this).val().indexOf('e.g.') >= 0 || $(this).val().indexOf('Find') >= 0){   
       $(this).attr('default',$(this).val());
       $(this).css("color","#000");
       $(this).attr('value','');
     }
   });
   $('input[type=text]').blur(function(){
     if($(this).val() == ''){
       $(this).attr('value',$(this).attr('default'));
       $(this).css("color","#999");
     }
   });
   
   $('.form-submit').click(function(){
     $('.search-form-cell input[type=text]').val(function(index,value){
      if(value.indexOf('e.g.') == 0){
        return '';
      }else{
        return value;
      }
     });
   });

 });

})(jQuery, Drupal, this, this.document);