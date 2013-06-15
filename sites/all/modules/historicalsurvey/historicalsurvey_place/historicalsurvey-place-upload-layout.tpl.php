<?php

/**
 * @file
 *
 * Available variables:
 * 
 * 
 */
  //print drupal_get_path('module', 'historicalsurvey_fields') . '/css/historicalsurvey_fields_upload.css

?> 
    <div class="grid-12 block full" id="top-upload-wrapper">   
      <div class="field-wrapper grid-12" id="field-location-wrapper">
        <?php 
          print render($content['field_location']); 
        ?>
      </div>
      <div class="field-wrapper grid-4 full" id="outer-map-wrapper">
        <?php 
            print render($content['map']);
        ?>
      </div>          
      <div class="field-wrapper grid-7 full" id="upload-header">
        <h4>Notice:</h4>
        <p>You warrant that all photographs and/or other images you submit to the Austin Historical Survey Wiki:</p>
        <ul>
          <li>1) are original to or owned by you; </li>
          <li>2) are in the public domain; or </li>
          <li>3) have been provided to you by the owner of the Material, who has granted express permission for you to submit it for publication on the Austin Historical Survey Wiki.</li>
        </ul>
        <p>You also grant permission to publish the Material on Austin Historical Survey Wiki with no monetary compensation and for an indefinite period of time. You also understand that Austin Historical Survey Wiki reserves the right to refuse any Material.</p>
      </div>
      <div class="clearfix" ></div>  
    </div>
    
    

    <?php print render($form['field_photo']); ?>
    <?php print render($form['field_document']); ?>  
    <div class="clearfix"></div>
    
  <?php 
  if (user_access('administer content types')){
    $form['additional_settings']['#type']='item';
    $form['additional_settings']['#theme_wrappers']=array('container');
    $form['additional_settings']['#attributes']=array('class'=>array('grid-12','full'));
     drupal_render($form['additional_settings']);
  }else{
    hide($form['additional_settings']);
  }
  print drupal_render_children($form);   
  ?>