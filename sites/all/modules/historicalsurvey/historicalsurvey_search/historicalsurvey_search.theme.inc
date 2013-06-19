<?php


/**
 * Implements hook_theme().
 */
function historicalsurvey_search_theme() {
  $themes['historicalsuvey_search_form'] = array(
    'template' => 'historicalsuvey-search-form',
    'render element' => 'form',
  );
  $themes['historicalsuvey_address_search_form'] = array(
    'template' => 'historicalsuvey-address-search-form',
    'render element' => 'form',
  );
  $themes['historicalsurvey_search_results'] = array(
    'template' => 'historicalsurvey-search-results',
    'variables' => array(
      'results' => array('result count' => 0),
      'view_mode' => 'historicalsurvey_search_result',
      'keys' => '',
      'pager' => '',
    ),
  );
  $themes['historicalsurvey_search_result'] = array(
    'variables' => array(
      'result' => NULL,
      'item' => NULL,
      'keys' => '',
    ),
  );

  return $themes;
}

function historicalsurvey_search_preprocess_historicalsuvey_search_form(&$variables) {

  drupal_add_css(drupal_get_path('module', 'historicalsurvey_search') . '/historicalsurvey_search_form.css');
  drupal_add_js(drupal_get_path('module', 'historicalsurvey_search') . '/historicalsurvey_search_form.js');
}
function historicalsurvey_search_preprocess_historicalsuvey_address_search_form(&$variables) {

  drupal_add_css(drupal_get_path('module', 'historicalsurvey_search') . '/historicalsurvey_search_form.css');
  drupal_add_js(drupal_get_path('module', 'historicalsurvey_search') . '/historicalsurvey_search_form.js');
}

/**
 * Function for preprocessing the variables for the historicalsurvey_search_results
 * theme.
 *
 * Parameters
 * $variables
 *   An associative array containing:
 *   - index: The index object this search was executed on.
 *   - results: An array of search results
 *   - keys: The keywords of the executed search.
 */
function historicalsurvey_search_preprocess_historicalsurvey_search_results(array &$variables) {
  
  drupal_add_css(drupal_get_path('module', 'historicalsurvey_search') . '/historicalsurvey_search_results.css');
  
  if (!empty($variables['results']['results'])) {
    $_SESSION['historicalsurvey_search']['results'] = $variables['results'];
  }
  
  
  if(isset($variables['results']['all_nids'])) {  
    $data = historicalsurvey_maps_get_data($variables['results']['all_nids']);    
    foreach ($data['locations'] as $nid => $location) {
      $redpins[] = array(
        'lat' => $location['lat'],
        'lng' => $location['lng'],
        'location' => $location,
      );
      $variables['location'][$nid] = $location;
      $variables['infowindows'][$nid] = $data['infowindows'][$nid];
    }
  }
  
  $variables['map'] = array(
    '#theme' => 'historicalsurvey_map',
    '#redpins' => isset($redpins) ? $redpins : array(),
    '#viewmode' => 'search',
  );

  if (!$variables['results']['result count']) {
    $variables['results_themed'] = array(
      '#markup' => '<div id="search-results-empty"><h2>Your search yielded no results</h2></div>',
    );
  }
  else {
    $variables['results_themed'] = array(
      '#prefix' => '<ol class="search-results">',
      '#markup' => '',
      '#suffix' => '</ol>',
    );
    foreach ($variables['results']['results'] as $nid=>$item) {
      $search_result = '<li class="search-result">';
      $search_result .= theme('historicalsurvey_search_result', array(
        'result' => $item,
        'nid' => $nid,
        'keys' => $variables['keys'],
        'location' => $variables['location'][$nid],
        'infowindow_data' => $variables['infowindows'][$nid]
      ));
      $search_result .= '</li>';
      $variables['results_themed']['#markup'] .= $search_result;
    }
  }
  $variables['keys_themed']['#markup'] = '';
  foreach ($variables['keys'] as $field_name => $result_array) {

    //format the field name
    $field_name_suffix = '';
    foreach ($result_array as $element_name => $element) {
    
      //format the field name options
      if ($element_name == 'options') {
        foreach ($element as $option) {
          $field_name_suffix .= !empty($option) ? $option . ', ' : '';
        }
        $field_name_suffix = rtrim($field_name_suffix, ', ');
        $field_name_suffix = !empty($field_name_suffix) ? ' (' . $field_name_suffix . ')' : '';
      }
    }

    $field_name_formatted = ucwords(str_replace(array('field_', '_'), array('', ' '), $field_name)) . $field_name_suffix . ': ';
    
    //format the keys      
    $keys_formatted = '';
    foreach ($result_array as $element_name => $element) {
      if (is_array($element) && $element_name != 'options') {
      
      
        foreach ($element as $value) {
          if(!empty($value)) {
            if(in_array($field_name,array('field_use','field_style')) && !empty($value) && is_numeric($value)) {
              $term=taxonomy_term_load($value);
              $parents = taxonomy_get_parents($term->tid);
              if(!empty($parents)) {
                $parent = reset($parents);
                $value=$parent->name.' - '.$term->name;
              }
              else{  
                $value=$term->name;
              }
            }
            $keys_formatted .= $value . ', ';
          }
        }
        
      }
      elseif ($element_name != 'options') {
        if($element_name == 'select') {  
        
          //then its a term key
            $term=taxonomy_term_load($element);
            $parents = taxonomy_get_parents($term->tid);
            if(!empty($parents)) {
              $parent = reset($parents);
              $term_name=$parent->name.' - '.$term->name;
            }
            else{  
              $term_name=$term->name;
            }
          $keys_formatted = $term_name;
          
        }
        else {
          if(!empty($element)) {
            $keys_formatted .= $element . ', ';
          }
        }
      }
    }
    $keys_formatted = rtrim($keys_formatted, ', ');

    //put them together
    $variables['keys_themed']['#markup'] .= '<div id="search-result-key"><span id="search-results-key-fields">' . $field_name_formatted . '</span><span id="search-results-keys">' . $keys_formatted . '</span></div>';
  }

}


