/**
 * $RCSfile: tiny_mce_src.js,v $
 * $Revision: 1.1.1.1 $
 * $Date: 2004-10-04 17:45:30 $
 *
 * @author Moxiecode
 * @copyright Copyright © 2004, Moxiecode Systems AB, All rights reserved.
 */

function TinyMCE() {
	this.instances = new Array();
	this.stickyClassesLookup = new Array();
	this.windowArgs = new Array();

	this.isMSIE = (navigator.appName == "Microsoft Internet Explorer");
	this.idCounter = 0;

	// * * Functions
	this.init = TinyMCE_init;
	this.addMCEControl = TinyMCE_addMCEControl;
	this.createMCEControl = TinyMCE_createMCEControl;
	this.triggerSave = TinyMCE_triggerSave;
	this.execCommand = TinyMCE_execCommand;
	this.execInstanceCommand = TinyMCE_execInstanceCommand;
	this.setEditMode = TinyMCE_setEditMode;
	this.handleEvent = TinyMCE_handleEvent;
	this.setupContent = TinyMCE_setupContent;
	this.switchClass = TinyMCE_switchClass;
	this.restoreAndSwitchClass = TinyMCE_restoreAndSwitchClass;
	this.switchClassSticky = TinyMCE_switchClassSticky;
	this.restoreClass = TinyMCE_restoreClass;
	this.setClassLock = TinyMCE_setClassLock;
	this.onLoad = TinyMCE_onLoad;
	this.removeMCEControl = TinyMCE_removeMCEControl;
	this._initCleanup = TinyMCE__initCleanup;
	this._cleanupHTML = TinyMCE__cleanupHTML;
	this._cleanupAttribute = TinyMCE__cleanupAttribute;
	this._fixInlineStyles = TinyMCE__fixInlineStyles;
	this._cleanupElementName = TinyMCE__cleanupElementName;
	this._verifyClass = TinyMCE__verifyClass;
	this.cleanupNode = TinyMCE_cleanupNode;
	this.convertStringToXML = TinyMCE_convertStringToXML;
	this.insertLink = TinyMCE_insertLink;
	this.insertImage = TinyMCE_insertImage;
	this.getElementByAttributeValue = TinyMCE_getElementByAttributeValue;
	this.getElementsByAttributeValue = TinyMCE_getElementsByAttributeValue;
	this.getParentElement = TinyMCE_getParentElement;
	this.getParam = TinyMCE_getParam;
	this.getLang = TinyMCE_getLang;
	this.replaceVar = TinyMCE_replaceVar;
	this.replaceVars = TinyMCE_replaceVars;
	this.triggerNodeChange = TinyMCE_triggerNodeChange;
	this.parseURL = TinyMCE_parseURL;
	this.convertAbsoluteURLToRelativeURL = TinyMCE_convertAbsoluteURLToRelativeURL;
	this.updateContent = TinyMCE_updateContent;
	this.getContent = TinyMCE_getContent;
	this.setContent = TinyMCE_setContent;
	this.importThemeLanguagePack = TinyMCE_importThemeLanguagePack;
	this.applyTemplate = TinyMCE_applyTemplate;
	this.openWindow = TinyMCE_openWindow;
	this.handleVisualAid = TinyMCE_handleVisualAid;
	this.setAttrib = TinyMCE_setAttrib;
	this.getAttrib = TinyMCE_getAttrib;
	this._selectNode = TinyMCE__selectNode;
	this._getThemeFunction = TinyMCE__getThemeFunction;
	this._setHTML = TinyMCE__setHTML;
	this._getElementById = TinyMCE__getElementById;
	this._getInstanceById = TinyMCE__getInstanceById;
	this.getEditorId = TinyMCE_getEditorId;
	this.queryInstanceCommandValue = TinyMCE_queryInstanceCommandValue;
	this.queryInstanceCommandState = TinyMCE_queryInstanceCommandState;
	this.getWindowArg = TinyMCE_getWindowArg;
	this.getCSSClasses = TinyMCE_getCSSClasses;
	this.regexpReplace = TinyMCE_regexpReplace;
}

function TinyMCE_init(settings) {
	var theme;

	// * * Check if valid browser (has execcommand and is JavaScript 1.2+, JScript 5.5+)
	if (typeof document.execCommand == 'undefined' || !new Array().push)
		return;

	// * * Get script base path
	if (!tinyMCE.baseURL) {
		var elements = document.getElementsByTagName('script');

		for (var i=0; i<elements.length; i++) {
			if (elements[i].src && (elements[i].src.indexOf("tiny_mce.js") != -1 || elements[i].src.indexOf("tiny_mce_src.js") != -1)) {
				var src = elements[i].src;

				src = src.substring(0, src.lastIndexOf('/'));

				tinyMCE.baseURL = src;
				break;
			}
		}
	}

	// * * Get document base path
	this.documentBasePath = document.location.href;
	if (this.documentBasePath.indexOf('?') != -1)
		this.documentBasePath = this.documentBasePath.substring(0, this.documentBasePath.indexOf('?'));
	this.documentBasePath = this.documentBasePath.substring(0, this.documentBasePath.lastIndexOf('/'));

	// * * If not HTTP absolute
	if (tinyMCE.baseURL.indexOf('://') == -1 && tinyMCE.baseURL.charAt(0) != '/') {
		// * * If site absolute
		tinyMCE.baseURL = this.documentBasePath + "/" + tinyMCE.baseURL;
	}

	this.settings = settings;
	this.settings['mode'] = tinyMCE.getParam("mode", "none");
	this.settings['theme'] = tinyMCE.getParam("theme", "default");
	this.settings['theme_plugins'] = tinyMCE.getParam("theme_plugins", "");
	this.settings['language'] = tinyMCE.getParam("language", "uk");
	this.settings['docs_language'] = tinyMCE.getParam("docs_language", this.settings['language']);
	this.settings['elements'] = tinyMCE.getParam("elements", "");
	this.settings['textarea_trigger'] = tinyMCE.getParam("textarea_trigger", "mce_editable");
	this.settings['valid_elements'] = tinyMCE.getParam("valid_elements", "a[name|href|target],strong/b[class],em/i[class],strike[class],u[class],p[class|align],ol,ul,li,br,img[class|src|border=0|alt|hspace|vspace|width|height|align],sub,sup,blockquote[dir|style],table[border=0|cellspacing|cellpadding|width|height|class|align],tr[class|rowspan],td[class|colspan|rowspan|width|height],div[class|align],span[class|align],pre[class|align],address[class|align],h1[class|align],h2[class|align],h3[class|align],h4[class|align],h5[class|align],h6[class|align],hr");
	this.settings['extended_valid_elements'] =  tinyMCE.getParam("extended_valid_elements", "");
	this.settings['invalid_elements'] = tinyMCE.getParam("invalid_elements", "");
	this.settings['encoding'] = tinyMCE.getParam("encoding", "");
	this.settings['urlconvertor_callback'] = tinyMCE.getParam("urlconvertor_callback", "TinyMCE_convertURL");
	this.settings['save_callback'] = tinyMCE.getParam("save_callback", "");
	this.settings['debug'] = tinyMCE.getParam("debug", false);
	this.settings['force_br_newlines'] = tinyMCE.getParam("force_br_newlines", false);
	this.settings['add_form_submit_trigger'] = tinyMCE.getParam("add_form_submit_trigger", true);
	this.settings['relative_urls'] = tinyMCE.getParam("relative_urls", true);
	this.settings['remove_script_host'] = tinyMCE.getParam("remove_script_host", true);
	this.settings['focus_alert'] = tinyMCE.getParam("focus_alert", true);
	this.settings['document_base_url'] = tinyMCE.getParam("document_base_url", "" + document.location.href);
	this.settings['visual'] = tinyMCE.getParam("visual", true);
	this.settings['visual_table_style'] = tinyMCE.getParam("visual_table_style", "border: 1px dashed #BBBBBB");
	this.settings['setupcontent_callback'] = tinyMCE.getParam("setupcontent_callback", "");
	this.settings['fix_content_duplication'] = tinyMCE.getParam("fix_content_duplication", true);
	this.settings['custom_undo_redo'] = tinyMCE.getParam("custom_undo_redo", true);
	this.settings['custom_undo_redo_levels'] = tinyMCE.getParam("custom_undo_redo_levels", -1);
	this.settings['verify_css_classes'] = tinyMCE.getParam("verify_css_classes", true);
	this.settings['trim_span_elements'] = tinyMCE.getParam("trim_span_elements", true);
	this.settings['verify_html'] = tinyMCE.getParam("verify_html", true);
	this.settings['apply_source_formatting'] = tinyMCE.getParam("apply_source_formatting", false);
	this.settings['directionality'] = tinyMCE.getParam("directionality", "ltr");
	this.settings['auto_cleanup_word'] = tinyMCE.getParam("auto_cleanup_word", true);
	this.settings['cleanup_on_startup'] = tinyMCE.getParam("cleanup_on_startup", false);
	this.settings['inline_styles'] = tinyMCE.getParam("inline_styles", false);

	// Setup baseHREF
	var baseHREF = tinyMCE.settings['document_base_url'];
	if (baseHREF.indexOf('?') != -1)
		baseHREF = baseHREF.substring(0, baseHREF.indexOf('?'));
	this.settings['base_href'] = baseHREF.substring(0, baseHREF.lastIndexOf('/')) + "/";

	theme = this.settings['theme'];

	if (!tinyMCE.isMSIE)
		this.settings['force_br_newlines'] = false;

	if (tinyMCE.getParam("content_css", false)) {
		var cssPath = tinyMCE.getParam("content_css", "");

		// Is relative
		if (cssPath.indexOf('://') == -1 && cssPath.charAt(0) != '/')
			this.settings['content_css'] = this.documentBasePath + "/" + cssPath;
		else
			this.settings['content_css'] = cssPath;
	} else
		this.settings['content_css'] = tinyMCE.baseURL + "/themes/" + theme + "/editor_content.css";

	if (tinyMCE.getParam("popups_css", false)) {
		var cssPath = tinyMCE.getParam("popups_css", "");

		// Is relative
		if (cssPath.indexOf('://') == -1 && cssPath.charAt(0) != '/')
			this.settings['popups_css'] = this.documentBasePath + "/" + cssPath;
		else
			this.settings['popups_css'] = cssPath;
	} else
		this.settings['popups_css'] = tinyMCE.baseURL + "/themes/" + theme + "/editor_popup.css";

	if (tinyMCE.getParam("editor_css", false)) {
		var cssPath = tinyMCE.getParam("editor_css", "");

		// Is relative
		if (cssPath.indexOf('://') == -1 && cssPath.charAt(0) != '/')
			this.settings['editor_css'] = this.documentBasePath + "/" + cssPath;
		else
			this.settings['editor_css'] = cssPath;
	} else
		this.settings['editor_css'] = tinyMCE.baseURL + "/themes/" + theme + "/editor_ui.css";

	this.settings['ask'] = tinyMCE.getParam("ask", false);

	if (tinyMCE.settings['debug']) {
		var msg = "Debug: \n";

		msg += "baseURL: " + this.baseURL + "\n";
		msg += "documentBasePath: " + this.documentBasePath + "\n";
		msg += "content_css: " + this.settings['content_css'] + "\n";
		msg += "popups_css: " + this.settings['popups_css'] + "\n";
		msg += "editor_css: " + this.settings['editor_css'] + "\n";

		alert(msg);
	}

	// Init HTML cleanup
	this._initCleanup();

	if (this.isMSIE)
		attachEvent("onload", TinyMCE_onLoad);
	else
		addEventListener("load", TinyMCE_onLoad, false);

	document.write('<sc'+'ript language="javascript" type="text/javascript" src="' + tinyMCE.baseURL + '/themes/' + this.settings['theme'] + '/editor_template.js"></script>');
	document.write('<sc'+'ript language="javascript" type="text/javascript" src="' + tinyMCE.baseURL + '/langs/' + this.settings['language'] +  '.js"></script>');
	document.write('<link href="' + this.settings['editor_css'] + '" rel="stylesheet" type="text/css">');

	// Add theme plugins
	var themePlugins = this.settings['theme_plugins'].split(',');
	if (this.settings['theme_plugins'] != '') {
		for (var i=0; i<themePlugins.length; i++)
			document.write('<sc'+'ript language="javascript" type="text/javascript" src="' + tinyMCE.baseURL + '/themes/' + themePlugins[i] + '/editor_plugin.js"></script>');
	}
}

function TinyMCE_confirmAdd(e) {
	if (tinyMCE.isMSIE)
		var targetElement = event.srcElement;
	else
		var targetElement = e.target;

	var elementId = targetElement.name ? targetElement.name : targetElement.id;

	if (!targetElement.getAttribute('mce_noask') && confirm(tinyMCELang['lang_edit_confirm']))
		tinyMCE.addMCEControl(targetElement, elementId, tinyMCE.createMCEControl(tinyMCE.settings));
	else
		targetElement.setAttribute('mce_noask', 'true');
}

function TinyMCE_updateContent(form_element_name) {
	// find MCE instance linked to given form element and copy it's value
	var formElement = document.getElementById(form_element_name);
	for (var instanceName in tinyMCE.instances) {
		var instance = tinyMCE.instances[instanceName];
		if (instance.formElement == formElement) {
			tinyMCE._setHTML(instance.contentWindow.document, instance.formElement.value);

			if (!tinyMCE.isMSIE)
				instance.contentWindow.document.body.innerHTML = tinyMCE._cleanupHTML(instance.contentWindow.document, this.settings, instance.contentWindow.document.body, instance.visualAid);
		}
	}
}

function TinyMCE_addMCEControl(replace_element, form_element_name, mce_control) {
	var editorId = "mce_editor_" + tinyMCE.idCounter++;
	mce_control.editorId = editorId;
	this.instances[editorId] = mce_control;
	mce_control.onAdd(replace_element, form_element_name);
}

function TinyMCE_createMCEControl(settings) {
	return new TinyMCEControl(settings);
}

