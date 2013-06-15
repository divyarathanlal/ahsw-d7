<?php

/**
 * Send the search keys to their respective search engines, and compile and return the results as a render array
 */
function historicalsurvey_search_get_query_results($keys_raw, $options, $pager) {

  $result_count = 0;

  //group records
  foreach ($keys_raw as $field_name => $key_array) {
    foreach ($key_array as $key_name => $key_value) {
      if ($key_name == 'text') {
        $keys['text'][$field_name] = $key_value;
      }
      elseif ($key_name == 'select') {
        $keys['term'][$field_name] = $key_value;
      }
      elseif ($key_name == 'year_early' || $key_name == 'year_late') {
        $keys['year'][$field_name][$key_name] = $key_value;
      }
      elseif ($key_name == 'location') {
        $keys['location'][$field_name] = $key_value;
      }
      elseif ($key_name == 'options') {
        $keys['options'][$field_name] = $key_value;
      }
    }
  } 

  //get and display results for text fields via Solr
  if (isset($keys['text'])) {
    foreach ($keys['text'] as $field => $key_array) {
      foreach ($key_array as $key) {
        if (!empty($key)) {    
        
          //deal with quote marks
          if(strpos($key,'"')!==FALSE){
            $key = str_replace('"','',$key);
          }
          
          //find matches for $key in all review levels of $field, returns a list of places and matched fields
          $field_options = isset($keys['options'][$field]) ? $keys['options'][$field] : array();
          $results = historicalsurvey_search_text_search_execute($key, $field, $field_options, $options);
          if (isset($results['results'])) {
          
            //combine matched places into master array with matches from other fields and keys
            foreach ($results['results'] as $nid => $result) {
              if(isset($result['narrative'])) {
                $ret['results']['#results']['results'][$nid]['narrative'] = $result['narrative'];
              }
              else {
                foreach($result as $field_name => $values) {
                  $ret['results']['#results']['results'][$nid][$field_name]['values'] = $values['values'];
                }
              }
              $nids[$field][$nid] = $nid;
            }

            $result_count = count($ret['results']['#results']['results']);
          }
         
          //does this ever happen?
          if (!empty($results['ignored'])) {
            drupal_set_message(t('The following search keys are too short or too common and were therefore ignored: "@list".', array('@list' => implode(t('", "'), $results['ignored']))), 'warning');
          }
          if (!empty($results['warnings'])) {
            foreach ($results['warnings'] as $warning) {
              drupal_set_message($warning, 'warning');
            }
          }
        }
      }
    }
  }
  if (isset($keys['term'])) {
    //$ret['results']['#keys']['term'] = $keys['term'];
    foreach ($keys['term'] as $field => $key) {
      //foreach($key_array as $key){
      if (!empty($key)) {
        $field_options = isset($keys['options'][$field]) ? $keys['options'][$field] : array();
        $results = historicalsurvey_search_term_search_execute($key, $field, $field_options);
        if ($results['results']) {
          foreach ($results['results'] as $nid => $field_array) {
            foreach ($field_array as $field_name => $values) {
              
              //highlighting
              foreach($values['values'] as $key=>$value) {
                $values['values'] = '<strong>'.$value.'</strong>';
              }
              
              $ret['results']['#results']['results'][$nid][$field_name]['values'] = $values['values'];
              $nids[$field][$nid]= $nid;
            }
          }
          $result_count = count($ret['results']['#results']['results']);
        }
      }
      //}     
    }
  }
  if (!empty($keys['year'])) {
    //$ret['results']['#keys']['year'] = $keys['year'];
    $results = historicalsurvey_search_year_search_execute($keys['year']['field_construction_year']['year_early'], $keys['year']['field_construction_year']['year_late']);
    if ($results['results']) {
      foreach ($results['results'] as $nid => $field_array) {
        foreach ($field_array as $field_name => $values) {
          $ret['results']['#results']['results'][$nid][$field_name]['values'] = $values['values'];
          $nids['field_construction_year'][$nid] = $nid;
        }
      }
      $result_count = count($ret['results']['#results']['results']);
    }
  }
  if (!empty($keys['location']['field_location'])) {
    //$ret['results']['#keys']['location']['field_location'] = $keys['location']['field_location'];
    $results = historicalsurvey_search_location_search_execute($keys['location']['field_location']);
    //dw($results);
    if ($results['results']) {
      foreach ($results['results'] as $nid => $field_array) {
        $top=$field_array['top'];
        unset($field_array['top']);
        foreach ($field_array as $field_name => $values) {
          
          //highlighting
          foreach($values['values'] as $key=>$value) {
            $values['values'][$key] = str_replace($keys['location']['field_location'],'<strong>'.$keys['location']['field_location'].'</strong>',$value);
          }
          
          //if top is set true, put these results at the top of the array          
          if ($top) {
            $ret['results']['#results']['results'][$nid]['field_location']['values'] = $values['values'];
            $nids['field_location'][$nid] = $nid;
          }
          else {
            $nontop_array[$nid]['field_location']['values'] = $values['values'];
            $nids['field_location'][$nid] = $nid;
          }
        }
      }
      
      //we might want to show the non-top results, but maybe not, must test
      /*if (isset($nontop_array)) {
        if (isset($ret['results']['#results']['results'])) {
          $ret['results']['#results']['results'] = array_merge($ret['results']['#results']['results'], $nontop_array);
        }
        else {
          $ret['results']['#results']['results'] = $nontop_array;
        }
      }*/
      
      $result_count = count($ret['results']['#results']['results']);
    }
  }
  
  //highlighting

  $ret['results']['#results']['result count'] = $result_count;

  //results are aleady in union form, for intersect we must unset
  $set_type = $options['set_type']; //`intersect` or `union`
  if ($set_type == 'intersect') {
    $nids_to_unset = array();    
    foreach ($nids as $field_name => $set) {
      $nids_to_unset += array_diff(array_keys($ret['results']['#results']['results']), $set);
    }
    $nids_to_unset = array_unique($nids_to_unset);
    if (!empty($nids_to_unset)) {
      foreach ($nids_to_unset as $unset_nid) {
        unset($ret['results']['#results']['results'][$unset_nid]);
      }
    }
    $ret['results']['#results']['result count'] = count($ret['results']['#results']['results']);
  }
  
  $ret['results']['#results']['all_nids'] = array_keys($ret['results']['#results']['results']);
  
  //paginate
  $ret['results']['#results']['results'] = array_slice($ret['results']['#results']['results'],$pager['offset'], $pager['num_per_page'],TRUE);
  
  return $ret['results'];
}


