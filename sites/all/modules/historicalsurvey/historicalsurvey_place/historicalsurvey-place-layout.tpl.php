<?php

/**
 * @file
 * HTML grid layout of fields in a place record, inlcuding The Table; accomodates view and edit modes
 *
 * Available variables:
 * - $content: the field content as it would be if it went straight to node.tpl.php
 * - $table_order: the ordering of fields in the table, set in historicalsurvey_fields_get_table_order()
 * - $op: either view or edit
 * 
 * - if in edit mode, we also have $form: the place edit form
 * 
 */
 
 
  global $base_url;
  $nid=is_numeric(arg(1))?arg(1):'';

  //JQuery UI Effects core, for animation functions
  //drupal_add_library('system', 'effects');   global $user;
  if($op=='edit' && !user_access("edit any place content")) {
    drupal_goto('admin');  //temp
  }
  if($op=='view'){drupal_add_js(drupal_get_path('module', 'historicalsurvey_place').'/js/historicalsurvey_place.view.js');}
  if($op=='edit'){drupal_add_js(drupal_get_path('module', 'historicalsurvey_place').'/js/historicalsurvey_place.edit.js');}

  //dw(array_keys($content));
  
?>
<div class="lightbox-shadow"></div>
<div class="grid-12 full" id="top-block-wrapper"> 

  <!-- Unreviewed Fields, row 1 -->
  <div class="grid-9 block full" id="fields-1-wrapper"> 
    <div class="field-wrapper grid-9" id="field-location-wrapper">
      <div class="edit-button"><a href="<?php print $base_url . '/place/' . arg(1) . '/edit' ?>"><img src="<?php print $base_url.'/'.drupal_get_path('module','historicalsurvey_place') ?>/img/pencil.png"></a></div>
      <?php 
        if($op=='edit') {  
          print render($form['field_location']); //edit mode only
        }
        else {
          print render($content['field_location']); 
        }        
      ?>
    </div>
    <div class="field-wrapper grid-3 full" id="outer-map-wrapper">
      <?php 
        if($op=='edit') {  
          print render($form['map']);
        }
        else {
          print render($content['map']);
        }
      ?>
    </div>
    <div class="field-wrapper grid-3" id="outer-photo-wrapper">
      <?php      
        if($op=='edit') {
          print "<div id='edit-add-files-link'>".l("Add/Edit Photos and Documents",'place/'.$nid.'/upload')."</div>";
        }   
        else{
          print render($content['field_photo']);
        }
      ?>
    </div>
    <div class="field-wrapper grid-3" id="outer-document-wrapper">
      <?php 
        if($op=='edit') {   
        }
        else{
          print render($content['field_document']);
        }
      ?>
    </div>
    <div class="clear" ></div>
  </div>    
  
  <!-- Guide -->
  <div class="block grid-3 full-right" id="guide-wrapper">
    <div>Guide</div>
    <div class="clear" ></div>
  </div>
  
  <!-- Unreviewed Fields, row 2 -->
  <div class="grid-9 block full" id="fields-2-wrapper">   

    <div class="field-wrapper grid-5" id="field-designation-official-wrapper">    
      <?php 
        if($op=='edit' && user_access('administer content types')) {  
          print render($form['field_designation_official']);
        }
        else{
          print render($content['field_designation_official']);
        }
      ?>
    </div>
    <div class="field-wrapper grid-4" id="field-survey-effort-tag-wrapper">
      <div class="edit-button"><a href="<?php print $base_url . '/place/' . arg(1) . '/edit' ?>"><img src="<?php print $base_url.'/'.drupal_get_path('module','historicalsurvey_place') ?>/img/pencil.png"></a></div>
      <?php 
        if($op=='edit') {  
          $content['field_survey_effort_tag']['#form_element_html']=drupal_render($form['field_survey_effort_tag']);
        }
        
        print render($content['field_survey_effort_tag']); 
      ?>
    </div><div class="field-wrapper grid-4" id="field-tag-wrapper">
      <?php 
        if($op=='edit') {  
          $content['field_tag']['#form_element_html']=drupal_render($form['field_tag']);
        }
        print render($content['field_tag']); 
      ?>
    </div>
    <div class="clear" ></div>
  </div>
  
</div>

