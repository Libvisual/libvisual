<?php

require "includes/common.inc.php";

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
  <title>Libvisual :: the stuff that makes you want to do 250mics</title>
  <meta name="GENERATOR" content="Quanta Plus" />
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
  <meta http-equiv="content-style-type" content="text/css" />
  <link rel="stylesheet" type="text/css" title="Dark Style" href="css/main.css" />
  <link rel="alternate stylesheet" type="text/css" title="Light Style" href="css/light.css" />
  <link rel="alternate stylesheet" type="text/css" title="No Top Menu" href="css/notop.css" />
  <link rel="alternate stylesheet" type="text/css" title="No Left Menu" href="css/noleft.css" />
</head>
<body>

<div class="logopicture"><img src="images/libvisual-banner1.png" alt="libvisual logo" /></div>

<div class="leftmenu">
<?php echo cms_vertmenu(); ?>
</div>

<div class="topmenu">
<?php echo cms_horizmenu(); ?>
</div>

<div class="mainbox">
	<?php echo $page->html; ?>
</div>

</body>
</html>
