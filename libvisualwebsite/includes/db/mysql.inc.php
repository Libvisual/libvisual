<?php

/**
 * 
 * ByteHoard2 MySQL Database Abstraction Layer include library
 *
 * Same as bh1, but with prefixes.
 *
 * $Id: mysql.inc.php,v 1.1.1.1 2004-10-04 17:45:18 andrewgodwin Exp $
 *
 * @copyright ByteHoard team 2003-2004 
 * @license
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
**/
 
# Must be here so 2.1 knows it can create and drop.
# Although preg_grep just looks for the create_bhdb stuff.
$dbmoduleversion = 2;

# Define what configuration you want. Key = what the dbconfig key will be. Value = array, with Data type (string / password) and description.
$dbconfigneeded = array(
	"host"=>array("type"=>"string", "name"=>"MySQL Hostname", "description"=>"The hostname of your MySQL server. If it's running on the same machine, this will be 'localhost'."),
	"username"=>array("type"=>"string", "name"=>"MySQL Username", "description"=>"The username you use to connect to this server."),
	"password"=>array("type"=>"password", "name"=>"MySQL Password", "description"=>"The password you use to connect to this server."),
	"db"=>array("type"=>"string", "name"=>"MySQL Database", "description"=>"The database you want ByteHoard to use."),
	"prefix"=>array("type"=>"string", "name"=>"Table Prefix", "description"=>"The prefix ByteHoard should use on its tables. Leave it as the default unless you're doing multiple installations using the same database.", "default"=>"bh2_"),
);


function insert_bhdb($table, $values) {
global $dbconfig;

# Login to MySQL database
$dblink = mysql_connect($dbconfig['host'], $dbconfig['username'], $dbconfig['password']);

# Select the database
mysql_select_db($dbconfig['db']);

# Parse given variables into SQL statement.

$sql = "INSERT INTO `".$dbconfig['prefix'].$table."` ";

$csql = "( ";
$vsql = " VALUES ( ";

$n = 0;

foreach ($values as $column => $data) {

	if ($n == 0) {
		$csql .= "`".$column."`";
		$vsql .= "'".addslashes($data)."'";
	} else {
		$csql .= ", `".$column."`";
		$vsql .= ", '".addslashes($data)."'";
	}

	$n++;
	
}

$csql .= " )";
$vsql .= " )";

$sql = $sql.$csql.$vsql.";";

$result = mysql_query($sql);


if ($result === FALSE) { die ("MySQL Error: ".mysql_error()."<br><br>Query: ".$sql); }

return $result;

}

function delete_bhdb($table, $where) {
global $dbconfig;

# Login to MySQL database
$dblink = mysql_connect($dbconfig['host'], $dbconfig['username'], $dbconfig['password']);

# Select the database
mysql_select_db($dbconfig['db']);

# Parse given variables into SQL statement.

$sql = "DELETE FROM `".$dbconfig['prefix'].$table."` ";

$wsql = "WHERE ";

$n = 0;

foreach ($where as $column => $data) {

	if ($n == 0) {
		$wsql .= "`".$column."` = '".addslashes($data)."' ";
	} else {
		$wsql .= "AND `".$column."` = '".addslashes($data)."' ";
	}

	$n++;
	
}

$sql = $sql.$wsql.";";

$result = mysql_query($sql);

if ($result === FALSE) { die ("MySQL Error: ".mysql_error()."<br><br>Query: ".$sql); }

return $result;

}

function select_bhdb($table, $where, $limit) {
global $dbconfig;

# Login to MySQL database
$dblink = mysql_connect($dbconfig['host'], $dbconfig['username'], $dbconfig['password']);

# Select the database
mysql_select_db($dbconfig['db']);

# Parse given variables into SQL statement.

$sql = "SELECT * FROM `".$dbconfig['prefix'].$table."` ";

if ($where != "") {

	$wsql = "WHERE ";

	$n = 0;

	foreach ($where as $column => $data) {

		if ($n == 0) {
			$wsql .= "`".$column."` = '".addslashes($data)."' ";
		} else {
			$wsql .= "AND `".$column."` = '".addslashes($data)."' ";
		}
	
		$n++;
	
	}

	$sql = $sql.$wsql."";


} else {

	$sql = "SELECT * FROM `".$dbconfig['prefix'].$table."`";

}

if ($limit != "") {
	$sql .= " LIMIT ".$limit.";";
} else {
	$sql .= ";";
}

$result0 = mysql_query($sql);

$array = array();

if ($result0 === FALSE) { die ("MySQL Error: ".mysql_error()."<br><br>Query: ".$sql); }

while ($av = mysql_fetch_array($result0, MYSQL_ASSOC)) {

	$array[] = $av;
	
}

return $array;

}

