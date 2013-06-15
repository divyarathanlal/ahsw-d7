<?php


/**
 *  Implements hook_theme().
 */
function historicalsurvey_fields_theme($existing, $type, $theme, $path) {
  return array(
    'historicalsurvey_location_widget_element_theme' => array(
      'variables' => array(
      ),
    ),
    'historicalsurvey_construction_year_widget_element_theme' => array(
      'variables' => array(
      ),
    ),
    'historicalsurvey_text_widget_element_theme' => array(
      'variables' => array(
      ),
    ),
/*     'historicalsurvey_text_type_widget_element_theme' => array(
      'variables' => array(
      ),
    ), */
    
    'historicalsurvey_multiple_value_form' => array(
      'render element' => 'element',
    ),
    'historicalsurvey_location_formatter_element_theme' => array(
      'variables' => array(
        'items' => array(),
        'field' => array(),
        'entity' => array(),
        'form_element_html' => array(),
        'op' => '',
      ),
    ),
    'historicalsurvey_node_reference_formatter_element_theme' => array(
      'variables' => array(
        'items' => array(),
        'field' => array(),
        'entity' => array(),
        'form_element_html' => array(),
        'op' => '',
      ),
    ),
    'historicalsurvey_construction_year_formatter_element_theme' => array(
      'variables' => array(
        'items' => array(),
        'field' => array(),
        'entity' => array(),
        'form_element_html' => array(),
        'op' => '',
      ),
    ),
    'historicalsurvey_text_formatter_element_theme' => array(
      'variables' => array(
        'items' => array(),
        'field' => array(),
        'entity' => array(),
        'form_element_html' => array(),
        'op' => '',
      ),
    ),
/*     'historicalsurvey_text_type_formatter_element_theme' => array(
      'variables' => array(
        'items' => array(),
        'field' => array(),
        'entity' => array(),
        'form_element_html' => array(),
        'op' => '',
      ),
    ), */
    'historicalsurvey_term_reference_formatter_element_theme' => array(
      'variables' => array(
        'items' => array(),
        'field' => array(),
        'entity' => array(),
        'form_element_html' => array(),
        'op' => '',
      ),
    ),
    'historicalsurvey_photo_formatter_element_theme' => array(
      'variables' => array(
        'items' => array(),
        'field' => array(),
        'entity' => array(),
        'form_element_html' => array(),
        'op' => '',
      ),
    ),
    'historicalsurvey_document_formatter_element_theme' => array(
      'variables' => array(
        'items' => array(),
        'field' => array(),
        'entity' => array(),
        'form_element_html' => array(),
        'op' => '',
      ),
    ),
    'historicalsurvey_notes_formatter_element_theme' => array(
      'variables' => array(
        'items' => array(),
        'form' => array(),
      ),
    ), 
    'historicalsurvey_username' => array(
      'variables' => array(
        'account' => array(),
      ),
    ),    
  );
}


/**
 *  Implements hook_theme_registry_alter().
 */
function historicalsurvey_fields_theme_registry_alter(&$theme_registry) {
  
  //reroute the multi-value draggable table field theme to a custom version
  $theme_registry['field_multiple_value_form']['theme path'] = drupal_get_path('module', 'historicalsurvey_fields');
  $theme_registry['field_multiple_value_form']['function'] = "theme_historicalsurvey_multiple_value_form";
}


/**
 * Rewrite of theme_field_multiple_value_form().
 * For Construction Year field
 */
