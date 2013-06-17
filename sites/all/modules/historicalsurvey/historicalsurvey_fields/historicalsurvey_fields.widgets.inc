<?php


/*
 *  Implements hook_field_widget_info()
 */
function historicalsurvey_fields_field_widget_info(){
  return array(
    'historicalsurvey_location_widget' => array(
      'label' => t("HistoricalSurvey Location Widget"),
      'field types' => array('historicalsurvey_location'),
    ),
    'historicalsurvey_construction_year_widget' => array(
      'label' => t("HistoricalSurvey Construction Year Widget"),
      'field types' => array('historicalsurvey_construction_year'),
    ),
    'historicalsurvey_text_widget' => array(
      'label' => t("HistoricalSurvey Text Widget"),
      'field types' => array('historicalsurvey_text'),
    ),
    'historicalsurvey_term_reference_widget' => array(
      'label' => t("HistoricalSurvey Term Reference Widget"),
      'field types' => array('historicalsurvey_term_reference'),
    ), 
    'historicalsurvey_node_reference_widget' => array(
      'label' => t("HistoricalSurvey Node Reference Widget"),
      'field types' => array('historicalsurvey_node_reference'),
    ), 
    'historicalsurvey_user_reference_widget' => array(
      'label' => t("HistoricalSurvey User Reference Widget"),
      'field types' => array('historicalsurvey_user_reference'),
    ), 
    'historicalsurvey_photo_widget' => array(
      'label' => t("HistoricalSurvey Photo Widget"),
      'field types' => array('historicalsurvey_photo'),
    ), 
    'historicalsurvey_document_widget' => array(
      'label' => t("HistoricalSurvey Document Widget"),
      'field types' => array('historicalsurvey_document'),
    ),
  );
}


/**
 *  Implements hook_field_widget_info_alter().
 */
function historicalsurvey_fields_field_widget_info_alter(&$info) {
  // Migrate image and file settings to custom photo and document fields
  $info['historicalsurvey_photo_widget']['settings']=$info['image_image']['settings'];
  $info['historicalsurvey_photo_widget']['behaviors']=$info['image_image']['behaviors'];
  
  $info['historicalsurvey_document_widget']['settings']=$info['file_generic']['settings'];
  $info['historicalsurvey_document_widget']['behaviors']=$info['file_generic']['behaviors'];  
}


/**
  *   Implements hook_menu()
  *   For autocomplete fields
  */
function historicalsurvey_fields_menu() {
  $items['historicalsurvey_fields/autocomplete'] = array(
    'page callback' => 'historicalsurvey_fields_autocomplete',
    'access arguments' => array('access content'),
    'type' => MENU_CALLBACK
  );
  return $items;
}

/**
  *   Page callback
  *   For autocomplete fields, based on taxonomy autocomplete function
  */
function historicalsurvey_fields_autocomplete($field_name='',$tags_typed='') {

  $tags_typed = drupal_explode_tags($tags_typed);
  $tag_last = drupal_strtolower(array_pop($tags_typed));
  
  $term_matches = array();
  if ($tag_last != '') {
    
    switch ($field_name) {
      case "field_survey_effort_tag":
        $vocab = 'value_survey_effort';
      break;
      case "field_tag":
        $vocab = 'value_tag';        
      break;
    }
    $vid = taxonomy_vocabulary_machine_name_load($vocab)->vid;
    $query = db_select('taxonomy_term_data', 't');

    // Do not select already entered terms.
    if (!empty($tags_typed)) {
      $query->condition('t.name', $tags_typed, 'NOT IN');
    }
    
    // Select rows that match by term name.
    $tags_return = $query
      ->fields('t', array('tid', 'name'))
      ->condition('t.vid', $vid)
      ->condition('t.name', '%' . db_like($tag_last) . '%', 'LIKE')
      ->range(0, 10)
      ->execute()
      ->fetchAllKeyed();

    $prefix = count($tags_typed) ? drupal_implode_tags($tags_typed) . ', ' : '';

    foreach ($tags_return as $tid => $name) {
      $n = $name;
      
      // Term names containing commas or quotes must be wrapped in quotes.
      if (strpos($name, ',') !== FALSE || strpos($name, '"') !== FALSE) {
        $n = '"' . str_replace('"', '""', $name) . '"';
      }
      $term_matches[$prefix . $n] = check_plain($name);
    }
  }

  drupal_json_output($term_matches);
}

