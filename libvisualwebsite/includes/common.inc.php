<?php

/*
 * Libvisual CMS
 * All code copyright (c) Andrew Godwin 2004
 *
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

$page = new cms_page($page);

?>