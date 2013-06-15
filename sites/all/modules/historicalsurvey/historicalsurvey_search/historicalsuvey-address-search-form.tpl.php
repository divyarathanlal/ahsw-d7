<?php
/*   dw(array_keys($form['keys']));
  dw(array_keys($form['keys']['term']),'term',1);
  dw(array_keys($form['keys']['text']),'text',1); 
  dw(array_keys($form['keys']['year']),'year',1); 
  dw(array_keys($form['keys']['years']),'years',1); */
?>
<div class="grid-12 full" id="search-form-wrapper"> 

  <div id="search-form-row2" class="grid-12 full search-form-row">
    <div class="search-form-cell">
    
      <div id="search-location">
        <div class="search-field"><?php print render($form['keys']['field_location']); ?></div>
        <div class="search-button"><?php print drupal_render_children($form); ?></div>
        <div class="help-text">Search any or all parts of an address. Enter numbered streets like "16th St". Or use the <a href="places">Advanced Search</a></div>  
      </div>
      
    </div>
  </div>

</div>
