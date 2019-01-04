<?php
session_start();

function checkSession()
{
	if(!isset($_SESSION['username']) || !isset($_SESSION['sessionEndTime'])) //todo: check if session is valid due time
	{
		session_unset();
    	session_destroy();
	  	header("Location: /index.html");
        exit(0);
	}

	if (time() > $_SESSION['sessionEndTime'])
    {
    	session_unset();
    	session_destroy();
        header('Location: /index.html'); 
        exit(0);
    }
}

checkSession();

?>