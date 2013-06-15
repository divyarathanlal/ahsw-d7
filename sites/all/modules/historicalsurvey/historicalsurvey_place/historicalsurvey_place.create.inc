<?php


/**
 *  Form Function for Add New Place, step 1
 *  Here we simply ask for the address, if there is one
 *
 */
function historicalsurvey_place_add_place_form($form, &$form_state) {

  $form['field_location']['#type'] = 'container';
  $form['field_location']['#tree'] = TRUE;
  $form['field_location']['location'] = array(
    '#type' => 'textfield',
    '#title' => 'Address of Place',
    '#suffix' => 'Austin, TX',
  );

  $form['actions'] = array(
    '#type' => "actions",
    'address' => array(
      '#type' => 'submit',
      '#value' => t('Find this Address'),
    ),
    'noaddress' => array(
      '#type' => 'submit',
      '#value' => t('This Place does not have an address'),
    ),
  );

  return $form;
}

/**
 *  Validate Function for Add New Place, step 1
 *  Geocode entry, determine if its in an acceptable jurisdiction
 *
 */
function historicalsurvey_place_add_place_form_validate($form, &$form_state) {
  if ($form_state['values']['op'] == 'This Place does not have an address') {
    $_SESSION['historicalsurvey_place_add_place']['noaddress'] = TRUE;
    drupal_goto('place/new/map');
  }

  if (!empty($form_state['values']['field_location']['location'])) {
    $address = $form_state['values']['field_location']['location'];
    $geo_data = geocoder('coa', $address);
    //dw($geo_data);
    $form_state['values']['field_location']['geo_data'] = $geo_data;

    //historicalsurvey_place_address_duplicate_validate($geo_data);

    if (isset($geo_data['data']['Jurisdiction'])) {
      //get variable list of acceptable jurisdictions, set by admin config
      //foreach of those acceptable jurisdictions, match them against the geo_data Jurisdiction       
      $jurs = variable_get('historicalsurvey_place_jurisdictions', array(210));
      $cities_str = '';
      $match = FALSE;
      foreach ($jurs as $jur) {
        if ($jur !== 0) {
          $city = historicalsurvey_place_get_city($jur);
          $cities_str .= $city . ', ';
          if ($city === $geo_data['data']['Jurisdiction']) {
            $match = TRUE;
          }
        }
      }
      $cities_str = rtrim($cities_str, ', ');
      if (!$match) {
        $city = !empty($geo_data['data']['Jurisdiction']) ? '<b>' . $geo_data['data']['Jurisdiction'] . '</b>' : 'a jurisdiction not covered by the Austin Historical Survey Wiki';
        $badcity = '<div>It appears that this address is located in the jurisdiction: ' . $city . '. The Austin Historical Survey Wiki is only accepting new places within the following jurisdictions: <b>' . $cities_str . '</b>.</div>';
        $badcity .= '<div>If you think this is an error, please contact us using our ' . l('Contact', 'contact') . ' form.</div>';
        form_set_error('field_location', $badcity);
      }
    }
  }
  else {
    form_set_error('field_location', t('Please enter an address, or select "This Place does not have an address"'));
  }
}

/**
 *  Submit Function for Add New Place, step 1
 *  If the geocode was exact (score=100) or had zero results, go straight to step 3 (map)
 *  else, goto step 2, the address selection form
 *
 */
function historicalsurvey_place_add_place_form_submit($form, &$form_state) {

  $geo_data = $form_state['values']['field_location']['geo_data'];
  if ($geo_data['data']['score'] === 100) {
    $_SESSION['historicalsurvey_place_add_place']['selection']['geo_data'] = $geo_data;
    drupal_goto('place/new/map');
  }
  elseif (empty($geo_data)) {
    $_SESSION['historicalsurvey_place_add_place']['selection']['user_address'] = $form_state['values']['field_location']['location'];
    drupal_goto('place/new/map');
  }
  else {
    $_SESSION['historicalsurvey_place_add_place']['results'] = $form_state['values']['field_location'];
    drupal_goto('place/new/address');
  }
}


/**
 *  Form Function for Add New Place, step 2
 *  Here we display the geocoded address options, and the option to continue with the originally-inputted address and no geocode data
 *
 */
