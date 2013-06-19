<?php

/*
 *  Implements hook_init()
 *  Runs on every non-cached page
 */
function historicalsurvey_admin_init(){
   
}

/*
 *  Implements hook_node_view()
 */
function historicalsurvey_admin_node_view($node, $view_mode, $langcode){

}

/*
 *  Main debugging output function, writes to debug_log.html
 */
function dw($variable,$name="<span style='color:#666'><em>variable</em></span>",$add=FALSE){
   ob_start();
    //var_dump($variable);
    _do_dump($variable,$name);
    $data = ob_get_contents();
  ob_end_clean();  
  //$data=var_export($variable,true);   
  //$data="<pre class='xdebug-var-dump' dir='ltr'><strong>". $name . "=</strong>\n<span style='text-indent:50px;'>" . $data . "</span>";
  
  $file = 'debug_log.html';
  
  if($add){    
    //delete the first line (datetime)    
    $data_array=file($file);
    array_shift($data_array); 
    $data_string='';
    for($i=0;$i<=count($data_array);$i++){
      if(isset($data_array[$i])){
        $data_string.=$data_array[$i];
      }
    }
    $data = empty($data_string)?$data:$data_string . "\n---\n". $data;
  }
  $data="<html style='background-color:black;font-size:12px'><h1 style='color:#666'>".date('H:i:s')."</h1>"."\n".$data;
  file_put_contents($file, $data);
}

/*
 *  Legacy debugging output function
 */
function debug_write($variable,$name="<span style='color:#666'><em>variable</em></span>",$add=FALSE){
  return dw($variable,$name,$add);
}

/**
 * Better GI than print_r or var_dump... from php.org
 */
function _do_dump(&$var, $var_name = "", $indent = NULL, $reference = NULL){
  $do_dump_indent = "<span style='color:#666666;'>.</span>&nbsp;";
  $reference = $reference.$var_name;
  $keyvar = 'the_do_dump_recursion_protection_scheme'; $keyname = 'referenced_object_name';
  
  // So this is always visible and always left justified and readable
  echo "<div style='text-align:left; background-color:black; font: 100% monospace; color:#e5c138;'>";

  if (is_array($var) && isset($var[$keyvar])){
    $real_var = &$var[$keyvar];
    $real_name = &$var[$keyname];
    $type = ucfirst(gettype($real_var));
    echo  "$indent$var_name <span style='color:#666666'>$type</span> = <span style='color:#e87800;'>&amp;$real_name</span><br>";
  }else{
    $var = array($keyvar => $var, $keyname => $reference);
    $avar = &$var[$keyvar];

    $type = gettype($avar);
    if($type == "string") $type_color = "<span style='color:#a8ff00'>";
    elseif($type == "integer") $type_color = "<span style='color:red'>";
    elseif($type == "double"){ $type_color = "<span style='color:#0099c5'>"; $type = "Float"; }
    elseif($type == "boolean") $type_color = "<span style='color:#92008d'>";
    elseif($type == "NULL") $type_color = "<span style='color:white'>";

    if(is_array($avar)){
      $count = count($avar);
      echo  "$indent" . ($var_name ? "<span style='color:#55e439'>$var_name</span> => ":"") . "<span style='color:#37a3ed'>$type</span><span style='color:#666666'>($count)</span>(<br>";
      $keys = array_keys($avar);
      foreach($keys as $name){
        $value = &$avar[$name];
        _do_dump($value, "<span style='color:#55e439'>'$name'</span>", $indent.$do_dump_indent, $reference);
      }
      echo  "$indent)<br>";
    }elseif(is_object($avar)){
      echo  "$indent$var_name <span style='color:#666666'>$type</span>(<br>";
      foreach($avar as $name=>$value){
        _do_dump($value, "$name", $indent.$do_dump_indent, $reference);
      }
      echo  "$indent)<br>";
    }elseif(is_int($avar)){
      echo  "$indent<span style='color:#55e439'>$var_name</span> = <span style='color:#37a3ed'>$type</span><span style='color:#666666'>(".strlen($avar).")</span> $type_color".htmlentities($avar)."</span><br>";
    }elseif(is_string($avar)){
      echo  "$indent<span style='color:#55e439'>$var_name</span> = <span style='color:#37a3ed'>$type</span><span style='color:#666666'>(".strlen($avar).")</span> $type_color\"".htmlentities($avar)."\"</span><br>";
    }elseif(is_float($avar)){
      echo  "$indent<span style='color:#55e439'>$var_name</span> = <span style='color:#37a3ed'>$type</span><span style='color:#666666'>(".strlen($avar).")</span> $type_color".htmlentities($avar)."</span><br>";
    }elseif(is_bool($avar)){
      echo  "$indent<span style='color:#55e439'>$var_name</span> = <span style='color:#37a3ed'>$type</span><span style='color:#666666'>(".strlen($avar).")</span> $type_color".($avar == 1 ? "TRUE":"FALSE")."</span><br>";
    }elseif(is_null($avar)){
      echo  "$indent<span style='color:#55e439'>$var_name</span> = <span style='color:#37a3ed'>$type</span><span style='color:#666666'>(".strlen($avar).")</span> {$type_color}NULL</span><br>";
    }else{
      echo  "$indent<span style='color:#55e439'>$var_name</span> = <span style='color:#37a3ed'>$type</span><span style='color:#666666'>(".strlen($avar).")</span> ".htmlentities($avar)."<br>";
    }
    $var = $var[$keyvar];
  }
  
  echo  "</div>";
}


/**
 * View any string as a hexdump.
 *
 * This is most commonly used to view binary data from streams
 * or sockets while debugging, but can be used to view any string
 * with non-viewable characters.
 *
 * @version     1.3.2
 * @author      Aidan Lister <aidan@php.net>
 * @author      Peter Waller <iridum@php.net>
 * @link        http://aidanlister.com/2004/04/viewing-binary-data-as-a-hexdump-in-php/
 * @param       string  $data        The string to be dumped
 * @param       bool    $htmloutput  Set to false for non-HTML output
 * @param       bool    $uppercase   Set to true for uppercase hex
 */
function hexdump ($data, $htmloutput = false, $uppercase = false)
{
    // Init
    $hexi   = '';
    $ascii  = '';
    $dump   = ($htmloutput === true) ? '<pre>' : '';
    $offset = 0;
    $len    = strlen($data);
  
    // Upper or lower case hexadecimal
    $x = ($uppercase === false) ? 'x' : 'X';
  
    // Iterate string
    for ($i = $j = 0; $i < $len; $i++)
    {
        // Convert to hexidecimal
        $hexi .= sprintf("%02$x ", ord($data[$i]));
  
        // Replace non-viewable bytes with '.'
        if (ord($data[$i]) >= 32) {
            $ascii .= ($htmloutput === true) ?
                            htmlentities($data[$i]) :
                            $data[$i];
        } else {
            $ascii .= '.';
        }
  
        // Add extra column spacing
        if ($j === 7) {
            $hexi  .= ' ';
            $ascii .= ' ';
        }
  
        // Add row
        if (++$j === 16 || $i === $len - 1) {
            // Join the hexi / ascii output
            $dump .= sprintf("%04$x  %-49s  %s", $offset, $hexi, $ascii);
             
            // Reset vars
            $hexi   = $ascii = '';
            $offset += 16;
            $j      = 0;
             
            // Add newline           
            if ($i !== $len - 1) {
                $dump .= "\n";
            }
        }
    }
  
    // Finish dump
    $dump .= $htmloutput === true ?
                '</pre>' :
                '';
    $dump .= "\n";
        
    return $dump;
}