<?php




function historicalsurvey_admin_menu() {
  $items['import'] = array(
    'page callback' => 'drupal_get_form',
    'page arguments' => array('historicalsurvey_admin_import'),
    'access callback' => TRUE,
  );
  $items['import-from-d6'] = array(
    'page callback' => 'historicalsurvey_admin_import_from_d6',
    'access callback' => TRUE,
  );
  $items['import-users-from-d6'] = array(
    'page callback' => 'historicalsurvey_admin_import_users_from_d6',
    'access callback' => TRUE,
  );
  $items['update-meta'] = array(
    'page callback' => 'historicalsurvey_admin_update_meta',
    'access callback' => TRUE,
  );
  $items['update-version-meta'] = array(
    'page callback' => 'historicalsurvey_admin_update_version_meta',
    'access callback' => TRUE,
  );
  
  return $items;
}

function historicalsurvey_admin_import_from_d6() {  
  set_time_limit(1000000);

  include(drupal_get_path('module','historicalsurvey_place').'/historicalsurvey_place.create.inc');
  
  if(!isset($_SESSION['meta'])) {  
    dw('');    
    
    //$filter_nids=array('272684');
    //$filter_nids=array('50073','272684');
    //$filter_nids=FALSE;
    
    //$records = get_data($filter_nids);  
        
    //$records = clear_duplicates($records);    
    
    //dw($records);
        
    //$data = convert_to_d7($records);
    
    //dw($data);
    
/*     $ser=base64_encode(serialize($data));
    file_put_contents('d6-migrate-encoded.txt',$ser);*/
    $file=file_get_contents('d6-migrate-encoded.txt');
    $data=unserialize(base64_decode($file));
    
    //dw(count($data));  //on 02-14-13 it was 3917, on 06-09-13 it was 5094
    
    //dw($data);  
    
    //$non_loaded_data = get_non_loaded($data);
    
    //dw($non_loaded_data);
    //dw(count($non_loaded_data),'',1);
   
    //check_migration($non_loaded_data);
    
    //migrate_fields($data);
    
    //migrate_files($data);

    return 'ok';
  }else{
    unset($_SESSION['meta']);
    //print 'done';
    return "done";
  }
}

function migrate_fields($data) {
  $meta_updates=array();    
  $count=0;
  $total_count=count($data);
  $starttime=microtime(TRUE);
  foreach($data as $place_nid=>$fields) {  
    if($count<6000) {
      $timenow=microtime(TRUE);
      //dw("started conversion of old_nid: ".$place_nid,round(($timenow-$starttime),4),1);
      $return=import_fields_to_d7($place_nid,$fields);
      if(!empty($return)) {
        db_query("UPDATE old_nid_to_new_nid SET converted=1 WHERE old_nid=".$place_nid);
        $count++;
        $timenow=microtime(TRUE);
        dw("converted ".$count." of ".$total_count." (old_nid: ".$place_nid.")",round(($timenow-$starttime),4),1);        
      }
      else{
        $timenow=microtime(TRUE);
        dw("ERROR nothing converted for old_nid:".$place_nid,round(($timenow-$starttime),4),1);
      }
      $meta_updates+=$return;   
    }
  }
  
  file_put_contents('d6-migrate-meta-updates-serialized.txt',serialize($meta_updates));
  
  if(!empty($meta_updates)) {
    dw('Starting to update the meta data now','',1);
    $_SESSION['meta']=$meta_updates;
    drupal_goto('update-version-meta');
  }
}

function migrate_files($data) {
  $meta_updates=array();
  $count=0;
  $total_count=count($data);
  $starttime=microtime(TRUE);
  //dw($data);
  
  foreach($data as $place_nid=>$fields) {
    $return=import_files_to_d7($place_nid,$fields);
    if(!empty($return)) {
      db_query("UPDATE old_nid_to_new_nid SET converted=1 WHERE old_nid=".$place_nid);
      $count++;
      $timenow=microtime(TRUE);
      dw("updated ".$count." of ".$total_count." (old_nid: ".$place_nid.")",round(($timenow-$starttime),4),1);
      $meta_updates+=$return;   
      //break; //temp
    }
    else{
      $timenow=microtime(TRUE);
      dw("nothing converted for old_nid:".$place_nid,round(($timenow-$starttime),4),1);
    } 
  }
  if(!empty($meta_updates)) {
    $_SESSION['meta']=$meta_updates;
    drupal_goto('update-version-meta');
  }
}