function historicalsurvey_place_add_place_address_form() {
  $results = isset($_SESSION['historicalsurvey_place_add_place']['results']) ? $_SESSION['historicalsurvey_place_add_place']['results'] : FALSE;
  $match = FALSE;
  if ($results) {
    $geo_data = $results['geo_data'];
    $user_address = $results['location'];
    if (isset($geo_data['error'])) {
      drupal_set_message($geo_data['error'], 'warning');
      //watchdog('Geocoding Error', 'Geocoder says: '.$geo_data['error']);
    }
    //dw($geo_data);
    $options['Match_addr'] = $geo_data['data']['Match_addr'] . " <span class='map-point-found'><small>(" . $geo_data['coords'][1] . ", " . $geo_data['coords'][0] . ")</small></span>";
    if (!empty($geo_data['data']['alternatives']) && count($geo_data['data']['alternatives'])) {
      foreach ($geo_data['data']['alternatives'] as $key => $result) {
        $result = (array) $result;
        $options[$key] = $result['data']['Match_addr'] . " <span class='map-point-found'><small>(" . $result['coords'][1] . ", " . $result['coords'][0] . ")</small></span>";
      }
    }
    $options['user_address'] = 'Use the address as entered ("' . $user_address . '") and select a custom point on the map';
    $form['field_location']['address']['alts'] = array(
      '#type' => 'radios',
      '#options' => $options,
      '#title' => t('We found the following addresses. Please select an option.'),
    );
    $form['actions'] = array(
      '#type' => "actions",
      'continue' => array(
        '#type' => 'submit',
        '#value' => t('Continue to Map'),
      ),
      'cancel' => array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
      ),
    );
  }
  else {
    //user reached the page improperly, so just reset
    drupal_goto('place/new/cancel');
  }
  return $form;
}

/**
 *  Validate Function for Add New Place, step 2
 *
 */
function historicalsurvey_place_add_place_address_form_validate($form, &$form_state) {
  if ($form_state['values']['op'] == 'Cancel') {
    drupal_goto('place/new/cancel');
  }
  if (is_null($form_state['values']['alts'])) {
    form_set_error('alts', 'Please select an address option.');
  }
}

/**
 *  Submit Function for Add New Place, step 2
 *  Set the selected option, and continue to step 3 *
 */
function historicalsurvey_place_add_place_address_form_submit($form, &$form_state) {
  //set the selection data
  unset($_SESSION['historicalsurvey_place_add_place']['selection']);
  if ($form_state['values']['alts'] == 'Match_addr') {
    $_SESSION['historicalsurvey_place_add_place']['selection']['geo_data'] = $_SESSION['historicalsurvey_place_add_place']['results']['geo_data'];
  }
  elseif ($form_state['values']['alts'] == 'user_address') {
    $_SESSION['historicalsurvey_place_add_place']['selection']['user_address'] = $_SESSION['historicalsurvey_place_add_place']['results']['location'];
  }
  elseif (is_numeric($form_state['values']['alts'])) {
    $key = $form_state['values']['alts'];
    $_SESSION['historicalsurvey_place_add_place']['selection']['geo_data'] = $_SESSION['historicalsurvey_place_add_place']['results']['geo_data']['data']['alternatives'][$key];
  }
  //unset($_SESSION['historicalsurvey_place_add_place']['results']);
  $_SESSION['historicalsurvey_place_add_place']['step2'] = TRUE;

  drupal_goto('place/new/map');
}

/**
 *  Form Function for Add New Place, step 3
 *  Display the map with pre-geocoded point, if any
 *  Allow user to change pin point
 *
 */
function historicalsurvey_place_add_place_map_form() {
  $selection = isset($_SESSION['historicalsurvey_place_add_place']['selection']) ? $_SESSION['historicalsurvey_place_add_place']['selection'] : FALSE;
  if (isset($selection['geo_data'])) {
    $redpin[] = array(
      'lng' => $selection['geo_data']['coords'][0],
      'lat' => $selection['geo_data']['coords'][1],
    );
    $instructions = '<div id="instructions">We\'ve estimated the location of <b>' . $selection['geo_data']['data']['Match_addr'] . '</b> below. Re-position the pin if needed.</div>';
  }
  elseif (isset($selection['user_address'])) {
    $instructions = '<div id="instructions">Select a point that best represents the location of <b>' . $selection['user_address'] . '</b>.</div>';
    $redpin = FALSE;
  }
  elseif (isset($_SESSION['historicalsurvey_place_add_place']['noaddress'])) {
    $instructions = '<div id="instructions">Select a point that best represents the location of this place.</div>';
    $redpin = FALSE;
  }
  else {
    drupal_set_message("Something went wrong. Please try again, or contact us about the problem using our " . l('Contact', 'contact') . " form.", 'warning');
    drupal_goto('node/add/place');
  }

  $form['instructions'] = array(
    '#type' => 'item',
    '#weight' => 1,
    '#markup' => $instructions,
  );
  $form['map'] = array(
    '#type' => 'item',
    '#weight' => 3,
    '#theme' => 'historicalsurvey_map',
    '#redpins' => $redpin ? $redpin : array(),
    '#viewmode' => 'create',
  );
  $form['selectedLat'] = array(
    '#type' => 'hidden',
    '#attributes' => array('id' => 'edit-selectedLat'),
  );
  $form['selectedLng'] = array(
    '#type' => 'hidden',
    '#attributes' => array('id' => 'edit-selectedLng'),
  );
  $form['actions'] = array(
    '#type' => "actions",
    '#weight' => 2,
    'continue' => array(
      '#weight' => 2,
      '#type' => 'submit',
      '#value' => t('Continue'),
    ),
    'cancel' => array(
      '#weight' => 3,
      '#type' => 'submit',
      '#value' => t('Cancel'),
    ),
  );
  if (isset($_SESSION['historicalsurvey_place_add_place']['step2'])) {
    $form['actions']['back'] = array(
      '#weight' => 1,
      '#type' => 'submit',
      '#value' => t('Back to Address Options'),
    );
  }
  return $form;
}