function TinyMCE_triggerSave() {
	// * * Cleanup and set all form fields
	for (var instanceName in tinyMCE.instances) {
		var instance = tinyMCE.instances[instanceName];
		tinyMCE.settings['preformatted'] = false;

		tinyMCE._setHTML(instance.contentWindow.document, instance.contentWindow.document.body.innerHTML);
		var cleanedHTML = tinyMCE._cleanupHTML(instance.contentWindow.document, this.settings, instance.contentWindow.document.body, this.visualAid, true);

		//var cleanedHTML = tinyMCE._cleanupHTML(instance.contentWindow.document, tinyMCE.settings, instance.contentWindow.document.body, false, true);

		if (tinyMCE.settings["encoding"] == "xml" || tinyMCE.settings["encoding"] == "html")
			cleanedHTML = tinyMCE.convertStringToXML(cleanedHTML);

		if (tinyMCE.settings['save_callback'] != "")
			var content = eval(tinyMCE.settings['save_callback'] + "(instance.formTargetElementId,cleanedHTML,instance.contentWindow.document.body);");

		// Use callback content if available
		if ((typeof content != "undefined") && content != null)
			cleanedHTML = content;

		instance.formElement.value = cleanedHTML;
	}
}

function TinyMCE_execInstanceCommand(editor_id, command, user_interface, value) {
	var mceControl = tinyMCE._getInstanceById(editor_id);
	if (mceControl) {
		mceControl.contentWindow.focus();
		this.selectedElement = mceControl.getFocusElement();
		this.selectedInstance = mceControl;
		tinyMCE.execCommand(command, user_interface, value);
	}
}

function TinyMCE_execCommand(command, user_interface, value) {
	// Command within editor instance?
	if (this.selectedInstance && tinyMCE.isMSIE) {
		var node = this.selectedInstance.getFocusElement();
		while (node = node.parentNode) {
			if (node.nodeName == "#document" && (node.location.href.indexOf('blank.htm') == -1)) {
				this.selectedInstance = null;
				break;
			}
		}
	}

	// Default input
	user_interface = user_interface ? user_interface : false;
	value = value ? value : null;

	switch (command) {
		case 'mceHelp':
			window.open(tinyMCE.themeURL + "/docs/" + this.settings['docs_language'] + "/index.htm", "mceHelp", "menubar=yes,toolbar=yes,scrollbars=yes,left=20,top=20,width=550,height=600");
		return;

		case 'mceFocus':
			var mceControl = tinyMCE._getInstanceById(value);
			if (mceControl)
				mceControl.contentWindow.focus();
		return;

		case "mceAddControl":
		case "mceAddEditor":
			tinyMCE.addMCEControl(tinyMCE._getElementById(value), value, tinyMCE.createMCEControl(tinyMCE.settings));
			return;

		case "mceRemoveControl":
		case "mceRemoveEditor":
			tinyMCE.removeMCEControl(value);
			return;
	}

	if (this.selectedInstance)
		this.selectedInstance.execCommand(command, user_interface, value);
	else if (tinyMCE.settings['focus_alert'])
		alert(tinyMCELang['lang_focus_alert']);
}

function TinyMCE_addEventHandlers(editor_id) {
	var instance = tinyMCE.instances[editor_id];
	instance.contentWindow.document.editor_id = editor_id;
	instance.contentWindow.document.addEventListener("keypress", tinyMCE.handleEvent, false); 
	instance.contentWindow.document.addEventListener("keydown", tinyMCE.handleEvent, false); 
	instance.contentWindow.document.addEventListener("keyup", tinyMCE.handleEvent, false); 
	instance.contentWindow.document.addEventListener("click", tinyMCE.handleEvent, false); 
	instance.contentWindow.document.addEventListener("focus", tinyMCE.handleEvent, false);
	instance.contentWindow.document.designMode = "on";
}

function TinyMCE_setEditMode(editor_id, mode) {
	var targetMCEControl = this.instances[editor_id];

	if (!this.isMSIE) {
		var targetElement = document.getElementById(editor_id);

		targetMCEControl.targetElement = targetElement;
		targetMCEControl.contentDocument = targetElement.contentDocument;
		targetMCEControl.contentWindow = targetElement.contentWindow;

		//targetMCEControl.contentWindow.document.designMode = mode ? "on" : "off";
	} else {
		var targetElement = document.frames[editor_id];

		targetMCEControl.targetElement = targetElement;
		targetMCEControl.contentDocument = targetElement.window.document;
		targetMCEControl.contentWindow = targetElement.window;

		targetMCEControl.contentDocument.designMode = mode ? "on" : "off";
	}

	if (tinyMCE.isMSIE) {
		document.frames[editor_id].document.ondeactive = tinyMCE.handleEvent;

		var patchFunc = function() {
			var event = document.frames[editor_id].event;

			event.target = event.srcElement;
			event.target.editor_id = editor_id;

			TinyMCE_handleEvent(event);
		};

		// * * Event patch
		document.frames[editor_id].document.onkeypress = patchFunc;
		document.frames[editor_id].document.onkeyup = patchFunc;
		document.frames[editor_id].document.onkeydown = patchFunc;

		// Due to stange focus bug in MSIE this line is disabled for now
		//document.frames[editor_id].document.onmousedown = patchFunc;
		document.frames[editor_id].document.onmouseup = patchFunc;
		document.frames[editor_id].document.onclick = patchFunc;
	}

	//window.setTimeout("tinyMCE.setupContent('" + editor_id + "');", (tinyMCE.isMSIE ? 1 : 1000));
	tinyMCE.setupContent(editor_id);
}

function TinyMCE_setupContent(editor_id) {
	var instance = tinyMCE.instances[editor_id];
	var doc = instance.contentWindow.document;
	var head = doc.getElementsByTagName('head').item(0);

	// Not loaded correctly hit it again, Mozilla bug #997860
	if (!tinyMCE.isMSIE && doc.title != "blank_page") {
		// This part will remove the designMode status
		doc.location.href = tinyMCE.baseURL + "/blank.htm";
		window.setTimeout("tinyMCE.setupContent('" + editor_id + "');", 1000);
		return;
	}

	if (!head) {
		window.setTimeout("tinyMCE.setupContent('" + editor_id + "');", 10);
		return;
	}

	doc.body.dir = this.settings['directionality'];
	doc.editorId = editor_id;

	// Setup base element
	base = doc.createElement("base");
	base.setAttribute('href', tinyMCE.settings['base_href']);
	head.appendChild(base);

	var oldElement = instance.oldTargetElement;
	if (oldElement.nodeName.toLowerCase() == "textarea")
		var content = oldElement.value;
	else
		var content = instance.oldTargetElement.innerHTML;

	if (tinyMCE.isMSIE) {
		var styleSheet = document.frames[editor_id].document.createStyleSheet(instance.settings['content_css']);

		window.setInterval('tinyMCE.getCSSClasses(document.frames["' + editor_id + '"].document, "' + editor_id + '");', 500);

		if (tinyMCE.settings["force_br_newlines"])
			document.frames[editor_id].document.styleSheets[0].addRule("p", "margin: 0px;");

		var patchFunc = function() {
			var event = document.frames[editor_id].event;

			event.target = document.frames[editor_id].document;

			TinyMCE_handleEvent(event);
		};

		document.frames[editor_id].document.body.onblur = patchFunc;
		document.frames[editor_id].document.body.onbeforepaste = patchFunc;
		document.frames[editor_id].document.body.onbeforecut = patchFunc;
		document.frames[editor_id].document.body.onpaste = patchFunc;

		document.frames[editor_id].document.body.editorId = editor_id;
	} else {
		var targetDocument = document.getElementById(editor_id).contentWindow.document;

		// * * Import editor css
		var cssImporter = targetDocument.createElement("link");
		cssImporter.rel = "stylesheet";
		cssImporter.href = instance.settings['content_css'];
		if (headArr = targetDocument.getElementsByTagName("head"));
			headArr[0].appendChild(cssImporter);
	}

	// Fix for bug #958637
	if (!tinyMCE.isMSIE) {
		if (tinyMCE.settings['cleanup_on_startup']) {
			var contentElement = instance.contentWindow.document.createElement("body");
			contentElement.innerHTML = content;
			instance.contentWindow.document.body.innerHTML = tinyMCE._cleanupHTML(instance.contentWindow.document, this.settings, contentElement);
		} else {
			var contentElement = instance.contentWindow.document.createElement("body");
			contentElement.innerHTML = content;

			// Convert img src nodes
			var elms = contentElement.getElementsByTagName("img");
			for (var i=0; i<elms.length; i++) {
				var src = elms[i].getAttribute('src');
				if (src && src != "") {
					src = eval(tinyMCE.cleanup_urlconvertor_callback + "(src, elms[i], tinyMCE.cleanup_on_save);");
					elms[i].setAttribute("src", src);
					elms[i].setAttribute("mce_real_src", src);
				}
			}

			// Convert anchor hrefs
			elms = contentElement.getElementsByTagName("a");
			for (var i=0; i<elms.length; i++) {
				var href = elms[i].getAttribute('href');
				if (href && href != "") {
					href = eval(tinyMCE.cleanup_urlconvertor_callback + "(href, elms[i], tinyMCE.cleanup_on_save);");
					elms[i].setAttribute("href", href);
					elms[i].setAttribute("mce_real_href", href);
				}
			}

			instance.contentWindow.document.body.innerHTML = contentElement.innerHTML;
		}
	} else {
		if (tinyMCE.settings['cleanup_on_startup']) {
			tinyMCE._setHTML(instance.contentWindow.document, content);
			// Produces permission denied error in MSIE 5.5
			eval('try {instance.contentWindow.document.body.innerHTML = tinyMCE._cleanupHTML(instance.contentDocument, this.settings, instance.contentDocument.body);} catch(e) {}');
		} else
			instance.contentWindow.document.body.innerHTML = content;
	}

	// Fix for bug #957681
	//instance.contentWindow.document.designMode = instance.contentWindow.document.designMode;

	// Setup element references
	var parentElm = document.getElementById(instance.editorId + '_parent');
	instance.formElement = parentElm.lastChild;

	if (tinyMCE.settings['handleNodeChangeCallback']) {
		var undoIndex = -1;
		var undoLevels = -1;

		if (tinyMCE.settings['custom_undo_redo']) {
			undoIndex = 0;
			undoLevels = 0;
		}

		eval(tinyMCE.settings['handleNodeChangeCallback'] + '("' + editor_id + '", tinyMCE.instances["' + editor_id + '"].contentWindow.document.body,undoIndex,undoLevels,tinyMCE.instances["' + editor_id + '"].visualAid);');
		//window.setTimeout(tinyMCE.settings['handleNodeChangeCallback'] + '("' + editor_id + '", tinyMCE.instances["' + editor_id + '"].contentWindow.document.body,undoIndex,undoLevels);', 10);
	}

	tinyMCE.handleVisualAid(instance.contentWindow.document.body, true, tinyMCE.settings['visual']);

	// Trigger setup content
	if (tinyMCE.settings['setupcontent_callback'] != "")
		eval(tinyMCE.settings['setupcontent_callback'] + '(editor_id,instance.contentWindow.document.body);');

	// Re-add design mode on mozilla
	if (!tinyMCE.isMSIE) {
		TinyMCE_addEventHandlers(editor_id);
	}
}