function historicalsurvey_admin_update_meta() {
  
  if(isset($_SESSION['meta'])) {
    $meta_updates = $_SESSION['meta'];

    foreach($meta_updates as $nid => $meta) {    
      db_query("UPDATE node SET uid=".$meta['uid'].", created=".$meta['timestamp'].", changed=".$meta['timestamp']." WHERE nid=".$nid);    
      db_query("UPDATE node_revision SET uid=".$meta['uid'].", timestamp=".$meta['timestamp']." WHERE nid=".$nid);    
    }
    
  }
  
  $_SESSION['meta']='';
  drupal_goto('import-from-d6');
}

function historicalsurvey_admin_update_version_meta() {
  
  if(isset($_SESSION['meta'])) {
    $meta_updates = $_SESSION['meta'];

    foreach($meta_updates as $nid => $vids) {
      foreach($vids as $vid => $meta) {
        db_query("UPDATE node SET uid=".$meta['uid'].", changed=".$meta['timestamp']." WHERE nid=".$nid." AND vid=".$vid);    
        db_query("UPDATE node_revision SET uid=".$meta['uid'].", timestamp=".$meta['timestamp']." WHERE nid=".$nid." AND vid=".$vid);  
      }
    }
  }
  else{
    dw('ERROR no meta data. Try the file','',1);
    
    /* $file=file_get_contents('d6-migrate-meta-updates-serialized.txt');
    $meta_updates=unserialize($file);
    foreach($meta_updates as $nid => $vids) {
      dw('Loading meta for: '.$nid,'',1);
      foreach($vids as $vid => $meta) {
        db_query("UPDATE node SET uid=".$meta['uid'].", changed=".$meta['timestamp']." WHERE nid=".$nid." AND vid=".$vid);    
        db_query("UPDATE node_revision SET uid=".$meta['uid'].", timestamp=".$meta['timestamp']." WHERE nid=".$nid." AND vid=".$vid);  
      }
    } */
  }
  
  $_SESSION['meta']='';
  drupal_goto('import-from-d6');
}

function get_non_loaded($data) {
  
  $old_nids = array_keys($data);
  
  $query="SELECT old_nid FROM old_nid_to_new_nid";   
  $result=db_query($query);
  if($result) {    
    while($row=$result->fetchAssoc()) {
      $loaded[]=$row['old_nid'];
    }
  }
  $return=array();
  foreach (array_diff($old_nids,$loaded) as $nid) {
    $return[$nid]=$data[$nid];
  }
  
  return $return;
  
}


function check_migration($places) {  

  $no_matches=array();
  $matches=FALSE;

  foreach ($places as $place_nid=>$fields) {  
    if(isset($fields['field_location'])) {
      $location = $fields['field_location']['location'];
      $matches=historicalsurvey_place_check_for_dup($location);
    }
    else{
      dw($fields,"No field_location in ".$place_nid,1);
    }
    if(!$matches) {
      $no_matches[$place_nid]=$fields;
    }
  }
  
  dw('found '.count($no_matches).' to be new records','',1);  
  import_to_d7($no_matches);
}