/**
 * Form Validate Function 
 * for term autocomplete element, based on taxonomy autocomplete validate
 */
function historicalsurvey_fields_autocomplete_validate($element, &$form_state) {

  // Autocomplete widgets do not send their tids in the form, so we must detect
  // them here and process them independently.
  $value = array();
  $field_name = $element['#parents'][0]; 
  $tags = $element['#value'];
     
    switch ($field_name) {
      case "field_survey_effort_tag":
        $vocab = 'value_survey_effort';
      break;
      case "field_tag":
        $vocab = 'value_tag';        
      break;
    }
    $vid = taxonomy_vocabulary_machine_name_load($vocab)->vid;

    // Translate term names into actual terms.
    $typed_terms = drupal_explode_tags($tags);
    if($tags != $element['#default_value']) {
      if(!empty($typed_terms)) {
        foreach ($typed_terms as $typed_term) {
        
          // See if the term exists in the chosen vocabulary and return the tid;
          // otherwise, create a new 'autocreate' term for insert/update.
          if ($possibilities = taxonomy_term_load_multiple(array(), array('name' => trim($typed_term), 'vid' => array($vid)))) { 
            $tids[]  = array(
              'tid' => array_pop($possibilities)->tid,
              'changed' => 1
            );
          }
          else {
            $term = array(
              'vid' => $vid,
              'name' => $typed_term,
            );
            $term = (object) $term;
            taxonomy_term_save($term);
            $tids[] = array(
              'tid' => $term->tid,
              'changed' => 1
            );
          }
        }
      }
      else {
        $tids[] = array(
          'tid' => NULL,
          'changed' => 1,
        );
      }
    }
    else {
      $tids[]=array('changed' => 0);
    }
  
  drupal_array_set_nested_value($form_state['values'], array($field_name,'und'), $tids, TRUE);
}


/*
 *  Implements hook_field_widget_form.
 */
