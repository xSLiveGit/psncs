<?php


if( !empty($_GET) && isset($_GET['ip']) ) 
{

    $ip = $_GET['ip'];
    if (!filter_var($ip, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4) ) 
    {
        // echo '<script language="javascript">';
        // echo 'alert("invalid ip")';
        // echo '</script>'; 
   
        echo $ip . " is invalid ip";
    } 
    else
    {
        $pingresult = exec("ping -n 3 $ip", $outcome, $status);
        if (0 == $status) {
            $status = "alive";
        } else {
            $status = "dead";
        }

        echo( "The IP address, $ip, is  " . $status );
  
        // echo '<script language="javascript">';
        // echo "alert('" . $result . "')";
        // echo '</script>'; 
    }
    
}
else
{
    // echo '<script language="javascript">';
    // echo 'alert("you hould provide an ip")';
    // echo '</script>'; 

    echo "you hould provide an ip";
}

    
?>