<?php

/**
 *  Search API alter callback
 *  Alters $items to include all the field's most current values in indexing
 */
class HistoricalSurveySearchAddRevisions extends SearchApiAbstractAlterCallback {

  /**
   * Alter items before indexing.
   *
   * Items which are removed from the array won't be indexed, but will be marked
   * as clean for future indexing.
   *
   * @param array $items
   *   An array of items to be altered, keyed by item IDs.
   *
   */
  public function alterItems(array &$items) {
    //dw($items);
    $index = search_api_index_load('solr_tomcat6');
    foreach ($items as $nid => $item) {
      foreach (array_keys($index->options['fields']) as $field_name) {
        if ($field_name != 'search_api_language') {

          //this is similar to how the field widget function pulls default values
          $results = historicalsurvey_fields_get_field_revisions($field_name, array($nid => $item));
          if (!empty($results)) {
            $revisions = $results[$nid];
            $current = reset($revisions); //the php function reset() returns the first element value
            $items[$nid]->$field_name = $current->$field_name;
          }

          //todo: add the full revision history?

        }
      }
    }
  }
}
