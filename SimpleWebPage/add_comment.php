<?php
require 'session_validation.php';

if(!empty($_POST) && isset($_POST['comment']) && isset($_SESSION['username']))
{
	try
	{
		$connection = new SQLite3('hw.db');
		$insertString ='INSERT INTO comments(content, username) VALUES (:content, :username)';
		$preparedStatement = $connection->prepare($insertString);
		$content = htmlspecialchars($_POST['comment']);//avoind metacharacter injection
		$preparedStatement->bindParam(':content', $content);
		$preparedStatement->bindParam(':username', $_SESSION['username']);

		$preparedStatement->execute();
		header("Location: /page2.php");
	}
	catch(Exception $exception)
	{
		echo '<script language="javascript">';
		echo 'alert("Internal error")';
		echo '</script>';
	}
}
else
{
	echo var_dump($_SESSION);
}

?>