function historicalsurvey_fields_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $field_name=$field['field_name'];
  $node=$element['#entity'];
  $nid=isset($node->nid)?$node->nid:FALSE;
  $current_field_values=FALSE;
  $has_review=FALSE;
  $has_replaced=TRUE;
  $has_changed=TRUE;
  $has_meta=TRUE;  

  if(isset($node->{$field_name}[$langcode][$delta])) {
    $items[$delta] = $node->{$field_name}[$langcode][$delta];
  }
  elseif(isset($node->{$field_name}[$langcode]['revisions'])) {
    $latest = reset($node->{$field_name}[$langcode]['revisions']);    
    $items[$delta]=isset($latest[$delta])?$latest[$delta]:NULL;
  }
  else{
    $items[$delta]=NULL;
  }
  
  switch ($instance['widget']['type']){
    case 'historicalsurvey_location_widget':
      $elements = array(
        '#type' => 'historicalsurvey_location_widget_element',
        '#title' => 'Location of Place',
        '#default_value'=>$items[$delta]?$items[$delta]:NULL,
      );
      $has_replaced=FALSE;
      break;
    case 'historicalsurvey_construction_year_widget':
      $elements = array(
        '#type' => 'historicalsurvey_construction_year_widget_element',
        '#title' => 'Construction Year',
        '#default_value'=>$items[$delta]?$items[$delta]:NULL,
      );
      $has_review=TRUE;
      break;
    case 'historicalsurvey_text_widget':      
      if(strpos($field['field_name'],'style') || strpos($field['field_name'],'use') || strpos($field['field_name'],'description')){
      
        //use field wizard
        $elements = array(
          '#type' => 'historicalsurvey_text_widget_element',
          '#default_value'=>isset($items[$delta])?$items[$delta]:NULL,        
          '#rows' => isset($field['settings']['rows'])?$field['settings']['rows']:1,
          '#attributes' => array('class' => array('text-full')),
        );
      }
      else {
        $elements['value'] = array(
          '#type' => 'textarea', 
          '#default_value' => $items[$delta]['value']?$items[$delta]['value'] : NULL, 
          '#rows' => isset($field['settings']['rows'])?$field['settings']['rows']:1,
          '#attributes' => array('class' => array('text-full')),
        );  
        $elements['notes'] = array(
          '#type' => 'textarea',
          '#default_value'=>isset($items[$delta]['notes'])?$items[$delta]['notes']:NULL,
          '#weight' => 99,
        );
        }
      break;
    case 'historicalsurvey_term_reference_widget':
    
      //we dont have meta data for designations, also, we have a custom widget
      if(strpos($field['field_name'],'designation')){
      
        //group the designation options by designation type
        $vocabulary = taxonomy_vocabulary_machine_name_load('value_designation');
        $vid=$vocabulary->vid;
        $top_level = taxonomy_get_tree($vid, $parent = 0, $max_depth = 1);
        foreach ($top_level as $id => $term) {
          $options[$term->name] = array();
          $children = taxonomy_get_tree($vid, $term->tid);
          foreach ($children as $child) {
            $options[$term->name][$child->tid] = $child->name;
          }
        }
        
        $default_values=FALSE;
        if(!empty($items['revisions'])) {
          $default_values = reset($items['revisions']);
        }
        
        $elements['tid'] = array(
          '#type' => 'historicalsurvey_designation_widget_element',
          '#default_value' => $default_values?$default_values:array(),
          '#options' => $options,
        );
        $has_meta=FALSE;
      }
      elseif($field['field_name'] == 'field_tag' || $field['field_name'] == 'field_survey_effort_tag'){

        $terms='';
        if(!empty($items['revisions'])) {
          foreach($items['revisions'] as $deltas) {
            foreach($deltas as $value_array) {
              if(!empty($value_array['tid'])) {
                $terms .= taxonomy_term_load($value_array['tid'])->name . ', ';
              }
            }
          }
          $terms = rtrim($terms,', '); 
        }
      
        $elements['tid'] = array(
          '#type' => 'historicalsurvey_autocomplete_term_reference_widget_element',
          '#default_value' => $terms,
        );
        $has_meta=FALSE;
      }
      else{
        $elements['tid'] = array(
          '#type' => 'radios',
          '#default_value' => $items[$delta]['tid']?$items[$delta]['tid']:array(),
          '#options' => historicalsurvey_fields_term_options_array($field['settings']['allowed_values'][0]['vocabulary']),
          '#description' => historicalsurvey_fields_term_description($field['field_name']),
        );
        $elements['notes'] = array(
          '#type' => 'textarea',
          '#default_value'=>isset($items[$delta]['notes'])?$items[$delta]['notes']:NULL,
          '#weight' => 99,
        );          
        $has_review=TRUE;
      }
      if(strpos($field['field_name'],'place_type')){
        $has_replaced=FALSE;
      }
      break;
    case 'historicalsurvey_node_reference_widget':
      $elements['nid'] = array(
        '#type' => 'textfield',
        '#default_value' => $items[$delta]['nid'] ? $items[$delta]['nid'] : NULL,
        '#size' => $instance['widget']['settings']['size'],
      );
      $elements['notes'] = array(
        '#type' => 'textarea',
        '#default_value'=>isset($items[$delta]['notes'])?$items[$delta]['notes']:NULL,
        '#weight' => 99,
      );
      break;
    case 'historicalsurvey_user_reference_widget':
      $elements['uid'] = array(
        '#type' => 'textfield',
        '#default_value' => $items[$delta]['uid'] ? $items[$delta]['uid'] : NULL,
        '#size' => $instance['widget']['settings']['size'],
      );
      break;      
    case 'historicalsurvey_photo_widget':   
      $elements=image_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      
      foreach (element_children($elements) as $delta) {
        $elements[$delta]['#default_value']['fid']=isset($items[$delta]['fid'])?$items[$delta]['fid']:NULL;
        $elements[$delta]['main_image'] = array(
          '#type' => 'checkbox',
          '#default_value'=>!empty($items[$delta]['main_image'])?$items[$delta]['main_image']:0,
          '#title' => t("Make this the cover image"),
        );
        $elements[$delta]['description'] = array(
          '#type' => 'textarea',
          '#default_value'=>isset($items[$delta]['description'])?$items[$delta]['description']:NULL,
          '#title' => t("Description of Photo"),
          '#description' => 'Include date taken, direction facing, and photographer or sources.',
        ); 
        $elements[$delta]['changed'] = array(
          '#type' => 'hidden',
          '#default_value'=>0,
        );
      }
      $elements['#attached']['css']=array(
        drupal_get_path('module', 'historicalsurvey_fields') . '/css/historicalsurvey_fields_upload.css',
      ); 
      $has_changed=FALSE;
      $has_meta=FALSE;
      break;      
    case 'historicalsurvey_document_widget': 
      $elements=file_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
      foreach (element_children($elements) as $delta) {
        $elements[$delta]['#default_value']['fid']=isset($items[$delta]['fid'])?$items[$delta]['fid']:0;
        $elements[$delta]['description'] = array(
          '#type' => 'textarea',
          '#default_value'=>isset($items[$delta]['description'])?$items[$delta]['description']:NULL,
          '#title' => t("Description of Document"),
        ); 
        $elements[$delta]['changed'] = array(
          '#type' => 'hidden',
          '#default_value'=>0,
        );
      } 
      $elements['#attached']['css']=array(
        drupal_get_path('module', 'historicalsurvey_fields') . '/css/historicalsurvey_fields_upload.css',
      ); 
      $has_changed=FALSE;
      $has_meta=FALSE;
      break;
  }
  
  //insert common elements
  $elements['#element_validate'] = array('historicalsurvey_fields_validate');
  $elements['#attached']['js'] = array(
    drupal_get_path('module', 'historicalsurvey_fields') . '/js/historicalsurvey_fields.js',
  ); 
  if($has_changed){
    $elements['changed'] = array(
      '#type' => 'hidden',
      '#default_value'=>0,
    );  
  }
  if($has_meta){
    if($has_replaced) {
      $elements['replaced'] = array(
        '#type' => 'checkbox',
        '#title' => 'Replace existing data from previous authors',
        '#default_value'=>0,
        '#prefix' => '<div class="replaced-option">',
        '#suffix' => '</div>',
      );    
    }
    else {
      $elements['replaced'] = array(
        '#type' => 'hidden',
        '#default_value'=>1,
      ); 
    }
    if($has_review){
      $elements['promoted_to_field_id'] = array(
        '#type' => 'hidden',
        '#default_value'=>0,
      );
      $elements['promoted_to_revision_id'] = array(
        '#type' => 'hidden',
        '#default_value'=>0,
      );
      $elements['promoted_by'] = array(
        '#type' => 'hidden',
        '#default_value'=>0,
      );
    }
  }
  return $elements;
}


