/* Import theme specific language pack */
tinyMCE.importThemeLanguagePack();

// Variable declarations
var TinyMCE_advanced_autoImportCSSClasses = true;
var TinyMCE_advanced_foreColor = "#000000";
var TinyMCE_advanced_anchorName = "";
var TinyMCE_advanced_buttons = [
		// Control id, button img, button title, command, user_interface, value
		['bold', '{$lang_bold_img}', '{$lang_bold_desc}', 'Bold'],
		['italic', '{$lang_italic_img}', '{$lang_italic_desc}', 'Italic'],
		['underline', 'underline.gif', '{$lang_underline_desc}', 'Underline'],
		['strikethrough', 'strikethrough.gif', '{$lang_striketrough_desc}', 'Strikethrough'],
		['justifyleft', 'left.gif', '{$lang_justifyleft_desc}', 'JustifyLeft'],
		['justifycenter', 'center.gif', '{$lang_justifycenter_desc}', 'JustifyCenter'],
		['justifyright', 'right.gif', '{$lang_justifyright_desc}', 'JustifyRight'],
		['justifyfull', 'full.gif', '{$lang_justifyfull_desc}', 'JustifyFull'],
		['bullist', 'bullist.gif', '{$lang_bullist_desc}', 'InsertUnorderedList'],
		['numlist', 'numlist.gif', '{$lang_numlist_desc}', 'InsertOrderedList'],
		['outdent', 'outdent.gif', '{$lang_outdent_desc}', 'Outdent'],
		['indent', 'indent.gif', '{$lang_indent_desc}', 'Indent'],
		['undo', 'undo.gif', '{$lang_undo_desc}', 'Undo'],
		['redo', 'redo.gif', '{$lang_redo_desc}', 'Redo'],
		['link', 'link.gif', '{$lang_link_desc}', 'mceLink', true],
		['unlink', 'unlink.gif', '{$lang_unlink_desc}', 'unlink'],
		['image', 'image.gif', '{$lang_image_desc}', 'mceImage', true],
		['cleanup', 'cleanup.gif', '{$lang_cleanup_desc}', 'mceCleanup'],
		['help', 'help.gif', '{$lang_help_desc}', 'mceHelp'],
		['code', 'code.gif', '{$lang_theme_code_desc}', 'mceCodeEditor'],
		['table', 'table.gif', '{$lang_theme_table_desc}', 'mceInsertTable', true],
		['row_before', 'table_insert_row_before.gif', '{$lang_theme_table_insert_row_before_desc}', 'mceTableInsertRowBefore'],
		['row_after', 'table_insert_row_after.gif', '{$lang_theme_table_insert_row_after_desc}', 'mceTableInsertRowAfter'],
		['delete_row', 'table_delete_row.gif', '{$lang_theme_table_delete_row_desc}', 'mceTableDeleteRow'],
		['col_before', 'table_insert_col_before.gif', '{$lang_theme_table_insert_col_before_desc}', 'mceTableInsertColBefore'],
		['col_after', 'table_insert_col_after.gif', '{$lang_theme_table_insert_col_after_desc}', 'mceTableInsertColAfter'],
		['delete_col', 'table_delete_col.gif', '{$lang_theme_table_delete_col_desc}', 'mceTableDeleteCol'],
		['hr', 'hr.gif', '{$lang_theme_hr_desc}', 'inserthorizontalrule'],
		['removeformat', 'removeformat.gif', '{$lang_theme_removeformat_desc}', 'removeformat'],
		['sub', 'sub.gif', '{$lang_theme_sub_desc}', 'subscript'],
		['sup', 'sup.gif', '{$lang_theme_sup_desc}', 'superscript'],
		['forecolor', 'forecolor.gif', '{$lang_theme_forecolor_desc}', 'mceForeColor'],
		['charmap', 'charmap.gif', '{$lang_theme_charmap_desc}', 'mceCharMap'],
		['visualaid', 'visualaid.gif', '{$lang_theme_visualaid_desc}', 'mceToggleVisualAid'],
		['anchor', 'anchor.gif', '{$lang_theme_anchor_desc}', 'mceInsertAnchor'],
		['custom1', 'custom_1.gif', '{$lang_theme_custom1_desc}', 'mceRemoveEditor']
	];

