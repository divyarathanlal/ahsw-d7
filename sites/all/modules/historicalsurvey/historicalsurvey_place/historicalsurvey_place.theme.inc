<?php

/**
 *  Implements hook_theme()
 *  Defines theme for Place view page and Place Edit form
 */
function historicalsurvey_place_theme($existing, $type, $theme, $path) {

  return array(
    //theme hook called by historicalsurvey_place_preprocess_node() for Place View page
    'historicalsurvey_place_layout' => array(
      'template' => 'historicalsurvey-place-layout',
      'variables' => array(
        'content' => array(),
      ),
    ),
    
    //overriding default place_node_form theme hook for Place Edit form
    'place_node_form' => array(
      'template' => 'historicalsurvey-place-layout',
      'render element' => 'form',
    ),    
    'place_node_upload_form' => array(
      'template' => 'historicalsurvey-place-upload-layout',
      'render element' => 'form',
    ),
    
    'historicalsurvey_guide_block_theme' => array(
      'variables' => array(
        'context' => array(),
      ),
    ),
  );
}


/**
 *  Preprocessor for node--place.tpl.php
 *  Creates $fields_layout variable for display of all place record fields
 */
function historicalsurvey_place_preprocess_node(&$variables, $hook) {
  $variables['fields_layout'] = array(
    '#theme' => 'historicalsurvey_place_layout',
    '#content' => $variables['content'],
  );
}


/**
 *  Preprocessor for the grid layout of fields, which is called by node--place.tpl.php via drupal_render
 */
function historicalsurvey_place_preprocess_historicalsurvey_place_layout(&$variables) {

  historicalsurvey_fields_content_insert_notes($variables);
  $variables['table_order'] = historicalsurvey_fields_get_table_order();
  $variables['op'] = 'view';
  
  //Text for the header bar, if the Place is not published
  if(isset($variables['content']['field_location']['#object']->nid)) {
    $nid=$variables['content']['field_location']['#object']->nid;
    $current_state = $variables['content']['field_location']['#object']->workbench_moderation['current']->state;
    
    if($current_state == 'in_moderation') {
      $variables['current_state'] = '<div id="current-state" class="in-moderation">This Place is currently in the moderation queue.</div>';
    }
    elseif($current_state == 'draft') {
      $variables['current_state'] = '<div id="current-state" class="draft">This Place is currently in Draft.</div>';
    }
    else{
      $variables['current_state']=FALSE;
    }
    
  }else{
    drupal_goto('');
  }
  
  //add the map  
  $locations = historicalsurvey_maps_get_locations(array($nid));
  $redpin[] = array(
    'lng' => $locations[$nid]['lng'],
    'lat' => $locations[$nid]['lat'],
    'location' => $locations[$nid],
  );

  $variables['content']['map'] = array(
    '#theme' => 'historicalsurvey_map',
    '#redpins' => $redpin ? $redpin : array(),
    '#viewmode' => 'mini',
  );
  
  //add the guide  
  $variables['content']['guide'] = array(
    '#theme' => 'historicalsurvey_guide_block_theme',
    '#context' => 'view',
  );
}

/**
 *  Preprocessor for the edit form
 *  The non-form fields use same layout as historicalsurvey_fields_layout theme
 */
function historicalsurvey_place_preprocess_place_node_form(&$variables) {

  /* for non-form fields */
  $variables['content'] = isset($variables['form']['#node']) ? node_view($variables['form']['#node']) : array();

  historicalsurvey_fields_content_insert_notes($variables);

  /* for the form fields */
  historicalsurvey_fields_form_insert_notes($variables);

  $variables['table_order'] = historicalsurvey_fields_get_table_order();
  $variables['op'] = 'edit';

  /*form mods*/
  unset($variables['form']['actions']['preview_changes']);
  
  //edit map
  $vids=array_keys($variables['content']['field_location']['#items']['revisions']);
  $vid = reset($vids);

  $redpin[] = array(
    'lng' => $variables['content']['field_location']['#items']['revisions'][$vid][0]['x'],
    'lat' => $variables['content']['field_location']['#items']['revisions'][$vid][0]['y'],
  );  
  
  $variables['form']['map'] = array(
    '#type' => 'item',
    '#theme' => 'historicalsurvey_map',
    '#redpins' => $redpin ? $redpin : array(),
    '#viewmode' => 'create',
  );  

  //Text for the header bar, if the Place is not published
  $current_state = $variables['content']['field_location']['#object']->workbench_moderation['current']->state;
  if($current_state == 'in_moderation') {
    $variables['current_state'] = '<div id="current-state" class="in-moderation">This Place is currently in the moderation queue.</div>';
  }
  elseif($current_state == 'draft') {
    $variables['current_state'] = '<div id="current-state" class="draft">This Place is currently in Draft.</div>';
  }
  else{
    $variables['current_state']=FALSE;
  }
  
  //add the guide  
  $variables['content']['guide'] = array(
    '#theme' => 'historicalsurvey_guide_block_theme',
    '#context' => 'edit',
  );
}

/**
 *  Preprocessor for the file upload form
 */
function historicalsurvey_place_preprocess_place_node_upload_form(&$variables) {

  $variables['content'] = isset($variables['form']['#node']) ? node_view($variables['form']['#node']) : array();  
  $vids=array_keys($variables['content']['field_location']['#items']['revisions']);
  $vid = reset($vids);
  $redpin[] = array(
    'lng' => $variables['content']['field_location']['#items']['revisions'][$vid][0]['x'],
    'lat' => $variables['content']['field_location']['#items']['revisions'][$vid][0]['y'],
  );  
  $variables['content']['map'] = array(
    '#type' => 'item',
    '#theme' => 'historicalsurvey_map',
    '#redpins' => $redpin ? $redpin : array(),
    '#viewmode' => 'mini',
  );

  foreach (array_keys(field_info_instances('node','place')) as $field_name) {
    if(!in_array($field_name, array('field_photo','field_document'))) {
      hide($variables['form'][$field_name]);
    }
  }
}

/*  Theme function for guide block*/
function theme_historicalsurvey_guide_block_theme($variables) {
  $html = '<p class="title">Tips</p>';
  
  if($variables['context'] == 'view') {
    $pencil = theme('image',array('path'=>drupal_get_path('module', 'historicalsurvey_place')."/img/pencil.png"));
    
    $html .= '<p>The edit button '.$pencil.' will show up when hovering over each field. However, you must be logged-in to edit.';
    
    $html .= '<p>Each field in the table below can contain data at three different review levels:</p>';
    
    $html .= '<p><strong>Unreviewed</strong> data has been submitted by the general public.</p>';
    $html .= '<p><strong>Professional</strong> data has been submitted or promoted by a professional in architectural history and/or preservation.</p>';
    $html .= '<p><strong>Preservation Office</strong> data has been submitted or promoted by the City of Austin\'s '.l("Historic Preservation Office","http://austintexas.gov/department/historic-preservation").'</p>';
  }
  elseif($variables['context'] == 'edit'){
    $html .= "<p>Use the text fields and select buttons in each field to contribute information about this place. You can only submit data at the Unreviewed level.</p>";
    
    $html .= "<p>Be sure to add notes and sources for your information. Textboxes for this will show up at the bottom of the page when you click to edit each field.</p>";
    
    $html .= "<p>Use the link to the left to add and edit data about Photos and Documents.</p>";
    
    $html .= "<p>Your contributions will be submitted to the moderation queue where we only check for spam and blatently inappropriate submissions before publishing.</p>";
  }
  $html .= "<p class='more-link'>View more tips at the ".l("User Guide","guide").'</p>';
  
  return $html;
}