function import_files_to_d7($place_nid,$fields) {
  
    //make sure weve imported this one already, and that we havent added data for it yet
  $query="SELECT new_nid,converted FROM old_nid_to_new_nid WHERE old_nid=".$place_nid;    
  $result=db_query($query);
  if($result) {      
    if($row=$result->fetchAssoc()) {
      $new_nid=$row['new_nid'];
      $converted=$row['converted'];
    }
  }
  
  if(isset($new_nid) && $converted!=1 /* && in_array($new_nid,array(83,628,170,201) )*/) {
    
    if(isset($fields['field_photo'])) {
      $files['field_photo']=$fields['field_photo'];
    }
    if(isset($fields['field_document'])) {
      $files['field_document']=$fields['field_document'];
    }
    if(!isset($files)) {
      return FALSE;
    }
    
    //dw($files);die;
    
    $node=node_load($new_nid); 
    $old_vid=$node->vid;
    $node->status = 1;
    $node->workbench_moderation_state_new='published';            
    node_object_prepare($node);
    
    
    $query="SELECT field_location_location FROM field_revision_field_location WHERE entity_id=".$new_nid;    
    $result=db_query($query);
    if($result) {      
      if($row=$result->fetchAssoc()) {
        $location=$row['field_location_location'];
      }
    }
    
    
    //arrange by timestamp
    foreach($files as $field_name => $deltas) {
      foreach($deltas as $delta => $columns) {
        if(is_numeric($delta) && is_array($columns)) {
          $timestamp = 1370226853;
          $files_timed[$timestamp]['uid']=$columns['uid'];
          $files_timed[$timestamp][$field_name][]=$columns;
        }        
      }
    }
    ksort($files_timed);
    $img_count=0;
    
    foreach($files_timed as $timestamp => $fields) {     
      foreach ($node as $node_field => $values) {       
        if(strpos($node_field,'field_')===0) {
          unset($node->{$node_field});
        }
      }    
      $uid=$fields['uid'];
      unset($fields['uid']);    
      
      foreach($fields as $field_name=>$deltas) {        
        foreach($deltas as $delta => $columns) {         
          if(is_numeric($delta) && is_array($columns)) {
            $filepath=$columns['filepath'];
            $filepath="/mnt/historicalsurvey_files/" . $columns['filepath'];
            $file = (object) array(
              'uid' => 1,
              'uri' => $filepath,
              'filemime' => file_get_mimetype($filepath),
              'status' => 1,
            );
            
            if($field_name=='field_photo') {
              $img_count++;
              // We save the file to the root of the files directory.
              $file = file_copy($file, 'public://photos');

              $file->main_image=$img_count==1?1:0;
              if(!empty($location)) {
                $file->alt=$location;
                $file->title=$location;
              }
              $image_size=getimagesize($filepath);
              if($image_size) {
                $file->width=$image_size[0];
                $file->height=$image_size[1];
              }
              
              $file->description = '';
              $file->description .= isset($columns['description'])?$columns['description'] . '; ' :'';
              $file->description .= isset($columns['type'])?'Type: '.$columns['type'] . '; ' :'';
              $file->description .= isset($columns['facing'])?'Heading: '.$columns['facing'] . '; ' :'';
              $file->description .= isset($columns['photographer'])?'Photographer: '.$columns['photographer'] . '; ' :'';
              $file->description .= isset($columns['date'])?'Date: '.$columns['date'] . '; ' :'';
              $file->description = rtrim($file->description,'; ');
            }
            elseif($field_name=='field_document') {            
              $file = file_copy($file, 'public://documents');
              
              $file->description=isset($columns['description'])?$columns['description']:'';
            }
            
            $file->display=1;
            $file->changed=1;
            $file->replaced=0;
            $node->{$field_name}[LANGUAGE_NONE][$delta] = (array)$file;
          }
        }
      }
      node_save($node);
      //dw($node,$timestamp,1);
      
      
      if($old_vid!=$node->vid) {
        $meta_updates[$new_nid][$node->vid] = array(
          'uid' => $uid,
          'timestamp' => $timestamp,
        );  
      }
    }   
  }
  return isset($meta_updates)?$meta_updates:array();
  
}

