<?php

/**
 * 
 * ByteHoard2 Email include library
 *
 * Hopefully these will work well.
 *
 * $Id: email.inc.php,v 1.1.1.1 2004-10-04 17:45:17 andrewgodwin Exp $
 *
 * @copyright ByteHoard team 2003-2004
 * @license
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
**/

# General emailing function
function email_bh($to, $subject, $message) {
	global $bhconfig;

	if ($bhconfig['enable_fm'] == 0) { return 255; }
	
	# Get the nice OO mimemail class
	require_once "includes/mimemail.inc.php";
	
	# Create & send email
	$mailobj = new MIMEMail();
	$mailobj->From($bhconfig['fromaddr'], 'ByteHoard @ '.$bhconfig['servername']);
	$mailobj->To($to);
	$mailobj->ReplyTo($bhconfig['fromaddr']);
	$mailobj->Subject($subject);
	$mailobj->setHeader('X-Mailer', 'ByteHoard '.$bhconfig['version']);
	$mailobj->Priority(2);
	$mailobj->MessageStream($message);
	
	$mailobj->Send();
}

# Send email to all admins
function email_userclass_bh($class, $subject, $message) {

	# Get admins
	$rows = select_bhdb("users", array("type"=>$class), "");
	
	# Cycle through, emailing each one
	foreach ($rows as $row) {
		email_bh($row['email'], $subject, $message);
	}

}