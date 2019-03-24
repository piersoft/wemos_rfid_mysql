<?php

$database = "myarduino"; // creare db myarduino
$db_record = 'card'; // creare tabella card nel DB myarduino e la chiave cardid di tipo text 
$hostname = "localhost";
$user = "UTENTE"; //utente che puo' accedere al db myarduino
$password = "PASSWORD";


mysql_connect($hostname, $user, $password)
or die('Could not connect: ' . mysql_error());

mysql_select_db($database)
or die ('Could not select database ' . mysql_error());

$query = mysql_query("SELECT cardid FROM ".$db_record);

$trovato=0;
while((list($cardid) = mysql_fetch_row($query)))
{
//  var_dump($cardid); // DEBUG
if ($_GET['card']==$cardid) $trovato=1;
}

if ($trovato !=0 ) echo "SI"; // messaggio di risposta che nel codice di arduino è citato in if(line.indexOf("SI") > 0)
else echo "NIENTE DA FARE";

?>