function import_fields_to_d7($place_nid,$fields) {

  //make sure weve imported this one already, and that we havent added data for it yet
  $query="SELECT new_nid,converted FROM old_nid_to_new_nid WHERE old_nid=".$place_nid;    
  $result=db_query($query);
  if($result) {      
    if($row=$result->fetchAssoc()) {
      $new_nid=$row['new_nid'];
      $converted=$row['converted'];
    }
    else {
      $bad_places['no new nid'][]=$place_nid;
    }
  }
  else {
    $bad_places['no new nid'][]=$place_nid;
  }
  
  $timestamps=arrange_by_timestamp($fields);    
  unset($fields);
  
  if(isset($new_nid) && $converted!=1) {
    //unset($timestamps[0]);
    
    //dw($timestamps);die;
    
    foreach($timestamps as $timestamp => $fields) {
      $formated_values=$uids=array();
      
      foreach($fields as $field_name => $values) {
        unset($values['#timestamp']);
        
        //filter out fields with incomplete meta
        if(isset($values['#uid']) || isset($values['uid'])) {
          
          //nid, ie survey effort
          if(isset($values['nid'])) {
            //we'll do this later
            unset($values['nid']);
          }
                  
          
          //values
          if(isset($values['value']) && !empty($values['value'])) {
            if(is_array($values['value'])) {
              foreach($values['value'] as $delta=>$value) {
                $formated_values[$field_name][$delta]['value'] = $value;
              }
            }
            else {
              $formated_values[$field_name][0]['value'] = $values['value'];
            }            
            unset($values['value']);
          }
          
          //terms
          if(isset($values['tid']) && !empty($values['tid'])) {
            if(is_array($values['tid'])) {
              foreach($values['tid'] as $delta=>$name) {   
                $term = taxonomy_get_term_by_name($name);
                $term = array_shift($term);
                if(empty($term)){
                
                  //designations...
                  if(strpos($name,'/')) {  
                    list($parent,$child)=explode('/',$name);
                    $parent_term = taxonomy_get_term_by_name($parent);
                    $parent_term = array_shift($parent_term);
                    $child_term = taxonomy_get_term_by_name($child);
                    if(!empty($parent_term) && !empty($child_term)) {                        
                      foreach(array_keys($child_term) as $tid) {
                        $parents = taxonomy_get_parents($tid);   
                        foreach($parents as $parent) {
                          if($parent->name == $parent_term->name) {
                            $term = $child_term[$tid];
                          }
                        }
                      }
                    }
                  }
                  
                }               
                if(isset($term->tid)) {
                  $formated_values[$field_name][$delta]['tid'] = (int) $term->tid;   
                  //dw("Term: '".$name."' found as '".$term->name."' (".$term->tid.")",$field_name,1);
                }
                else{                           
                  dw('Term not found: '.$name,'',1);
                }
              }
            }
            else {
              $term=taxonomy_get_term_by_name($values['tid']);
              $term=array_shift($term);
              if(isset($term->tid)) {
                $formated_values[$field_name][0]['tid'] = (int) $term->tid;
              }
            }
            
            unset($values['tid']);
          }
          
          //placetype
          if(isset($values['place_type']) && !empty($values['place_type'])) {
            $term=taxonomy_get_term_by_name($values['place_type']);
            $term=array_shift($term);
            $formated_values[$field_name][0]['tid'] = (int) $term->tid;
            
            unset($values['place_type']);
          }
          
          if(isset($values['estimated_early_year']) && !empty($values['estimated_early_year'])) {
            $formated_values[$field_name][0]['estimated_early_year'] = $values['estimated_early_year'][0];
            //unset($values['estimated_early_year']);
          }            
          if(isset($values['estimated_late_year']) && !empty($values['estimated_late_year'])) {
            $formated_values[$field_name][0]['estimated_late_year'] = $values['estimated_late_year'][0];
            //unset($values['estimated_late_year']);
          }            
          if(isset($values['actual_early_year']) && !empty($values['actual_early_year'])) {
            $formated_values[$field_name][0]['actual_early_year'] = $values['actual_early_year'][0];
            //unset($values['actual_early_year']);
          }            
          if(isset($values['actual_late_year']) && !empty($values['actual_late_year'])) {
            $formated_values[$field_name][0]['actual_late_year'] = $values['actual_late_year'][0];
            //unset($values['actual_late_year']);
          }
          
          $notes='';
          if(isset($values['#sources']) && !empty($values['#sources'])) {
            $notes .= "Sources: ".$values['#sources'];
          }            
          if(isset($values['#notes']) && !empty($values['#notes'])) {    
            $notes .= "Notes: ".$values['#notes'];
          }
          if(!empty($notes)) {        
            $formated_values[$field_name][0]['notes'] = $notes;
            unset($values['#sources']);
            unset($values['#notes']);
          }
          
          $uid = isset($values['#uid'])?$values['#uid']:$values['uid'];            
          $uids[$uid]=$uid;
          if(isset($values['#uid'])){unset($values['#uid']);}
          if(isset($values['uid'])){unset($values['uid']);}
          if(isset($values['review_level'])){unset($values['review_level']);}
          
          
          if(!empty($values)) {
            //dw($values,'',1);
          }
        }          
      }
      $uid=array_shift($uids);
      
      //dw($formated_values,'',1);     
      //$vid=FALSE;
      $vid=add_fields_to_place($new_nid,$formated_values);
      if($vid) {
        $meta_updates[$new_nid][$vid] = array(
          'uid' => $uid,
          'timestamp' => $timestamp,
        );
      }
    }
    //dw($newnode,'',1);      
  }
  return isset($meta_updates)?$meta_updates:array();
}