function TinyMCE_handleEvent(e) {
	//window.status = e.type + " " + e.target.nodeName + " " + (e.relatedTarget ? e.relatedTarget.nodeName : "");

	switch (e.type) {
		case "paste":
			if (tinyMCE.settings['auto_cleanup_word'])
				window.setTimeout("tinyMCE.execInstanceCommand('" + e.target.editorId + "', 'mceCleanupWord', false, null);", 1);
			break;

		case "beforecut":
		case "beforepaste":
			tinyMCE.selectedInstance.execCommand("mceAddUndoLevel");
			break;

		case "keypress":
			if (e.target.editorId) {
				tinyMCE.selectedInstance = tinyMCE.instances[e.target.editorId];
			} else {
				if (e.target.ownerDocument.editorId)
					tinyMCE.selectedInstance = tinyMCE.instances[e.target.ownerDocument.editorId];
			}

			// Mozilla custom key handling
			if (!tinyMCE.isMSIE && e.ctrlKey && tinyMCE.settings['custom_undo_redo']) {
				if (e.charCode == 120 || e.charCode == 118) { // Ctrl+X, Ctrl+V
					tinyMCE.selectedInstance.execCommand("mceAddUndoLevel");
					return;
				}

				if (e.charCode == 122) { // Ctrl+Z
					tinyMCE.selectedInstance.execCommand("Undo");

					// Cancel event
					e.preventDefault();
					return false;
				}

				if (e.charCode == 121) { // Ctrl+Y
					tinyMCE.selectedInstance.execCommand("Redo");

					// Cancel event
					e.preventDefault();
					return false;
				}
			}

			if (tinyMCE.settings['custom_undo_redo']) {
				// Check if it's a position key press
				var keys = new Array(13,45,36,35,33,34,37,38,39,40);
				var posKey = false;
				for (var i=0; i<keys.length; i++) {
					if (keys[i] == e.keyCode) {
						tinyMCE.selectedInstance.typing = false;
						posKey = true;
						break;
					}
				}

				// Add typing undo level
				if (!tinyMCE.selectedInstance.typing && !posKey) {
					tinyMCE.selectedInstance.execCommand("mceAddUndoLevel");
					tinyMCE.selectedInstance.typing = true;
				}
			}

			//window.status = e.keyCode;
			//window.status = e.type + " " + e.target.nodeName;

			// Return key pressed
			if (tinyMCE.isMSIE && tinyMCE.settings['force_br_newlines'] && e.keyCode == 13) {
				if (e.target.editorId)
					tinyMCE.selectedInstance = tinyMCE.instances[e.target.editorId];

				if (tinyMCE.selectedInstance) {
					var sel = tinyMCE.selectedInstance.contentWindow.document.selection;
					var rng = sel.createRange();

					if (tinyMCE.getParentElement(rng.parentElement(), "li") != null)
						return false;

					if (tinyMCE.getParentElement(rng.parentElement(), "div") == null)
						return false;

					// Cancel event
					e.returnValue = false;
					e.cancelBubble = true;

					// Insert BR element
					rng.pasteHTML("<br>");
					rng.collapse(false);
					rng.select();
				}
			}

			return false;
		break;

		case "keyup":
		case "keydown":
			if (e.target.editorId)
				tinyMCE.selectedInstance = tinyMCE.instances[e.target.editorId];
			else
				return;

			tinyMCE.selectedElement = null;
			var elm = tinyMCE.selectedInstance.getFocusElement();
			tinyMCE.linkElement = tinyMCE.getParentElement(elm, "a");
			tinyMCE.imgElement = tinyMCE.getParentElement(elm, "img");
			tinyMCE.selectedElement = elm;

			tinyMCE.triggerNodeChange(false);

			// MSIE custom key handling
			if (tinyMCE.isMSIE && tinyMCE.settings['custom_undo_redo']) {
				// Check if it's a position key press
				var keys = new Array(13,45,36,35,33,34,37,38,39,40);
				var posKey = false;
				for (var i=0; i<keys.length; i++) {
					if (keys[i] == e.keyCode) {
						tinyMCE.selectedInstance.typing = false;
						posKey = true;
						break;
					}
				}

				// Add typing undo level (unless pos keys or shift, alt, ctrl, capslock)
				if (!tinyMCE.selectedInstance.typing && !posKey && (e.keyCode < 16 || e.keyCode > 18 && e.keyCode != 255)) {
					tinyMCE.selectedInstance.execCommand("mceAddUndoLevel");
					tinyMCE.selectedInstance.typing = true;
				}

				var ctrlKeys = new Array(66,73,85,86,88); // B/I/U/V/X
				var keys = new Array(8,46); // Backspace,Delete
				for (var i=0; i<keys.length; i++) {
					if ((ctrlKeys[i] == e.keyCode && e.ctrlKey) || keys[i] == e.keyCode) {
						tinyMCE.selectedInstance.execCommand("mceAddUndoLevel");
						return;
					}
				}

				if (e.keyCode == 90 && e.ctrlKey && e.type == "keydown") { // Ctrl+Z
					tinyMCE.selectedInstance.execCommand("Undo");

					// Cancel event
					e.returnValue = false;
					e.cancelBubble = true;
					return false;
				}

				if (e.keyCode == 89 && e.ctrlKey && e.type == "keydown") { // Ctrl+Y
					tinyMCE.selectedInstance.execCommand("Redo");

					// Cancel event
					e.returnValue = false;
					e.cancelBubble = true;
					return false;
				}
			}
		break;

		case "mousedown":
		case "mouseup":
		case "click":
		case "focus":
			// Check instance event trigged on
			var targetBody = tinyMCE.getParentElement(e.target, "body");
			for (var instanceName in tinyMCE.instances) {
				var instance = tinyMCE.instances[instanceName];
				if (instance.contentWindow.document.body == targetBody) {
					tinyMCE.selectedInstance = instance;
					tinyMCE.selectedElement = e.target;
					tinyMCE.linkElement = tinyMCE.getParentElement(tinyMCE.selectedElement, "a");
					tinyMCE.imgElement = tinyMCE.getParentElement(tinyMCE.selectedElement, "img");

					// Reset typing
					tinyMCE.selectedInstance.typing = false;
					tinyMCE.triggerNodeChange(false);
					break;
				}
			}

			// Just in case
			if (!tinyMCE.selectedInstance && e.target.editorId)
				tinyMCE.selectedInstance = tinyMCE.instances[e.target.editorId];
		break;
	}
}

function TinyMCE_switchClass(element, class_name, lock_state) {
	var lockChanged = false;

	if (typeof lock_state != "undefined" && element != null) {
		element.classLock = lock_state;
		lockChanged = true;
	}

	if (element != null && (lockChanged || !element.classLock)) {
		element.oldClassName = element.className;
		element.className = class_name;
	}
}

function TinyMCE_restoreAndSwitchClass(element, class_name) {
	if (element != null && !element.classLock) {
		this.restoreClass(element);
		this.switchClass(element, class_name);
	}
}

function TinyMCE_switchClassSticky(element_name, class_name, lock_state) {
	var element, lockChanged = false;

	// Performance issue
	if (!this.stickyClassesLookup[element_name])
		this.stickyClassesLookup[element_name] = document.getElementById(element_name);

//	element = document.getElementById(element_name);
	element = this.stickyClassesLookup[element_name];

	if (typeof lock_state != "undefined" && element != null) {
		element.classLock = lock_state;
		lockChanged = true;
	}

	if (element != null && (lockChanged || !element.classLock)) {
		element.className = class_name;
		element.oldClassName = class_name;
	}
}

function TinyMCE_restoreClass(element) {
	if (element != null && element.oldClassName && !element.classLock) {
		element.className = element.oldClassName;
		element.oldClassName = null;
	}
}

function TinyMCE_setClassLock(element, lock_state) {
	if (element != null)
		element.classLock = lock_state;
}

function TinyMCE_onLoad() {
	// Add submit triggers
	if (tinyMCE.isMSIE) {
		if (document.forms && tinyMCE.settings['add_form_submit_trigger']) {
			for (var i=0; i<document.forms.length; i++)
				document.forms[i].attachEvent("onsubmit", TinyMCE_triggerSave);
		}
	} else {
		if (document.forms && tinyMCE.settings['add_form_submit_trigger']) {
			for (var i=0; i<document.forms.length; i++)
				document.forms[i].addEventListener("submit", TinyMCE_triggerSave, false);
		}
	}

	// Add editor instances based on mode
	switch (tinyMCE.settings['mode']) {
		case "exact":
			var elements = tinyMCE.settings['elements'].split(',');

			for (var i=0; i<elements.length; i++) {
				var element = tinyMCE._getElementById(elements[i]);

				if (element)
					tinyMCE.addMCEControl(element, elements[i], tinyMCE.createMCEControl(tinyMCE.settings));
				else
					alert("Error: Could not find element by id or name: " + elements[i]);
			}
		break;

		case "specific_textareas":
		case "textareas":
			var nodeList = document.getElementsByTagName("textarea");
			var elementRefAr = new Array();

			for (var i=0; i<nodeList.length; i++) {
				if (tinyMCE.settings['mode'] != "specific_textareas" || nodeList.item(i).getAttribute(tinyMCE.settings['textarea_trigger']) == "true")
					elementRefAr[elementRefAr.length] = nodeList.item(i);
			}

			for (var i=0; i<elementRefAr.length; i++) {
				var element = elementRefAr[i];
				var elementId = element.name ? element.name : element.id;

				if (tinyMCE.settings['ask']) {
					if (tinyMCE.isMSIE)
						element.attachEvent("onmousedown", TinyMCE_confirmAdd);
					else
						element.addEventListener("mousedown", TinyMCE_confirmAdd, false);
				} else
					tinyMCE.addMCEControl(element, elementId, tinyMCE.createMCEControl(tinyMCE.settings));
			}
		break;
	}
}

function TinyMCE_removeMCEControl(editor_id) {
	var mceControl = tinyMCE._getInstanceById(editor_id);
	if (mceControl) {
		editor_id = mceControl.editorId;
		var html = tinyMCE.getContent(editor_id);

		// Remove editor instance from instances array
		var tmpInstances = new Array();
		for (var instanceName in tinyMCE.instances) {
			var instance = tinyMCE.instances[instanceName];
			if (instanceName != editor_id)
					tmpInstances[instanceName] = instance;
		}
		tinyMCE.instances = tmpInstances;

		tinyMCE.selectedElement = null;
		tinyMCE.selectedInstance = null;

		// Remove element
		var replaceElement = document.getElementById(editor_id + "_parent");
		var oldTargetElement = mceControl.oldTargetElement;
		var targetName = oldTargetElement.nodeName.toLowerCase();
		if (targetName == "textarea" || targetName == "input")
			oldTargetElement.value = html;
		else
			oldTargetElement.innerHTML = html;

		replaceElement.parentNode.insertBefore(oldTargetElement, replaceElement);
		replaceElement.parentNode.removeChild(replaceElement);
	}
}

function TinyMCE__cleanupElementName(element_name, element) {
	element_name = element_name.toLowerCase();

	// Never include body
	if (element_name == "body")
		return null;

	// If verification mode
	if (tinyMCE.cleanup_verify_html) {
		// Check if invalid element
		for (var i=0; i<tinyMCE.cleanup_invalidElements.length; i++) {
			if (tinyMCE.cleanup_invalidElements[i] == element_name)
				return null;
		}

		// Check if valid element
		var validElement = false;
		var elementAttribs = null;
		for (var i=0; i<tinyMCE.cleanup_validElements.length && !elementAttribs; i++) {
			for (var x=0; x<tinyMCE.cleanup_validElements[i][0].length; x++) {
				var elmMatch = tinyMCE.cleanup_validElements[i][0][x];

				// Handle wildcard/regexp
				if (elmMatch.match(new RegExp('\\*|\\?|\\+', 'g')) != null) {
					elmMatch = elmMatch.replace(new RegExp('\\?', 'g'), '(\\S?)');
					elmMatch = elmMatch.replace(new RegExp('\\+', 'g'), '(\\S+)');
					elmMatch = elmMatch.replace(new RegExp('\\*', 'g'), '(\\S*)');
					elmMatch = "^" + elmMatch + "$";
					if (element_name.match(new RegExp(elmMatch, 'g'))) {
						elementAttribs = tinyMCE.cleanup_validElements[i];
						validElement = true;
						break;
					}
				}

				// Handle non regexp
				if (element_name == elmMatch) {
					elementAttribs = tinyMCE.cleanup_validElements[i];
					validElement = true;
					element_name = elementAttribs[0][0];
					break;
				}
			}
		}

		if (!validElement)
			return null;
	}

	// Special Mozilla stuff
	if (!tinyMCE.isMSIE) {
		// Fix for bug #958498
		if (element_name == "strong" && !tinyMCE.cleanup_on_save)
			element_name = "b";
		else if (element_name == "em" && !tinyMCE.cleanup_on_save)
			element_name = "i";
	}

	// Special MSIE stuff
	if (tinyMCE.isMSIE) {
		if (element_name == "table") {
			var attribValue = element.style.pixelWidth == 0 ? element.getAttribute("width") : element.style.pixelWidth;
			element.setAttribute("width", attribValue);

			attribValue = element.style.pixelHeight == 0 ? element.getAttribute("height") : element.style.pixelHeight;
			element.setAttribute("height", attribValue);
		}
	}

	var elmData = new Object();

	elmData.element_name = element_name;
	elmData.valid_attribs = elementAttribs;

	return elmData;
}

/**
 * Converts some element attributes to inline styles.
 */
function TinyMCE__fixInlineStyles(elm) {
	// Handle non table elements
	if (elm.nodeName.toLowerCase() != "table") {
		var value;

		// Setup width
		value = tinyMCE.isMSIE ? elm.width : elm.getAttribute("width");
		if (value && value != "") {
			if (typeof (value) != "string" || !value.indexOf("%"))
				value += "px";

			elm.style.width = value;
		}

		// Setup height
		value = tinyMCE.isMSIE ? elm.height : elm.getAttribute("height");
		if (value && value != "") {
			if (typeof (value) != "string" || !value.indexOf("%"))
				value += "px";

			elm.style.height = value;
		}

		// Setup border
		value = tinyMCE.isMSIE ? elm.border : elm.getAttribute("border");
		if (value && value != "")
			elm.style.border = "solid " + value + "px";
	}

	// Setup align
	value = elm.getAttribute("align");
	if (value && value != "")
		elm.style.textAlign = value;

	// Setup vspace
	value = elm.getAttribute("vspace");
	if (value && value != "")
		elm.style.marginTop = value + "px";

	// Setup hspace
	value = elm.getAttribute("hspace");
	if (value && value != "")
		elm.style.marginBottom = value + "px";
}