function theme_historicalsurvey_multiple_value_form($variables) {
  $element = $variables['element'];
  $html = '';

  if ($element['#cardinality'] > 1 || $element['#cardinality'] == FIELD_CARDINALITY_UNLIMITED) {
    $table_id = drupal_html_id($element['#field_name'] . '_values');
    $order_class = $element['#field_name'] . '-delta-order';
    $header = array();
    $rows = array();

    // Sort items according to '_weight' (needed when the form comes back after
    // preview or failed validation)
    $items = array();
    foreach (element_children($element) as $key) {
      if ($key === 'add_more') {
        $add_more_button = &$element[$key];
      }
      else {
        $items[] = &$element[$key];
      }
    }
    usort($items, '_field_sort_items_value_helper');

    // Add the items as table rows.
    foreach ($items as $key => $item) {
      $item['_weight']['#attributes']['class'] = array($order_class);
      $delta_element = drupal_render($item['_weight']);
      $cells = array(
        array(
          'data' => '',
          'class' => array('field-multiple-drag'),
        ),
        drupal_render($item),
        array(
          'data' => $delta_element,
          'class' => array('delta-order'),
        ),
      );
      $rows[] = array(
        'data' => $cells,
        'class' => array('draggable'),
      );
    }

    $html = '<div class="form-item">';
    $html .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => $table_id, 'class' => array('field-multiple-table'))));
    $html .= '<div class="clearfix">' . drupal_render($add_more_button) . '</div>';
    $html .= '</div>';

    drupal_add_tabledrag($table_id, 'order', 'sibling', $order_class);
  }
  else {
    foreach (element_children($element) as $key) {
      $html .= drupal_render($element[$key]);
    }
  }

  return $html;
}

/* function theme_historicalsurvey_location_widget_element_theme($variables) {
  return 'TODO: FIX ME';
}
function theme_historicalsurvey_construction_year_widget_element_theme($variables) {
  return 'TODO: FIX ME';
}
function theme_historicalsurvey_text_widget_element_theme($variables) {
  return 'TODO: FIX ME';
} */
/* function theme_historicalsurvey_text_type_widget_element_theme($variables) {
  return 'TODO: FIX ME';
} */


/**
 *  Theme function for custom username display
 */
function theme_historicalsurvey_username($variables) {
  return $variables['account']->field_user_name[LANGUAGE_NONE][0]['value'];
}


/**
 *  Theme function for custom location element, forwards to general theme function.
 */
function theme_historicalsurvey_location_formatter_element_theme($variables) {
  return historicalsurvey_fields_formatter_element_theme($variables, "historicalsurvey_location");
}

/**
 *  Theme function for custom node reference element, forwards to general theme function.
 */
function theme_historicalsurvey_node_reference_formatter_element_theme($variables) {
  return historicalsurvey_fields_formatter_element_theme($variables, "historicalsurvey_node_reference");
}

/**
 *  Theme function for custom text element, forwards to general theme function.
 */
function theme_historicalsurvey_text_formatter_element_theme($variables) {
  return historicalsurvey_fields_formatter_element_theme($variables, "historicalsurvey_text");
}

/**
 *  Theme function for custom text element, forwards to general theme function.
 */
/* function theme_historicalsurvey_text_type_formatter_element_theme($variables) {
  return historicalsurvey_fields_formatter_element_theme($variables, "historicalsurvey_text_type");
}
 */
/**
 *  Theme function for custom construction year element, forwards to general theme function.
 */
function theme_historicalsurvey_construction_year_formatter_element_theme($variables) {
  return historicalsurvey_fields_formatter_element_theme($variables, "historicalsurvey_construction_year");
}

/**
 *  Theme function for custom term reference element.
 */
function theme_historicalsurvey_term_reference_formatter_element_theme($variables) {
  return historicalsurvey_fields_formatter_element_theme($variables, "historicalsurvey_term_reference");
}

/**
 *  Theme function for custom photo element.
 */