/*  
 *  Implements hook_element_info()
 */
function historicalsurvey_fields_element_info() {
  $types['historicalsurvey_location_widget_element'] = array(
    '#input' => TRUE,
    '#process' => array('historicalsurvey_location_widget_element_process'),
  );
  $types['historicalsurvey_designation_widget_element'] = array(
    '#input' => TRUE,
    '#process' => array('historicalsurvey_designation_widget_element_process'),
  );
  $types['historicalsurvey_autocomplete_term_reference_widget_element'] = array(
    '#input' => TRUE,
    '#process' => array('historicalsurvey_autocomplete_term_reference_widget_element_process'),
  );
  $types['historicalsurvey_construction_year_widget_element'] = array(
    '#input' => TRUE,
    '#process' => array('historicalsurvey_construction_year_widget_element_process'),
  );
  $types['historicalsurvey_text_widget_element'] = array(
    '#input' => TRUE,
    '#process' => array('historicalsurvey_text_widget_element_process'),
  );
  return $types;
}

/*  
 *  Process callback
 *  For location widget
 */
function historicalsurvey_location_widget_element_process($element, &$form_state) {

  //create the form interface for choosing location via form and map
  $element['#tree'] = TRUE;
  
  $element['location'] = array(
    '#type' => 'textfield',
    '#title' => 'Address of Place',
    '#default_value'=>$element['#default_value']['location']?$element['#default_value']['location']:$element['#default_value']['Match_addr'],
  );
  $element['Match_addr'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['Match_addr']?$element['#default_value']['Match_addr']:NULL,
  );
  $element['HouseNum'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['HouseNum']?$element['#default_value']['HouseNum']:NULL,
  );
  $element['HouseNumFrac'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['HouseNumFrac']?$element['#default_value']['HouseNumFrac']:NULL,
  );
  $element['PreDir'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['PreDir']?$element['#default_value']['PreDir']:NULL,
  );
  $element['StreetName'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['StreetName']?$element['#default_value']['StreetName']:NULL,
  );
  $element['StreetType'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['StreetType']?$element['#default_value']['StreetType']:NULL,
  );
  $element['SufDir'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['SufDir']?$element['#default_value']['SufDir']:NULL,
  );
  $element['Ref_ID'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['Ref_ID']?$element['#default_value']['Ref_ID']:NULL,
  );
  $element['Jurisdiction'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['Jurisdiction']?$element['#default_value']['Jurisdiction']:NULL,
  );
  $element['x'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['x']?$element['#default_value']['x']:NULL,
    '#attributes' => array('id' => 'edit-selectedLng'),
  );
  $element['y'] = array(
    '#type' => 'hidden',
    '#default_value'=>$element['#default_value']['y']?$element['#default_value']['y']:NULL,
    '#attributes' => array('id' => 'edit-selectedLat'),
  );
  return $element;
}

