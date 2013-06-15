<?php
/*   dw(array_keys($form['keys']));
  dw(array_keys($form['keys']['term']),'term',1);
  dw(array_keys($form['keys']['text']),'text',1); 
  dw(array_keys($form['keys']['year']),'year',1); 
  dw(array_keys($form['keys']['years']),'years',1); */
?>
<div class="grid-12 full" id="search-form-wrapper"> 
  <div id="search-form-row1" class="grid-12 full search-form-row">
    <div id="search-form-header" class="grid-7" >Find Places</div>
    <div id="search-submit-wrapper" class="grid-5" >
      <div id="search-options-wrapper" ><?php print render($form['keys']['options']); ?></div>
      <div id="search-button-wrapper" ><?php print render($form['submit']); ?></div>
    </div>
  </div>

  <div id="search-form-row2" class="grid-12 full search-form-row">
    <div class="search-form-cell whole grid-4">
    
      <div id="search-location">
        <div class="search-title">Street Address</div>
        <div class="search-field"><?php print render($form['keys']['field_location']); ?></div>
        <div class="help-text">Enter any or all parts of an address.<br/>Enter numbered streets like "16th St".</div>  
      </div>
      
    </div>
    <div class="grid-4"> 
      <div class="search-form-cell half">
      
        <div id="search-status">
          <div class="search-title">Status</div>
          <div class="search-field"><?php print render($form['keys']['field_status']); ?></div>
        </div>   
        
      </div>
      <div class="search-form-cell half"> 
      
        <div id="search-type">
          <div class="search-title">Place Type</div>
          <div class="search-field"><?php print render($form['keys']['field_place_type']); ?></div>
        </div>
        
      </div>
    </div>
    <div class="search-form-cell whole grid-4">  
    
      <div id="search-year">
        <div class="search-title">Construction Year</div>
        <div class="search-field"><?php print render($form['keys']['field_construction_year']); ?></div>
        <div class="help-text">Enter a range of years, or enter a single year to search an open-ended range. For example, to search for all years before 1900 simply enter `1900` into the `To` box and leave the `From` box empty.</div>  
      </div>
      
    </div>
  </div>

  <div id="search-form-row3" class="grid-12 full search-form-row">  
    <div class="search-form-cell whole grid-4">
    
      <div id="search-narrative">
        <div class="search-title">Narratives</div>
        <div class="search-field" id="search-narrative-text"><?php print render($form['keys']['field_narrative']['text']); ?></div>
        <div class="help-text">Enter a word or phrase that might be found in the narrative fields selected below.</div>  
        <div class="search-field" id="search-narrative-select"><?php print render($form['keys']['field_narrative']['options']); ?></div>
      </div>  
      
    </div>      
    <div class="search-form-cell whole grid-4">
    
      <div id="search-use">
        <div class="search-title">Current / Historic Use</div>
        <div class="search-field"><?php print render($form['keys']['field_use']); ?></div>
        <div class="help-text">Submitting both a term and text search will find places matching ANY of them.</div> 
     </div>
     
    </div>    
    <div class="grid-4"> 
      <div class="search-form-cell half">
      
        <div id="search-name">
          <div class="search-title">Current / Historic Name</div>
          <div class="search-field"><?php print render($form['keys']['field_name']); ?></div>
        </div>     
        
      </div>
      <div class="search-form-cell half">
      
        <div id="search-designation">
          <div class="search-title">Designations</div>
          <div class="search-field"><?php print render($form['keys']['field_designation_official']); ?></div>
        </div>
        
      </div>
    </div>
  </div>

  <div id="search-form-row4" class="grid-12 full search-form-row">
    <div class="search-form-cell whole grid-4">  
    
      <div id="search-style">
        <div class="search-title">Architectural Style</div>
        <div class="search-field"><?php print render($form['keys']['field_style']); ?></div>
        <div class="help-text">Submitting both a term and text search will find places matching ANY of them.</div>  
      </div> 
      
    </div>
    <div class="search-form-cell whole grid-4">
    
      <div id="search-people">
        <div class="search-title">Associated People</div>
        <div class="search-field"><?php print render($form['keys']['field_person']); ?></div>
        <div class="help-text">Associated people include designers, builders, owners and notable residents, among others.</div>  
      </div>
      
    </div>    
    <div class="grid-4">   
      <div class="search-form-cell half">
      
        <div id="search-effort">
          <div class="search-title">Survey Effort</div>
          <div class="search-field"><?php print render($form['keys']['field_survey_effort']); ?></div>
        </div>
        
      </div>
      <div class="search-form-cell half">
      
        <div id="search-tags">
          <div class="search-title">Tags</div>
          <div class="search-field"><?php print render($form['keys']['field_tag']); ?></div>
        </div>
        
      </div>
    </div>
  </div>
</div>
<?php print drupal_render_children($form); ?>