function theme_historicalsurvey_photo_formatter_element_theme($variables) {

  global $base_url;
  $items = isset($variables['items']['revisions']) ? reset($variables['items']['revisions']) : array();
  $node = $variables['entity'];
  $field = $variables['field'];
  $form_element_html = !empty($variables['form_element_html']) ? $variables['form_element_html'] : FALSE; //only in edit mode
  $op = $variables['op']; //view or edit

  $html = '';
  $hash = 'edit-' . str_replace("_", "-", $field['field_name']);
  $edit_link = ($op == 'view') ? ' href="' . $base_url . '/place/' . $node->nid . '/edit#' . $hash . '"' : '';

  //show edit button on view page, and on upper review levels of edit page
  /* if ($op == 'view') {
    $html .= '<div class="edit-button"><a id=""' . $edit_link . '><img src="'.$base_url.'/'.drupal_get_path('module','historicalsurvey_place').'/img/pencil.png"></a></div>';
  } */
  $html .= '<div class="field-photo-view">';
  if (!empty($items) && !empty($items[0]['fid'])) {
  
    //split out main and thumbnail images    
    foreach ($items as $delta => $values) {
      if (is_numeric($delta) && !empty($values['fid'])) {
        if(!empty($values['main_image'])) {
          $images['main']=$values;
        }
        else{
          $images['thumbs'][]=$values;
        }
      }
    }
    
    $html .= '<div class="field-name">' . $field['label'] . '</div>';
    $html .= '<div class="main-image">';
    if(isset($images['main'])) {
      if (!empty($images['main']['fid'])) {
        $file = file_load($images['main']['fid']);
        if ($file) {
        
            $alt_text = trim($images['main']['alt'].'; '.$images['main']['description'],'; '); 
            $title_text = trim($images['main']['title'].'; '.$images['main']['description'],'; ');  
            
            $image = array(
              '#theme' => 'image_style',
              '#style_name' => 'medium',
              '#path' => $file->uri,
              '#alt' => empty($alt_text)?'no description':$alt_text,
              '#title' => empty($title_text)?'no description':$title_text,
            );
            $html .= drupal_render($image);
        }
        else {
          $html .= '<span class="no-photo"><em>error loading photos (errno:1)</em></span>';
        }
      }
      else {
        $html .= '<span class="no-photo"><em>error loading photos (errno:2)</em></span>';
      }           
    }
    else {
      if(!empty($images['thumbs'])) {
        $html .= '<span class="no-photo"><em>Select a photo to be the main image</em></span>';
      }
    }
    $html .= '</div>'; //main-image  
      
    if(isset($images['thumbs'])) {
      $html .= '<div class="thumb-images-wrapper">';
      $html .= '<div class="thumb-images" style="width:'.(count($images['thumbs'])*79).'px">';
      foreach ($images['thumbs']  as $columns) {
        if (!empty($columns['fid'])) {
          $file = file_load($columns['fid']);
          if ($file) {
          
            $alt_text=trim($columns['alt'].'; '.$columns['description'],'; ');
            $title_text=trim($columns['title'].'; '.$columns['description'],'; '); 
            
            $html .= '<span class="thumb-image">';
            $image = array(
              '#theme' => 'image_style',
              '#style_name' => 'thumbnail',
              '#path' => $file->uri,
              '#alt' => empty($alt_text)?'no description':$alt_text,
              '#title' => empty($title_text)?'no description':$title_text,
            );
            $html .= drupal_render($image);
            $html .= '</span>'; //thumb-image
          }
          else {
            $html .= '<span class="no-photo"><em>error loading photos (errno:3)</em></span>';
          }
        }
        else {
          $html .= '<span class="no-photo"><em>error loading photos (errno:4)</em></span>';
        }
      }
      $html .= '</div>'; //thumb-images
      $html .= '</div>'; //thumb-images-wrapper
    }
  }
  else {
    $html .= '<div class="field-name">' . $field['label'] . '</div>';
    $html .= '<div class="main-value">';
      $html .= '<span class="empty"><em>none</em></span>';
    $html .= '</div>'; //main-value  
  }
  $html .= '</div>'; //field-photo-view

  //add $form field edit element (minus the notes) if called for
  if ($form_element_html) {
    $html .= $form_element_html;
  }

  return $html;
}


/**
 *  Theme function for custom document element, forwards to general theme function.
 */