/**
 * Executes a search in a single text field via Search API/Solr
 *
 * @param $keys
 *   The keywords to search for.
 *
 * @param $orig_field
 *   The field within which we are searching
 *
 * @param $field_options
 *   An array of options specifying the field further
 *
 * @param $options
 *   An array of options concerning search mode
 *    see http://drupalcontrib.org/api/drupal/contributions!search_api!search_api.module/function/search_api_query/7
 *
 * @param $pager
 *   An array of options concerning pagination
 *
 * @return array
 *   The search results as returned by SearchApiQueryInterface::execute().
 */
function historicalsurvey_search_text_search_execute($search_key, $orig_field, $field_options = array(), $options = array()) {
   
  //todo: search in the revision history too...

  //Search Whole Phrase (single) or Separate Words (terms)
  $options['parse mode'] = isset($options['parse mode']) ? $options['parse mode'] : 'terms';
  $options['conjunction'] = 'AND'; //for separate words

  if (!empty($field_options)) {
    foreach ($field_options as $suffix => $value) {
      if (!empty($value)) {
        if ($suffix == 'references') {
          $fields_raw[] = 'field_reference'; //slight fix...
        }
        elseif ($suffix == 'description') {
          $fields_raw[] = 'field_description'; //slight fix...
        }
        else {
          $fields_raw[] = $orig_field . '_' . $suffix;
        }
      }
    }
  }
  else {
    $fields_raw[] = $orig_field;
  }

  $review_levels = variable_get('historicalsurvey_place_review_levels', array());
  foreach ($fields_raw as $key => $field) {
    foreach ($review_levels as $level) {
      $suffix = $level['machine_suffix'];
      $fields[] = $field . $suffix;
    }
  }
  // Send to Search API/Solr
  $query = search_api_query('solr_tomcat6', $options)
    ->keys($search_key)
    ->fields($fields);
    /* ->condition($field,$key);*/ //meaning, WHERE $field=$key
    
  $r = $query->execute();

  if ($r['result count'] > 0) {
    //save just the fields we want
    $result = $r['results'];
    foreach ($result as $nid => $values) {
      foreach ($fields as $field) {
        if(strpos($field,'field')===0 && isset($values['excerpt'])) {
          $results['results'][$nid]['narrative']['values'] = $values['excerpt'];
        }
        elseif(isset($values['fields'][$field])) {
          if(strpos($values['fields'][$field][0],$search_key)) {
            $value=str_replace($search_key,'<strong>'.$search_key.'</strong>',$values['fields'][$field][0]);
            $results['results'][$nid][$field]['values'] = $value;
          }
        }        
      }
    }
  }
  
  return isset($results) ? $results : FALSE;
}


