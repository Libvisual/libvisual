// Get tinyMCE reference
var tinyMCE = window.opener.tinyMCE;
var tinyMCELang = window.opener.tinyMCELang;

// Setup title
var re = new RegExp('{|\\\$|}', 'g');
var title = document.title.replace(re, "");
if (typeof tinyMCELang[title] != "undefined") {
	var divElm = document.createElement("div");
	divElm.innerHTML = tinyMCELang[title];
	document.title = divElm.innerHTML;
}

function TinyMCEPlugin_onLoad() {
	document.body.innerHTML = tinyMCE.applyTemplate(document.body.innerHTML, tinyMCE.windowArgs);
}

// Add onload trigger
if (tinyMCE.isMSIE)
	attachEvent("onload", TinyMCEPlugin_onLoad);
else
	addEventListener("load", TinyMCEPlugin_onLoad, false);

// Output Popup CSS class
document.write('<link href="' + tinyMCE.getParam("popups_css") + '" rel="stylesheet" type="text/css">');