function add_fields_to_place($nid,$formated_values) {
  
  $node=node_load($nid);  
  $node->status = 1;
  $node->workbench_moderation_state_new='published';            
  node_object_prepare($node);
  $lang=$node->language;  
  
  foreach ($formated_values as $field_name => $values) {
    $replace[$field_name]=empty($node->{$field_name})?1:0;
  }
  
  foreach ($node as $node_field => $values) {       
    if(strpos($node_field,'field_')===0) {
      unset($node->{$node_field});
    }
  }
  
  $count=0;
  foreach ($formated_values as $field_name => $values) {
    foreach($values as $delta => $value_array) {
      $values[$delta]['replaced'] = $replace[$field_name];
      $values[$delta]['changed'] = 1;
    }
    $node->{$field_name}[$lang]=$values;
    $count++;
  }  

  node_save($node);  
  return $node->vid?$node->vid:FALSE;
}

function arrange_by_timestamp($fields) {
  foreach($fields as $field_name => $versions) {
    if(in_array($field_name,array('field_location','field_photo','field_document'))) {
      //$newfields[0][$field_name] = $versions;
    }
    else{
      foreach($versions as $timestamp => $values) {
        
        //while we're at it, filter out values that have incomplete meta
        if(isset($values['#uid']) || isset($values['uid'])) {          
          $newfields[$timestamp][$field_name]=$values;
        }
        
      }
    }
  }
  ksort($newfields);
  return $newfields;
}

function import_to_d7($places){
  $places_with_bad_geocode=$duplicates=$meta_updates=array();

  //dw($places);
  $count=$nid_count=0;
  
  foreach ($places as $place_nid=>$fields) {
    
    //make sure we havent imported this one yet
    $addit=FALSE;
    $query="SELECT new_nid FROM old_nid_to_new_nid WHERE old_nid=".$place_nid;
    $result=db_query($query);
    if($result) {      
      if(!$result->fetchAssoc()) {
        $addit=TRUE;
      }
    }
    
    //if we havent added it yet, geocode it to get COA location data
    if($addit) { 
      $reverse_it=FALSE;
      $create=TRUE;
      $address_to_geocode=NULL;
      
      $coords=array('lat'=>$fields['field_location']['y'],'lng'=>$fields['field_location']['x']);
      $location=$fields['field_location']['location'];
      
      //$matches=historicalsurvey_place_check_for_dup($location);
      
      if(!empty($fields['field_location']['geocode_num']) && !empty($fields['field_location']['geocode_street'])) {
        $address_to_geocode = $fields['field_location']['geocode_num'] . ' ' . $fields['field_location']['geocode_street'];
        
        $geo_data = geocoder('coa', $address_to_geocode);  

        if(isset($geo_data['error'])) {
          $places_with_bad_geocode[$place_nid]['error']=array(
            'error'=>$geo_data['error'],
            'address_to_geocode' =>$address_to_geocode,
          );
          $create=FALSE;
        }
        
        elseif($geo_data['data']['score'] != 100) {
          
          //no good matches, so just leave geo-data blank
          if(!empty($geo_data['data']['Match_addr'])) {
            $geo_data=array();
            $create=TRUE;
          }
          else{
            $reverse_it=TRUE;
          }
        }            
      }else{
        $reverse_it=TRUE;
      }
      
      //if the address didnt geocode, try the lat/lng points
      if($reverse_it) {
        $geo_data = geocoder('coa', $coords, array('reverse'));
        
        if(isset($geo_data['error'])) {
          $places_with_bad_geocode[$place_nid]['error']=array(
            'error'=>$geo_data['error'],
            'coords_to_geocode' =>$coords,
          );        
          $create=FALSE;
        }
        
        elseif(!empty($geo_data['data']['Match_addr'])) {        
          $create=TRUE;
        }else{          
          //no good matches, so just leave geo-data blank
          $geo_data=array();
          $create=TRUE;
        }
      }
      
      //after attempting geocode, make the new place record
      if($create) {
        if(is_array($coords)) { 
        
          if(!is_array($geo_data)) {
            $geo_data=array();
          }        
          
          //all places should have an non-empty location
          if(!empty($location)) { 
          
            $nid=historicalsurvey_place_create_place($coords,$location,$geo_data,TRUE);
            
            db_query("INSERT INTO old_nid_to_new_nid (old_nid,new_nid) VALUES (".$place_nid.",".$nid.")");
            
            $meta_updates[$nid] = array(
              'uid' => $fields['field_location']['uid'],
              'timestamp' => $fields['field_location']['timestamp'],
            ); 
            
            $nid_count++;
          }
          else {
            $places_with_bad_geocode[$place_nid]['nolocation'] = array (
              'coords'=>$coords,
              'location'=>$location,
            );
          }
        }
      }
    }
    else{
      dw($place_nid." already in old>new table");
    }
    $count++;
    /* if($nid_count>2){
      break;
    } */
  }
  
  if(!empty($places_with_bad_geocode)) {
    dw($places_with_bad_geocode,'bad_geocode',1);
  }
  dw('created ' . $nid_count.' of '.$count,'new_nids',1);
  //dw($duplicates,'duplicates',1);
  
  $_SESSION['meta']=$meta_updates;
  drupal_goto('update-meta');
}

