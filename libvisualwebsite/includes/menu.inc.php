<?php

/*
 * Libvisual CMS
 * All code copyright (c) Andrew Godwin 2004
 * Menu Include
 */
 
function cms_menuarray() {

	$items = select_bhdb("menu", "", "");
	
	if (!empty($items)) { 
	
		foreach ($items as $item) {
			$sorteditems[$item['order']] = $item;
		}
		
		array_multisort($sorteditems);
		
		return $sorteditems;
	
	} else {
	
		return array();
	
	}

}


function cms_vertmenu() {

	$items = cms_menuarray();
	
	foreach ($items as $item) {
	
		$str .= '<div class="menuitem"><a href="'.$item['link'].'" class="menuitem"><img src="images/menuicons/'.$item['icon'].'" alt="'.$item['title'].'" class="menuitem" />'.$item['title'].'</a></div>
';
	
	}
	
	return $str;

}


function cms_horizmenu() {

	$items = cms_menuarray();
	
	foreach ($items as $item) {
	
		$str .= '<a href="'.$item['link'].'" class="menuitem"><img src="images/menuicons/'.$item['icon'].'" alt="'.$item['title'].'" class="menuitem" /></a>
';
	
	}
	
	return $str;

}

?>