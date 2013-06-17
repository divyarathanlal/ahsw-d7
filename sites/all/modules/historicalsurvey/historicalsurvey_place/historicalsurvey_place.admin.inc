<?php


/**
 *  Form function for admin page callback
 *  TODO: make more of these
 */
function historicalsurvey_place_config_form($form, &$form_state) {
  $form['historicalsurvey_place_geocoder_url'] = array(
    '#type' => 'textfield',
    '#title' => t("City of Austin ArcGIS REST API Geocoder URL"),
    '#size' => 120,
    '#default_value' => variable_get('historicalsurvey_place_geocoder_url', ''),
    '#required' => TRUE,
  );

  $form['historicalsurvey_place_reverse_geocoder_url'] = array(
    '#type' => 'textfield',
    '#title' => t("City of Austin ArcGIS REST API Reverse Geocoder URL"),
    '#description' => t("Please include 'http://' and no trailing slashes.<br/>eg. http://www.austintexas.gov/GIS/REST/Geocode/COA_Composite_Locator/GeocodeServer/findAddressCandidates/"),
    '#size' => 120,
    '#default_value' => variable_get('historicalsurvey_place_reverse_geocoder_url', ''),
    '#required' => TRUE,
  );

  $form['historicalsurvey_place_proximity_precision'] = array(
    '#type' => 'textfield',
    '#title' => t("Duplication Warning Radius (in Feet)"),
    '#description' => t("During New Place creation, existing records approximately within this radius of a New Place will be listed as possible duplicates."),
    '#size' => 6,
    '#field_suffix' => t('ft'),
    '#default_value' => variable_get('historicalsurvey_place_proximity_precision', '150'),
    '#required' => TRUE,
  );

  $form['historicalsurvey_place_jurisdictions'] = array(
    '#type' => 'checkboxes',
    '#title' => t("Select all acceptable jurisdictions for New Places"),
    '#options' => historicalsurvey_place_get_city(),
    '#default_value' => variable_get('historicalsurvey_place_jurisdictions', array(210)),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}