function historicalsurvey_admin_import() {

  $custom_fields=variable_get('historicalsurvey_place_custom_fields', array());
  $field_filter_out = array("contains","within","location","survey_effort","tag","photo","document");
  
  foreach($custom_fields['unreviewed_fields'] as $type => $fields){
    foreach($fields as $machine => $label){
      if(!in_array($machine,$field_filter_out)){
        $field_options[$machine]=array($label);
      }
    }
  }foreach($custom_fields['reviewed_fields'] as $type => $fields){
    foreach($fields as $machine => $label){
      if(!in_array($machine,$field_filter_out)){
        $field_options[$machine]=array($label);
      }
    }
  }
  
  $form['template'] = array(
    '#type' => 'fieldset',
    '#title' => 'Build CSV Import Template',
  );
  $form['template']['placeid'] = array(
    '#type' => 'checkbox',
    '#title' => 'Include Existing Place ID',
    '#description' => "Adds a column for an existing place ID number",
  );
  $form['template']['revisionid'] = array(
    '#type' => 'checkbox',
    '#title' => 'Include Revisions',
    '#description' => "(For Migration Only) Adds a column to accomodate for multiple field versions",
  );
  $form['template']['author'] = array(
    '#type' => 'checkbox',
    '#title' => 'Include Author(s) per Field',
    '#description' => "Adds a column per field for author(s), otherwise all data will submit under your name.",
  );
  $form['template']['timestamp'] = array(
    '#type' => 'checkbox',
    '#title' => 'Include Timestamp per Field',
    '#description' => "Adds a column per field for timestamp, otherwise all data will submit under current time.",
  );
  $form['template']['review_level'] = array(
    '#type' => 'radios',
    '#title' => 'Select Review Level',
    '#default_value' => '1',
    '#options' => array('1' => 'Unreviewed', '2' => 'Professional Review' , '3' => 'Preservation Office Review'),
  );  
  $form['template']['fields'] = array(
    '#type' => 'tableselect',
    '#header' => array('Select Fields'),
    '#options' => $field_options,
  );
  $form['template']['actions'] = array( 
    'get_template' => array(
      '#type' => 'submit',
      '#value' => 'Download',
      '#submit' => array('historicalsurvey_admin_get_template'),
    ),
  );  
  return $form;
}

