

<div class="grid-12 full" id="top-block-wrapper"> 

  <div class="grid-9 block full" id="address-map-wrapper"> 
    <div class="field-wrapper" id="address-wrapper">
      <?php 
        $form=drupal_get_form('historicalsurvey_search_address_search_form');
        print drupal_render($form); 
      ?>
    </div>
    <div class="field-wrapper" id="map-wrapper">     
      <?php print render($map); ?>
    </div>
    <div class="clear" ></div>
  </div>    
  
  <!-- Guide -->
  <div class="block grid-3 full-right" id="guide-wrapper">
    <div id='welcome'>The Austin Historical Survey Wiki is a new interactive tool for the City of Austin. The Wiki allows you to find and contribute information about historic buildings, sites, and landscapes of the past and present that tell the history of Austin. 
    </div>
    <div><a href="places">Find Places</a> to see what has been added to the Wiki.</div>
    <div><a href="user/create">Create a login</a> to add or edit place records.</div>
    <?php //<div><a href="guide">Learn more</a> about the project on the Wiki User Guide.</div> ?>
    <div> Wiki Stats: </div>
</div>