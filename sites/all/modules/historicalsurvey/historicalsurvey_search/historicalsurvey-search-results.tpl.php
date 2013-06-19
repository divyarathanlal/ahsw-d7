<?php
/*   $results = $variables['results'];
  $items = $variables['items'];
  $keys = $variables['keys']; // search terms, todo:use these for display or something */
  //dw($keys);
?>
<div class="grid-12 full" id="search-results-wrapper"> 

  <div id="search-results-list" class="grid-4">
    <div id="search-results-stats">    
      <div id="search-result-result-count">
        <?php print(format_plural($results['result count'],'1 result','@count results')); ?>
      </div>
      <div id="search-result-result-key-wrapper">
        <?php print render($keys_themed); ?>              
        <div id="search-result-go-back">
          <?php print(l(t("Reset"), 'search/cancel')); ?>
        </div>
      </div>    
    </div>
    
    <?php print '<div id="pager">'.$pager.'</div>'; ?>
    
    <?php print render($results_themed); ?>    
    
    <div id="search-results-bottom">
      <div id="search-results-batch-edit"><?php //print(l(t("Batch Edit these Results"), 'search/edit/select')); ?></div>
      <div id="search-results-batch-review"><?php //print(l(t("Batch Review These Results"), 'search/review/select')); ?></div>
      <!--<div>Download as CSV</div>-->
    </div>
  </div>
  
  
  <div id="search-results-map" class="grid-8 full">
    <?php print render($map); ?>
  </div>  

</div>