<?php

/**
 * @file
 * Default theme implementation to display a single Drupal page.
 *
 * The doctype, html, head and body tags are not in this template. Instead they
 * can be found in the html.tpl.php template in this directory.
 *
 * Available variables:
 *
 * General utility variables:
 * - $base_path: The base URL path of the Drupal installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $is_admin: TRUE if the user has permission to access administration pages.
 *
 * Site identity:
 * - $front_page: The URL of the front page. Use this instead of $base_path,
 *   when linking to the front page. This includes the language domain or
 *   prefix.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $site_name: The name of the site, empty when display has been disabled
 *   in theme settings.
 * - $site_slogan: The slogan of the site, empty when display has been disabled
 *   in theme settings.
 *
 * Navigation:
 * - $main_menu (array): An array containing the Main menu links for the
 *   site, if they have been configured.
 * - $secondary_menu (array): An array containing the Secondary menu links for
 *   the site, if they have been configured.
 * - $breadcrumb: The breadcrumb trail for the current page.
 *
 * Page content (in order of occurrence in the default page.tpl.php):
 * - $title_prefix (array): An array containing additional output populated by
 *   modules, intended to be displayed in front of the main title tag that
 *   appears in the template.
 * - $title: The page title, for use in the actual HTML content.
 * - $title_suffix (array): An array containing additional output populated by
 *   modules, intended to be displayed after the main title tag that appears in
 *   the template.
 * - $messages: HTML for status and error messages. Should be displayed
 *   prominently.
 * - $tabs (array): Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links (array): Actions local to the page, such as 'Add menu' on the
 *   menu administration interface.
 * - $feed_icons: A string of all feed icons for the current page.
 * - $node: The node object, if there is an automatically-loaded node
 *   associated with the page, and the node ID is the second argument
 *   in the page's path (e.g. node/12345 and node/12345/revisions, but not
 *   comment/reply/12345).
 *
 * Regions:
 * - $page['help']: Dynamic help text, mostly for admin pages.
 * - $page['highlighted']: Items for the highlighted content region.
 * - $page['content']: The main content of the current page.
 * - $page['sidebar_first']: Items for the first sidebar.
 * - $page['sidebar_second']: Items for the second sidebar.
 * - $page['header']: Items for the header region.
 * - $page['footer']: Items for the footer region.
 *
 * @see template_preprocess()
 * @see template_preprocess_page()
 * @see template_process()
 * @see html.tpl.php
 *
 * @ingroup themeable
 */
  global $base_url;
  $addaplace_img = theme('image',array('path'=> drupal_get_path('theme','historicalsurvey_theme') . "/img/hs-menu-addaplace.png"));
  $menu_img = theme('image',array('path'=> drupal_get_path('theme','historicalsurvey_theme') . "/img/hs-menu-map.png"));
  $places_img = theme('image',array('path'=> drupal_get_path('theme','historicalsurvey_theme') . "/img/hs-menu-findplaces.png"));
  $surveys_img = theme('image',array('path'=> drupal_get_path('theme','historicalsurvey_theme') . "/img/hs-menu-surveys.png"));
  $guide_img = theme('image',array('path'=> drupal_get_path('theme','historicalsurvey_theme') . "/img/hs-menu-guide.png"));
  $contact_img = theme('image',array('path'=> drupal_get_path('theme','historicalsurvey_theme') . "/img/hs-menu-contact.png"));
  
?>
<script src="http://www.cityofaustin.org/brandbar/coa.brandbar.js" type="text/javascript"></script>

<div id="page" class="container-12">
  <header id="header" role="banner"> 
  
    <?php print $user_bar; ?>   
    
    <div id="logo">
      <a href=<?php echo $base_url; ?>><img src="<?php echo $base_url; ?>/sites/all/themes/historicalsurvey_theme/img/ahsw-logo-horz.png"></a>
    </div>
    
    <div id="main-menu"> 
      <?php if(user_access('create place content')): ?>        
        <span class="menu-item" id="menu-item-addaplace"><?php print l($addaplace_img,'place/new',array('html'=>TRUE)); ?></span>
      <?php endif; ?>      
      <span class="menu-item" id="menu-item-map"><?php print l($menu_img,'',array('html'=>TRUE)); ?></span>
      <span class="menu-item" id="menu-item-places"><?php print l($places_img,'places',array('html'=>TRUE)); ?></span>
      <?php /*<span class="menu-item" id="menu-item-surveys"><?php print l($surveys_img,'surveys',array('html'=>TRUE)); ?></span>*/ ?>
      <span class="menu-item" id="menu-item-guide"><?php print l($guide_img,'guide',array('html'=>TRUE)); ?></span>      
      <span class="menu-item" id="menu-item-contact"><?php print l($contact_img,'contact',array('html'=>TRUE)); ?></span>      
    </div>
    
  </header>  
  
  <div id="main">  
    <div id="content" class="column" role="main">
      <a id="main-content"></a>      
      <?php print $messages; ?>
      <?php print render($page['content']); ?>      
    </div><!-- /#content -->
  </div><!-- /#main -->  
  <div id="footer"><div class="section">
    <?php print render($page['footer']); ?>
  </div></div> <!-- /.section, /#footer -->

</div><!-- /#page -->