function historicalsurvey_search_preprocess_historicalsurvey_search_result(&$variables) {
  
  $infowindow_data = $variables['infowindow_data'];  
  
  if(!empty($infowindow_data['place_type'][2])){
    $term=taxonomy_term_load($infowindow_data['place_type'][2]);
    $variables['place_type']=$term->name;
  }
  elseif(!empty($infowindow_data['place_type'][1])) {
    $term=taxonomy_term_load($infowindow_data['place_type'][1]);
    $variables['place_type']=$term->name;
  }
  elseif(!empty($infowindow_data['place_type'][0])) {
    $term=taxonomy_term_load($infowindow_data['place_type'][0]);
    $variables['place_type']=$term->name;
  }
  else {
    $variables['place_type']='';
  }
  
  if(!empty($infowindow_data['name_current'][2])){
    $variables['name_current']=$infowindow_data['name_current'][2];
  }
  elseif(!empty($infowindow_data['name_current'][1])) {
    $variables['name_current']=$infowindow_data['name_current'][1];
  }
  elseif(!empty($infowindow_data['name_current'][0])) {
    $variables['name_current']=$infowindow_data['name_current'][0];
  }
  else {
    $variables['name_current']='';
  }   
  if (isset($infowindow_data['photo_uri']) && !empty($infowindow_data['photo_uri'])) {    
    $variables['photo_variables'] = array(
      'style_name' => 'thumbnail',
      'path' => $infowindow_data['photo_uri'],
      'prefix' => '<div class="photo">',
      'suffix' => '</div>',
    );
  }
}

/**
 * Theme function for displaying search results.
 *
 * @param array $variables
 *   An associative array containing:
 *   - index: The index this search was executed on.
 *   - result: One item of the search results
 *   - item: The loaded item corresponding to the result.
 *   - keys: The keywords of the executed search.
 */
function theme_historicalsurvey_search_result(array $variables) {

  $html = '<div class="search-result" id="search-result-' . $variables['location']['nid'] . '">';

  $html .= '<div class="info-wrapper">';
  $location = '';
  if (!empty($variables['location']['Match_addr'])) {
    $location = $variables['location']['Match_addr'];
  }
  elseif (!empty($variables['location']['location'])) {
    $location = $variables['location']['location'];
  }

  $location = !empty($location) ? '<div class="name">' . $location . '</div>' : '<em>no address</em>';
  $name = !empty($variables['name_current']) ? '<div class="name">' . $variables['name_current'] . '</div>' : '';
  $type = !empty($variables['place_type']) ? '<div class="type">' . $variables['place_type'] . '</div>' : '';


  $html .= '<div class="info">' . $type . $location . $name . '</div>';

  $html .= '</div>'; //info-wrapper 


  $html .= '<div class="result-wrapper">';

  if (isset($variables['photo_variables'])) {
    $html .= l(theme('image_style',$variables['photo_variables']), 'node/' . $variables['location']['nid'], array('html' => TRUE));
  }
  else {
    $html .= l('<div class="photo empty"><span>no image</span></div>', 'node/' . $variables['location']['nid'], array('html' => TRUE));
  }

  $html .= '<div class="results">';
  //get the text keys 
  $text_keys = array();
  foreach ($variables['keys'] as $types) {
    if (isset($types['text'])) {
      $text_keys += $types['text'];
    }
  }
  $text = '';
  if(isset($variables['result'])) {
    foreach ($variables['result'] as $field_name => $values) {
      $result_value = '';
      $field_label = historicalsurvey_fields_get_field_label($field_name);
      if(empty($field_label)) {
        $field_label = ucwords(str_replace('_',' ',$field_name));
      }     
      if(is_array($values['values'])) {
        foreach($values['values'] as $column=>$value) {
          if(!empty($value)) {
            $column_name = str_replace($field_name,'',$column);
            $column_name = ucwords(str_replace('_',' ',$column_name));
            $result_value .= '<p><small>' . $column_name."</small>: ".$value . '</p>';
          }
        }
      }
      else {
        $result_value .= '<p>' . $values['values'] . '</p>';
      }
      
      $text .= '<div id="result">';
      $text .= '<div id="result-title">' . $field_label . ': </div>';
      $text .= '<div id="result-value">' . $result_value . '</div>';
      $text .= '</div>';
    }
  }
  
  $html .= $html ? $text : '';
  $html .= '</div>'; //results
  $html .= '</div>'; //result-wrapper

  $html .= '</div>'; //search-result

  return $html;
}