function theme_historicalsurvey_document_formatter_element_theme($variables) {
  
  global $base_url;

  $items = isset($variables['items']['revisions']) ? reset($variables['items']['revisions']) : array();
  $node = $variables['entity'];
  $field = $variables['field'];
  $form_element_html = !empty($variables['form_element_html']) ? $variables['form_element_html'] : FALSE; //only in edit mode
  $op = $variables['op']; //view or edit

  $html = '';
  $hash = 'edit-' . str_replace("_", "-", $field['field_name']);
  $edit_link = ($op == 'view') ? ' href="' . $base_url . '/place/' . $node->nid . '/edit#' . $hash . '"' : '';

  //show edit button on view page, and on upper review levels of edit page
  /* if ($op == 'view') {
    $html .= '<div class="edit-button"><a id=""' . $edit_link . '><img src="'.$base_url.'/'.drupal_get_path('module','historicalsurvey_place').'/img/pencil.png"></a></div>';
  } */
  
  $html .= '<div class="field-document-view">';
  if (!empty($items)) {
  
    $html .= '<div class="field-name">' . $field['label'] . '</div>';
    if(isset($items)) {
      foreach($items as $columns) {
        $html .= '<div class="main-value">';
        if (!empty($columns['fid'])) {
          $file = file_load($columns['fid']);
          if ($file) {
              $name = theme('file_icon', array('file' => $file)) . "<span class='document-name'>" . $file->filename . "</span><span class='document-size'>(" . format_size($file->filesize) . ")</span>";
              $html .= l($name, file_create_url($file->uri), array('html' => TRUE));

          }
          else {
            $html .= '<span class="no-files"><small><em>error loading file (errno:1)</em></small></span>';
          }
        }
        else {
          $html .= '<span class="no-photo"><small><em>error loading file (errno:2)</em></small></span>';
        }               
        $html .= '</div>'; //main-value
      }
    }
  }    
  else {
    $html .= '<div class="field-name">' . $field['label'] . '</div>';
    $html .= '<div class="main-value">';
      $html .= '<span class="empty"><em>none</em></span>';
    $html .= '</div>'; //main-value  
  }  
  $html .= '</div>'; //field-document-view
  
  //add $form field edit element (minus the notes) if called for
  if ($form_element_html) {
    $html .= $form_element_html;
  }
    
  return $html;
}
/**
 *  Theme function for custom notes element.
 */
function theme_historicalsurvey_notes_formatter_element_theme($variables) {
  //dw($variables);
  //$form_element_html=!empty($variables['form_element_html'])?$variables['form_element_html']:FALSE; //only in edit mode

  $html = '<div>';
  $html .= '<div class="notes-title">SOURCES AND NOTES</div>';
  $html .= '<table><tbody>';
  if (!empty($variables['items'])) {
    //dw($variables['items']);
    foreach ($variables['items'] as $field_name => $revisions) {
      $label = $revisions['label'];
      $form_element = isset($variables['form'][$field_name]) ? $variables['form'][$field_name] : FALSE;
      foreach ($revisions as $delta_array) {
        if (is_array($delta_array)) {
          foreach ($delta_array as $notes_array) {
            $html .= '<tr id="note-' . $notes_array['number'] . '">
                <td class="note-number">' . $notes_array['number'] . '. </td>
                <td class="note-label">for ' . $label . '</td>';
            $html .= '<td class="note-value">';
            if(isset($notes_array['sub-label'])) {              
              $html .= '<span class="notes-label">' . ucwords($notes_array['sub-label']) . '</span>';
            }
            $html .= '<span class="view-item">' . $notes_array['value'] . '</span>';
            $html .= $form_element ? render($form_element) : '';
            $html .= '</td>';
            $html .= '</tr>';
          }
        }
      }
    }
  }
  /* new notes */
  if (!empty($variables['form']['#new'])) {
    foreach ($variables['form']['#new'] as $field_name => $form_element) {
      if (!empty($form_element)) {
        $note_id = 'note-' . str_replace("_", "-", $field_name);
        $html .= '<tr class="new-note" id="' . $note_id . '">';
        $html .= '<td class="note-number"></td>'; // no number for these
        $html .= '<td class="note-label">' . $form_element['label'] . '</td>';
        $html .= '<td class="note-value">' . render($form_element) . '</td>';
        $html .= '</tr>';
      }
    }
  }
  $html .= '</tbody></table>';
  $html .= '</div>';

  return $html;
}