/*  
 *  Process callback
 *  For designation widget
 */
function historicalsurvey_designation_widget_element_process($element, &$form_state) {

  $default_value=0;
  $match=FALSE;
  $element_options=array();
  $field_name=$element['#parents'][0];
  if(strpos($field_name,'designation')) {  
  
    //remove the drupal default add-more/multiple value interface
    $lang=$form_state['complete form'][$field_name]['#language'];
    unset($form_state['complete form'][$field_name][$lang]['#theme']); 
    unset($form_state['complete form'][$field_name][$lang]['add_more']); 
    unset($form_state['complete form'][$field_name][$lang][0]['_weight']);
    foreach(element_children($form_state['complete form'][$field_name][$lang]) as $delta) {
      if($delta>0) {
        unset($form_state['complete form'][$field_name][$lang][$delta]);
      }      
    }    
  }
  
  //create the form interface for choosing designation
  $delta=0;
  $element_options=array();
  foreach($element['#options'] as $group_name => $options_array) {
    $element_group = strtolower(str_replace(" ","-",$group_name));
    
    //set default values
    $default_value=0;
    if(isset($element['#default_value']) && is_array($element['#default_value'])){      
      foreach($element['#default_value'] as $value) {
        if(isset($value['tid']) && array_key_exists($value['tid'],$options_array)) {
          $default_value=$value['tid'];
        }
      }
    }
    
    $title_value = '';
    if(!empty($default_value)) {
      $title_value .= ' : '.taxonomy_term_load($default_value)->name;
    }
    
    $options=array('none/unknown')+$options_array; 
    $element[] = array(
      '#tree' => TRUE,
      '#type' => 'radios',
      '#default_value'=>$default_value!==FALSE?$default_value:NULL,
      '#options' => $options,
      '#prefix' => '<div class="designation-group"><div class="designation-group-title '.$element_group.'"><span class="expand">+</span> '.$group_name.'<span class="value">'.$title_value.'</span></div>',
      '#suffix' => '</div>',
    );       
    $element_options += $options;
  }
  $element['#options']=$element_options;
  return $element;
}

/*  
 *  Process callback
 *  For term reference widget
 */
