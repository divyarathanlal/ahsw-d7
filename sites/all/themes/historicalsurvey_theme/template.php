<?php

function historicalsurvey_theme_preprocess_page(&$variables) {

  //Insert Custom User Bar
  global $user;
  $user_info = user_load($user->uid);
  $output = '';

  if (!$user->uid) {     
    $form = drupal_get_form('user_login_block');
    $form['name']['#weight']=0;    
    $form['pass']['#weight']=1;
    $form['actions']['#weight']=2;
    $form['links']['#weight']=3;
    $form['links']['#markup']='<div class="item-list"><ul><li class="first"><a href="/user/password" title="Forgot password via e-mail.">Forgot password</a></li><li class="last"><a href="/user/register" title="Join the Wiki.">Join the Wiki</a></li></ul></div>';
    $output .= render($form);   
    $variables['user_bar'] = '<div id="login-bar" class="user-bar">'.$output.'</div>';                         
  }                                                                            
  else {              
    $output .= '<span class="user-info">'.theme('historicalsurvey_username', array('account' => $user_info)).'</span>';
    //$output .= '<span class="user-dashboard">'.l('My Dashboard', 'dashboard').'</span>';
    $output .= '<span class="user-logout">'.l('Logout', 'user/logout').'</span>';
    $variables['user_bar'] = '<div id="loggedin-bar" class="user-bar">'.$output.'</div>';
  }
  
}