function TinyMCE__cleanupAttribute(valid_attributes, element_name, attribute_node, element_node) {
	var attribName = attribute_node.nodeName.toLowerCase();
	var attribValue = attribute_node.nodeValue;
	var attribMustBeValue = null;

	// Inline styling
	if (tinyMCE.cleanup_inline_styles && ((element_name != "table" && (attribName == "width" || attribName == "height" || attribName == "border")) || attribName == "align" || attribName == "valign" || attribName == "hspace"))
		return null;

	// Mozilla attibute, remove them
	if (attribName.indexOf('moz_') != -1)
		return null;

	// Mozilla fix for drag-drop/copy/paste images
	if (!tinyMCE.isMSIE && (attribName == "mce_real_href" || attribName == "mce_real_src")) {
		if (!tinyMCE.cleanup_on_save) {
			var attrib = new Object();

			attrib.name = attribName;
			attrib.value = attribValue;

			return attrib;
		} else
			return null;
	}

	// Verify attrib
	if (tinyMCE.cleanup_verify_html) {
		var verified = false;
		for (var i=1; i<valid_attributes.length; i++) {
			var attribMatch = valid_attributes[i][0];
			var re = null;

			// Build regexp from wildcard
			if (attribMatch.match(new RegExp('\\*|\\?|\\+', 'g')) != null) {
				attribMatch = attribMatch.replace(new RegExp('\\?', 'g'), '(\\S?)');
				attribMatch = attribMatch.replace(new RegExp('\\+', 'g'), '(\\S+)');
				attribMatch = attribMatch.replace(new RegExp('\\*', 'g'), '(\\S*)');
				attribMatch = "^" + attribMatch + "$";
				re = new RegExp(attribMatch, 'g');
			}

			if ((re && attribName.match(re) != null) || attribName == attribMatch) {
				verified = true;
				attribMustBeValue = valid_attributes[i][3];
				break;
			}
		}

		// Allways pass styles on table and td elements if visual_aid
		if (!verified && (attribName != "style" && element_name != "table" && element_name != "td"))
			return false;
	} else
		verified = true;

	// Treat some attribs diffrent
	switch (attribName) {
		case "width":
			// MSIE 5.5 issue
			if (tinyMCE.isMSIE)
				attribValue = element_node.width;
			break;

		case "height":
			// MSIE 5.5 issue
			if (tinyMCE.isMSIE)
				attribValue = element_node.height;
			break;

		case "border":
			// MSIE 5.5 issue
			if (tinyMCE.isMSIE)
				attribValue = element_node.border;
			break;

		case "class":
			if (!tinyMCE._verifyClass(element_node))
				return null;
			break;

		case "style":
			if (element_name == "table" || element_name == "td") {
				// Handle visual aid
				if (tinyMCE.cleanup_visual_table_style != "") {
					// Find parent table
					var tableElement = element_node;
					if (element_name == "td")
						tableElement = tinyMCE.getParentElement(tableElement, "table");

					if (tableElement && tableElement.getAttribute("border") == 0) {
						if (tinyMCE.cleanup_visual_aid)
							attribValue = tinyMCE.cleanup_visual_table_style;
						else
							return null;
					}
				}
			} else
				attribValue = element_node.style.cssText.toLowerCase();

			// Compress borders some
			if (tinyMCE.isMSIE) {
				var border = element_node.style.border;
				var bt = element_node.style.borderTop;
				var bl = element_node.style.borderLeft;
				var br = element_node.style.borderRight;
				var bb = element_node.style.borderBottom;

				// All the same
				if (border != "" && (bt == border && bl == border && br == border && bb == border)) {
					attribValue = tinyMCE.regexpReplace(attribValue, 'border-top: ' + border + '?; ?', '');
					attribValue = tinyMCE.regexpReplace(attribValue, 'border-left: ' + border  + '?; ?', '');
					attribValue = tinyMCE.regexpReplace(attribValue, 'border-right: ' + border  + '?; ?', '');
					attribValue = tinyMCE.regexpReplace(attribValue, 'border-bottom: ' + border + '?;( ?)', 'border: ' + border + ';$1');
				}
			}
			break;

		// Convert the URLs of these
		case "href":
		case "src":
		case "mceoversrc":
			// Fix for dragdrop/copy paste Mozilla issue
			if (!tinyMCE.isMSIE && attribName == "href" && element_node.getAttribute("mce_real_href"))
				attribValue = element_node.getAttribute("mce_real_href");

			// Fix for dragdrop/copy paste Mozilla issue
			if (!tinyMCE.isMSIE && attribName == "src" && element_node.getAttribute("mce_real_src"))
				attribValue = element_node.getAttribute("mce_real_src");

			attribValue = eval(tinyMCE.cleanup_urlconvertor_callback + "(attribValue, element_node, tinyMCE.cleanup_on_save);");
			break;

		case "colspan":
		case "rowspan":
			// Not needed
			if (attribValue == "1")
				return null;
			break;

		// Skip these
		case "_moz-userdefined":
		case "editorid":
		case "editor_id":
		case "mce_real_href":
		case "mce_real_src":
			return null;
	}

	// Not the must be value
	if (attribMustBeValue != null) {
		var isCorrect = false;
		for (var i=0; i<attribMustBeValue.length; i++) {
			if (attribValue == attribMustBeValue[i]) {
				isCorrect = true;
				break;
			}
		}

		if (!isCorrect)
			return null;
	}

	var attrib = new Object();

	attrib.name = attribName;
	attrib.value = attribValue;

	return attrib;
}

function TinyMCE__verifyClass(node) {
	var className = node.getAttribute('class');

	// Trim CSS class
	if (tinyMCE.isMSIE)
		var className = node.getAttribute('className');

	if (tinyMCE.cleanup_verify_css_classes && tinyMCE.cleanup_on_save) {
		var csses = tinyMCE.getCSSClasses();
		nonDefinedCSS = true;
		for (var c=0; c<csses.length; c++) {
			if (csses[c] == className) {
				nonDefinedCSS = false;
				break;
			}
		}

		if (nonDefinedCSS) {
			node.removeAttribute('className');
			node.removeAttribute('class');
			return false;
		}
	}

	return true;
}

function TinyMCE_cleanupNode(node) {
	var output = "";

	switch (node.nodeType) {
		case 1: // Element
			var elementData = tinyMCE._cleanupElementName(node.nodeName, node);
			var elementName = elementData ? elementData.element_name : null;
			var elementValidAttribs = elementData ? elementData.valid_attribs : null;
			var elementAttribs = "";

			// Element not valid (only render children)
			if (!elementName) {
				if (node.hasChildNodes()) {
					for (var i=0; i<node.childNodes.length; i++)
						output += this.cleanupNode(node.childNodes[i]);
				}

				return output;
			}

			// Has mso/microsuck crap or empty attrib
			if (node.style && (node.style.cssText.indexOf('mso-') != -1 && tinyMCE.settings['auto_cleanup_word']) || node.style.cssText == "") {
				node.style.cssText = "";
				node.removeAttribute("style");
			}

			// Handle inline styles
			if (tinyMCE.cleanup_inline_styles)
				tinyMCE._fixInlineStyles(node);

			// Set attrib data
			if (elementValidAttribs) {
				for (var a=1; a<elementValidAttribs.length; a++) {
					var attribName, attribDefaultValue, attribForceValue, attribValue;

					attribName = elementValidAttribs[a][0];
					attribDefaultValue = elementValidAttribs[a][1];
					attribForceValue = elementValidAttribs[a][2];

					if (attribDefaultValue || attribForceValue) {
						var attribValue = node.getAttribute(attribName);
						if (node.getAttribute(attribName) == "")
							attribValue = attribDefaultValue;

						attribValue = attribForceValue ? attribForceValue : attribValue;

						// Is to generate id
						if (attribValue == "{$uid}")
							attribValue = "uid_" + (tinyMCE.cleanup_idCount++);

						node.setAttribute(attribName, attribValue);
					}
				}
			}

			// Remove non needed span elements
			if (elementName == "span" && tinyMCE.cleanup_trim_span_elements) {
				var re = new RegExp('^[ \t]+', 'g');
				var onlyWhiteSpace = true;
				for (var a=0; a<node.childNodes.length; a++) {
					var tmpNode = node.childNodes[a];
					if ((tmpNode.nodeType == 3 && !tmpNode.nodeValue.match(re)) || tmpNode.nodeName.toLowerCase() != "span") {
						onlyWhiteSpace = false;
						break;
					}
				}

				// Count attributes
				tinyMCE._verifyClass(node);
				var numAttribs = 0;
				for (var i=0; i<node.attributes.length; i++) {
					if (node.attributes[i].specified)
						numAttribs++;
				}

				// Is not a valid span, remove it
				if (onlyWhiteSpace || numAttribs == 0) {
					if (node.hasChildNodes()) {
						for (var i=0; i<node.childNodes.length; i++)
							output += this.cleanupNode(node.childNodes[i]);
					}

					return output;
				}
			}

			// Add some visual aids
			if (elementName == "table" || elementName == "td") {
				// Handle visual aid
				if (tinyMCE.cleanup_visual_table_style != "") {
					// Find parent table
					var tableElement = node;
					if (elementName == "td")
						tableElement = tinyMCE.getParentElement(tableElement, "table");

					if (tableElement && tableElement.getAttribute("border") == 0) {
						if (tinyMCE.cleanup_visual_aid)
							elementAttribs += " style=" + '"' + tinyMCE.cleanup_visual_table_style + '"';
					}
				}
			}

			// Handle element attributes
			if (node.attributes.length > 0) {
				for (var i=0; i<node.attributes.length; i++) {
					if (node.attributes[i].specified) {
						var attrib = tinyMCE._cleanupAttribute(elementValidAttribs, elementName, node.attributes[i], node);
						if (attrib)
							elementAttribs += " " + attrib.name + "=" + '"' + attrib.value + '"';
					}
				}
			}

			// Add nbsp to some elements
			if ((elementName == "p" || elementName == "td") && (node.innerHTML == "" || node.innerHTML == "&nbsp;"))
				return "<" + elementName + elementAttribs + ">&nbsp;</" + elementName + ">";

			// Is MSIE script element
			if (tinyMCE.isMSIE && elementName == "script")
				return "<" + elementName + elementAttribs + ">" + node.text + "</" + elementName + ">";

			// Clean up children
			if (node.hasChildNodes()) {
				// Force BR
				if (elementName == "p" && tinyMCE.cleanup_force_br_newlines)
					output += "<div" + elementAttribs + ">";
				else
					output += "<" + elementName + elementAttribs + ">";

				for (var i=0; i<node.childNodes.length; i++)
					output += this.cleanupNode(node.childNodes[i]);

				// Force BR
				if (elementName == "p" && tinyMCE.cleanup_force_br_newlines)
					output += "</div><br />";
				else
					output += "</" + elementName + ">";
			} else // No children
				output += "<" + elementName + elementAttribs + " />";

			return output;

		case 3: // Text
			// Do not convert script elements
			if (node.parentNode.nodeName.toLowerCase() == "script")
				return node.nodeValue;

			return this.convertStringToXML(node.nodeValue);

		case 8: // Comment
			return "<!--" + node.nodeValue + "-->";

		default: // Unknown
			return "[UNKNOWN NODETYPE " + node.nodeType + "]";
	}
}

function TinyMCE_convertStringToXML(html_data) {
	var output = "";

	if (!html_data)
		return null;

	for (var i=0; i<html_data.length; i++) {
		var chr = html_data.charAt(i);

		// * * Check and convert to XML format
		switch (chr) {
			case ''+String.fromCharCode(8482):
				output += "&#x2122;";
			break;

			case ''+String.fromCharCode(8211):
				output += "-";
			break;

			case '\u0093':
			case '\u0094':
			case ''+String.fromCharCode(8220):
			case ''+String.fromCharCode(8221):
			case '"':
				output += "&quot;";
			break;

			case ''+String.fromCharCode(8217):
			case ''+String.fromCharCode(180):
			case '\'':
				output += "&#39;";
			break;

			case '<':
				output += "&lt;";
			break;

			case '>':
				output += "&gt;";
			break;

			case '&':
				output += "&amp;";
			break;

			case ''+String.fromCharCode(8230):
				output += "...";
			break;

			case '\\':
				output += "&#92;";
			break;

			case ''+String.fromCharCode(160):
				output += "&#160;";
			break;

			default:
				output += chr;
		}
	}

	return output;
}

function TinyMCE__initCleanup() {
	function getElementName(chunk) {
		var pos;

		if ((pos = chunk.indexOf('/')) != -1)
			chunk = chunk.substring(0, pos);

		if ((pos = chunk.indexOf('[')) != -1)
			chunk = chunk.substring(0, pos);

		return chunk;
	}

	// Parse valid elements and attributes
	var validElements = tinyMCE.settings["valid_elements"];
	validElements = validElements.split(',');

	// Handle extended valid elements
	var extendedValidElements = tinyMCE.settings["extended_valid_elements"];
	extendedValidElements = extendedValidElements.split(',');
	for (var i=0; i<extendedValidElements.length; i++) {
		var elementName = getElementName(extendedValidElements[i]);
		var skipAdd = false;

		// Check if it's defined before, if so override that one
		for (var x=0; x<validElements.length; x++) {
			if (getElementName(validElements[x]) == elementName) {
				validElements[x] = extendedValidElements[i];
				skipAdd = true;
				break;
			}
		}

		if (!skipAdd)
			validElements[validElements.length] = extendedValidElements[i];
	}

	for (var i=0; i<validElements.length; i++) {
		var item = validElements[i];
		item = item.replace('[','|');
		item = item.replace(']','');

		// * * Split and convert
		var attribs = item.split('|');
		for (var x=0; x<attribs.length; x++)
			attribs[x] = attribs[x].toLowerCase();

		// * * Handle change elements
		attribs[0] = attribs[0].split('/');

		// * * Handle default attribute values
		for (var x=1; x<attribs.length; x++) {
			var attribName = attribs[x];
			var attribDefault = null;
			var attribForce = null;
			var attribMustBe = null;

			// Default value
			if ((pos = attribName.indexOf('=')) != -1) {
				attribDefault = attribName.substring(pos+1);
				attribName = attribName.substring(0, pos);
			}

			// Force check
			if ((pos = attribName.indexOf(':')) != -1) {
				attribForce = attribName.substring(pos+1);
				attribName = attribName.substring(0, pos);
			}

			// Force check
			if ((pos = attribName.indexOf('<')) != -1) {
				attribMustBe = attribName.substring(pos+1).split('?');
				attribName = attribName.substring(0, pos);
			}

			attribs[x] = new Array(attribName, attribDefault, attribForce, attribMustBe);
		}

		validElements[i] = attribs;
	}

	var invalidElements = tinyMCE.settings['invalid_elements'].split(',');
	for (var i=0; i<invalidElements.length; i++)
		invalidElements[i] = invalidElements[i].toLowerCase();

	// Set these for performance
	tinyMCE.cleanup_validElements = validElements;
	tinyMCE.cleanup_invalidElements = invalidElements;
	tinyMCE.cleanup_verify_html = tinyMCE.settings['verify_html'];
	tinyMCE.cleanup_force_br_newlines = tinyMCE.settings['force_br_newlines'];
	tinyMCE.cleanup_urlconvertor_callback = tinyMCE.settings['urlconvertor_callback'];
	tinyMCE.cleanup_verify_css_classes = tinyMCE.settings['verify_css_classes'];
	tinyMCE.cleanup_visual_table_style = tinyMCE.settings['visual_table_style'];
	tinyMCE.cleanup_apply_source_formatting = tinyMCE.settings['apply_source_formatting'];
	tinyMCE.cleanup_urlconvertor_callback = tinyMCE.settings['urlconvertor_callback'];
	tinyMCE.cleanup_trim_span_elements = tinyMCE.settings['trim_span_elements'];
	tinyMCE.cleanup_inline_styles = tinyMCE.settings['inline_styles'];
}