/**
 * Returns HTML code for the specificed control.
 */
function TinyMCE_advanced_getControlHTML(button_name) {
	// Lockup button in button list
	for (var i=0; i<TinyMCE_advanced_buttons.length; i++) {
		var but = TinyMCE_advanced_buttons[i];
		if (but[0] == button_name)
			return '<img id="{$editor_id}_' + but[0] + '" src="{$themeurl}/images/' + but[1] + '" title="' + but[2] + '" width="20" height="20" class="mceButtonNormal" onmouseover="tinyMCE.switchClass(this,\'mceButtonOver\');" onmouseout="tinyMCE.restoreClass(this);" onmousedown="tinyMCE.restoreAndSwitchClass(this,\'mceButtonDown\');" onclick="tinyMCE.execInstanceCommand(\'{$editor_id}\',\'' + but[3] + '\', ' + (but.length > 4 ? but[4] : false) + (but.length > 5 ? ', \'' + but[5] + '\'' : '') + ')">';
	}

	// Custom controlls other than buttons
	switch (button_name) {
		case "formatselect":
			return '<select id="{$editor_id}_formatSelect" name="{$editor_id}_formatSelect" onchange="tinyMCE.execInstanceCommand(\'{$editor_id}\',\'FormatBlock\',false,this.options[this.selectedIndex].value);" class="mceSelectList">\
					<option value="<p>">{$lang_theme_paragraph}</option>\
					<!-- <option value="<div>">{$lang_theme_div}</option> -->\
					<option value="<address>">{$lang_theme_address}</option>\
					<option value="<pre>">{$lang_theme_pre}</option>\
					<option value="<h1>">{$lang_theme_h1}</option>\
					<option value="<h2>">{$lang_theme_h2}</option>\
					<option value="<h3>">{$lang_theme_h3}</option>\
					<option value="<h4>">{$lang_theme_h4}</option>\
					<option value="<h5>">{$lang_theme_h5}</option>\
					<option value="<h6>">{$lang_theme_h6}</option>\
					</select>';

		case "styleselect":
			return '<select id="{$editor_id}_styleSelect" onmousedown="TinyMCE_advanced_setupCSSClasses(\'{$editor_id}\');" name="{$editor_id}_styleSelect" onchange="tinyMCE.execInstanceCommand(\'{$editor_id}\',\'mceSetCSSClass\',false,this.options[this.selectedIndex].value);" class="mceSelectList">{$style_select_options}</select>';

		case "fontselect":
			return '<select id="{$editor_id}_fontNameSelect" name="{$editor_id}_fontNameSelect" onchange="tinyMCE.execInstanceCommand(\'{$editor_id}\',\'FontName\',false,this.options[this.selectedIndex].value);" class="mceSelectList">\
					<option value="">{$lang_theme_fontdefault}</option>\
					<option value="arial,helvetica,sans-serif">Arial</option>\
					<option value="times new roman,times,serif">Times New Roman</option>\
					<option value="verdana,arial,helvetica,sans-serif">Verdana</option>\
					<option value="courier new,courier,monospace">Courier</option>\
					<option value="georgia,times new roman,times,serif">Georgia</option>\
					<option value="tahoma,arial,helvetica,sans-serif">Tahoma</option>\
					</select>';

		case "fontsizeselect":
			return '<select id="{$editor_id}_fontSizeSelect" name="{$editor_id}_fontSizeSelect" onchange="tinyMCE.execInstanceCommand(\'{$editor_id}\',\'FontSize\',false,this.options[this.selectedIndex].value);" class="mceSelectList">\
					<option value="1">1 (8 pt)</option>\
					<option value="2">2 (10 pt)</option>\
					<option value="3">3 (12 pt)</option>\
					<option value="4">4 (14 pt)</option>\
					<option value="5">5 (18 pt)</option>\
					<option value="6">6 (24 pt)</option>\
					<option value="7">7 (36 pt)</option>\
					</select>';

		case "separator":
			return '<img src="{$themeurl}/images/spacer.gif" width="1" height="15" class="mceSeparatorLine">';

		case "rowseparator":
			return '<br>';
	}

	return "";
}

