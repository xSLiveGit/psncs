<?php

require 'session_validation.php';

try
{
	$filePath = $_POST['filePath'];

	if( !file_exists( $filePath ) )
	{
		echo "Provide a valid file path";
	}
	else if( !is_file( $filePath ) )
	{
		echo "Given path is not a file valid path";
	}
	else
	{
		$fileContent = file_get_contents( $filePath ); // get the contents, and echo it out.
		$fileContent = htmlspecialchars( $fileContent );
		echo( $fileContent );	
	}
}
catch( Exception $exception )
{
	// echo $exception->getMessage();
	echo 'Internal error';
}
exit(0);

?>