function TinyMCE__cleanupHTML(doc, config, element, visual, on_save) {
	// Set these for performance
	tinyMCE.cleanup_visual_aid = visual;
	tinyMCE.cleanup_on_save = on_save;
	tinyMCE.cleanup_idCount = 0;

	var startTime = new Date().getTime();
	var html = this.cleanupNode(element);

	if (tinyMCE.settings['debug'])
		alert("Cleanup process executed in: " + (new Date().getTime()-startTime) + " ms.");

	// Remove pesky HR paragraphs
	html = tinyMCE.regexpReplace(html, '<p><hr /></p>', '<hr />');
	html = tinyMCE.regexpReplace(html, '<p>&nbsp;</p><hr /><p>&nbsp;</p>', '<hr />');

	// Remove some mozilla crap
	if (!tinyMCE.isMSIE) {
		html = html.replace(new RegExp('<o:p _moz-userdefined="" />', 'g'), "");
	}

	if (tinyMCE.settings['apply_source_formatting']) {
		html = html.replace(new RegExp('<(p|div)([^>]*)>', 'g'), "\n<$1$2>\n");
		html = html.replace(new RegExp('<\/(p|div)([^>]*)>', 'g'), "\n</$1$2>\n");
		html = html.replace(new RegExp('<br />', 'g'), "<br />\n");
	}

	if (tinyMCE.settings['force_br_newlines']) {
		var re = new RegExp('<p>&nbsp;</p>', 'g');
		html = html.replace(re, "<br />");
	}

	// Emtpy node, return empty
	if (html == "<br />" || html == "<p>&nbsp;</p>")
		html = "";

	if (tinyMCE.settings["preformatted"])
		return "<pre>" + html + "</pre>";

	return html;
}

function TinyMCE_insertLink(href, target) {
	this.execCommand("mceAddUndoLevel");

	if (this.selectedInstance && this.selectedElement && this.selectedElement.nodeName.toLowerCase() == "img") {
		var doc = this.selectedInstance.contentWindow.document;

		var linkElement = doc.createElement("a");

		href = eval(tinyMCE.settings['urlconvertor_callback'] + "(href, linkElement);");
		linkElement.setAttribute('href', href);
		linkElement.setAttribute('target', target);
		linkElement.appendChild(this.selectedElement.cloneNode(true));

		this.selectedElement.parentNode.replaceChild(linkElement, this.selectedElement);

		return;
	}

	if (!this.linkElement && this.selectedInstance) {
		this.selectedInstance.contentDocument.execCommand("createlink", false, "#mce_temp_url#");
		tinyMCE.linkElement = this.getElementByAttributeValue(this.selectedInstance.contentDocument.body, "a", "href", "#mce_temp_url#");

		var elementArray = this.getElementsByAttributeValue(this.selectedInstance.contentDocument.body, "a", "href", "#mce_temp_url#");

		for (var i=0; i<elementArray.length; i++) {
			href = eval(tinyMCE.settings['urlconvertor_callback'] + "(href, elementArray[i]);");
			elementArray[i].setAttribute('href', href);
			elementArray[i].setAttribute('mce_real_href', href);
			elementArray[i].setAttribute('target', target);
		}

		tinyMCE.linkElement = elementArray[0];
	}

	if (this.linkElement) {
		href = eval(tinyMCE.settings['urlconvertor_callback'] + "(href, this.linkElement);");
		this.linkElement.setAttribute('href', href);
		this.linkElement.setAttribute('mce_real_href', href);
		this.linkElement.setAttribute('target', target);
	}
}

function TinyMCE_insertImage(src, alt, border, hspace, vspace, width, height, align) {
	this.execCommand("mceAddUndoLevel");

	function setAttrib(element, name, value, no_fix_value) {
		if (!no_fix_value && value != null) {
			var re = new RegExp('[^0-9%]', 'g');
			value = value.replace(re, '');
		}

		if (value != null && value != "")
			element.setAttribute(name, value);
		else
			element.removeAttribute(name);
	}

	if (!this.imgElement && this.selectedInstance) {
		this.selectedInstance.contentDocument.execCommand("insertimage", false, "#mce_temp_url#");
		tinyMCE.imgElement = this.getElementByAttributeValue(this.selectedInstance.contentDocument.body, "img", "src", "#mce_temp_url#");
	}

	if (this.imgElement) {
		src = eval(tinyMCE.settings['urlconvertor_callback'] + "(src, tinyMCE.imgElement);");

		tinyMCE.setAttrib(this.imgElement, 'src', src, true);
		tinyMCE.setAttrib(this.imgElement, 'mce_real_src', src, true);
		tinyMCE.setAttrib(this.imgElement, 'alt', alt, true);
		tinyMCE.setAttrib(this.imgElement, 'align', align, true);
		tinyMCE.setAttrib(this.imgElement, 'border', border);
		tinyMCE.setAttrib(this.imgElement, 'hspace', hspace);
		tinyMCE.setAttrib(this.imgElement, 'vspace', vspace);
		tinyMCE.setAttrib(this.imgElement, 'width', width);
		tinyMCE.setAttrib(this.imgElement, 'height', height);
		tinyMCE.setAttrib(this.imgElement, 'border', border);

		// Fix for bug #989846 - Image resize bug
		if (width && width != "")
			this.imgElement.style.pixelWidth = width;

		if (height && height != "")
			this.imgElement.style.pixelHeight = height;
	}
}

function TinyMCE_getElementByAttributeValue(node, element_name, attrib, value) {
	var elements = this.getElementsByAttributeValue(node, element_name, attrib, value);
	if (elements.length == 0)
		return null;

	return elements[0];
}

function TinyMCE_getElementsByAttributeValue(node, element_name, attrib, value) {
	var elements = new Array();

	if (node && node.nodeName.toLowerCase() == element_name) {
		if (node.getAttribute(attrib).indexOf(value) != -1)
			elements[elements.length] = node;
	}

	if (node.hasChildNodes) {
		for (var x=0; x<node.childNodes.length; x++) {
			var childElements = this.getElementsByAttributeValue(node.childNodes[x], element_name, attrib, value);
			for (var i=0; i<childElements.length; i++)
				elements[elements.length] = childElements[i];
		}
	}

	return elements;
}

function TinyMCE_getParentElement(node, names, attrib_name, attrib_value) {
	var namesAr = names.split(',');

	if (node == null)
		return null;

	do {
		for (var i=0; i<namesAr.length; i++) {
			if (node.nodeName.toLowerCase() == namesAr[i].toLowerCase() || names == "*") {
				if (typeof attrib_name == "undefined")
					return node;
				else if (node.getAttribute(attrib_name)) {
					if (typeof attrib_value == "undefined") {
						if (node.getAttribute(attrib_name) != "")
							return node;
					} else if (node.getAttribute(attrib_name) == attrib_value)
						return node;
				}
			}
		}
	} while (node = node.parentNode);

	return null;
}

function TinyMCE_convertURL(url, node, on_save) {
	var fileProto = (document.location.protocol == "file:");

	// Mailto link or anchor (Pass through)
	if (url.indexOf('mailto:') != -1 || url.indexOf('javascript:') != -1 || tinyMCE.regexpReplace(url,'[ \t\r\n\+]|%20','').charAt(0) == "#")
		return url;

	// Fix relative/Mozilla
	if (!tinyMCE.isMSIE && !on_save && url.indexOf("://") == -1 && url.charAt(0) != '/')
		return tinyMCE.settings['base_href'] + url;

	// Convert to relative urls
	if (on_save && tinyMCE.settings['relative_urls']) {
		var urlParts = tinyMCE.parseURL(url);

		// If not absolute url, do nothing (Mozilla)
		if (!urlParts['protocol'] && !tinyMCE.isMSIE) {
			var urlPrefix = "http://";
			urlPrefix += document.location.hostname;
			if (document.location.port != "")
				urlPrefix += document.location.port;

			url = urlPrefix + url;
			urlParts = tinyMCE.parseURL(url);
		}

		var tmpUrlParts = tinyMCE.parseURL(tinyMCE.settings['document_base_url']);

		// Link is within this site
		if (urlParts['host'] == tmpUrlParts['host'] && (!urlParts['port'] || urlParts['port'] == tmpUrlParts['port']))
			return tinyMCE.convertAbsoluteURLToRelativeURL(tinyMCE.settings['document_base_url'], url);
	}

	// Remove current domain
	if (!fileProto && tinyMCE.settings['remove_script_host']) {
		var start = document.location.protocol + "//" + document.location.hostname + "/";
		if (url.indexOf(start) == 0)
			url = url.substring(start.length-1);
	}

	return url;
}

/**
 * Parses a URL in to its diffrent components.
 */
function TinyMCE_parseURL(url_str) {
	var urlParts = new Array();

	if (url_str) {
		var pos, lastPos;

		// Parse protocol part
		pos = url_str.indexOf('://');
		if (pos != -1) {
			urlParts['protocol'] = url_str.substring(0, pos);
			lastPos = pos + 3;
		}

		// Find port or path start
		for (var i=lastPos; i<url_str.length; i++) {
			var chr = url_str.charAt(i);

			if (chr == ':')
				break;

			if (chr == '/')
				break;
		}
		pos = i;

		// Get host
		urlParts['host'] = url_str.substring(lastPos, pos);

		// Get port
		lastPos = pos;
		if (url_str.charAt(pos) == ':') {
			pos = url_str.indexOf('/', lastPos);
			urlParts['port'] = url_str.substring(lastPos+1, pos);
		}

		// Get path
		lastPos = pos;
		pos = url_str.indexOf('?', lastPos);
		if (pos == -1)
			pos = url_str.length;

		urlParts['path'] = url_str.substring(lastPos, pos);

		// Get query
		lastPos = pos;
		if (url_str.charAt(pos) == '?') {
			pos = url_str.length;
			urlParts['query'] = url_str.substring(lastPos+1, pos);
		}
	}

	return urlParts;
}

/**
 * Converts an absolute path to relative path.
 */
function TinyMCE_convertAbsoluteURLToRelativeURL(base_url, url_to_relative) {
	var strTok1;
	var strTok2;
	var breakPoint = 0;
	var outputString = "";

	// * * Crop away last path part
	base_url = base_url.substring(0, base_url.lastIndexOf('/'));
	strTok1 = base_url.split('/');
	strTok2 = url_to_relative.split('/');

	if (strTok1.length >= strTok2.length) {
		for (var i=0; i<strTok1.length; i++) {
			if (i >= strTok2.length || strTok1[i] != strTok2[i]) {
				breakPoint = i + 1;
				break;
			}
		}
	}

	if (strTok1.length < strTok2.length) {
		for (var i=0; i<strTok2.length; i++) {
			if (i >= strTok1.length || strTok1[i] != strTok2[i]) {
				breakPoint = i + 1;
				break;
			}
		}
	}

	if (breakPoint == 1)
		return url_to_relative;

	for (var i=0; i<(strTok1.length-(breakPoint-1)); i++)
		outputString += "../";

	for (var i=breakPoint-1; i<strTok2.length; i++) {
		if (i != (breakPoint-1))
			outputString += "/" + strTok2[i];
		else
			outputString += strTok2[i];
	}

	return outputString;
}

function TinyMCE_getParam(name, default_value) {
	return (typeof this.settings[name] == "undefined") ? default_value : this.settings[name];
}

function TinyMCE_getLang(name, default_value) {
	return (typeof tinyMCELang[name] == "undefined") ? default_value : tinyMCELang[name];
}

function TinyMCE_replaceVar(replace_haystack, replace_var, replace_str) {
	var re = new RegExp('{\\\$' + replace_var + '}', 'g');
	return replace_haystack.replace(re, replace_str);
}

function TinyMCE_replaceVars(replace_haystack, replace_vars) {
	var variables = replace_haystack.match(new RegExp('{\\\$.*?}', 'g'));

	if (variables != null) {
		for (var i=0; i<variables.length; i++) {
			var variableName = variables[i].substring(2);
			variableName = variableName.substring(0, variableName.length-1);
			if (typeof replace_vars[variableName] != "undefined")
				replace_haystack = replace_haystack.replace(variables[i], replace_vars[variableName]);
		}
	}

	return replace_haystack;
}

function TinyMCE_triggerNodeChange(focus) {
	if (tinyMCE.settings['handleNodeChangeCallback']) {
		if (tinyMCE.selectedInstance) {
			var editorId = tinyMCE.selectedInstance.editorId;
			var elm = tinyMCE.selectedInstance.getFocusElement();
			var undoIndex = -1;
			var undoLevels = -1;

			if (tinyMCE.settings['custom_undo_redo']) {
				undoIndex = tinyMCE.selectedInstance.undoIndex;
				undoLevels = tinyMCE.selectedInstance.undoLevels.length;
			}

			eval(tinyMCE.settings['handleNodeChangeCallback'] + "(editorId, elm, undoIndex, undoLevels, tinyMCE.selectedInstance.visualAid);");
		}
	}

	if (tinyMCE.selectedInstance && (typeof focus == "undefined" || focus))
		this.selectedInstance.contentWindow.focus();
}

function TinyMCE_getContent(editor_id) {
	if (typeof editor_id != "undefined")
		tinyMCE.selectedInstance = tinyMCE._getInstanceById(editor_id);

	if (tinyMCE.selectedInstance) {
		var cleanedHTML = tinyMCE._cleanupHTML(this.selectedInstance.contentWindow.document, tinyMCE.settings, this.selectedInstance.contentWindow.document.body, false, true);
		return cleanedHTML;
	}

	return null;
}

function TinyMCE_setContent(html_content) {
	if (tinyMCE.selectedInstance) {
		var doc = this.selectedInstance.contentWindow.document;
		tinyMCE._setHTML(doc, html_content);
		doc.body.innerHTML = tinyMCE._cleanupHTML(doc, tinyMCE.settings, doc.body);
		tinyMCE.handleVisualAid(doc.body, true, tinyMCE.selectedInstance.visualAid);
	}
}

function TinyMCE_importThemeLanguagePack(theme_name) {
	if (typeof theme_name == "undefined")
		theme_name = tinyMCE.settings['theme'];

	document.write('<script language="javascript" type="text/javascript" src="' + tinyMCE.baseURL + '/themes/' + theme_name + '/langs/' + tinyMCE.settings['language'] +  '.js"></script>');	
}

/**
 * Adds themeurl, settings and lang to HTML code.
 */
function TinyMCE_applyTemplate(html, args) {
	html = tinyMCE.replaceVar(html, "themeurl", tinyMCE.themeURL);

	if (typeof args != "undefined")
		html = tinyMCE.replaceVars(html, args);

	html = tinyMCE.replaceVars(html, tinyMCE.settings);
	html = tinyMCE.replaceVars(html, tinyMCELang);

	return html;
}