/**
 * Theme specific exec command handeling.
 */
function TinyMCE_advanced_execCommand(editor_id, element, command, user_interface, value) {
	switch (command) {
		case "mceForeColor":
			var template = new Array();
			var inputColor = TinyMCE_advanced_foreColor;

			if (!inputColor)
				inputColor = "#000000";

			template['file'] = 'color_picker.htm';
			template['width'] = 170;
			template['height'] = 205;

			tinyMCE.openWindow(template, {editor_id : editor_id, command : "forecolor", input_color : inputColor});
			return true;

		case "mceCodeEditor":
			var template = new Array();

			template['file'] = 'source_editor.htm';
			template['width'] = tinyMCE.getParam("theme_advanced_source_editor_width", 440);
			template['height'] = tinyMCE.getParam("theme_advanced_source_editor_height", 370);

			tinyMCE.openWindow(template, {editor_id : editor_id, resizable : "yes", scrollbars : "yes"});
			return true;

		case "mceCharMap":
			var template = new Array();

			template['file'] = 'charmap.htm';
			template['width'] = 320;
			template['height'] = 190;

			tinyMCE.openWindow(template, {editor_id : editor_id});
			return true;

		case "mceInsertAnchor":
			var template = new Array();

			template['file'] = 'anchor.htm';
			template['width'] = 320;
			template['height'] = 110;

			tinyMCE.openWindow(template, {editor_id : editor_id, name : TinyMCE_advanced_anchorName});
			return true;
	}

	// Default behavior
	return false;
}

/**
 * Editor instance template function.
 */
function TinyMCE_advanced_getEditorTemplate(settings) {
	function removeFromArray(in_array, remove_array) {
		var outArray = new Array();
		for (var i=0; i<in_array.length; i++) {
			skip = false;

			for (var j=0; j<remove_array.length; j++) {
				if (in_array[i] == remove_array[j])
					skip = true;
			}

			if (!skip)
				outArray.push(in_array[i]);
		}

		return outArray; 
	}

	var template = new Array();
	var toolbarHTML = "";
	var toolbarLocation = tinyMCE.getParam("theme_advanced_toolbar_location", "bottom");

	// Render row 1
	var buttonNamesRow1 = tinyMCE.getParam("theme_advanced_buttons1", "bold,italic,underline,strikethrough,separator,justifyleft,justifycenter,justifyright,justifyfull,separator,styleselect,formatselect").split(',');
	buttonNamesRow1 = removeFromArray(buttonNamesRow1, tinyMCE.getParam("theme_advanced_disable", "").split(','));
	for (var i=0; i<buttonNamesRow1.length; i++)
		toolbarHTML += TinyMCE_advanced_getControlHTML(buttonNamesRow1[i]);

	if (buttonNamesRow1.length > 0)
		toolbarHTML += "<br>";

	// Render row 2
	var buttonNamesRow2 = tinyMCE.getParam("theme_advanced_buttons2", "bullist,numlist,separator,outdent,indent,separator,undo,redo,separator,link,unlink,anchor,image,cleanup,help,code").split(',');
	buttonNamesRow2 = removeFromArray(buttonNamesRow2, tinyMCE.getParam("theme_advanced_disable", "").split(','));
	for (var i=0; i<buttonNamesRow2.length; i++)
		toolbarHTML += TinyMCE_advanced_getControlHTML(buttonNamesRow2[i]);

	if (buttonNamesRow2.length > 0)
		toolbarHTML += "<br>";

	// Render row 3
	var buttonNamesRow3 = tinyMCE.getParam("theme_advanced_buttons3", "table,separator,row_before,row_after,delete_row,separator,col_before,col_after,delete_col,separator,hr,removeformat,visualaid,separator,sub,sup,separator,charmap").split(',');
	buttonNamesRow3 = removeFromArray(buttonNamesRow3, tinyMCE.getParam("theme_advanced_disable", "").split(','));
	for (var i=0; i<buttonNamesRow3.length; i++)
		toolbarHTML += TinyMCE_advanced_getControlHTML(buttonNamesRow3[i]);

	// Setup template html
	template['html'] = '<table class="mceEditor" border="0" cellpadding="0" cellspacing="0" width="{$width}" height="{$height}">';

	if (toolbarLocation == "top")
		template['html'] += '<tr><td class="mceToolbarTop" align="center" height="1">' + toolbarHTML + '</td></tr>';

	template['html'] += '<tr><td align="center">\
		<iframe id="{$editor_id}" class="mceEditorArea" border="1" frameborder="0" src="{$default_document}" marginwidth="0" marginheight="0" leftmargin="0" topmargin="0" style="width:{$area_width};height:{$area_height}" width="{$area_width}" height="{$area_height}"></iframe>\
		</td></tr>';

	if (toolbarLocation == "bottom")
		template['html'] += '<tr><td class="mceToolbarBottom" align="center" height="1">' + toolbarHTML + '</td></tr>';

	template['html'] += '</table>';

	// Setup style select options
	var styleSelectHTML = '<option value="">-- {$lang_theme_style_select} --</option>';
	if (settings['theme_advanced_styles']) {
		var stylesAr = settings['theme_advanced_styles'].split(';');
		for (var i=0; i<stylesAr.length; i++) {
			var key, value;

			key = stylesAr[i].split('=')[0];
			value = stylesAr[i].split('=')[1];

			styleSelectHTML += '<option value="' + value + '">' + key + '</option>';
		}

		TinyMCE_advanced_autoImportCSSClasses = false;
	}

	template['html'] = tinyMCE.replaceVar(template['html'], 'style_select_options', styleSelectHTML);
	template['delta_width'] = 0;
	template['delta_height'] = -40;

	return template;
}