/**
 *  Validation function for Add New Place, step 3
 */
function historicalsurvey_place_add_place_map_form_validate($form, &$form_state) {
  if ($form_state['values']['op'] == 'Back to Address Options') {
    drupal_goto('place/new/address');
  }
  if ($form_state['values']['op'] == 'Cancel') {
    drupal_goto('place/new/cancel');
  }

  if (empty($form_state['values']['selectedLat']) || empty($form_state['values']['selectedLng'])) {
    form_set_error('map', 'Please select a point on the map');
  }
  else {
    //reverse geocode the point, just to make sure we are still within an acceptable location
    $lat_lng = $form_state['values']['selectedLat'] . ',' . $form_state['values']['selectedLng'];

    //citys reverse geocoder doesnt work right now
    //$geo_data=geocoder('coa',$lat_lng,array('reverse'));
  }
}

/**
 *  Submit function for Add New Place, step 3
 */
function historicalsurvey_place_add_place_map_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == 'Continue') {
    //create the record, goto edit form
    $selected_point = array(
      'lat' => $form_state['values']['selectedLat'],
      'lng' => $form_state['values']['selectedLng'],
    );
    $geo_data = array();
    $user_address = '';
    if (isset($_SESSION['historicalsurvey_place_add_place']['selection'])) {
      if (isset($_SESSION['historicalsurvey_place_add_place']['selection']['geo_data'])) {
        $geo_data = $_SESSION['historicalsurvey_place_add_place']['selection']['geo_data'];
      }
      if (isset($_SESSION['historicalsurvey_place_add_place']['selection']['user_address'])) {
        $user_address = $_SESSION['historicalsurvey_place_add_place']['selection']['user_address'];
      }
    }
    if ($nid = historicalsurvey_place_create_place($selected_point, $user_address, $geo_data)) {
      unset($_SESSION['historicalsurvey_place_add_place']);
      drupal_goto('node/' . $nid . '/edit/');
    }
    else {
      drupal_set_message("There was an error creating a new place. Please contact us", 'error');
      drupal_goto('place/new/cancel');
    }
  }
  else {
    drupal_goto('place/new/cancel');
  }
}

/**
 *  Callback Function for cancelling the Add New Place process
 */
function historicalsurvey_place_add_place_cancel() {
  unset($_SESSION['historicalsurvey_place_add_place']);
  if (isset($_GET['destination']) && drupal_valid_path($_GET['destination'])) {
    drupal_goto($_GET['destination']);
  }
  else {
    drupal_goto('place/new');
  }
}

/**
 *  Create blank place records
 *  At minimum, requires only a lat-lng point: $selected_point, an array
 *  Also accepts both:
 *    $user_address , a string, for un-geocoded or custom addresses
 *    $geo_data, an array of geocoded and parsed out address components from the City of Austin Geocoding Service
 *
 *  Returns the new node ID if sucessful
 */
