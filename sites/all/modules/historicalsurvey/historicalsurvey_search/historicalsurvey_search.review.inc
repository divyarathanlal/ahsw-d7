<?php

function historicalsurvey_search_review_view($op = NULL) {
  $results = isset($_SESSION['historicalsurvey_search']['results']) ? $_SESSION['historicalsurvey_search']['results'] : FALSE;
  $ret = FALSE;
  if ($results) {
    switch ($op) {
      case 'select':
        $ret['results'] = drupal_get_form('historicalsurvey_search_review_select_form');
        break;
      case NULL:
        $ret['results'] = drupal_get_form('historicalsurvey_search_review_form');
        break;
      case 'confirm':
        $ret['results'] = drupal_get_form('historicalsurvey_search_review_confirm_form');
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


function historicalsurvey_search_review_select_form(array $form, array &$form_state) {
  $keys = isset($_SESSION['historicalsurvey_search']['keys']) ? $_SESSION['historicalsurvey_search']['keys'] : FALSE;
  $items = isset($_SESSION['historicalsurvey_search']['items']) ? $_SESSION['historicalsurvey_search']['items'] : FALSE;
  $results = isset($_SESSION['historicalsurvey_search']['results']) ? $_SESSION['historicalsurvey_search']['results'] : FALSE;

  unset($_SESSION['historicalsurvey_search']['fields_to_review']);
  unset($_SESSION['historicalsurvey_search']['level_to_review']);

  if ($results) {
    $header = array('title' => 'Places to Review');
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
  foreach ($custom_fields['reviewed_fields'] as $type_array) {
    foreach ($type_array as $machine_name => $human_name) {
      $fields_options['field_' . $machine_name] = $human_name;
    }
  }
  $review_levels = variable_get('historicalsurvey_place_review_levels', array());
  foreach ($review_levels as $key => $level) {
    $level_options[$key] = $level['label'];
  }

  $form['fields_to_review'] = array(
    '#type' => 'checkboxes',
    '#options' => $fields_options,
    '#title' => t('Select the fields you would like to review for each place'),
  );

  $form['level_to_review'] = array(
    '#type' => 'radios',
    '#options' => $level_options,
    '#title' => t('Select the review level for the above fields that you would like to review.'),
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


function historicalsurvey_search_review_select_form_submit(array $form, array &$form_state) {
  ///debug_write($form_state['values']['fields_to_review']);
  if (!isset($form_state['values']['level_to_review'])) {
    drupal_set_message('Please select at least one field and level to review.', 'error');
    drupal_goto('search/review/select');
  }

  $review_levels = variable_get('historicalsurvey_place_review_levels', array());
  $level_suffix = $review_levels[$form_state['values']['level_to_review']]['machine_suffix'];

  foreach ($form_state['values']['fields_to_review'] as $field => $selected) {
    if (!empty($selected)) {
      $fields_to_review[$field . $level_suffix] = $field . $level_suffix;
    }
  }
  if (isset($fields_to_review)) {
    $_SESSION['historicalsurvey_search']['fields_to_review'] = $fields_to_review;
    $_SESSION['historicalsurvey_search']['level_to_review'] = $form_state['values']['level_to_review'];
  }
  drupal_goto('search/review');
}



function historicalsurvey_search_review_form(array $form, array &$form_state) {
  $keys = isset($_SESSION['historicalsurvey_search']['keys']) ? $_SESSION['historicalsurvey_search']['keys'] : FALSE;
  $items = isset($_SESSION['historicalsurvey_search']['items']) ? $_SESSION['historicalsurvey_search']['items'] : FALSE;
  $results = isset($_SESSION['historicalsurvey_search']['results']) ? $_SESSION['historicalsurvey_search']['results'] : FALSE;
  $fields_to_review = isset($_SESSION['historicalsurvey_search']['fields_to_review']) ? $_SESSION['historicalsurvey_search']['fields_to_review'] : FALSE;
  $level_to_review = isset($_SESSION['historicalsurvey_search']['level_to_review']) ? $_SESSION['historicalsurvey_search']['level_to_review'] : FALSE;

  if ($fields_to_review && $level_to_review) {
    $review_levels = variable_get('historicalsurvey_place_review_levels', array());
    foreach ($review_levels as $key => $level) {
      if ($key != $level_to_review) {
        $select_options[$key] = $level['label'];
      }
    }

    $header = array('result' => 'Select All');
    $fields['fields'] = $fields_to_review;
    foreach ($results['results'] as $item) {
      $options[$item['id']] = array(
        'result' => theme('historicalsurvey_search_result', array('result' => $fields, 'item' => $items[$item['id']])),
      );
      //debug_write(theme('historicalsurvey_search_result', array('result' => $fields, 'item' => $items[$item['id']])));
      //debug_write($fields);
    }

    $form['options'] = array(
      '#type' => 'select',
      '#options' => $select_options,
      '#title' => t('Migrate selected fields to:'),
      '#description' => t('Only the fields listed under each Place below will be migrated.'),
    );
    $form['results'] = array(
      '#type' => 'tableselect',
      '#options' => $options,
      '#header' => $header,
    );
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['back'] = array(
      '#markup' => l(t('Back'), 'search/review/select'),
    );
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Migrate'),
    );
    $form['actions']['cancel'] = array(
      '#markup' => l(t('Cancel'), 'search/cancel'),
    );

    return $form;
  }
  else {
    drupal_set_message('Please select at least one field and level to review.', 'error');
    drupal_goto('search/review/select');
  }
}

/**
 * Submit callback for historicalsurvey_search_review_results_form().
 */
function historicalsurvey_search_review_form_validate(array $form, array &$form_state) {

}

/**
 * Submit callback for historicalsurvey_search_review_results_form().
 */
function historicalsurvey_search_review_form_submit(array $form, array &$form_state) {
  //put $form_state['values']['keys'] into $_SESSION['keys']
  //debug_write($form_state['values']);

  $_SESSION['historicalsurvey_search']['review'] = $form_state['values'];
  $form_state['redirect'] = 'search/review/confirm';
}


/**
 *  Page callback form for menu item `review/confirm`
 */

function historicalsurvey_search_review_confirm_form($form, &$form_state) {

  if (isset($_SESSION['historicalsurvey_search'])) {

    $no_migrate = $migrate = array();
    $markup = $markup_no = '';

    $fields_to_review = isset($_SESSION['historicalsurvey_search']['fields_to_review']) ? $_SESSION['historicalsurvey_search']['fields_to_review'] : FALSE;
    $selected = $_SESSION['historicalsurvey_search']['review']['results'];
    $selected_level_key = $_SESSION['historicalsurvey_search']['review']['options'];
    $items = $_SESSION['historicalsurvey_search']['items'];

    $review_levels = variable_get('historicalsurvey_place_review_levels', array());

    //filter out the non-applicable fields (no review level, or already at review level)
    //todo: proper theme this  
    $markup = "The following fields will be migrated to review level: <strong>" . $review_levels[$selected_level_key]['label'] . "</strong><br/>";
    foreach ($selected as $nid) {
      if (!empty($nid)) {
        $markup .= '<strong>' . $items[$nid]->title . ': </strong>';
        $field_markup = '';
        foreach ($fields_to_review as $field_name) {
          //validate field_name for if it has review levels
          $level_key = historicalsurvey_search_review_get_review_level($field_name); //returns FALSE if no review level    
          if ($level_key && $level_key != $selected_level_key) {
            $instance = field_info_instance('node', $field_name, 'place');
            $field_markup .= $instance['label'] . ', ';
            $migrate[$nid][$field_name] = $field_name;
          }
          else {
            $no_migrate[$nid][$field_name] = $field_name;
          }
        }
        $field_markup = rtrim($field_markup, ', ');
        $markup .= empty($field_markup) ? '<em>none</em>' : $field_markup;
        $markup .= '<br/>';
      }
    }
    if (!empty($no_migrate)) {
      $markup_no = "The following fields will not be migrated because they are already listed at <strong>" . $review_levels[$selected_level_key]['label'] . "</strong><br/>";
      foreach ($no_migrate as $nid => $field_array) {
        $markup_no .= '<strong>' . $items[$nid]->title . ': </strong>';
        $field_markup = '';
        foreach ($field_array as $field_name => $field_value) {
          $instance = field_info_instance('node', $field_name, 'place');
          $field_markup .= $instance['label'] . ', ';
        }

        $field_markup = rtrim($field_markup, ', ');
        $markup_no .= empty($field_markup) ? '<em>none</em>' : $field_markup;
        $markup_no .= '<br/>';
      }
    }

    $num_migrations = 0;
    foreach ($migrate as $field_array) {
      $num_migrations += count($field_array);
    }

    $form['count'] = array(
      '#weight' => 0,
      '#markup' => "<div><h2>You are migrating " . format_plural($num_migrations, '1 field', '@count fields') . "</h2></div>",
    );

    $form['actions'] = array(
      '#type' => 'actions',
      '#weight' => 1,
    );
    $form['actions']['back'] = array(
      '#markup' => l(t('Back'), 'search/review'),
    );
    if ($num_migrations > 0) {
      $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Migrate'),
      );
      $form['confirm'] = array(
        '#markup' => $markup_no . "<br/>" . $markup,
      );
    }
    else {
      $form['confirm'] = array(
        '#markup' => $markup_no,
      );
    }
    $form['actions']['cancel'] = array(
      '#markup' => l(t('Cancel'), 'search/cancel'),
    );

    $form['confirm']['migrate'] = array(
      '#type' => 'hidden',
      '#value' => $migrate,
    );

    return $form;
  }
  else {
    drupal_goto('search/cancel');
  }
}

function historicalsurvey_search_review_confirm_form_submit(array $form, array &$form_state) {
  //debug_write($form_state['values']);

  $selected_level_key = $_SESSION['historicalsurvey_search']['review']['options'];
  $items = $_SESSION['historicalsurvey_search']['items'];

  historicalsurvey_search_review_migrate($form_state['values']['migrate'], $selected_level_key, $items);

  foreach ($items as $nid => $item) {
    $goto = 'node/' . $nid;
  }
  $form_state['redirect'] = array(
    'search/cancel',
    array(
      'query' => array(
        'destination' => $goto,
      ),
    ),
  );
}


function historicalsurvey_search_review_get_review_level($field_name) {

  $review_levels = variable_get('historicalsurvey_place_review_levels', array());
  foreach ($review_levels as $level_key => $level) {
    if (strstr($field_name, $level['machine_suffix'])) {
      return $level_key;
    }
  }
  return FALSE;
}


function historicalsurvey_search_review_migrate($field_array, $level_key, $items) {

  module_load_include('inc', 'node', 'node.pages');
  global $user;

  $review_levels = variable_get('historicalsurvey_place_review_levels', array());
  $suffix_new = $review_levels[$level_key]['machine_suffix'];

  foreach ($field_array as $nid => $fields) {
    //reset
    $node = $items[$nid];
    $form_state = array();
    //debug_write($node);    

    $lang = $items[$nid]->language;
    //debug_write($form);

    $node = (array) $node;
    foreach ($fields as $field_name => $search_result) {
      //get field's current review level and base name
      $current_level_key = historicalsurvey_search_review_get_level_key($field_name);
      $field_base = str_replace($review_levels[$current_level_key]['machine_suffix'], '', $field_name);
      $field_name_new = $field_base . $suffix_new;

      //copy the data over and clear the old field    
      $field = $node[$field_name];
      if (isset($field[$lang])) {
        $field_instance = field_info_instance('node', $field_name, 'place');
        foreach ($field[$lang] as $delta => $values) {

          //initalize the new field in $node if it is not set (otherwise it wont take, for some reason)        
          if (!isset($node[$field_name_new][$lang][$delta])) {
            $node[$field_name_new][$lang][$delta] = array();
          }

          //debug_write($values);
          foreach ($values as $key => $value) {
            if (!in_array($key, array('changed', 'promoted_to_field_id', 'promoted_to_revision_id', 'promoted_by'))) {
              //create new reviewed field
              $form_state['values'][$field_name_new][$lang][$delta][$key] = $value;
              //blank out old field
              $form_state['values'][$field_name][$lang][$delta][$key] = NULL; //$field_instance['default_value'];
            }
          }
          $form_state['values'][$field_name][$lang][$delta]['notes'] = "This field's data has been promoted to the " . $review_levels[$level_key]['label'] . " level.";
        }
      }
      else {
        drupal_set_message('Warning in historicalsurvey_search_review_migrate() (historicalsurvey_search.review.inc): ' . $field_name . '=>' . $lang . ' does not exist.', 'warning');
        debug_print_backtrace();
      }
    }
    $node = (object) $node;

    $node_orig = $node;
    $form_state['values']['workbench_moderation_state_new'] = 'published';
    $form_state['values']['op'] = t('Save'); //somehow necessary
    node_object_prepare($node);
    //debug_write($form_state,'review_form');
    drupal_form_submit('place_node_form', $form_state, $node);
    //debug_write($node);

    if ($errors = form_get_errors()) {
      drupal_set_message($errors, 'error');
    }

    //get the new and old vids, and update the promoted-to fields for the revision
    if (isset($node->old_vid)) {
      $vid_old = $node->old_vid;
      $vid_new = $node->vid;
      if ($vid_new !== $vid_old) {
        foreach ($fields as $field_name => $search_result) {
          $field = field_info_field($field_name);
          $field_id = $field['id'];
          $field_array = $node_orig->$field_name;
          foreach ($field_array as $lang => $delta_array) {
            foreach ($delta_array as $delta => $row) {
              $table_revision = 'field_revision_' . $field_name;
              $update = db_update($table_revision)
                ->fields(array(
                $field_name . '_promoted_to_revision_id' => $vid_new,
                $field_name . '_promoted_to_field_id' => $field_id,
                $field_name . '_promoted_by' => $user->uid,
              ))
                ->condition('revision_id', $vid_old, '=')
                ->condition('language', $lang, '=')
                ->condition('delta', $delta, '=')
              ->execute();
            }
          }
        }
        /* $uri=entity_uri('node',$node);
         $message='Migration succeeded for <strong>'.$node->title.'</strong> ('.l('view',$uri['path'],$uri['options']).'):<br/><ul>';
         foreach($field_base_array as $field_base=>$from_to_array){
         $instance=field_info_instance('node',$field_base,'place');
         $field_label=$instance['label'];
         $message.='<li><strong>'.$field_label.' </strong>migrated from <strong>'.$from_to_array['level_from'].'</strong> to <strong>'.$review_levels[$level_key]['label'].'</strong>.';
         }
         $message.='</ul>';
         drupal_set_message($message); */
      }
      else {
        drupal_set_message('Migration failed for ' . $node->title . '(Place #' . $nid . '). Contact the site administrator', 'error');
      }
    }
  }
}


function historicalsurvey_search_review_get_level_key($field_name) {
  $review_levels = variable_get('historicalsurvey_place_review_levels', array());
  foreach ($review_levels as $key => $array) {
    if (strstr($field_name, $array['machine_suffix'])) {
      return $key;
    }
  }
}


function historicalsurvey_search_review_clear_changed(&$node, $except = array()) {
  foreach ($node as $key => $attr) {
    if (strstr($key, 'field_') && !empty($attr) && !in_array($key, $except)) {
      foreach ($attr[$node->language] as $delta => $row) {
        if (isset($row['changed'])) {
          $attr[$node->language][$delta]['changed'] = 0;
        }
      }
    }
    $node->$key = $attr;
  }
}