/**
 * Insert link template function.
 */
function TinyMCE_advanced_getInsertLinkTemplate() {
	var template = new Array();

	template['file'] = 'link.htm';
	template['width'] = 320;
	template['height'] = 130;

	// Language specific width and height addons
	template['width'] += tinyMCE.getLang('lang_insert_link_delta_width', 0);
	template['height'] += tinyMCE.getLang('lang_insert_link_delta_height', 0);

	return template;
}

/**
 * Insert image template function.
 */
function TinyMCE_advanced_getInsertImageTemplate() {
	var template = new Array();

	template['file'] = 'image.htm';
	template['width'] = 340;
	template['height'] = 260;

	// Language specific width and height addons
	template['width'] += tinyMCE.getLang('lang_insert_image_delta_width', 0);
	template['height'] += tinyMCE.getLang('lang_insert_image_delta_height', 0);

	return template;
}

/**
 * Insert table template function.
 */
function TinyMCE_advanced_getInsertTableTemplate(settings) {
	var template = new Array();

	template['file'] = 'table.htm';
	template['width'] = 330;
	template['height'] = 200;

	// Language specific width and height addons
	template['width'] += tinyMCE.getLang('lang_insert_table_delta_width', 0);
	template['height'] += tinyMCE.getLang('lang_insert_table_delta_height', 0);

	return template;
}

/**
 * Node change handler.
 */
