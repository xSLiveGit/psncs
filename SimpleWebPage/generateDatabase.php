<?php

class DataBaseGenerator
{
	private $connection; 

	public function __construct()
	{
		$this->connection = new SQLite3('hw.db');
		$this->connection->exec('PRAGMA foreign_keys = ON;');

		$this->createUsersTable();
		$this->createCommentsTable();
		
		$this->connection->close();
	}

	private function createUsersTable()
	{
		try
		{
			$createUsersTableStmt = 'CREATE TABLE IF NOT EXISTS users (
                                        username TEXT PRIMARY KEY,
                                        password TEXT NOT NULL,
                                        type     TEXT CHECK( type in (\'normal\', \'admin\')))';
                                        
            $this->connection->exec($createUsersTableStmt);
            $this->populateUserTableWithUsers();
		}
		catch(Exception $exception)
		{
			echo $exception->getMessage();
		}
	}

	private function populateUserTableWithUsers()
	{
		try
		{
			$this->insertNewUser('admin', 'admin', 'admin');
			$this->insertNewUser('test', 'test', 'normal');
			$this->insertNewUser('admin2', 'admin2', 'admin');
			$this->insertNewUser('pop', 'popescu', 'normal');
		}
		catch(Exception $exception)
		{
			echo $exception->getMessage();
		}
	}

	private function insertNewUser($username, $password, $type)
	{
		try
		{
			$insertString = 'INSERT OR REPLACE INTO users(username, password, type) VALUES (:username, :password, :type)';
			$insertStatement = $this->connection->prepare($insertString);
			
			$encryptedPass = hash("sha256", $password);

			$insertStatement->bindParam(':username', $username);
			$insertStatement->bindParam(':password', $encryptedPass);
			$insertStatement->bindParam(':type', $type);

			$insertStatement->execute();
		}
		catch(Exception $exception)
		{
			echo $exception->getMessage();
		}
	}

	private function createCommentsTable()
	{
		try
		{
			$createCommsTableStmt = 'CREATE TABLE IF NOT EXISTS comments (
									commentId INTEGER PRIMARY KEY AUTOINCREMENT,
									content   TEXT,
									username  TEXT,
									FOREIGN KEY(username) REFERENCES users(username))';

			$this->connection->exec($createCommsTableStmt);
			$this->insertSomePredefinedComments();
		}
		catch(Exception $exception)
		{
			echo $exception->getMessage();
		}
	}

	private function insertSomePredefinedComments()
	{
		if($this->connection == NULL)
			echo 'connection is null';

		$this->insertNewComment('acesta este un comentariu de admin', 'admin');
		$this->insertNewComment('ana are mere', 'test');
		$this->insertNewComment('maria are pere', 'pop');
		$this->insertNewComment('eu nu am nimic', 'admin2');
		$this->insertNewComment('acesta este un alt comentariu de admin', 'admin');
	}

	private function insertNewComment($comment, $owningUser)
	{
		if($this->connection == NULL)
			echo 'connection is null2' . $comment . ' ' . $owningUser;
		try
		{
			$insertString ='INSERT INTO comments(content, username) VALUES (:content, :username)';
			$preparedStatement = $this->connection->prepare($insertString);
			$preparedStatement->bindParam(':content', $comment);
			$preparedStatement->bindParam(':username', $owningUser);

			$preparedStatement->execute();
		}
		catch(Exception $exception)
		{
			echo $exception->getMessage();
		}
	}
}

$dbGenerator = new DataBaseGenerator();

?>