<?php

/*
 * Libvisual CMS
 * All code copyright (c) Andrew Godwin 2004
 * News Include
 */
 
class cms_news {
	var $section;
	var $items;
	var $html;
	var $numitems;
	
	function cms_news($section) {
	
		if (empty($section)) {
			$newsarray = select_bhdb("news", "", "");
		} else {
			$newsarray = select_bhdb("news", array("section"=>$section), 1);
		}
		
		foreach ($newsarray as $item) {
			$newsarray2[$item['date']] = $item;
		}
		
		array_multisort($newsarray2, SORT_ASC);
		
		$this->section = $section;
		$this->items = $newsarray2;
		$this->numitems = 3;
		$this->parse_news();
		
	}
	
	function parse_news() {
	
		$n = 0;
	
		foreach ($this->items as $item) {
		
			$n++;
			
			if ($n > $this->numitems) { break; }
			else {
			$pagerow = select_bhdb("content", array("id"=>$item), 1);
			$pagerow = $pagerow[0];
		
				$str .= "		<div class=\"newsitem\">
		<span class=\"newsheadline\">".$item['title']."</span><br />
		<span class=\"newsdate\">".$item['author']." &nbsp; ".date("l jS F Y", $item['date'])."</span><br />
		<span class=\"newsbody\">".$item['content']."</span>
		</div>\n\n";
			}
		}
	
		$this->html = $str;
	
	}
}

?>