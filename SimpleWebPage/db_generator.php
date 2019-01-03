<?php

class DataBaseGenerator
{
	public function __construct()
	{
		$connection = new SQLite3('hw.db');
		$connection->exec('PRAGMA foreign_keys = ON;');

		$this->createUsersTable($connection);
		$this->createCommentsTable($connection);

		$connection->close();
	}

	private function createUsersTable($connection)
	{
		try
		{
			$createUsersTableStmt = 'CREATE TABLE IF NOT EXISTS users (
                                        username TEXT PRIMARY KEY,
                                        password TEXT NOT NULL,
                                        type     TEXT CHECK( type in (\'normal\', \'admin\')))';
                                        
            $connection->exec($createUsersTableStmt);
            populateUserTableWithUsers($connection);
		}
		catch(Exception $exception)
		{
			//TODO
		}
	}

	private function populateUserTableWithUsers($connection)
	{
		try
		{
			insertNewUser($connection, 'admin', 'admin', 'admin');
			insertNewUser($connection, 'test', 'test', 'normal');
			insertNewUser($connection, 'admin2', 'admin2', 'admin');
			insertNewUser($connection, 'pop', 'popescu', 'normal');
		}
		catch(Exception $exception)
		{
			//todo
		}
	}

	private function insertNewUser($connection, $username, $password, $type)
	{
		try
		{
			$insertString = 'INSERT INTO users(username, password, role) 
									VALUES (:username, :password, :type)
							';
			$insertStatement = $connection->prepare($insertString);
			
			$encryptedPass = hash("sha256", $password);

			$insertStatement->bindParam(':username', $username);
			$insertStatement->bindParam(':password', $encryptedPass);
			$insertStatement->bindParam(':type', $type);

			$insertStatement->execute();
		}
		catch(Exception $exception)
		{
			//TODO
		}
	}

	private function createCommentsTable($connection)
	{
		try
		{
			$createCommsTableStmt = 'CREATE TABLE IF NOT EXISTS users (
									commentId INTEGER PRIMARY KEY AUTOINCREMENT,
									content   TEXT,
									username  TEXT,
									FOREIGN KEY(username) REFERENCES users(username))';

			$connection->exec($createCommsTableStmt);
			$this->insertSomePredefinedComments($connection);
		}
		catch(Exception $exception)
		{
			//todo
		}
	}

	private function insertSomePredefinedComments($connection)
	{
		$this->insertNewComment($connetion, 'acesta este un comentariu de admin', 'admin');
		$this->insertNewComment($connetion, 'ana are mere', 'test');
		$this->insertNewComment($connetion, 'maria are pere', 'pop');
		$this->insertNewComment($connetion, 'eu nu am nimic', 'admin2');
		$this->insertNewComment($connetion, 'acesta este un alt comentariu de admin', 'admin');
	}

	private function insertNewComment($connection, $comment, $owningUser)
	{
		try
		{
			$insertString ='INSERT INTO comments(content, username) VALUES (:content, :username)';
			$preparedStatement = $connection->prepare($insertString);

			$preparedStatement->bindParam(':content', $comment);
			$preparedStatement->bindParam(':username', $owningUser);

			$preparedStatement->execute();
		}
		catch(Exception $exception)
		{
			//todo
		}
	}
}

?>