/**
 * Executes a search in a single taxonomy term field.
 *
 * @param $keys
 *   The keywords to search for.
 *
 * @param $orig_field
 *   The field within which we are searching
 *
 * @return array
 *   The search results array
 */
function historicalsurvey_search_term_search_execute($search_key, $orig_field, $field_options = array()) {
  
  $term=taxonomy_term_load($search_key);
  $term_name=$term->name;

  if (!empty($field_options)) {
    foreach ($field_options as $suffix => $value) {
      if (!empty($value)) {
        $fields_raw[] = $orig_field . '_' . $suffix;
      }
    }
  }
  else {
    $fields_raw[] = $orig_field;
  }
  $review_levels = array(0);
  if (!in_array($orig_field, array('field_designation_official','field_survey_effort','field_tag'))) {
    $review_levels = variable_get('historicalsurvey_place_review_levels', array());
    foreach ($fields_raw as $key => $field) {
      foreach ($review_levels as $level) {
        $suffix = isset($level['machine_suffix']) ? $level['machine_suffix'] : '';
        $fields[] = $field . $suffix;
      }
    }
  }
  else {
    $fields = array($orig_field);
  }
  foreach ($fields as $field) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'place')
      ->propertyCondition('status', 1)
      ->fieldCondition($field, 'tid', $search_key)
      ->age(FIELD_LOAD_REVISION);
    $result = $query->execute();

    if (!empty($result)) {
      foreach ($result['node'] as $place_stub) {
        $results['results'][$place_stub->nid][$field]['values']['term'] = $term_name;
      }
    }
  }
  if (!empty($results)) {
    //debug_write($results,'x',true);
    return $results;
  }
  return FALSE;
}


/**
 * Executes a search in a single construction year field.
 *
 * @param $keys
 *   The keywords to search for.
 *
 * @param $field
 *   The field within which we are searching
 *
 * @return array
 *    The search results array
 */
function historicalsurvey_search_year_search_execute($early, $late) {

  $orig_field = 'field_construction_year';
  $review_levels = variable_get('historicalsurvey_place_review_levels', array());
  //dw('');
  foreach ($review_levels as $level) {
    $suffix = $level['machine_suffix'];
    $field = $orig_field . $suffix;
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
          ->entityCondition('bundle', 'place')
          ->propertyCondition('status', 1)
          ->addTag('year')
          ->addMetaData('year', array($field, $early, $late))
          //->age(FIELD_LOAD_REVISION);  //make this an options
          ->execute();
    
    if(isset($query->ordered_results)) {
      $result =$query->ordered_results;
      //dw($result,'',1);
      if (!empty($result)) {
        foreach ($result as $place_stub) {
          $nid=$place_stub->entity_id;
          unset($place_stub->entity_id);
          unset($place_stub->revision_id);
          unset($place_stub->bundle);
          unset($place_stub->entity_type);
          
          $results['results'][$nid][$field]['values'] = (array) $place_stub;
        }
      }
    }
  }
  if (!empty($results)) {
    //$results['results']=array_slice($results['results'], 0, 5, 1);
    return $results;
  }
  return FALSE;
}