function historicalsurvey_admin_import_validate($form,&$form_state) {
  $ok=FALSE;
  foreach($form_state['values']['fields'] as $field){
    if(!empty($field)){
      $ok=TRUE;
    }
  }
  if(!$ok){
    form_set_error('','Select a Field');
  }
  
}

function historicalsurvey_admin_get_template($form,$form_state) {

  $template['name'][]='';
  $template['label'][]='Label';
  $template['description'][]='Description';
  $template['allowed_values'][]='Allowed Values';
  
  
  
 
  $timestamp=$form_state['values']['timestamp'];
  $author=$form_state['values']['author'];
  
  //dw($form_state['values']);
  if($version=$form_state['values']['placeid']){    
    $template['name'][]='place_id';
    $template['label'][]='Place ID';
    $template['description'][]='Must be an existing Place ID number.';
    $template['allowed_values'][]='Only Integers';
  }
  if($version=$form_state['values']['revisionid']){    
    $template['name'][]='version_number';
    $template['label'][]='Version Number';
    $template['description'][]='Start at 0 for earliest version, count up by 1 for each newer revision.';
    $template['allowed_values'][]='Only Integers, starting at zero, increasing by 1';
  }
  
  $column_filter_out = array('changed','replaced','promoted','format','alt','title','width','height','main_image','Match_addr','HouseNum','HouseNumFrac','PreDir','PreType','StreetName','StreetType','SufDir','Ref_ID','Jurisdiction');
  $review_level = $form_state['values']['review_level'];
  
  //always add location
  $fields_to_include[] = 'location';
  foreach($form_state['values']['fields'] as $field){
    if(!empty($field)){
      $fields_to_include[] = $field;
    }
  }
  
  $fields=field_info_instances("node","place");
  foreach($fields as $field_name => $field_info) {
    $field_include=FALSE;
    foreach($fields_to_include as $field) {      
      //if selected
      if(strpos($field_name,$field)!==FALSE){
        //and if of the selected revied level
        if(strpos($field_name,'_' . $review_level)!==FALSE) {
          $field_include=TRUE;
        }
        //or if non-reviewed 
        elseif(strpos($field_name,'_1')===FALSE && strpos($field_name,'_2')===FALSE && strpos($field_name,'_3')===FALSE){
          $field_include=TRUE;
        }
      }      
    }
    if($field_include){
      $field_schema=drupal_get_schema('field_data_' . $field_name);
      foreach($field_schema['fields'] as $column_name => $column_schema) {
        if(strpos($column_name,'field_')===0){
        
          //filter out internal values
          $column_skip=FALSE;
          foreach($column_filter_out as $filter) {
            if(strpos($column_name,$filter)!==FALSE){
              $column_skip=TRUE;
              break;
            }
          }
          
          if(!$column_skip){
            $values=$description='';
                        
            //get/alter column_label
            $column_label = trim(str_replace(array($field_name,'_'),array('',' '),$column_name));
            
            //location
            if($column_label == 'location'){
              $column_label = 'Location Description';
            }
            
            // lat/lng switcharoo
            if($column_label == 'x'){
              $column_name='field_location_y';
              $column_label = 'Latitude';
            }
            if($column_label == 'y'){              
              $column_name='field_location_x';
              $column_label = 'Longitude';
            }

            if($column_label == 'tid') {
              $column_label = 'Value';
              if(strpos($field_name,'field_status')===0){  
                $name='status';
              }elseif(strpos($field_name,'field_designation_official')===0){
                $name='designation';
              }elseif(strpos($field_name,'field_place_type')===0){
                $name='place_type';
              }
              if(isset($name)){                
                $vid=taxonomy_vocabulary_machine_name_load($name)->vid;
                $terms=taxonomy_get_tree($vid);
                
                foreach($terms as $term){
                  $values .= '"'.$term->name . '", ';
                }
                $values = rtrim($values,', ');
              }
            }
            
            $column_label = ucwords($column_label);          
            
            if(empty($values)){
              if($column_schema['type'] == 'int'){
                $values = 'Only Integers';
              }else{
                $values = 'Any Text';
              }
            }
            
            $template['name'][] = $column_name;
            $template['label'][]='' . $column_label . ' for ' . $field_info['label'];
            $template['description'][]='' . $description;
            $template['allowed_values'][]='' . $values;
          }
        }
      } 
      //add author and/or timestamp, if selected
      if($author){
        $template['name'][]=$field_name . '_author_id';
        $template['label'][]='Author ID for ' . $field_info['label'];
        $template['description'][]='Must be an existing Author ID number.';
        $template['allowed_values'][]='Only Integers';
      }
      if($timestamp){
        $template['name'][]=$field_name . '_timestamp';
        $template['label'][]='Timestamp for ' . $field_info['label'];
        $template['description'][]='The timestamp in UNIX time.';
        $template['allowed_values'][]='Only Integers';
      }
    }     
  }
    
  //dw($template);
  _send_to_csv($template);
  
}