function update_bhdb($table, $values, $where) {
global $dbconfig;

# Login to MySQL database
$dblink = mysql_connect($dbconfig['host'], $dbconfig['username'], $dbconfig['password']);

# Select the database
mysql_select_db($dbconfig['db']);

# Parse given variables into SQL statement.

$sql = "UPDATE `".$dbconfig['prefix'].$table."` ";

$ssql = "SET ";

$n = 0;


foreach ($values as $column => $data) {

	if ($n == 0) {
		$ssql .= "`".$column."` = '".addslashes($data)."' ";
	} else {
		$ssql .= ", `".$column."` = '".addslashes($data)."' ";
	}

	$n++;

}

if ($where != "") {

	$wsql = "WHERE ";

	$n = 0;

	foreach ($where as $column => $data) {

		if ($n == 0) {
			$wsql .= "`".$column."` = '".addslashes($data)."' ";
		} else {
			$wsql .= "AND `".$column."` = '".addslashes($data)."' ";
		}
	
		$n++;
	
	}

	$sql = $sql.$ssql.$wsql.";";


} else {

	$sql = $sql.$ssql.";";

}

$result = mysql_query($sql);

if ($result === FALSE) { die ("MySQL Error: ".mysql_error()."<br><br>Query: ".$sql); }

return $result;

}

function create_bhdb($table, $fields) {
global $dbconfig, $dbmoderror;

# Login to MySQL database
$dblink = mysql_connect($dbconfig['host'], $dbconfig['username'], $dbconfig['password']);

# Select the database
mysql_select_db($dbconfig['db']);

# start SQL query
$sql = "CREATE TABLE `".$dbconfig['prefix'].$table."` ( ";

foreach ($fields as $fieldname => $attributes) {

	# Get them all uppercase
	foreach ($attributes as $key=>$value) { $value = strtoupper($value); $attributes[$key] = $value; }

	if ($attributes['type'] == "VARCHAR") { $attributes['type'] = "VARCHAR(255)"; }
	if ($attributes['primary'] == 1) { $primarykey = $fieldname; }
	
	# Create SQL
	$sql .= "`".$fieldname."` ".$attributes['type']." NOT NULL, ";

}

# If there's a primary key, add it to the sql.
if (!empty($primarykey)) { $sql .= "PRIMARY KEY (`".$primarykey."`) )"; }
else { $sql = substr($sql, 0, -2); $sql .= ")"; }

$result = mysql_query($sql);

if ($result === FALSE) { $dbmoderror = ("MySQL Error: ".mysql_error()); }

return $result;

}

function drop_bhdb($table) {
global $dbconfig;

# Login to MySQL database
$dblink = mysql_connect($dbconfig['host'], $dbconfig['username'], $dbconfig['password']);

# Select the database
mysql_select_db($dbconfig['db']);

$sql = "DROP TABLE `".$dbconfig['prefix'].$table."`";

$result = mysql_query($sql);

if ($result === FALSE) { die ("MySQL Error: ".mysql_error()."<br><br>Query: ".$sql); }

return $result;

}

function test_bhdb($dbconfig) {
global $dbmoderror;

# Login to MySQL database
$dblink = @mysql_connect($dbconfig['host'], $dbconfig['username'], $dbconfig['password']);

if ($dblink === FALSE) { $dbmoderror = "MySQL Error: ".mysql_error(); return FALSE; }

# Select the database
$seldb = @mysql_select_db($dbconfig['db']);

if ($seldb === FALSE) { 
	$cdb = mysql_query("CREATE DATABASE `".$dbconfig['db']."`");
	if ($cdb === FALSE) {
		$dbmoderror = "MySQL Error: ".mysql_error(); return FALSE; 
	} else {
		return TRUE;
	}
}

return TRUE;

}

function table_exists_bhdb($table) {

$dblink = @mysql_connect($dbconfig['host'], $dbconfig['username'], $dbconfig['password']);

if ($dblink === FALSE) { $dbmoderror = "MySQL Error: ".mysql_error(); return FALSE; }

# Select the database
mysql_select_db($dbconfig['db']);

# Select the table
$seldb = mysql_query("SELECT * FROM `".$table."`");

if ($seldb === FALSE) { 
	return FALSE;
} else {
	return TRUE;
}

}

?>