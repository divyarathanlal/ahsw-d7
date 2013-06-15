<?php

function historicalsurvey_search_edit_view($op = NULL) {
  $results = isset($_SESSION['historicalsurvey_search']['results']) ? $_SESSION['historicalsurvey_search']['results'] : FALSE;
  $ret = FALSE;
  if ($results) {
    switch ($op) {
      case 'select':
        $ret['results'] = drupal_get_form('historicalsurvey_search_edit_select_form');
        break;
      case NULL:
        $ret['results'] = drupal_get_form('historicalsurvey_search_edit_form');
        break;
    }
  }
  if ($ret) {
    return $ret;
  }
  else {
    drupal_goto('search/cancel');
  }
}


function historicalsurvey_search_edit_select_form(array $form, array &$form_state) {
  $keys = isset($_SESSION['historicalsurvey_search']['keys']) ? $_SESSION['historicalsurvey_search']['keys'] : FALSE;
  $items = isset($_SESSION['historicalsurvey_search']['items']) ? $_SESSION['historicalsurvey_search']['items'] : FALSE;
  $results = isset($_SESSION['historicalsurvey_search']['results']) ? $_SESSION['historicalsurvey_search']['results'] : FALSE;

  unset($_SESSION['historicalsurvey_search']['fields_to_edit']);

  if ($results) {
    $header = array('title' => 'Places to Edit');
    foreach ($results['results'] as $item) {
      $uri = entity_uri('node', $items[$item['id']]);
      $rows[$item['id']] = array(
        'title' => l($items[$item['id']]->title, $uri['path'], $uri['options']),
      );
    }
  }
  else {
    drupal_goto('search/cancel');
  }

  $form['results'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
  );

  $custom_fields = variable_get('historicalsurvey_place_custom_fields', array());
  foreach ($custom_fields as $level_array) {
    foreach ($level_array as $type_array) {
      foreach ($type_array as $machine_name => $human_name) {
        $fields_options['field_' . $machine_name] = $human_name;
      }
    }
  }

  $form['fields_to_edit'] = array(
    '#type' => 'checkboxes',
    '#options' => $fields_options,
    '#title' => t('Select the fields you would like to edit for each place'),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'search/cancel'),
  );
  return $form;
}


function historicalsurvey_search_edit_select_form_submit(array $form, array &$form_state) {
  $custom_fields = variable_get('historicalsurvey_place_custom_fields', array());
  foreach ($form_state['values']['fields_to_edit'] as $field => $selected) {
    if (!empty($selected)) {
      //set reviewed fields to unreviewed level as default
      if ($info = historicalsurvey_fields_info_field_no_review($field, '1')) {
        $fields_to_edit[str_replace('field_', '', $field)] = $info['field_name'];
      }
    }
  }
  if (isset($fields_to_edit)) {
    $_SESSION['historicalsurvey_search']['fields_to_edit'] = $fields_to_edit;
  }
  drupal_goto('search/edit');
}


function historicalsurvey_search_edit_form(array $form, array &$form_state) {
  $keys = isset($_SESSION['historicalsurvey_search']['keys']) ? $_SESSION['historicalsurvey_search']['keys'] : FALSE;
  $items = isset($_SESSION['historicalsurvey_search']['items']) ? $_SESSION['historicalsurvey_search']['items'] : FALSE;
  $results = isset($_SESSION['historicalsurvey_search']['results']) ? $_SESSION['historicalsurvey_search']['results'] : FALSE;
  $fields_to_edit = $_SESSION['historicalsurvey_search']['fields_to_edit'];

  if ($fields_to_edit) {

    /*   $custom_fields=variable_get('historicalsurvey_place_custom_fields',array());
     $header=array('title'=>'Place');
     foreach($fields_to_edit as $field=>$field_with_review){
     $info=field_info_field($field_with_review);
     //$header[$field_with_review]=($field==$field_with_review)?$custom_fields['unreviewed_fields'][$info['type']][$field]:$custom_fields['reviewed_fields'][$info['type']][$field];
     } */

    $form['rows'] = array('#tree' => TRUE);
    module_load_include('inc', 'node', 'node.pages');
    foreach ($results['results'] as $item) {
      $form['rows'][$item['id']] = array('#tree' => TRUE);
      $place_form = drupal_get_form('place_node_form', $items[$item['id']]);
      //debug_write($place_form);
      //$uri=entity_uri('node', $items[$item['id']]);
      /* $form['rows'][$item['id']]['title']=array(
       '#markup'=>l($items[$item['id']]->title,$uri['path'],$uri['options']),
       '#weight'=>-99,
       ); */
      foreach ($fields_to_edit as $field_with_review) {
        $form['rows'][$item['id']][$field_with_review] = historicalsurvey_search_get_field_widget($field_with_review, $items[$item['id']], $place_form);
      }
    }

//    debug_write($form);
    /*     $form['rows'][1]['field_test']=array(
     '#type'=>'textarea'
     );
     $form['rows'][2]['field_test']=array(
     '#type'=>'textarea'how do i purrchase a money order

     ); */

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['back'] = array(
      '#markup' => l(t('Back'), 'search/edit/select'),
    );
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit Edits'),
    );
    $form['actions']['cancel'] = array(
      '#markup' => l(t('Cancel'), 'search/cancel'),
    );

    return $form;
  }
  else {
    drupal_set_message('Please select at least one field to edit.', 'error');
    drupal_goto('search/edit/select');
  }
}


function historicalsurvey_search_edit_form_submit(array $form, array &$form_state) {

  module_load_include('inc', 'node', 'node.pages');
  $items = isset($_SESSION['historicalsurvey_search']['items']) ? $_SESSION['historicalsurvey_search']['items'] : FALSE;

  foreach ($form_state['values']['rows'] as $nid => $value_array) {
    $lang = $items[$nid]->language;
    foreach ($value_array as $field_name => $field_values) {

      $submit['values'] = $value_array;
      $submit['values']['workbench_moderation_state_new'] = 'published';
      $submit['values']['op'] = t('Save'); //somehow necessary

      $node = $items[$nid];
      node_object_prepare($node);

      drupal_form_submit('place_node_form', $submit, $node);

      if ($errors = form_get_errors()) {
        drupal_set_message('<pre>' . print_r($errors, true) . '</pre>', 'error');
      }
    }
  }
  drupal_goto('search/cancel');
}


function historicalsurvey_search_get_field_widget($field_name, $entity, $form) {
  $field = field_info_field($field_name);
  $instance = field_info_instance('node', $field_name, 'place');
  $lang = $entity->language;
  $items = isset($entity->{$field_name}[$lang]) ? $entity->{$field_name}[$lang] : NULL;
  $form_state = array();

  $widget = field_default_form('node', $entity, $field, $instance, $lang, $items, $form, $form_state);
  return $widget[$field_name];
}
