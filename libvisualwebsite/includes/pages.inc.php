<?php

/*
 * Libvisual CMS
 * All code copyright (c) Andrew Godwin 2004
 * Page Include
 */
 
class cms_page {
	var $id;
	var $title;
	var $content;
	var $html;
	
	function cms_page($page) {
	
		$pagearray = select_bhdb("pages", array("id"=>$page), 1);
		$pagearray = $pagearray[0];
		
		$this->id = $page;
		$this->title = $pagearray['title'];
		$this->content = $pagearray['content'];
	
		$this->parse_page();
		
	}
	
	function parse_page() {
	
		$pagecontentarray = explode(",", $this->content);
		foreach ($pagecontentarray as $item) {
		
			if ($item == "news") {
			
				$newsobj = new cms_news("");
				$str .= "<div class='mainitem'>
	<span class='sectiontitle'>Latest News</span><br />
	<div class='sectioncontent'>".$newsobj->html."</div>
	</div>\n\n";
			
			} else {
		
				$pagerow = select_bhdb("content", array("id"=>$item), 1);
				$pagerow = $pagerow[0];
		
				$str .= "<div class='mainitem'>
	<span class='sectiontitle'>".$pagerow['title']."</span><br />
	<div class='sectioncontent'>".$pagerow['content']."</div>
	</div>\n\n";
			}
	
		}
	
		$this->html = $str;
	
	}
}

?>