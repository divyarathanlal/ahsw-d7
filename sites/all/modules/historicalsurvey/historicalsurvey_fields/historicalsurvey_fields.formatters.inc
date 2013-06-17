<?php


/*
 *  Implements hook_field_formatter_info()
 */
function historicalsurvey_fields_field_formatter_info(){
  return array(
    'historicalsurvey_location_formatter' => array(
      'label' => t("Default"),
      'field types' => array('historicalsurvey_location'),
    ),   
    'historicalsurvey_construction_year_formatter' => array(
      'label' => t("Default"),
      'field types' => array('historicalsurvey_construction_year'),
    ),
    'historicalsurvey_text_formatter' => array(
      'label' => t("Default"),
      'field types' => array('historicalsurvey_text'),
    ),
    'historicalsurvey_term_reference_formatter' => array(
      'label' => t("Default"),
      'field types' => array('historicalsurvey_term_reference'),
    ),
    'historicalsurvey_node_reference_formatter' => array(
      'label' => t("Default"),
      'field types' => array('historicalsurvey_node_reference'),
    ),
    'historicalsurvey_user_reference_formatter' => array(
      'label' => t("Default"),
      'field types' => array('user_reference'),
    ),
    'historicalsurvey_photo_formatter' => array(
      'label' => t("Default"),
      'field types' => array('historicalsurvey_photo'),
    ),
    'historicalsurvey_document_formatter' => array(
      'label' => t("Default"),
      'field types' => array('historicalsurvey_document'),
    ),
  );
}


/*
 *  Implements hook_field_formatter_view()
 */
function historicalsurvey_fields_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display){
  $element = array();
  $output='';
  $show_meta=TRUE;  
  
  switch ($display['type']){
    case 'historicalsurvey_node_reference_formatter':   
      $element = array(
        '#theme' => 'historicalsurvey_node_reference_formatter_element_theme',
        '#items' => $items,
        '#field' => $instance,
        '#entity' => $entity,
      );
      $show_meta=FALSE;
    break;    
    case 'historicalsurvey_location_formatter':
      $element = array(
        '#theme' => 'historicalsurvey_location_formatter_element_theme',
        '#items' => $items,
        '#field' => $instance,
        '#entity' => $entity,
      );
      $show_meta=FALSE;
    break;
    case 'historicalsurvey_construction_year_formatter':    
      $element = array(
        '#theme' => 'historicalsurvey_construction_year_formatter_element_theme',
        '#items' => $items,
        '#field' => $instance,
        '#entity' => $entity,
      );
    break;
    case 'historicalsurvey_text_formatter':    
      $element = array(
        '#theme' => 'historicalsurvey_text_formatter_element_theme',
        '#items' => $items,
        '#field' => $instance,
        '#entity' => $entity,
      );
    break;
    case 'historicalsurvey_term_reference_formatter':     
      $element = array(
        '#theme' => 'historicalsurvey_term_reference_formatter_element_theme',
        '#items' => $items,
        '#field' => $instance,
        '#entity' => $entity,
      );
    break;    
    case 'historicalsurvey_photo_formatter':     
      $element = array(
        '#theme' => 'historicalsurvey_photo_formatter_element_theme',
        '#items' => $items,
        '#field' => $instance,
        '#entity' => $entity,
      );
    break;
    case 'historicalsurvey_document_formatter':     
      $element = array(
        '#theme' => 'historicalsurvey_document_formatter_element_theme',
        '#items' => $items,
        '#field' => $instance,
        '#entity' => $entity,
      );
    break;
  }  
  $element['#attached']['css'][]=drupal_get_path('module','historicalsurvey_fields') . '/css/historicalsurvey_fields.css';
  return $element;
}