/**
 * Implements hook_query_TAG_alter
 *
 * So that we can add OR statments for construction year query using where method for SQL
 *
 * @param QueryAlterableInterface $query
 */
function historicalsurvey_search_query_year_alter(QueryAlterableInterface $query) {
  $field = $query->alterMetaData['year'][0];
  $early = !empty($query->alterMetaData['year'][1]) ? $query->alterMetaData['year'][1] : 0;
  $late = !empty($query->alterMetaData['year'][2]) ? $query->alterMetaData['year'][2] : date('Y');

  //maybe there is a better way to do this, perhaps with db_or(), dunno...
  $query->fields('y',array($field . '_actual_early_year',$field . '_actual_late_year',$field . '_estimated_early_year',$field . '_estimated_late_year'));
  $query->leftJoin('field_revision_' . $field, 'y', 'node.nid = y.entity_id');  
  $query->where(
      $field . '_actual_early_year BETWEEN :early AND :late
      OR ' . $field . '_actual_late_year BETWEEN :early AND :late
      OR ' . $field . '_estimated_early_year BETWEEN :early AND :late
      OR ' . $field . '_estimated_late_year BETWEEN :early AND :late
      OR :early BETWEEN ' . $field . '_actual_early_year AND ' . $field . '_actual_late_year
      OR :late BETWEEN ' . $field . '_actual_early_year AND ' . $field . '_actual_late_year
      OR :early BETWEEN ' . $field . '_estimated_early_year AND ' . $field . '_estimated_late_year
      OR :late BETWEEN ' . $field . '_estimated_early_year AND ' . $field . '_estimated_late_year', 
      array(':early' => $early, ':late' => $late));
}


/**
 * Executes a search in a single location field.
 *
 * @param $location
 *   The keywords to search for.
 *
 * @return array
 *   The search results array
 */
function historicalsurvey_search_location_search_execute($location) {
  
  $location = historicalsurvey_search_normalize_location($location);

  //search whole phrase
  $result = array();
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'place')
        ->propertyCondition('status', 1)        
        ->addTag('addr_full')
        ->addMetaData('location', $location)
        //->age(FIELD_LOAD_REVISION);  //make this an options
        ->execute();
  
  if(isset($query->ordered_results)) {
    $result = $query->ordered_results;
  }

  if (!empty($result)) {
    foreach ($result as $place_stub) {
      $nid=$place_stub->entity_id;
      unset($place_stub->entity_id);
      unset($place_stub->revision_id);
      unset($place_stub->bundle);
      unset($place_stub->entity_type);
      $results['results'][$nid]['field_location']['values'] = (array) $place_stub;

      //these results always go at the top
      $results['results'][$nid]['top'] = TRUE;
    }
    //if these exist, just return them
    return $results;
  }
  
  //search individual terms
  $result = array();
  $key_array = explode(' ', $location);
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'place')
        ->propertyCondition('status', 1)        
        ->addTag('addr_parts')
        ->addMetaData('keys', $key_array)
        //->age(FIELD_LOAD_REVISION);  //make this an options
        ->execute();
  
  if(isset($query->ordered_results)) {
    $result = $query->ordered_results;
  }
  
  if (!empty($result)) {
    foreach ($result as $place_stub) {
      $nid=$place_stub->entity_id;
      unset($place_stub->entity_id);
      unset($place_stub->revision_id);
      unset($place_stub->bundle);
      unset($place_stub->entity_type);
          
      //if we already found this place, it is more probably the correct result, so put it at the top
      if (isset($results['results'][$nid])) {
        $results['results'][$nid]['top'] = TRUE;
      }
      else {
        $results['results'][$nid]['top'] = FALSE;
      }
      $results['results'][$nid]['field_location']['values'] = (array) $place_stub;
    }
  }
  
  //also, search the geocoded addr, if any
  if ($geo_data = geocoder('coa', $location)) {
    $addr_array[] = $geo_data['data']['Match_addr'];
    if (isset($geo_data['data']['alternatives'])) {
      foreach ($geo_data['data']['alternatives'] as $alt) {
        $addr_array[] = $alt['data']['Match_addr'];
      }
    }
    if (!empty($addr_array)) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', 'place')
            ->propertyCondition('status', 1)
            ->addTag('geoaddr')
            ->addMetaData('addr', $addr_array)
            ->execute();
        
      $result = $query->ordered_results;

      if (!empty($result)) {
        foreach ($result as $place_stub) {
          $nid=$place_stub->entity_id;
          unset($place_stub->entity_id);
          unset($place_stub->revision_id);
          unset($place_stub->bundle);
          unset($place_stub->entity_type);          
          
          //if we already found this place, it is more probably the correct result, so put it at the top
          if (isset($results['results'][$nid])) {
            $results['results'][$nid]['top'] = TRUE;
          }
          else {
            $results['results'][$nid]['top'] = FALSE;
          }
          $results['results'][$nid]['field_location']['values'] = (array) $place_stub;
        }
      }
    }
  }
  if (!empty($results)) {
    return $results;
  }
  return FALSE;
}

