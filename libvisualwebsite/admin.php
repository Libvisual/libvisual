<?php

require "includes/admin.inc.php";

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
  <title>Libvisual :: Administration Center</title>
  <meta name="GENERATOR" content="Quanta Plus" />
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
  <meta http-equiv="content-style-type" content="text/css" />
  <link rel="stylesheet" type="text/css" title="Admin Style" href="css/admin.css" />
</head>
<body>

<div class="topbar">
<span class="topbartitle">Libvisual<strong>CMS</strong></span> &nbsp; <?php echo date("l dS F Y ")."".date("h:i A"); ?>
</div>

<?php

$action = $_GET['action'];

if ($action == "editcontent") {

	echo "<form action='admin.php?action=savecontent'>\n".
	"<";

}



?>

</body>
</html>