/**
 *  Helper function to efficiently theme fields, works with both view and edit modes
 */
function historicalsurvey_fields_formatter_element_theme($variables, $field_type) {
  global $base_url;

  $items = isset($variables['items']['revisions']) ? $variables['items']['revisions'] : array();
  $notes = isset($variables['items']['notes']) ? $variables['items']['notes'] : array();
  $node = $variables['entity'];
  $field = $variables['field'];
  $form_element_html = !empty($variables['form_element_html']) ? $variables['form_element_html'] : FALSE; //only in edit mode
  $op = $variables['op']; //view or edit

  $count = 0;
  $html = $revision_timestamp = $revision_author = '';
  $hash = 'edit-' . str_replace("_", "-", $field['field_name']);

  $link = ($op == 'view') ? $base_url . '/place/' . $node->nid . '/edit#' . $hash : '';

  //show edit button for reviewed fields on view page, and on upper review levels of edit page
  if ($count == 0 && ($op == 'view' || !strpos($field['field_name'], "_1"))
  && !in_array($field['field_name'], array('field_location','field_photo','field_document','field_designation_official','field_survey_effort','field_survey_effort_tag','field_tag'))) {
    $html .= '<div class="edit-button"><a href=' . $link . '><img src="'.$base_url.'/'.drupal_get_path('module','historicalsurvey_place').'/img/pencil.png"></a></div>';
  }

  if (!empty($variables['items'])) {
    $revision_count = count($items) -1;
    foreach ($items as $vid => $item) {
      if (is_numeric($vid)) {
        $html .= ($count == 0) ? '<div class="current-version">' : '<div class="revision">';
        if (empty($field['review_level']) && $field['label'] !== "Location") {
          $html .= '<div class="field-name">' . $field['label'] . '</div>';
        }
        foreach ($item as $delta => $values) {
          $revision_authors = '';
          foreach ($item['#revision_uid'] as $uid) {
            if($user=user_load($uid)){
              $revision_authors .= theme('historicalsurvey_username', array('account' => $user)) . ", ";
            }else{
              $revision_authors .= $uid . ", ";
            }
          }
          $revision_authors = rtrim($revision_authors, ', ');

          $revision_timestamp = date("m/d/Y - h:i A", $item['#revision_timestamp']);
          if (is_numeric($delta)) {
            $html .= '<div class="main-value">';

            switch ($field_type) {
              case "historicalsurvey_location":
                $street = !empty($values['location']) ? $values['location'] : (!empty($values['Match_addr']) ? $values['Match_addr'] : '');
                $html .= !empty($street) ? '<div class="street">' . $street . '</div>' : '<div class="no-street"><em>no address</em></div>';
                $html .= '<span class="lat-lng">' . $values['y'] . ', ' . $values['x'] . '</span>';
                break;

              case "historicalsurvey_node_reference":
                if (isset($values['nid'])) {
                  $node_ref = node_load($values['nid']);
                  $html .= '<span class="node">' . $node_ref->title . '</span>';
                }
                else {
                  $html .= '<span class="node"><em>none</em></span>';
                }
                break;

              case "historicalsurvey_text":
                $html .= '<span class="value">' . check_markup($values['value']) . '</span>';
                break;

              /* case "historicalsurvey_text_type":
                $html .= '<span class="sub-field-label">' . ucwords($values['type']) . '</span><span class="sub-values">' . $values['value'] . '</span>';
                break; */

              case "historicalsurvey_construction_year":
                if (isset($values['actual_early_year']) || isset($values['actual_late_year'])) {
                  $html .= '<div class="actual-years"><span class="label">Actual</span><span class="value">' . $values['actual_early_year'] . '-' . $values['actual_late_year'] . '<span></div>';
                }
                if (isset($values['estimated_early_year']) || isset($values['estimated_late_year'])) {
                  $html .= '<div class="estimated-years"><span class="label">Estimated</span><span class="value">' . $values['estimated_early_year'] . '-' . $values['estimated_late_year'] . '</span></div>';
                }
                break;

              case "historicalsurvey_term_reference":
                if(strpos($field['field_name'],'designation')) {
                  if (!empty($values['tid'])) {
                    $term = taxonomy_term_load($values['tid']);
                    $parents=taxonomy_get_parents($values['tid']);
                    $parent = array_pop($parents);
                    $html .= '<span class="term">'.$parent->name . ' : ' . $term->name . '</span>';
                  }
                  else {
                    $html .= '<span class="term"><em>none</em></span>';
                  }
                }
                else {
                  if (!empty($values['tid'])) {
                    $term = taxonomy_term_load($values['tid']);
                    $html .= '<span class="term">' . $term->name . '</span>';
                  }
                  else {
                    $html .= '<span class="term"><em>none</em></span>';
                  }
                }
                break;

              case "historicalsurvey_document":
                $file = file_load($values['fid']);
                if ($file) {
                  $name = theme('file_icon', array('file' => $file)) . "<span class='document-name'>" . $file->filename . "</span><span class='document-size'>(" . format_size($file->filesize) . ")</span>";
                  $html .= l($name, file_create_url($file->uri), array('html' => TRUE));
                }
                break;
            }

            //add footnote
            if (!empty($notes[$vid][$delta])) {
              if ($op == 'view') {
                $html .= '<span class="footnote-link">(see <a note="note-' . $notes[$vid][$delta]['number'] . '" title="' . str_replace('"',"'",strip_tags($notes[$vid][$delta]['value'])) . '">footnote ' . $notes[$vid][$delta]['number'] . '</a>)</span>';
              }
              else {
                $html .= '<span class="footnote-link"><a note="note-' . $notes[$vid][$delta]['number'] . '" title="' . str_replace('"',"'",strip_tags($notes[$vid][$delta]['value'])) . '"></a></span>';
              }
            }
            $html .= '</div>'; //main-value
            if(isset($values['notes'])) {
              $html .= ($count != 0) ? '<div class="notes"><span class="label">Notes</span><span class="value">' . $values['notes'] . '</span></div>' : '';
            }
          }
        }
      }

      $html .= '<div class="revision-info">';
      $html .= '<div class="time"><span class="label">Updated</span><span class="value">' . $revision_timestamp . '</span></div>';
      $html .= '<div class="author"><span class="label">By</span><span class="value">' . $revision_authors . '</span></div>';
      $html .= '</div>'; //revision-info      
      $html .= '</div>'; //current-version or revisions     
      //if edit, skip loop into the revision block
      if ($op != 'view') {
        break;
      }
      if ($revision_count !== 0) {
        $html .= ($count == 0) ? '<div class="revision-count">' . format_plural($revision_count, '1 revision', '@count revisions') . '</div>' : '';
      }
      $count++;
    }
  }
  else {
    $html .= '<div class="current-version">';
    if (empty($field['review_level'])) {
      $html .= '<div class="field-name">' . $field['label'] . '</div>';
    }
    $html .= '<div class="main-value">';
    $html .= '<span class="empty"><em>none</em></span>';
    $html .= '</div>'; //main-value  
    if ($op == 'view') {
      $html .= '<div class="revision-info">';
      $html .= '<div class="notice">Please use the edit link to add data.</div>';
      $html .= '</div>'; //revision-info
    }
    $html .= '</div>'; //current-version
  }

  //add $form field edit element (minus the notes) if called for
  if ($form_element_html) {
    $html .= $form_element_html;
    $html .= '<div class="edit-instruction">Please edit the sources and notes as needed at bottom of page</div>';
  }

  return $html;
}

