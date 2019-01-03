<?php
session_start();
?>
<!DOCTYPE html>
<html>
<head>
	<style>
	table, th, td {
  	border: 1px solid black;
	}
	</style>
	<script src="jquery-3.3.1.js"></script>
	<title>
		Tema psncs - Maries Sergiu
	</title>
</head>
<body>

<form id="add_comment_form" action="add_comment.php " method='post'>
  <div class="container">
    <label for="cmt"><b>Comment: </b></label>
    <input type="text" placeholder="Enter new comment" name="comment" required maxlength="30">
    <button type="submit">Add</button>
  </div>
</form>

<form id="ping_form" onsubmit="ping()">
  <div class="container">
    <label for="ipl"><b>Ip: </b></label>
    <input type="text" placeholder="Enter target ip" name="ip" id="ip">
    <button type="submit"> Ping </button>
  </div>
</form>

<!-- <form onsubmit="ping()">
  IP Address: <input type="text" id="ip">
  <input type="submit">
</form> -->

<div id="comments"></div>

<script type="text/javascript">
	window.onload = function(){ 
    $.get("load_comments.php", { }, function(data, status){
          $("#comments").html(data); 
          console.log(data);
        }
    );
  } 

function ping() {
    var ip = $("#ip").val();
    $.get("action_ping.php", { ip : ip }, function(data, status){
            console.log(data);
        }
    );
}
 

</script>

</body> 
</html>