function historicalsurvey_place_create_place($selected_point, $user_address = '', $geo_data = array(),$publish_it=FALSE) {
  //dw($geo_data);
  $node = new stdClass();
  $node->type = 'place';
  $node->status = 1;
  $node->language = LANGUAGE_NONE;
  
  node_object_prepare($node);

  $node->field_location[LANGUAGE_NONE][0]['x'] = $selected_point['lng'];
  $node->field_location[LANGUAGE_NONE][0]['y'] = $selected_point['lat'];

  if (!empty($user_address)) {
    $node->field_location[LANGUAGE_NONE][0]['location'] = $user_address;
  }
  if (!empty($geo_data)) {
    $node->field_location[LANGUAGE_NONE][0]['Match_addr'] = $geo_data['data']['Match_addr'];
    $node->field_location[LANGUAGE_NONE][0]['HouseNum'] = $geo_data['data']['HouseNum'];
    $node->field_location[LANGUAGE_NONE][0]['HouseNumFrac'] = $geo_data['data']['HouseNumFrac'];
    $node->field_location[LANGUAGE_NONE][0]['PreDir'] = $geo_data['data']['PreDir'];
    $node->field_location[LANGUAGE_NONE][0]['PreType'] = $geo_data['data']['PreType'];
    $node->field_location[LANGUAGE_NONE][0]['StreetName'] = $geo_data['data']['StreetName'];
    $node->field_location[LANGUAGE_NONE][0]['StreetType'] = $geo_data['data']['StreetType'];
    $node->field_location[LANGUAGE_NONE][0]['SufDir'] = $geo_data['data']['SufDir'];
    $node->field_location[LANGUAGE_NONE][0]['Ref_ID'] = $geo_data['data']['Ref_ID'];
    $node->field_location[LANGUAGE_NONE][0]['Jurisdiction'] = $geo_data['data']['Jurisdiction'];
  }
  $node->field_location[LANGUAGE_NONE][0]['changed'] = 1;
  $node->field_location[LANGUAGE_NONE][0]['replaced'] = 1;
  
  if($publish_it) {
    $node->workbench_moderation_state_new='published';
  }  
  
  node_save($node);
  
  if (isset($node->nid)) {
    return $node->nid;
  }
  else {
    return;
  }
}

/**
 *
 */
function historicalsurvey_place_address_duplicate_validate($geo_data) {
  $matches = array();

  //REF ID
  //http://drupal.org/node/1343708
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'place')
    ->fieldCondition('field_location', 'Ref_ID', $form['field_location'][$lang][0]['Ref_ID']['#value'], '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    $items_nids = array_keys($result['node']);
    $matches += entity_load('node', $items_nids);
  }

  //address
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'place')
    ->fieldCondition('field_location', 'HouseNum', $form['field_location'][$lang][0]['HouseNum']['#value'], '=')
    ->fieldCondition('field_location', 'HouseNumFrac', $form['field_location'][$lang][0]['HouseNumFrac']['#value'], '=')
    ->fieldCondition('field_location', 'PreDir', $form['field_location'][$lang][0]['PreDir']['#value'], '=')
    ->fieldCondition('field_location', 'StreetName', $form['field_location'][$lang][0]['StreetName']['#value'], '=')
    ->fieldCondition('field_location', 'StreetType', $form['field_location'][$lang][0]['StreetType']['#value'], '=')
    ->fieldCondition('field_location', 'SufDir', $form['field_location'][$lang][0]['SufDir']['#value'], '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    $items_nids = array_keys($result['node']);
    $matches += entity_load('node', $items_nids);
  }

  //lat,lon
  $lat = $form['field_location'][$lang][0]['y']['#value'];
  $lon = $form['field_location'][$lang][0]['x']['#value'];

  $radius = variable_get('historicalsurvey_place_proximity_precision', 150); //in feet, variable set by admin

  $var_lat = round(($radius * 0.001) / (364.829 / 2), 5); //364.829 feet per 0.001 degree lat @ 30 deg lat
  $var_lon = round(($radius * 0.001) / (315.026 / 2), 5); //315.026 feet per 0.001 degree lon @ 30 deg lat
  $lat_low = round($lat, 4) -$var_lat;
  $lat_high = round($lat, 4) + $var_lat;
  $lon_low = round($lon, 4) -$var_lon;
  $lon_high = round($lon, 4) + $var_lon;

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'place')
    ->fieldCondition('field_geo', 'lat', array($lat_low, $lat_high), 'BETWEEN')
    ->fieldCondition('field_geo', 'lon', array($lon_low, $lon_high), 'BETWEEN');
  $result = $query->execute();
  if (isset($result['node'])) {
    $items_nids = array_keys($result['node']);
    $matches += entity_load('node', $items_nids);
  }

  if (!empty($matches)) {
    $text_matches = '<div id="matches-text">';
    $text_matches .= t("There " . format_plural(count($matches), 'is 1 existing place that potentially matches', 'are @count existing places that potentially match') . " this location.<br/>Please check to make sure you are not adding a duplicate record:");
    $text_matches .= "<ul>";
    foreach ($matches as $node) {
      $text_matches .= "<li>" . l($node->field_location['location'], 'node/' . $node->nid);
    }
    $text_matches .= "</ul>";
    $form['field_location'][$lang][0]['location']['#description']['matches'] = $text_matches;
    $text_matches .= '</div>';
  }
}

/**
 *
 */
function historicalsurvey_place_check_for_dup($location) {
  $matches = array();

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'place')
    ->fieldCondition('field_location', 'location', $location, '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    $items_nids = array_keys($result['node']);
    $matches += entity_load('node', $items_nids);
  }
  if (!empty($matches)) {
    return $matches;
  }
  return FALSE;
}