/**
 * Implements hook_query_TAG_alter
 *
 * So that we can add OR statments for search on the match_addr and location fields
 *
 * @param QueryAlterableInterface $query
 */
function historicalsurvey_search_query_addr_parts_alter(QueryAlterableInterface $query) {
  $key_array = $query->alterMetaData['keys'];

  //maybe there is a better way to do this, perhaps with db_or(), dunno...
  $query->fields('l',array('field_location_location'));
  $query  
    ->leftJoin('field_revision_field_location', 'l', 'node.nid = l.entity_id');
  $x = 0;
  foreach ($key_array as $key) {
    if ($x === 0) {
      $where = 'field_location_location LIKE :key' . $x . ' OR field_location_Match_addr LIKE :key' . $x . '';
    }
    else {
      $where .= ' OR field_location_location LIKE :key' . $x . ' OR field_location_Match_addr LIKE :key' . $x . '';
    }
    $replacements[':key' . $x] = '%' . $key . '%';
    $x++;
  }
  $query->where($where, $replacements);
}

/**
 * Implements hook_query_TAG_alter
 *
 * So that we can add OR statments for search on the match_addr and location fields
 *
 * @param QueryAlterableInterface $query
 */
function historicalsurvey_search_query_addr_full_alter(QueryAlterableInterface $query) {
  $location = $query->alterMetaData['location'];

  //maybe there is a better way to do this, perhaps with db_or(), dunno...
  $query->fields('l',array('field_location_location'))
        ->leftJoin('field_revision_field_location', 'l', 'node.nid = l.entity_id');

  $where = 'field_location_location LIKE :location OR field_location_Match_addr LIKE :location';
  $replacements[':location'] = '%' . $location . '%';

  $query->where($where, $replacements);
}


/**
 * Implements hook_query_TAG_alter
 *
 * So that we can add OR statments for search on the various geocoder match_addr
 *
 * @param QueryAlterableInterface $query
 */
function historicalsurvey_search_query_geoaddr_alter(QueryAlterableInterface $query) {
  $addr_array = $query->alterMetaData['addr'];
  //maybe there is a better way to do this, perhaps with db_or(), dunno...
  $query->fields('l',array('field_location_location'));
  $query  
    ->leftJoin('field_revision_field_location', 'l', 'node.nid = l.entity_id');
  $x = 0;
  foreach ($addr_array as $addr) {
    if ($x === 0) {
      $where = 'field_location_Match_addr LIKE :addr' . $x;
    }
    else {
      $where .= ' OR field_location_Match_addr LIKE :addr' . $x;
    }
    $replacements[':addr' . $x] = '%' . $addr . '%';
    $x++;
  }
  $query->where($where, $replacements);
}


/*  Helper function to convert common location terms to normalized values */
function historicalsurvey_search_normalize_location($location) {

  $location = strtolower($location);
  
  $list = array(
    'street' => 'st',
    'road' => 'rd',
    'avenue' => 'ave',
    'court' => 'ct'
  );

  foreach($list as $from=>$to) {
    if(strpos($location,$from)!==FALSE) {
      $location = str_replace($from,$to,$location);
    }
  }
  return $location;
}