function _send_to_csv($datas){
  header("Content-type: application/csv");
  header("Content-Disposition: attachment; filename=\"AustinHistoricalSurveyWiki_ImportTemplate_".date('M.j.y').".csv\"");
  header("Pragma: no-cache");
  header("Expires: 0");

  $handle = fopen('php://output', 'w');
  foreach ($datas as $data) {
    fputcsv($handle, $data);
  }

  fclose($handle);
  exit;
}

function historicalsurvey_admin_import_users_from_d6() {


// updateing city and state by zip
/* $query = db_select('users', 'u');
$query->fields('u', array('uid'));
$result = $query->execute();
while($record = $result->fetchAssoc()) {

  $user = user_load($record['uid']);
  
  if(isset($user->field_user_zip_code['und'][0]['value'])) {
    $zip = $user->field_user_zip_code['und'][0]['value'];
    if(!empty($zip) && is_numeric($zip)) {    
    
      if(empty($user->field_user_state['und'][0]['value'])) {
        
        $zip_data=geocoder('google',$zip);
        if(!isset($zip_data->data['geocoder_address_components'][1]->long_name) || empty($zip_data->data['geocoder_address_components'][1]->long_name)) {
          dw($zip_data,'',1);
        }
        else {
          $city = $zip_data->data['geocoder_address_components'][1]->long_name;
          $state = $zip_data->data['geocoder_address_components'][3]->long_name;
          
          $user->field_user_city['und'][0]['value'] = $city;
          $user->field_user_state['und'][0]['value'] = $state; 
          
          user_save($user);
        }
      }
    }
  }
} */

//importing from d6
/*   include_once('../tools/d6-migrate.php');
  
  $users = get_d6_users();
  dw('');
  
  foreach($users as $uid => $user) {
    if(!in_array($user['profile_street'], array('genevieve','a')) && $uid>1) { //spam, tests, nonsense, admin        
      if(!user_load($uid)) {
        import_user_to_d7($uid, $user);
      }
    }
  }
  
  //dw(user_load(1)); */
  
  return 'DONE';
}

function import_user_to_d7($uid, $user) {
  
  $account = new stdClass();
  $account->pass = $user['pass'];
  $account->mail = $user['mail'];
  $account->created = $user['created'];
  $account->access = $user['access'];
  $account->login = $user['login'];
  
  $account->status = 1;
  
  user_save($account);
  
  // Replace with correct uid
  db_update('users')
    ->fields(array('uid' => $uid))
    ->condition('uid', $account->uid)
    ->execute();
  $account->uid = $uid;
  
  // then add the foreign table data that is dependant on correct uid
  $account->field_user_zip_code['und'][0]['value'] = is_numeric($user['profile_zip'])?$user['profile_zip']:'';
  $account->field_user_find_out['und'][0]['value'] = $user['referer'];
  $account->field_user_street['und'][0]['value'] = $user['profile_street'];
  $account->field_user_name['und'][0]['value'] = $user['profile_firstname'].' '.$user['profile_lastname'];
  $account->field_user_city['und'][0]['value'] = $user['profile_city'];
  $account->field_user_state['und'][0]['value'] = '';  
  
  if(!empty($user['profile_zip']) && is_numeric($user['profile_zip'])) {
    $zip_data = get_zip_info($user['profile_zip']);
    $account->field_user_city['und'][0]['value'] = $zip_data['city'];
    $account->field_user_state['und'][0]['value'] = $zip_data['state'];    
  }
  
  user_save($account);
  
}

