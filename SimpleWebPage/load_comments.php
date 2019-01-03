<?php

function loadCommentsAsHtmlTableFormat()
{
	try
	{

		$htmlStringFormat = '<table><tr><th> Username </th><th> Comment </th> </tr>';

		$connection = new SQLite3('hw.db');
		$stmt = $connection->prepare('SELECT * FROM comments'); 
		$result = $stmt->execute();
		
		while($value = $result->fetchArray(1))
		{
			$htmlStringFormat = $htmlStringFormat . '<tr><td>' . htmlspecialchars($value['username']) . '</td><td>' . htmlspecialchars($value['content']) . '</td></tr>';
		}
		

		$htmlStringFormat = $htmlStringFormat . '</table>';

		return $htmlStringFormat;
	}
	catch(Exceotion $exception)
	{
		return "<p>Internal error </p>";
	}
}

function loadComments()
{

}

echo loadCommentsAsHtmlTableFormat();

?>