function TinyMCE_openWindow(template, args, skip_lang) {
	var html, width, height, x, y, resizable, scrollbars;

	tinyMCE.windowArgs = args;

	html = template['html'];
	if (!(width = template['width']))
		width = 320;

	if (!(height = template['height']))
		height = 200;

	x = parseInt(screen.width / 2.0) - (width / 2.0);
	y = parseInt(screen.height / 2.0) - (height / 2.0);

	resizable = (args && args['resizable']) ? args['resizable'] : "no";
	scrollbars = (args && args['scrollbars']) ? args['scrollbars'] : "no";

	if (html) {
		html = tinyMCE.replaceVar(html, "css", this.settings['popups_css']);
		html = tinyMCE.applyTemplate(html, args, skip_lang);

		var win = window.open("", "mcePopup", "top=" + y + ",left=" + x + ",scrollbars=" + scrollbars + ",modal=yes,width=" + width + ",height=" + height + ",resizable=" + resizable);
		win.document.write(html);
		win.document.close();
	} else
		var win = window.open(tinyMCE.baseURL + "/themes/" + tinyMCE.getParam("theme") + "/" + template['file'], "mcePopup", "top=" + y + ",left=" + x + ",scrollbars=" + scrollbars + ",modal=yes,width=" + width + ",height=" + height + ",resizable=" + resizable);
}

function TinyMCE_handleVisualAid(element, deep, state) {
	var tableElement = null;

	// Remove visual aid elements
/*	if (!state && element.getAttribute && element.getAttribute('mceVisualAid') == "true") {
		element.parentNode.removeChild(element);
		return;
	}*/

	switch (element.nodeName.toLowerCase()) {
		case "table":
			var cssText = element.getAttribute("border") == 0 ? tinyMCE.settings['visual_table_style'] : "";

			element.style.cssText = state ? cssText : "";

			for (var y=0; y<element.rows.length; y++) {
				for (var x=0; x<element.rows[y].cells.length; x++)
					element.rows[y].cells[x].style.cssText = state ? cssText : "";
			}

			break;

/*		case "a":
			var name = element.getAttribute("name");
			if (name && name != "" && state) {
				//element.innerHTML += '<img mceVisualAid="true" src="' + (tinyMCE.themeURL + "/images/anchor.gif") + '" />';
				return;
			}

			break;*/
	}

	if (deep && element.hasChildNodes()) {
		for (var i=0; i<element.childNodes.length; i++)
			tinyMCE.handleVisualAid(element.childNodes[i], deep, state);
	}
}

function TinyMCE_getAttrib(elm, name, default_value) {
	var v = elm.getAttribute(name);
	return (v && v != "") ? v : default_value;
}

function TinyMCE_setAttrib(element, name, value, no_fix_value) {
	if (typeof value == "number")
		value = "" + value;

	if (!no_fix_value && value != null && value != -1) {
		var re = new RegExp('[^0-9%]', 'g');
		value = value.replace(re, '');
	}

	if (value != null && value != "" && value != -1)
		element.setAttribute(name, value);
	else
		element.removeAttribute(name);
}

function TinyMCE__setHTML(doc, html_content) {
	// Weird MSIE bug, <p><hr /></p> breaks runtime?
	if (tinyMCE.isMSIE) {
		var re = new RegExp('<p><hr /></p>', 'g');
		html_content = html_content.replace(re, "<hr>");
	}

	doc.body.innerHTML = html_content;

	// * * Content duplication bug fix
	if (tinyMCE.isMSIE && tinyMCE.settings['fix_content_duplication']) {
		// Remove P elements in P elements
		var paras = doc.getElementsByTagName("P");
		for (var i=0; i<paras.length; i++) {
			var node = paras[i];
			while ((node = node.parentNode) != null) {
				if (node.nodeName.toLowerCase() == "p")
					node.outerHTML = node.innerHTML;
			}
		}

		// Content duplication bug fix (Seems to be word crap)
		var html = doc.body.createTextRange().htmlText;
		if (html.indexOf('="mso') != -1) {
/*			html = tinyMCE.regexpReplace(html, "<p>\s*(<p>\s*)?", "<p>");
			html = tinyMCE.regexpReplace(html, "<\/p>\s*(<\/p>\s*)?", "</p>");
			//html = tinyMCE.regexpReplace(html, "<o:p></o:p>", "<br />");
			html = tinyMCE.regexpReplace(html, "/<o:p>&nbsp;<\/o:p>/g", "");
			html = tinyMCE.regexpReplace(html, "/o:/g", "");
			html = tinyMCE.regexpReplace(html, "/<st1:.*?>/g", "");*/

			for (var i=0; i<doc.body.all.length; i++) {
				var el = doc.body.all[i];
				el.removeAttribute("className","",0);
				el.removeAttribute("style","",0);
			}

			html = doc.body.createTextRange().htmlText;

//			html = tinyMCE.regexpReplace(html, "<o:", "<");
			html = tinyMCE.regexpReplace(html, "<o:p><\/o:p>", "<br />");
			html = tinyMCE.regexpReplace(html, "<o:p>&nbsp;<\/o:p>", "");
			html = tinyMCE.regexpReplace(html, "<st1:.*?>", "");
			html = tinyMCE.regexpReplace(html, "<p><\/p>", "");
			html = tinyMCE.regexpReplace(html, "<p><\/p>\r\n<p><\/p>", "");
			html = tinyMCE.regexpReplace(html, "<p>&nbsp;<\/p>", "<br />");
			html = tinyMCE.regexpReplace(html, "<p>\s*(<p>\s*)?", "<p>");
			html = tinyMCE.regexpReplace(html, "<\/p>\s*(<\/p>\s*)?", "</p>");

			doc.body.innerHTML = html;
		}
	}
}

function TinyMCE__getElementById(element_id) {
	var elm = document.getElementById(element_id);
	if (!elm) {
		// Check for element in forms
		for (var j=0; j<document.forms.length; j++) {
			for (var k=0; k<document.forms[j].elements.length; k++) {
				if (document.forms[j].elements[k].name == element_id) {
					elm = document.forms[j].elements[k];
					break;
				}
			}
		}
	}

	return elm;
}

function TinyMCE_getEditorId(form_element) {
	var mceControl = this._getInstanceById(form_element);
	if (!mceControl)
		return null;

	return mceControl.editorId;
}

function TinyMCE__getInstanceById(editor_id) {
	var mceControl = this.instances[editor_id];
	if (!mceControl) {
		for (var instanceName in tinyMCE.instances) {
			var instance = tinyMCE.instances[instanceName];
			if (instance.formTargetElementId == editor_id) {
				mceControl = instance;
				break;
			}
		}
	}

	return mceControl;
}

function TinyMCE_queryInstanceCommandValue(editor_id, command) {
	var mceControl = tinyMCE._getInstanceById(editor_id);
	if (mceControl)
		return mceControl.queryCommandValue(command);

	return false;
}

function TinyMCE_queryInstanceCommandState(editor_id, command) {
	var mceControl = tinyMCE._getInstanceById(editor_id);
	if (mceControl)
		return mceControl.queryCommandState(command);

	return null;
}

function TinyMCE_getWindowArg(name, default_value) {
	return (typeof this.windowArgs[name] == "undefined") ? default_value : this.windowArgs[name];
}

function TinyMCE_getCSSClasses(editor_id, doc) {
	var output = new Array();

	// Is cached, use that
	if (typeof tinyMCE.cssClasses != "undefined")
		return tinyMCE.cssClasses;

	if (typeof editor_id == "undefined" && typeof doc == "undefined") {
		var instance;

		for (var instanceName in tinyMCE.instances) {
			instance = tinyMCE.instances[instanceName];
			break;
		}

		doc = instance.contentWindow.document;
	}

	if (typeof doc == "undefined") {
		var instance = tinyMCE._getInstanceById(editor_id);
		doc = instance.contentWindow.document;
	}

	if (doc) {
		var styles = tinyMCE.isMSIE ? doc.styleSheets : doc.styleSheets;

		if (styles && styles.length > 0) {
			var csses = null;

			// Just ignore any errors
			eval("try {var csses = tinyMCE.isMSIE ? doc.styleSheets(0).rules : doc.styleSheets[0].cssRules;} catch(e) {}");
			if (!csses)
				return null;

			for (var i=0; i<csses.length; i++) {
				var selectorText = csses[i].selectorText;

				// Can be multiple rules per selector
				var rules = selectorText.split(',');
				for (var c=0; c<rules.length; c++) {
					// Is class rule
					if (rules[c].indexOf('.') != -1) {
						//alert(rules[c].substring(rules[c].indexOf('.')));
						output.push(rules[c].substring(rules[c].indexOf('.')+1));
					}
				}
			}
		}
	}

	// Cache em
	if (output.length > 0)
		tinyMCE.cssClasses = output;

	return output;
}

function TinyMCE_regexpReplace(in_str, reg_exp, replace_str) {
	var re = new RegExp(reg_exp, 'g');
	return in_str.replace(re, replace_str);
}

// * * TinyMCEControl
function TinyMCEControl(settings) {
	// * * Undo levels
	this.undoLevels = new Array();
	this.undoIndex = 0;

	// * * Default settings
	this.settings = settings;
	this.settings['theme'] = tinyMCE.getParam("theme", "default");
	this.settings['width'] = tinyMCE.getParam("width", -1);
	this.settings['height'] = tinyMCE.getParam("height", -1);

	// * * Functions
	this.execCommand = TinyMCEControl_execCommand;
	this.queryCommandValue = TinyMCEControl_queryCommandValue;
	this.queryCommandState = TinyMCEControl_queryCommandState;
	this.onAdd = TinyMCEControl_onAdd;
	this.getFocusElement = TinyMCEControl_getFocusElement;
}