function TinyMCE_advanced_handleNodeChange(editor_id, node, undo_index, undo_levels, visual_aid) {
	function selectByValue(select_elm, value) {
		if (select_elm) {
			for (var i=0; i<select_elm.options.length; i++) {
				if (select_elm.options[i].value == value) {
					select_elm.selectedIndex = i;
					return true;
				}
			}
		}

		return false;
	}

	// Get element color
	var colorElm = tinyMCE.getParentElement(node, "font", "color");
	if (colorElm)
		TinyMCE_advanced_foreColor = "" + colorElm.color.toUpperCase();

	// Reset old states
	tinyMCE.switchClassSticky(editor_id + '_justifyleft', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_justifyright', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_justifycenter', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_justifyfull', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_bold', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_italic', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_underline', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_strikethrough', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_bullist', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_numlist', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_sub', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_sup', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_table', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_anchor', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_link', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_unlink', 'mceButtonNormal');
	tinyMCE.switchClassSticky(editor_id + '_row_before', 'mceButtonDisabled', true);
	tinyMCE.switchClassSticky(editor_id + '_row_after', 'mceButtonDisabled', true);
	tinyMCE.switchClassSticky(editor_id + '_delete_row', 'mceButtonDisabled', true);
	tinyMCE.switchClassSticky(editor_id + '_col_before', 'mceButtonDisabled', true);
	tinyMCE.switchClassSticky(editor_id + '_col_after', 'mceButtonDisabled', true);
	tinyMCE.switchClassSticky(editor_id + '_delete_col', 'mceButtonDisabled', true);
	tinyMCE.switchClassSticky(editor_id + '_outdent', 'mceButtonDisabled', true);

	// Get anchor name
	var anchorName = tinyMCE.getParentElement(node, "a", "name");
	TinyMCE_advanced_anchorName = "";
	if (anchorName) {
		TinyMCE_advanced_anchorName = anchorName.getAttribute("name");
		tinyMCE.switchClassSticky(editor_id + '_anchor', 'mceButtonSelected');
	}

	// Get link
	var anchorLink = tinyMCE.getParentElement(node, "a", "href");
	if (anchorLink) {
		tinyMCE.switchClassSticky(editor_id + '_link', 'mceButtonSelected');
		tinyMCE.switchClassSticky(editor_id + '_unlink', 'mceButtonSelected');
	}

	// Handle visual aid
	tinyMCE.switchClassSticky(editor_id + '_visualaid', visual_aid ? 'mceButtonSelected' : 'mceButtonNormal', false);

	if (undo_levels != -1) {
		tinyMCE.switchClassSticky(editor_id + '_undo', 'mceButtonDisabled', true);
		tinyMCE.switchClassSticky(editor_id + '_redo', 'mceButtonDisabled', true);
	}

	// Within a td element
	if (tinyMCE.getParentElement(node, "td")) {
		tinyMCE.switchClassSticky(editor_id + '_row_before', 'mceButtonNormal', false);
		tinyMCE.switchClassSticky(editor_id + '_row_after', 'mceButtonNormal', false);
		tinyMCE.switchClassSticky(editor_id + '_delete_row', 'mceButtonNormal', false);
		tinyMCE.switchClassSticky(editor_id + '_col_before', 'mceButtonNormal', false);
		tinyMCE.switchClassSticky(editor_id + '_col_after', 'mceButtonNormal', false);
		tinyMCE.switchClassSticky(editor_id + '_delete_col', 'mceButtonNormal', false);
	}

	// Within table
	if (tinyMCE.getParentElement(node, "table"))
		tinyMCE.switchClassSticky(editor_id + '_table', 'mceButtonSelected');

	// Within li, blockquote
	if (tinyMCE.getParentElement(node, "li,blockquote"))
		tinyMCE.switchClassSticky(editor_id + '_outdent', 'mceButtonNormal', false);

	// Has redo levels
	if (undo_index != -1 && (undo_index < undo_levels-1 && undo_levels > 0))
		tinyMCE.switchClassSticky(editor_id + '_redo', 'mceButtonNormal', false);

	// Has undo levels
	if (undo_index != -1 && (undo_index > 0 && undo_levels > 0))
		tinyMCE.switchClassSticky(editor_id + '_undo', 'mceButtonNormal', false);

	// Select class in select box
	var selectElm = document.getElementById(editor_id + "_styleSelect");
	if (selectElm) {
		TinyMCE_advanced_setupCSSClasses(editor_id);

		classNode = node;
		breakOut = false;
		var index = 0;

		do {
			if (classNode && classNode.className) {
				for (var i=0; i<selectElm.options.length; i++) {
					if (selectElm.options[i].value == classNode.className) {
						index = i;
						breakOut = true;
						break;
					}
				}
			}
		} while (!breakOut && (classNode = classNode.parentNode));

		selectElm.selectedIndex = index;
	}

	// Select formatblock
	var selectElm = document.getElementById(editor_id + "_formatSelect");
	if (selectElm) {
		var elm = tinyMCE.getParentElement(node, "p,div,h1,h2,h3,h4,h5,h6,pre,address");
		if (elm) {
			selectByValue(selectElm, "<" + elm.nodeName.toLowerCase() + ">");
		} else
			selectByValue(selectElm, "<p>");
	}

	// Select fontselect
	var selectElm = document.getElementById(editor_id + "_fontNameSelect");
	if (selectElm) {
		var elm = tinyMCE.getParentElement(node, "font", "face");
		if (elm)
			selectByValue(selectElm, elm.getAttribute("face"));
		else
			selectByValue(selectElm, "");
	}

	// Select fontsize
	var selectElm = document.getElementById(editor_id + "_fontSizeSelect");
	if (selectElm) {
		var elm = tinyMCE.getParentElement(node, "font", "size");
		if (elm)
			selectByValue(selectElm, elm.getAttribute("size"));
		else
			selectByValue(selectElm, "1");
	}

	// Handle align attributes
	alignNode = node;
	breakOut = false;
	do {
		if (!alignNode.getAttribute || !alignNode.getAttribute('align'))
			continue;

		switch (alignNode.getAttribute('align').toLowerCase()) {
			case "left":
				tinyMCE.switchClassSticky(editor_id + '_justifyleft', 'mceButtonSelected');
				breakOut = true;
			break;

			case "right":
				tinyMCE.switchClassSticky(editor_id + '_justifyright', 'mceButtonSelected');
				breakOut = true;
			break;

			case "middle":
			case "center":
				tinyMCE.switchClassSticky(editor_id + '_justifycenter', 'mceButtonSelected');
				breakOut = true;
			break;

			case "justify":
				tinyMCE.switchClassSticky(editor_id + '_justifyfull', 'mceButtonSelected');
				breakOut = true;
			break;
		}
	} while (!breakOut && (alignNode = alignNode.parentNode));

	// Handle elements
	do {
		switch (node.nodeName.toLowerCase()) {
			case "b":
			case "strong":
				tinyMCE.switchClassSticky(editor_id + '_bold', 'mceButtonSelected');
			break;

			case "i":
			case "em":
				tinyMCE.switchClassSticky(editor_id + '_italic', 'mceButtonSelected');
			break;

			case "u":
				tinyMCE.switchClassSticky(editor_id + '_underline', 'mceButtonSelected');
			break;

			case "strike":
				tinyMCE.switchClassSticky(editor_id + '_strikethrough', 'mceButtonSelected');
			break;
			
			case "ul":
				tinyMCE.switchClassSticky(editor_id + '_bullist', 'mceButtonSelected');
			break;

			case "ol":
				tinyMCE.switchClassSticky(editor_id + '_numlist', 'mceButtonSelected');
			break;

			case "sub":
				tinyMCE.switchClassSticky(editor_id + '_sub', 'mceButtonSelected');
			break;

			case "sup":
				tinyMCE.switchClassSticky(editor_id + '_sup', 'mceButtonSelected');
			break;
		}
	} while ((node = node.parentNode));
}

// This function auto imports CSS classes into the class selection droplist
function TinyMCE_advanced_setupCSSClasses(editor_id) {
	if (!TinyMCE_advanced_autoImportCSSClasses)
		return;

	var selectElm = document.getElementById(editor_id + '_styleSelect');

	if (selectElm && selectElm.getAttribute('cssImported') != 'true') {
		var csses = tinyMCE.getCSSClasses(editor_id);
		if (csses && selectElm) {
			for (var i=0; i<csses.length; i++)
				selectElm.options[selectElm.length] = new Option(csses[i], csses[i]);
		}

		// Only do this once
		if (csses != null && csses.length > 0)
			selectElm.setAttribute('cssImported', 'true');
	}
}
