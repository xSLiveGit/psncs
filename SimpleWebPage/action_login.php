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
            $con = new SQLite3('hw.db');
            
            $passwd = hash("sha256", $_POST['password']);
            $stmt = $con->prepare('SELECT * FROM users WHERE username=:username AND password=:password'); 

            $stmt->bindParam(':username', $_POST['username']);
            $stmt->bindParam(':password', $passwd);

            $result = $stmt->execute();

            if ($user = $result->fetchArray(1))
            {
            	if ((($user['type'] == 'admin') || ($_POST['type'] == 'normal'))) 
                {
                    $sessionEndTime = time() + 24 * 60 * 60;//24 hours

                    $_SESSION['sessionEndTime'] = $sessionEndTime;
            		$_SESSION['username'] = $_POST['username'];
                    $_SESSION['type'] = $_POST['type'];

                    header("Location: /page2.php");
            	}
                else
                {
                    echo('invalid credential for given type');
                }
            }
            else
            {
                echo('invalid credential for given type');
            }
        }
        catch(Exception $Exception ) 
        {
            echo('Internal error');
        }
    }
}
?>