function TinyMCEControl_execCommand(command, user_interface, value) {
	// Mozilla issue
	if (!tinyMCE.isMSIE && !this.useCSS) {
		this.contentWindow.document.execCommand("useCSS", false, true);
		this.useCSS = true;
	}

	//alert("command: " + command + ", user_interface: " + user_interface + ", value: " + value);
	this.contentDocument = this.contentWindow.document; // <-- Strange!!

	var execCommandFunction = tinyMCE._getThemeFunction('_execCommand');
	if (eval("typeof " + execCommandFunction) != 'undefined') {
		if (eval(execCommandFunction + '(this.editorId, this.contentDocument.body, command, user_interface, value);'))
			return;
	}

	// Add undo level of operation
	if (command != "mceAddUndoLevel" && command != "Undo" && command != "Redo" && command != "mceImage" && command != "mceLink" && command != "mceToggleVisualAid" && (command != "mceInsertTable" && !user_interface))
		this.execCommand("mceAddUndoLevel");

	// Fix align on images
	if (this.getFocusElement() && this.getFocusElement().nodeName.toLowerCase() == "img") {
		var align = this.getFocusElement().getAttribute('align');

		switch (command) {
			case "JustifyLeft":
				if (align == 'left')
					this.getFocusElement().removeAttribute('align');
				else
					this.getFocusElement().setAttribute('align', 'left');

				tinyMCE.triggerNodeChange();
				return;

			case "JustifyCenter":
				if (align == 'middle')
					this.getFocusElement().removeAttribute('align');
				else
					this.getFocusElement().setAttribute('align', 'middle');

				tinyMCE.triggerNodeChange();
				return;

			case "JustifyRight":
				if (align == 'right')
					this.getFocusElement().removeAttribute('align');
				else
					this.getFocusElement().setAttribute('align', 'right');

				tinyMCE.triggerNodeChange();
				return;
		}
	}

	if (tinyMCE.settings['force_br_newlines']) {
		var documentRef = this.contentWindow.document;
		var alignValue = "";

		if (documentRef.selection.type != "Control") {
			switch (command) {
					case "JustifyLeft":
						alignValue = "left";
						break;

					case "JustifyCenter":
						alignValue = "center";
						break;

					case "JustifyFull":
						alignValue = "justify";
						break;

					case "JustifyRight":
						alignValue = "right";
						break;
			}

			if (alignValue != "") {
				var rng = documentRef.selection.createRange();

				if ((divElm = tinyMCE.getParentElement(rng.parentElement(), "div")) != null)
					divElm.setAttribute("align", alignValue);
				else if (rng.pasteHTML && rng.htmlText.length > 0)
					rng.pasteHTML('<div align="' + alignValue + '">' + rng.htmlText + "</div>");

				tinyMCE.triggerNodeChange();
				return;
			}
		}
	}

	switch (command) {
		case "mceLink":
			var href = "", target = "";

			if (tinyMCE.selectedElement.nodeName.toLowerCase() == "a")
				tinyMCE.linkElement = tinyMCE.selectedElement;

			if (tinyMCE.linkElement) {
				href= tinyMCE.linkElement.getAttribute('href') ? tinyMCE.linkElement.getAttribute('href') : "";
				target = tinyMCE.linkElement.getAttribute('target') ? tinyMCE.linkElement.getAttribute('target') : "";

				// Fix for drag-drop/copy paste bug in Mozilla
				mceRealHref = tinyMCE.linkElement.getAttribute('mce_real_href') ? tinyMCE.linkElement.getAttribute('mce_real_href') : "";
				if (mceRealHref != "")
					href = mceRealHref;

				href = eval(tinyMCE.settings['urlconvertor_callback'] + "(href, tinyMCE.linkElement, true);");
			}

			if (this.settings['insertlink_callback']) {
				var returnVal = eval(this.settings['insertlink_callback'] + "(href, target);");
				if (returnVal && returnVal['href'])
					tinyMCE.insertLink(returnVal['href'], returnVal['target']);
			} else {
				tinyMCE.openWindow(this.insertLinkTemplate, {href : href, target : target});
			}
		break;

		case "mceImage":
			var src = "", alt = "", border = "", hspace = "", vspace = "", width = "", height = "", align = "";

			if (tinyMCE.selectedElement != null && tinyMCE.selectedElement.nodeName.toLowerCase() == "img")
				tinyMCE.imgElement = tinyMCE.selectedElement;

			if (tinyMCE.imgElement) {
				src = tinyMCE.imgElement.getAttribute('src') ? tinyMCE.imgElement.getAttribute('src') : "";
				alt = tinyMCE.imgElement.getAttribute('alt') ? tinyMCE.imgElement.getAttribute('alt') : "";
				border = tinyMCE.imgElement.getAttribute('border') ? tinyMCE.imgElement.getAttribute('border') : "";
				hspace = tinyMCE.imgElement.getAttribute('hspace') ? tinyMCE.imgElement.getAttribute('hspace') : "";
				vspace = tinyMCE.imgElement.getAttribute('vspace') ? tinyMCE.imgElement.getAttribute('vspace') : "";
				width = tinyMCE.imgElement.getAttribute('width') ? tinyMCE.imgElement.getAttribute('width') : "";
				height = tinyMCE.imgElement.getAttribute('height') ? tinyMCE.imgElement.getAttribute('height') : "";
				align = tinyMCE.imgElement.getAttribute('align') ? tinyMCE.imgElement.getAttribute('align') : "";

				// Fix for drag-drop/copy paste bug in Mozilla
				mceRealSrc = tinyMCE.imgElement.getAttribute('mce_real_src') ? tinyMCE.imgElement.getAttribute('mce_real_src') : "";
				if (mceRealSrc != "")
					src = mceRealSrc;

				src = eval(tinyMCE.settings['urlconvertor_callback'] + "(src, tinyMCE.imgElement, true);");
			}

			if (this.settings['insertimage_callback']) {
				var returnVal = eval(this.settings['insertimage_callback'] + "(src, alt, border, hspace, vspace, width, height, align);");
				if (returnVal && returnVal['src'])
					tinyMCE.insertImage(returnVal['src'], returnVal['alt'], returnVal['border'], returnVal['hspace'], returnVal['vspace'], returnVal['width'], returnVal['height'], returnVal['align']);
			} else
				tinyMCE.openWindow(this.insertImageTemplate, {src : src, alt : alt, border : border, hspace : hspace, vspace : vspace, width : width, height : height, align : align});
		break;

		case "mceCleanupWord":
			if (tinyMCE.isMSIE) {
				var html = this.contentDocument.body.createTextRange().htmlText;
				if (html.indexOf('="mso') != -1) {
					tinyMCE._setHTML(this.contentDocument, this.contentDocument.body.innerHTML);
					var cleanedHTML = tinyMCE._cleanupHTML(this.contentDocument, this.settings, this.contentDocument.body, this.visualAid);
					this.contentDocument.body.innerHTML = cleanedHTML;
				}
			}
		break;

		case "mceCleanup":
			tinyMCE._setHTML(this.contentDocument, this.contentDocument.body.innerHTML);
			var cleanedHTML = tinyMCE._cleanupHTML(this.contentDocument, this.settings, this.contentDocument.body, this.visualAid);
			this.contentDocument.body.innerHTML = cleanedHTML;
		break;

		case "mceAnchor":
			if (!user_interface) {
				var aElm = tinyMCE.getParentElement(this.getFocusElement(), "a", "name");
				if (aElm)
					aElm.setAttribute('name', value);
				else {
					this.contentDocument.execCommand("fontname", false, "#mce_temp_font#");
					var elementArray = tinyMCE.getElementsByAttributeValue(this.contentDocument.body, "font", "face", "#mce_temp_font#");
					for (var x=0; x<elementArray.length; x++) {
						elm = elementArray[x];

						var aElm = this.contentDocument.createElement("a");
						aElm.setAttribute('name', value);

						if (elm.hasChildNodes()) {
							for (var i=0; i<elm.childNodes.length; i++)
								aElm.appendChild(elm.childNodes[i].cloneNode(true));
						}

						elm.parentNode.replaceChild(aElm, elm);
					}
				}

				tinyMCE.triggerNodeChange();
			}
			break;

		case "mceReplaceContent":
			var selectedText = "";

			if (tinyMCE.isMSIE) {
				var documentRef = this.contentWindow.document;
				var rng = documentRef.selection.createRange();
				selectedText = rng.text;
			} else
				selectedText = this.contentWindow.getSelection().toString();

			if (selectedText.length > 0) {
				value = tinyMCE.replaceVar(value, "selection", selectedText);
				tinyMCE.execCommand('mceInsertContent',false,value);
			}

			tinyMCE.triggerNodeChange();
		break;

		case "mceSetAttribute":
			if (typeof value == 'object') {
				var targetElms = (typeof value['targets'] == "undefined") ? "p,img,span,div,td,h1,h2,h3,h4,h5,h6,pre,address" : value['targets'];
				var targetNode = tinyMCE.getParentElement(this.getFocusElement(), targetElms);

				if (targetNode) {
					targetNode.setAttribute(value['name'], value['value']);
					tinyMCE.triggerNodeChange();
				}
			}
		break;

		case "mceSetCSSClass":
			var selectedText = false;

			if (tinyMCE.isMSIE) {
				var documentRef = this.contentWindow.document;
				var rng = documentRef.selection.createRange();
				selectedText = (rng.text && rng.text.length > 0);
			} else
				selectedText = (this.contentWindow.getSelection().toString().length > 0);

			if (selectedText) {
				this.contentDocument.execCommand("removeformat", false, null);
				this.contentDocument.execCommand("fontname", false, "#mce_temp_font#");
				var elementArray = tinyMCE.getElementsByAttributeValue(this.contentDocument.body, "font", "face", "#mce_temp_font#");
/*				this.contentDocument.execCommand("createlink", false, "#mce_temp_url#");
				var elementArray = tinyMCE.getElementsByAttributeValue(this.contentDocument.body, "a", "href", "#mce_temp_url#");
*/
				// Change them all
				for (var x=0; x<elementArray.length; x++) {
					elm = elementArray[x];
					if (elm) {
						var spanElm = this.contentDocument.createElement("span");
						spanElm.className = value;
						if (elm.hasChildNodes()) {
							for (var i=0; i<elm.childNodes.length; i++)
								spanElm.appendChild(elm.childNodes[i].cloneNode(true));
						}

						elm.parentNode.replaceChild(spanElm, elm);
					}
				}

				tinyMCE.setContent(this.contentDocument.body.innerHTML);
			} else {
				var targetElm = this.getFocusElement();
				if (tinyMCE.selectedElement.nodeName.toLowerCase() == "img" || tinyMCE.selectedElement.nodeName.toLowerCase() == "table")
					targetElm = tinyMCE.selectedElement;

				var targetNode = tinyMCE.getParentElement(targetElm, "p,img,span,div,td,h1,h2,h3,h4,h5,h6,pre,address");

				// Mozilla img patch
				if (!tinyMCE.isMSIE && !targetNode)
					targetNode = tinyMCE.imgElement;

				if (targetNode) {
					if (targetNode.nodeName.toLowerCase() == "span" && (!value || value == "")) {
						if (targetNode.hasChildNodes()) {
							for (var i=0; i<targetNode.childNodes.length; i++)
								targetNode.parentNode.insertBefore(targetNode.childNodes[i].cloneNode(true), targetNode);
						}

						targetNode.parentNode.removeChild(targetNode);
					} else {
						if (value != null && value != "")
							targetNode.className = value;
						else {
							targetNode.removeAttribute("className");
							targetNode.removeAttribute("class");
						}
					}
				}
			}

			tinyMCE.triggerNodeChange();
		break;

		case "mceInsertContent":
			if (!tinyMCE.isMSIE) {
				this.contentDocument.execCommand("insertimage", false, "#mce_temp_url#");
				elm = tinyMCE.getElementByAttributeValue(this.contentDocument.body, "img", "src", "#mce_temp_url#");

				if (elm) {
					var rng = elm.ownerDocument.createRange();
					rng.setStartBefore(elm);
					var fragment = rng.createContextualFragment(value);
					elm.parentNode.replaceChild(fragment, elm);
				}
			} else {
				var rng = this.contentWindow.document.selection.createRange();

				if (rng.item)
					rng.item(0).outerHTML = value;
				else
					rng.pasteHTML(value);
			}

			tinyMCE.triggerNodeChange();
		break;

		case "mceInsertTable":
			if (user_interface) {
				var cols = 2, rows = 2, border = 0, cellpadding = "", cellspacing = "", align = "", width = "", height = "", action = "insert", className = "";

				tinyMCE.tableElement = tinyMCE.getParentElement(this.getFocusElement(), "table");

				if (tinyMCE.tableElement) {
					var rowsAr = tinyMCE.tableElement.rows;
					var cols = 0;
					for (var i=0; i<rowsAr.length; i++)
						if (rowsAr[i].cells.length > cols)
							cols = rowsAr[i].cells.length;

					cols = cols;
					rows = rowsAr.length;

					border = tinyMCE.getAttrib(tinyMCE.tableElement, 'border', border);
					cellpadding = tinyMCE.getAttrib(tinyMCE.tableElement, 'cellpadding', "");
					cellspacing = tinyMCE.getAttrib(tinyMCE.tableElement, 'cellspacing', "");
					width = tinyMCE.getAttrib(tinyMCE.tableElement, 'width', width);
					height = tinyMCE.getAttrib(tinyMCE.tableElement, 'height', height);
					align = tinyMCE.getAttrib(tinyMCE.tableElement, 'align', align);
					className = tinyMCE.getAttrib(tinyMCE.tableElement, tinyMCE.isMSIE ? 'className' : "class", "");

					if (tinyMCE.isMSIE) {
						width = tinyMCE.tableElement.style.pixelWidth == 0 ? tinyMCE.tableElement.getAttribute("width") : tinyMCE.tableElement.style.pixelWidth;
						height = tinyMCE.tableElement.style.pixelHeight == 0 ? tinyMCE.tableElement.getAttribute("height") : tinyMCE.tableElement.style.pixelHeight;
					}

					action = "update";
				}

				tinyMCE.openWindow(this.insertTableTemplate, {editor_id : this.editorId, cols : cols, rows : rows, border : border, cellpadding : cellpadding, cellspacing : cellspacing, align : align, width : width, height : height, action : action, className : className});
			} else {
				var html = '';
				var cols = 2, rows = 2, border = 0, cellpadding = -1, cellspacing = -1, align, width, height, className;

				if (typeof value == 'object') {
					cols = value['cols'];
					rows = value['rows'];
					border = value['border'] != "" ? value['border'] : 0;
					cellpadding = value['cellpadding'] != "" ? value['cellpadding'] : -1;
					cellspacing = value['cellspacing'] != "" ? value['cellspacing'] : -1;
					align = value['align'];
					width = value['width'];
					height = value['height'];
					className = value['className'];
				}

				// Update table
				if (tinyMCE.tableElement) {
					tinyMCE.setAttrib(tinyMCE.tableElement, 'cellPadding', cellpadding);
					tinyMCE.setAttrib(tinyMCE.tableElement, 'cellSpacing', cellspacing);
					tinyMCE.setAttrib(tinyMCE.tableElement, 'border', border);
					tinyMCE.setAttrib(tinyMCE.tableElement, 'width', width);
					tinyMCE.setAttrib(tinyMCE.tableElement, 'height', height);
					tinyMCE.setAttrib(tinyMCE.tableElement, 'align', align, true);
					tinyMCE.setAttrib(tinyMCE.tableElement, tinyMCE.isMSIE ? 'className' : "class", className, true);

					if (tinyMCE.isMSIE) {
						tinyMCE.tableElement.style.pixelWidth = (width == null || width == "") ? 0 : width;
						tinyMCE.tableElement.style.pixelHeight = (height == null || height == "") ? 0 : height;
					}

					tinyMCE.handleVisualAid(tinyMCE.tableElement, false, this.visualAid);

					// Fix for stange MSIE align bug
					tinyMCE.tableElement.outerHTML = tinyMCE.tableElement.outerHTML;

					//this.contentWindow.dispatchEvent(createEvent("click"));

					tinyMCE.triggerNodeChange();
					return;
				}

				// Create new table
				html += '<table border="' + border + '" ';
				var visualAidStyle = this.visualAid ? tinyMCE.settings['visual_table_style'] : "";

				if (cellpadding != -1)
					html += 'cellpadding="' + cellpadding + '" ';

				if (cellspacing != -1)
					html += 'cellspacing="' + cellspacing + '" ';

				if (width != 0 && width != "")
					html += 'width="' + width + '" ';

				if (height != 0 && height != "")
					html += 'height="' + height + '" ';

				if (align)
					html += 'align="' + align + '" ';

				if (className)
					html += 'class="' + className + '" ';

				if (border == 0 && tinyMCE.settings['visual'])
					html += 'style="' + visualAidStyle + '" ';

				html += '>';

				for (var y=0; y<rows; y++) {
					html += "<tr>";
					for (var x=0; x<cols; x++) {
						if (border == 0 && tinyMCE.settings['visual'])
							html += '<td style="' + visualAidStyle + '">';
						else
							html += '<td>';

						html += "&nbsp;</td>";
					}
					html += "</tr>";
				}

				html += "</table>";

				this.execCommand('mceInsertContent', false, html);
			}
			break;

		case "mceTableInsertRowBefore":
		case "mceTableInsertRowAfter":
		case "mceTableDeleteRow":
		case "mceTableInsertColBefore":
		case "mceTableInsertColAfter":
		case "mceTableDeleteCol":
			var trElement = tinyMCE.getParentElement(this.getFocusElement(), "tr");
			var tdElement = tinyMCE.getParentElement(this.getFocusElement(), "td");
			var tableElement = tinyMCE.getParentElement(this.getFocusElement(), "table");

			// No table just return (invalid command)
			if (!tableElement)
				return;

			var documentRef = this.contentWindow.document;
			var tableBorder = tableElement.getAttribute("border");
			var visualAidStyle = this.visualAid ? tinyMCE.settings['visual_table_style'] : "";

			// Table has a tbody use that reference
			if (tableElement.firstChild && tableElement.firstChild.nodeName.toLowerCase() == "tbody")
				tableElement = tableElement.firstChild;

			if (tableElement && trElement) {
				switch (command) {
					case "mceTableInsertRowBefore":
						var numcells = trElement.cells.length;
						var rowCount = 0;
						var tmpTR = trElement;

						// Count rows
						while (tmpTR) {
							if (tmpTR.nodeName.toLowerCase() == "tr")
								rowCount++;

							tmpTR = tmpTR.previousSibling;
						}

						var r = tableElement.insertRow(rowCount == 0 ? 1 : rowCount-1);
						for (var i=0; i<numcells; i++) {
							var newTD = documentRef.createElement("td");
							newTD.innerHTML = "&nbsp;";

							if (tableBorder == 0)
								newTD.style.cssText = visualAidStyle;

							var c = r.appendChild(newTD);

							if (tdElement.parentNode.childNodes[i].colSpan)
								c.colSpan = tdElement.parentNode.childNodes[i].colSpan;
						}
					break;

					case "mceTableInsertRowAfter":
						var numcells = trElement.cells.length;
						var rowCount = 0;
						var tmpTR = trElement;
						var documentRef = this.contentWindow.document;

						// Count rows
						while (tmpTR) {
							if (tmpTR.nodeName.toLowerCase() == "tr")
								rowCount++;

							tmpTR = tmpTR.previousSibling;
						}

						var r = tableElement.insertRow(rowCount == 0 ? 1 : rowCount);
						for (var i=0; i<numcells; i++) {
							var newTD = documentRef.createElement("td");
							newTD.innerHTML = "&nbsp;";

							if (tableBorder == 0)
								newTD.style.cssText = visualAidStyle;

							var c = r.appendChild(newTD);

							if (tdElement.parentNode.childNodes[i].colSpan)
								c.colSpan = tdElement.parentNode.childNodes[i].colSpan;
						}
					break;

					case "mceTableDeleteRow":
						// Remove whole table
						if (tableElement.rows.length <= 1) {
							tableElement.parentNode.removeChild(tableElement);
							tinyMCE.triggerNodeChange();
							return;
						}

						var selElm = this.contentWindow.document.body;
						if (trElement.previousSibling)
							selElm = trElement.previousSibling.cells[0];

						// Delete row
						trElement.parentNode.removeChild(trElement);

						tinyMCE._selectNode(this.contentWindow, selElm);
					break;

					case "mceTableInsertColBefore":
						var cellCount = tdElement.cellIndex;

						// Add columns
						for (var y=0; y<tableElement.rows.length; y++) {
							var cell = tableElement.rows[y].cells[cellCount];

							// Can't add cell after cell that doesn't exist
							if (!cell)
								break;

							var newTD = documentRef.createElement("td");
							newTD.innerHTML = "&nbsp;";

							if (tableBorder == 0)
								newTD.style.cssText = visualAidStyle;

							cell.parentNode.insertBefore(newTD, cell);
						}
					break;

					case "mceTableInsertColAfter":
						var cellCount = tdElement.cellIndex;

						// Add columns
						for (var y=0; y<tableElement.rows.length; y++) {
							var append = false;
							var cell = tableElement.rows[y].cells[cellCount];
							if (cellCount == tableElement.rows[y].cells.length-1)
								append = true;
							else
								cell = tableElement.rows[y].cells[cellCount+1];

							var newTD = documentRef.createElement("td");
							newTD.innerHTML = "&nbsp;";

							if (tableBorder == 0)
								newTD.style.cssText = visualAidStyle;

							if (append)
								cell.parentNode.appendChild(newTD);
							else
								cell.parentNode.insertBefore(newTD, cell);
						}
					break;

					case "mceTableDeleteCol":
						var index = tdElement.cellIndex;
						var selElm = this.contentWindow.document.body;

						var numCols = 0;
						for (var y=0; y<tableElement.rows.length; y++) {
							if (tableElement.rows[y].cells.length > numCols)
								numCols = tableElement.rows[y].cells.length;
						}

						// Remove whole table
						if (numCols <= 1) {
							tinyMCE._selectNode(this.contentWindow, selElm);
							tableElement.parentNode.removeChild(tableElement);
							tinyMCE.triggerNodeChange();
							return;
						}

						// Remove columns
						for (var y=0; y<tableElement.rows.length; y++) {
							var cell = tableElement.rows[y].cells[index];
							if (cell)
								cell.parentNode.removeChild(cell);
						}

						if (index > 0)
							selElm = tableElement.rows[0].cells[index-1];

						tinyMCE._selectNode(this.contentWindow, selElm);
					break;
				}

				tinyMCE.triggerNodeChange();
			}
		break;

		case "mceAddUndoLevel":
			if (tinyMCE.settings['custom_undo_redo']) {
				var customUndoLevels = tinyMCE.settings['custom_undo_redo_levels'];

				var newHTML = this.contentWindow.document.body.innerHTML;
				if (newHTML != this.undoLevels[this.undoLevels.length-1]) {
					// Time to compress
					if (customUndoLevels != -1 && this.undoLevels.length > customUndoLevels) {
						for (var i=0; i<this.undoLevels.length-1; i++) {
							//alert(this.undoLevels[i] + "=" + this.undoLevels[i+1]);
							this.undoLevels[i] = this.undoLevels[i+1];
						}

						this.undoLevels.length--;
						this.undoIndex--;
					}

					//alert(newHTML + "=" + this.undoLevels[this.undoIndex]);
					// Add new level
					this.undoLevels[this.undoIndex++] = newHTML;
					this.undoLevels.length = this.undoIndex;
					//window.status = "mceAddUndoLevel - undo levels:" + this.undoLevels.length + ", undo index: " + this.undoIndex;
				}

				tinyMCE.triggerNodeChange(false);
			}
			break;

		case "Undo":
			if (tinyMCE.settings['custom_undo_redo']) {
				// Is first level
				if (this.undoIndex == this.undoLevels.length) {
					this.execCommand("mceAddUndoLevel");
					this.undoIndex--;
				}

				// Do undo
				if (this.undoIndex > 0) {
					this.undoIndex--;
					this.contentWindow.document.body.innerHTML = this.undoLevels[this.undoIndex];
				}

				//window.status = "Undo - undo levels:" + this.undoLevels.length + ", undo index: " + this.undoIndex;
				tinyMCE.triggerNodeChange();
			} else
				this.contentDocument.execCommand(command, user_interface, value);
			break;

		case "Redo":
			if (tinyMCE.settings['custom_undo_redo']) {
				if (this.undoIndex < (this.undoLevels.length-1)) {
					this.undoIndex++;
					this.contentWindow.document.body.innerHTML = this.undoLevels[this.undoIndex];
					//window.status = "Redo - undo levels:" + this.undoLevels.length + ", undo index: " + this.undoIndex;
				}

				tinyMCE.triggerNodeChange();
			} else
				this.contentDocument.execCommand(command, user_interface, value);
			break;

		case "mceToggleVisualAid":
			this.visualAid = !this.visualAid;
			tinyMCE.handleVisualAid(this.contentWindow.document.body, true, this.visualAid);
			tinyMCE.triggerNodeChange();
			break;
/*
		case "removeformat":
			//this.contentDocument.execCommand('FormatBlock', user_interface, '<span>');
			var documentRef = this.contentWindow.document;
			var rng = documentRef.selection.createRange();
			var elm = rng.item ? rng.item(0) : rng.parentElement();

			html = "</" + elm.nodeName + ">" + rng.text + "<" + elm.nodeName + ">";

			this.contentDocument.execCommand('FontName', user_interface, '#mce_temp_name#');
			var html = this.contentDocument.innerHTML;
			html.replace('<font face=');

			rng.pasteHTML(html);

			alert(html);
	
			if (tinyMCE.isMSIE) {
				var documentRef = this.contentWindow.document;
				var rngs = documentRef.selection.createRangeCollection();

				for (var i=0; i<rngs.length; i++)
					alert(rngs[i].htmlText);

				var html = rng.htmlText;
				var tmpElm = documentRef.createElement("div");
				tmpElm.innerHTML = html;
				for (var i=0; i<tmpElm.all.length; i++) {
					tmpElm.all[i].removeAttribute("style");
					tmpElm.all[i].removeAttribute("className");
				}
//alert(tmpElm.innerHTML);
				rng.pasteHTML(tmpElm.innerHTML);
				//rng.pasteHTML(rng.text);

				this.contentDocument.execCommand('FontName', user_interface, 'arial,helvetica,sans-serif');
			} else
				this.contentDocument.execCommand(command, user_interface, value);

			tinyMCE.triggerNodeChange();
			break;
*/
		default:
			this.contentDocument.execCommand(command, user_interface, value);
			tinyMCE.triggerNodeChange();
	}
}

