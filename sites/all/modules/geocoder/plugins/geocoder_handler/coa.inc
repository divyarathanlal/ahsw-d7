<?php
// $Id$

/**
 * @file
 * Plugin to provide a geocoder via City of Austin GIS REST API.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t("City of Austin GIS Geocoder"),
  'description' => t('Geocodes via City of Austin GIS REST API'),
  'callback' => 'geocoder_coa',
  'field_types' => array('historicalsurvey_location'),
  'field_callback' => 'geocoder_coa_field',
  'settings_callback' => 'geocoder_coa_form',
);

/**
 * Process Markup
 */
function geocoder_coa($location, $options = array()) {
  //geophp_load();
  if(in_array('reverse',$options)){
    return geocoder_coa_reverse($location);
  }

  //http://sampleserver1.arcgisonline.com/ArcGIS/SDK/REST/index.html
  //http://www.austintexas.gov/GIS/REST/Geocode/COA_Composite_Locator/GeocodeServer/
  $query = array(
    //'Street' => str_replace(" ","+",$location),
    'outFields' => 'Score,Jurisdiction,Ref_ID,HouseNum,HouseNumFrac,PreDir,PreType,StreetName,StreetType,SufDir,Match_addr',
    'outSR' => '4326',
    'f' => 'pjson',
  );
    
  //variable set via OHS Place module admin settings page
  $path = variable_get('historicalsurvey_place_geocoder_url','http://www.austintexas.gov/GIS/REST/Geocode/COA_Address_Locator/GeocodeServer/findAddressCandidates/');
  $url = url($path, array('query' => $query));
  $url .= '&Street='.str_replace(" ","+",$location); //keep the plus signs in
  $result = drupal_http_request($url);
  if($json_request = (json_decode($result->data))){
    if(isset($json_request->candidates)){
        $candidates = $json_request->candidates;
        foreach ($candidates as $item){
          $geom['coords'] = array($item->location->x, $item->location->y);
              
          $geom['data']['Match_addr']=!empty($item->attributes->Match_addr)?$item->attributes->Match_addr:NULL;
          $geom['data']['HouseNum']=!empty($item->attributes->HouseNum)?$item->attributes->HouseNum:NULL;
          $geom['data']['HouseNumFrac']=!empty($item->attributes->HouseNumFrac)?$item->attributes->HouseNumFrac:NULL;
          $geom['data']['PreDir']=!empty($item->attributes->PreDir)?$item->attributes->PreDir:NULL;
          $geom['data']['PreType']=!empty($item->attributes->PreType)?$item->attributes->PreType:NULL;
          $geom['data']['StreetName']=!empty($item->attributes->StreetName)?$item->attributes->StreetName:NULL;
          $geom['data']['StreetType']=!empty($item->attributes->StreetType)?$item->attributes->StreetType:NULL;
          $geom['data']['SufDir']=!empty($item->attributes->SufDir)?$item->attributes->SufDir:NULL;
          $geom['data']['Ref_ID']=!empty($item->attributes->Ref_ID)?$item->attributes->Ref_ID:NULL;
          $geom['data']['Jurisdiction']=!empty($item->attributes->Jurisdiction)?historicalsurvey_place_get_city($item->attributes->Jurisdiction):NULL;
          $geom['data']['x']=!empty($item->location->x)?$item->location->x:NULL;
          $geom['data']['y']=!empty($item->location->y)?$item->location->y:NULL;
          $geom['data']['score']=$item->attributes->Score?$item->attributes->Score:NULL;
          
          $geometries[] = $geom;
        }

        if (empty($geometries)){
          return;  //no results
        }
        // The connonical geometry is the first result (best guess)
        $geometry = array_shift($geometries);
        
        // If there are any other geometries, these are auxiliary geometries that represent "alternatives"
        if (count($geometries)){
          $geometry['data']['alternatives'] = $geometries;
        }
        return $geometry;
    }else{
      $geometry['error']=t('Error retrieving geocoding data from ') . $path . '<br/>' . t("Please contact the webmaster");
      return $geometry;
    }
  }else{
    $error=$result->error?$result->error.'<br/>':'';
    $error.=$result->code?'CODE:'.$result->code.'<br/>':'';
    $error.=t("Please contact the webmaster");
    
    $geometry['error']=t('Error retrieving geocoding data from ') . $path . '<br/>' . $error;
    return $geometry;
  }
}

function geocoder_coa_reverse($location,$distance="10"){
  //http://www.austintexas.gov/GIS/REST/Geocode/COA_Address_Locator/GeocodeServer/reverseGeocode
  //http://help.arcgis.com/en/arcgisserver/10.0/apis/rest/index.html?reverse.html
  
  $location_query=array(
    'x'=>$location['lng'],
    'y'=>$location['lat'],
    'spatialReference'=>array(
      'wkid' => 104199,
    ),
  );
  
  $query = array(
    //'Street' => str_replace(" ","+",$location),
    'location' => json_encode($location_query),
    'distance' => $distance,
    'outSR' => '104199',
    'f' => 'pjson',
  );
  $path = 'http://www.austintexas.gov/GIS/REST/Geocode/COA_Composite_Locator/GeocodeServer/reverseGeocode/';
  $url = url($path, array('query' => $query));
  $result = drupal_http_request($url);
  if($json_request = (json_decode($result->data))){
    if(isset($json_request->address)){
      return geocoder('coa', $json_request->address->Street);      
    }else{
      //often it works to increase the search distance
      if($distance != "100") {
        geocoder_coa_reverse($location,"100");
      }else{      
        $address['error']=t('Error retrieving geocoding data from ') . $path . '<br/>' . t("Please contact the webmaster");
        return $address;
      }
    }
  }
  else{
    $error=$result->error?$result->error.'<br/>':'';
    $error.=$result->code?'CODE:'.$result->code.'<br/>':'';
    $error.=t("Please contact the webmaster");
    
    $address['error']=t('Error retrieving geocoding data from ') . $path . '<br/>' . $error;
    return $address;
  }
}


function geocoder_coa_field($field, $field_item, $options = array()) {
  //debug_write($field_item);
  if ($field['type'] == 'historicalsurvey_location' && !empty($field_item['location'])){
    return geocoder_coa($field_item['location'], $options);
  }
}

function geocoder_coa_form($default_values = array()) {
  $form = array();
  
  return $form;
}