function historicalsurvey_autocomplete_term_reference_widget_element_process($element, &$form_state) {

  $field_name=$element['#parents'][0];
  
  //remove the drupal default add-more/multiple value interface
  $lang=$form_state['complete form'][$field_name]['#language'];
  unset($form_state['complete form'][$field_name][$lang]['#theme']); 
  unset($form_state['complete form'][$field_name][$lang]['add_more']); 
  unset($form_state['complete form'][$field_name][$lang][0]['_weight']);
  foreach(element_children($form_state['complete form'][$field_name][$lang]) as $delta) {
    if($delta>0) {
      unset($form_state['complete form'][$field_name][$lang][$delta]);
    }      
  }
  
  $element['tid'] = array(
    '#type' => 'textfield',
    '#default_value' => $element['#default_value'],
    '#description' => historicalsurvey_fields_term_description($field_name),
    '#autocomplete_path' => 'historicalsurvey_fields/autocomplete/'.$field_name,
    '#maxlength' => 200,
    '#size' => 45, 
    '#resizable' => FALSE,
    '#element_validate' => array('historicalsurvey_fields_autocomplete_validate'),
  );
  return $element;
}

/*  
 *  Process callback
 *  For construction year widget
 */
function historicalsurvey_construction_year_widget_element_process($element, &$form_state) {

  //decide on estimated/actual and single/range
  $est_act = (isset($element['#default_value']['actual_early_year']) || isset($element['#default_value']['actual_late_year']))?1:0;

  if(
    (empty($element['#default_value']['actual_early_year']) && !empty($element['#default_value']['actual_late_year'])) ||
    (empty($element['#default_value']['actual_late_year']) && !empty($element['#default_value']['actual_early_year'])) ||
    (empty($element['#default_value']['estimated_early_year']) && !empty($element['#default_value']['estimated_late_year'])) ||
    (empty($element['#default_value']['estimated_late_year']) && !empty($element['#default_value']['estimated_early_year'])) ){
      $sing_rang = 1;
  }
  else {
    $sing_rang = 0;
  }
  
  //create the form interface for choosing construction year
  $element['est_act'] = array(
    '#type' => 'radios',
    '#default_value' => $est_act,
    '#prefix' => '<div class="construction-year-radios">',
    '#options' => array('Estimated Year(s)','Actual Year(s)'),
    '#weight'=>-12
  );    
  $element['sing_rang'] = array(
    '#type' => 'radios',
    '#default_value' => $sing_rang,
    '#options' => array('Range of Years','Single Year'),
    '#weight'=>-11
  );      
  $element['actual_early_year'] = array(
    '#type' => 'textfield',
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value'=>isset($element['#default_value']['actual_early_year'])?$element['#default_value']['actual_early_year']:NULL,
    '#prefix' => '<div class="construction-year-actual">',
    '#weight'=>-10
  );
  $element['actual_late_year'] = array(
    '#type' => 'textfield',
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value'=>isset($element['#default_value']['actual_late_year'])?$element['#default_value']['actual_late_year']:NULL,
    '#prefix' => '<div class="construction-year-actual-range">to',
    '#suffix' => '</div></div>',
    '#weight'=>-9
  );    
  $element['estimated_early_year'] = array(
    '#type' => 'textfield',
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value'=>isset($element['#default_value']['estimated_early_year'])?$element['#default_value']['estimated_early_year']:NULL,
    '#prefix' => '<div class="construction-year-estimated">',
    '#weight'=>-8
  );
  $element['estimated_late_year'] = array(
    '#type' => 'textfield',
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value'=>isset($element['#default_value']['estimated_late_year'])?$element['#default_value']['estimated_late_year']:NULL,
    '#prefix' => '<div class="construction-year-estimated-range">to',
    '#suffix' => '</div></div>',
    '#weight'=>-7
  );
  $element['notes'] = array(
    '#type' => 'textarea',
    '#default_value'=>isset($element['#default_value']['notes'])?$element['#default_value']['notes']:NULL,
    '#weight' => 99,
  );
  
  return $element;
}

/*  
 *  Process callback
 *  For text widget
 */
