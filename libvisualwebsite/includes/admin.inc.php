<?php

/*
 * Libvisual CMS
 * All code copyright (c) Andrew Godwin 2004
 * Admin include
 */
 
# Start session
session_start();
 
# Get config
require "config.inc.php";
  
# Get database library
require "includes/db/mysql.inc.php";

# Get other libraries
require "includes/menu.inc.php";
require "includes/pages.inc.php";
require "includes/news.inc.php";
require "includes/email.inc.php";
require "includes/mimemail.inc.php";

# Load requested page
$page = $_GET['page'];
if (empty($page)) { $page = "main"; }


function cms_login($username, $password) {

	$rows = select_bhdb("users", array("username"=>$username, "password"=>md5($password)), 1);
	if (empty($rows)) {
		cms_error("invalid_login");
	} else {
		$_SESSION['username'] = $username;
	}

}

function cms_logout() {
	$_SESSION['username'] = "";
}



?>