<!-- Reviewed Fields -->
<div class="grid-12 block full" id="reviewed-fields-wrapper"> 
  <div class="table-wrapper" id="<?php //print $section_id; ?>"><table id="reviewed-fields">
  <col><col><col><col>
    <thead><tr>
      <td class="field-name"><div class="grid-2">Review Level</div></td>
      <td class="review-level column-closed" column="preservation-office-review"><div class="grid-1">Preservation Office</div></td>
      <td class="review-level column-closed" column="professional-review"><div class="grid-1">Professional</div></td>
      <td class="review-level column-open" column="unreviewed"><div class="grid-1">Unreviewed</div></td>
    </tr></thead>
    <tbody>
    <?php foreach($table_order as $field_machine => $field_label): ?>
      <tr>
        <td class="field-name row-closed" row="<?php print $field_machine; ?>"><div class="grid-2"><?php print $field_label; ?></div></td>       
        <td class="field-value row-closed column-closed" column="preservation-office-review" row="<?php print $field_machine; ?>">
          <div class="fuzz"></div><div class="field-wrapper grid-3 full">
          <?php 
            $content['field_'.$field_machine.'_3']['#op']=$op;
            if($op=='edit'){
              $lang=$form['field_'.$field_machine.'_3']['#language'];
              $max_delta=isset($form['field_'.$field_machine.'_1'][$lang]['#max_delta'])?$form['field_'.$field_machine.'_1'][$lang]['#max_delta']:0;
              for($delta=0;$delta<=$max_delta;$delta++){
                hide($form['field_'.$field_machine.'_3'][$lang][$delta]['notes']); //we print notes separately
              }          
              if (user_access('administer content types')){
                //$content['field_'.$field_machine.'_3']['#form_element_html']=drupal_render($form['field_'.$field_machine.'_3']);
                $content['field_'.$field_machine.'_3']['#form_element_html']='<div id="edit-field-'.str_replace("_","-",$field_machine).'-3" class="form-wrapper"></div>';
                hide($form['field_'.$field_machine.'_3']); //not allowed to edit
              }else{
                hide($form['field_'.$field_machine.'_3']); //not allowed to edit
              }
            }
            print render($content['field_'.$field_machine.'_3']); 
          ?>
          </div>
        </td>
        <td class="field-value row-closed column-closed" column="professional-review" row="<?php print $field_machine; ?>">  
          <div class="fuzz"></div><div class="field-wrapper grid-3 full">
          <?php                 
            $content['field_'.$field_machine.'_2']['#op']=$op;
            if($op=='edit'){
              $lang=$form['field_'.$field_machine.'_2']['#language'];
              $max_delta=isset($form['field_'.$field_machine.'_1'][$lang]['#max_delta'])?$form['field_'.$field_machine.'_1'][$lang]['#max_delta']:0;
              for($delta=0;$delta<=$max_delta;$delta++){
                hide($form['field_'.$field_machine.'_2'][$lang][$delta]['notes']); //we print notes separately
              }
              if (user_access('administer content types')){
                //$content['field_'.$field_machine.'_2']['#form_element_html']=drupal_render($form['field_'.$field_machine.'_2']);
                $content['field_'.$field_machine.'_2']['#form_element_html']='<div id="edit-field-'.str_replace("_","-",$field_machine).'-2" class="form-wrapper"></div>';
                hide($form['field_'.$field_machine.'_2']); //not allowed to edit
              }else{
                hide($form['field_'.$field_machine.'_2']); //not allowed to edit
              }
            }
            print render($content['field_'.$field_machine.'_2']); ?>
          </div>
        </td>
        <td class="field-value row-closed column-open" column="unreviewed" row="<?php print $field_machine; ?>"> 
          <div class="fuzz"></div><div class="field-wrapper grid-3 full">
          <?php                 
            $content['field_'.$field_machine.'_1']['#op']=$op;
            if($op=='edit'){
              $lang=$form['field_'.$field_machine.'_1']['#language'];
              $max_delta=isset($form['field_'.$field_machine.'_1'][$lang]['#max_delta'])?$form['field_'.$field_machine.'_1'][$lang]['#max_delta']:0;
              for($delta=0;$delta<=$max_delta;$delta++){
                hide($form['field_'.$field_machine.'_1'][$lang][$delta]['notes']); //we print notes separately
              }
              $content['field_'.$field_machine.'_1']['#form_element_html']=drupal_render($form['field_'.$field_machine.'_1']);
            }
            print render($content['field_'.$field_machine.'_1']); 
          ?>
          </div>
        </td>
      </tr>
    <?php endforeach; ?>
    </tbody>
  </table></div>
</div>

<!-- Notes -->
<div class="grid-12 block full" id="notes-wrapper">
  <?php print drupal_render($content['notes']);?>
</div>

<?php hide($form['field_photo']);?>
<?php hide($form['field_document']);?>

<?php if($op=='edit'): ?>
  <?php 
    hide($form['field_contains']);
    hide($form['field_survey_effort']);
    if (user_access('administer content types')){
      $form['additional_settings']['#type']='item';
      $form['additional_settings']['#theme_wrappers']=array('container');
      $form['additional_settings']['#attributes']=array('class'=>array('grid-12','full'));
      print drupal_render($form['additional_settings']);
    }else{
      hide($form['additional_settings']);
    }
    print drupal_render_children($form);
  ?>
<?php endif; ?>
  
  