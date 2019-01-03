<?php
session_start(); //begin new session

//check for post method
if (!empty( $_POST )) 
{
    if ( isset( $_POST['username'] ) && isset( $_POST['password'] ) && isset($_POST['type']) ) 
    {
        try 
        {
            // $con = new SQLiteDatabase('db.sqllite');
            $con = new SQLite3('db.sqllite');
            $encryptedPass = $_POST['password'];

            echo 'username: ' . $_POST['username'] . 'password: ' . $_POST['password'];
            
            $stmt = $con->prepare('SELECT * FROM users WHERE username=:username AND password=:password'); 

            $stmt->bindParam(':username', $_POST['username']);
            $stmt->bindParam(':password', $encryptedPass);

            // $stmt = $con->prepare('SELECT * FROM users');
            $result = $stmt->execute();

            if($result)
            {
                echo("am rezultat");
                echo ($result->numColumns());
                // echo ($result->fetchArray());
                // while($user = $result->fetchArray(1))
                // {
                //     echo($user['username']);
                //     echo($user['password']);
                //     echo($user['type']);
                // }
            }
            if ($user = $result->fetchArray(1))
            {
                echo('am un rezultat');
            	// Verify user password and set $_SESSION
            	if ((($user['type'] == 'admin') || ($_POST['type'] == 'normal'))) 
                {
            		$_SESSION['user_id'] = $user['id'];
                    $_SESSION['type'] = $_POST['type'];
                    header('page2.html');
            	}
                else
                {
                    echo('aici invalid credential for given type');
                }
            }
            else
            {
                echo('invalid credential for given type');
            }
        }
        catch(Exception $Exception ) 
        {
            echo($Exception->getMessage());
        }
    }
}
?>