function historicalsurvey_text_widget_element_process($element, &$form_state) {

  //create the form interface for text
  $element['#tree'] = TRUE;
  $field_name=$element['#parents'][0];
  $lang=$element['#parents'][1];
  $field=$form_state['field'][$field_name][$lang]['field'];
  
  //create form interface for field wizard
  if(strpos($field_name, 'style') || strpos($field_name, 'use') || strpos($field_name, 'description')) {
    if(strpos($field_name, 'style')){$vocab_name='value_style';}
    if(strpos($field_name, 'use')){$vocab_name='value_use';}
    if(strpos($field_name, 'description')){$vocab_name='category_physical_description';}
    $vocabulary = taxonomy_vocabulary_machine_name_load($vocab_name);
    $vid=$vocabulary->vid;
    $top_level = taxonomy_get_tree($vid, $parent = 0, $max_depth = 1);
    
    foreach ($top_level as $id => $term) {
      $options1[$term->tid] = $term->name;
      $children = taxonomy_get_tree($vid, $term->tid, $max_depth = 1);
      foreach ($children as $child) {
        $options2[$term->tid][$child->tid] = $child->name;        
        $sub_children = taxonomy_get_tree($vid, $child->tid);
        foreach ($sub_children as $sub_child) {
          $options3[$term->tid][$child->tid][$sub_child->tid] = $sub_child->name;
        }
      }
    }  
    $element['wizard'] = array(
      '#type' => 'container',
      '#prefix'=>'<div id="field-wizard-'.str_replace("_","-",$field_name).'" class="field-wizard">',
      '#suffix'=>'</div>',
      '#weight'=>-9999,
    );
    $element['wizard']['select1'] = array(
      '#type'=>'select',
      '#title'=>"Select a Category",
      '#options'=>$options1,
      '#prefix'=>'<div class="selects"><div class="select1">',
      '#suffix'=>'</div>',
    );         
    $element['wizard']['select2'] = array(
      '#type' => 'container',
      '#prefix'=>'<div class="select2">',
      '#suffix'=>'</div>',
    );       
    foreach($options2 as $parent_tid=>$options){
      $element['wizard']['select2'][$parent_tid] = array(
        '#type'=>'select',
        '#title'=>count($options)==1?"Sub-Category":"Select a Sub-Category (optional)",
        '#options'=>count($options)==1?$options:array('')+$options,
        '#prefix'=>'<div id="parent-'.$parent_tid.'" class="select2-options">',
        '#suffix'=>'</div>',
      );
      if(isset($options3[$parent_tid])) {
        foreach($options3[$parent_tid] as $sub_parent_tid => $sub_options) {
          $element['wizard']['select3'][$parent_tid][$sub_parent_tid] = array(
            '#type'=>'select',
            '#title'=>"Select value (optional)",
            '#options'=>array('')+$sub_options,
            '#prefix'=>'<div id="parent-'.$sub_parent_tid.'" class="select3-options">',
            '#suffix'=>'</div>',
          );
        }
      }      
    }
    if(isset($element['wizard']['select3'])) {
      $element['wizard']['select3']['#type'] = 'container';
      $element['wizard']['select3']['#prefix'] = '<div class="select3">';
      $element['wizard']['select3']['#suffix'] = '</div></div>';
    }
    else {
      $element['wizard']['select2']['#suffix'] .= '</div>';
    }

    $element['wizard']['insert'] = array(
      '#type' => 'item',
      '#markup' => '<div class="wizard-button insert-button">Insert</div>',
    );
    $element['wizard']['cancel'] = array(
      '#type' => 'item',
      '#markup' => '<div class="wizard-button cancel-button">Cancel</div>',
    );
  }
  
  
  $element['value'] = array(
    '#type' => 'textarea', 
    '#default_value' => $element['#default_value']['value']?$element['#default_value']['value'] : NULL, 
    '#rows' => isset($field['settings']['rows'])?$field['settings']['rows']:5,
    '#attributes' => array('class' => array('text-full')),
    '#weight' => -99,
  );   
  $element['wizard-trigger'] = array(
    '#type' => 'item',
    '#markup' => '<div class="wizard-trigger">Use Field Wizard</div>',
    '#weight'=>0,
  ); 
  $element['notes'] = array(
    '#type' => 'textarea',
    '#default_value'=>isset($element['#default_value']['notes'])?$element['#default_value']['notes']:NULL,
    '#weight' => 99,
  );
  return $element;
}
