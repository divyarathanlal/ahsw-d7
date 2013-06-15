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
  
  //dw($variables['content']['field_location']['#object']);
  
  if(isset($variables['content']['field_location']['#object']->nid)) {
    $nid=$variables['content']['field_location']['#object']->nid;
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
}

/**
 *  Preprocessor for the edit form
 *  The non-form fields use same layout as historicalsurvey_fields_layout theme
 */
function historicalsurvey_place_preprocess_place_node_form(&$variables) {
  //$node_array=(array) node_load(3932);
  //dw($node_array['content']);

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
  /* $variables['form']['instructions'] = array(
    '#type' => 'item',
    '#weight' => 1,
    '#markup' => 'test',
  ); */
  $variables['form']['map'] = array(
    '#type' => 'item',
    '#theme' => 'historicalsurvey_map',
    '#redpins' => $redpin ? $redpin : array(),
    '#viewmode' => 'create',
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

  //module_load_include('inc', 'node', 'node.pages');
  foreach (array_keys(field_info_instances('node','place')) as $field_name) {
    if(!in_array($field_name, array('field_photo','field_document'))) {
      hide($variables['form'][$field_name]);
    }
  }
  
}