function TinyMCE__selectNode(win, node, expand) {
	if (!tinyMCE.isMSIE) {
		var rng = win.document.createRange(); 
		rng.selectNode(node); 
		sel = win.getSelection(); 
		sel.addRange(rng);
		if (expand)
			sel.extend(node, 0);
		else
			sel.collapse(node, 0);
	}
}

function TinyMCE__getThemeFunction(suffix) {
	var themePlugins = tinyMCE.settings['theme_plugins'].split(',');
	var templateFunction;

	// Is it defined in any plugins
	for (var i=themePlugins.length; i>=0; i--) {
		templateFunction = 'TinyMCE_' + themePlugins[i] + suffix;
		if (eval("typeof " + templateFunction) != 'undefined')
			return templateFunction;
	}

	return 'TinyMCE_' + tinyMCE.settings['theme'] + suffix;
}

function TinyMCEControl_queryCommandValue(command) {
	return this.contentWindow.document.queryCommandValue(command);
}

function TinyMCEControl_queryCommandState(command) {
	return this.contentWindow.document.queryCommandState(command);
}

function TinyMCEControl_onAdd(replace_element, form_element_name) {
	tinyMCE.themeURL = tinyMCE.baseURL + "/themes/" + this.settings['theme'];
	this.settings['themeurl'] = tinyMCE.themeURL;

	if (!replace_element) {
		alert("Error: Could not find the target element.");
		return false;
	}

	var templateFunction = tinyMCE._getThemeFunction('_getInsertTableTemplate');
	if (eval("typeof " + templateFunction) != 'undefined')
		this.insertTableTemplate = eval(templateFunction + '(this.settings);');

	var templateFunction = tinyMCE._getThemeFunction('_getInsertLinkTemplate');
	if (eval("typeof " + templateFunction) != 'undefined')
		this.insertLinkTemplate = eval(templateFunction + '(this.settings);');

	var templateFunction = tinyMCE._getThemeFunction('_getInsertImageTemplate');
	if (eval("typeof " + templateFunction) != 'undefined')
		this.insertImageTemplate = eval(templateFunction + '(this.settings);');

	var templateFunction = tinyMCE._getThemeFunction('_getEditorTemplate');
	if (eval("typeof " + templateFunction) == 'undefined') {
		alert("Error: Could not find the template function: " + templateFunction);
		return false;
	}

	var editorTemplate = eval(templateFunction + '(this.settings, this.editorId);');

	var deltaWidth = editorTemplate['delta_width'] ? editorTemplate['delta_width'] : 0;
	var deltaHeight = editorTemplate['delta_height'] ? editorTemplate['delta_height'] : 0;
	var html = '<span id="' + this.editorId + '_parent">' + editorTemplate['html'];

	var templateFunction = tinyMCE._getThemeFunction('_handleNodeChange');
	if (eval("typeof " + templateFunction) != 'undefined')
		this.settings['handleNodeChangeCallback'] = templateFunction;

	html = tinyMCE.replaceVar(html, "editor_id", this.editorId);
	html = tinyMCE.replaceVar(html, "default_document", tinyMCE.baseURL + "/blank.htm");

	this.settings['old_width'] = this.settings['width'];
	this.settings['old_height'] = this.settings['height'];

	// * * Set default width, height
	if (this.settings['width'] == -1)
		this.settings['width'] = replace_element.offsetWidth;

	if (this.settings['height'] == -1)
		this.settings['height'] = replace_element.offsetHeight;

	this.settings['area_width'] = this.settings['width'];
	this.settings['area_height'] = this.settings['height'];
	this.settings['area_width'] += deltaWidth;
	this.settings['area_height'] += deltaHeight;

	// * * Special % handling
	if (("" + this.settings['width']).indexOf('%') != -1)
		this.settings['area_width'] = "100%";

	if (("" + this.settings['height']).indexOf('%') != -1)
		this.settings['area_height'] = "100%";

	if (("" + replace_element.style.width).indexOf('%') != -1) {
		this.settings['width'] = replace_element.style.width;
		this.settings['area_width'] = "100%";
	}

	if (("" + replace_element.style.height).indexOf('%') != -1) {
		this.settings['height'] = replace_element.style.height;
		this.settings['area_height'] = "100%";
	}

	html = tinyMCE.applyTemplate(html);

	this.settings['width'] = this.settings['old_width'];
	this.settings['height'] = this.settings['old_height'];

	this.visualAid = this.settings['visual'];
	this.oldTargetElement = replace_element.cloneNode(true);
	this.formTargetElementId = form_element_name;

	html = html + '<input type="hidden" id="' + form_element_name + '" name="' + form_element_name + '" value=""></span>';

	// * * Output HTML and set editable
	if (!tinyMCE.isMSIE) {
		var rng = replace_element.ownerDocument.createRange();
		rng.setStartBefore(replace_element);

		var fragment = rng.createContextualFragment(html);
		replace_element.parentNode.replaceChild(fragment, replace_element);
	} else
		replace_element.outerHTML = html;

	//window.setTimeout("tinyMCE.setEditMode('" + this.editorId + "', true)", 1);
	tinyMCE.setEditMode(this.editorId, true);

	return true;
}

function TinyMCEControl_getFocusElement() {
	if (tinyMCE.isMSIE) {
		var documentRef = this.contentWindow.document;
		var rng = documentRef.selection.createRange();
		var elm = rng.item ? rng.item(0) : rng.parentElement();
	} else {
		var sel = this.contentWindow.getSelection();
		var elm = sel.anchorNode;

		if (tinyMCE.selectedElement != null && tinyMCE.selectedElement.nodeName.toLowerCase() == "img")
			elm = tinyMCE.selectedElement;
	}

	return elm;
}

// * * Global instances
var tinyMCE = new TinyMCE();
var tinyMCELang = new Array();
