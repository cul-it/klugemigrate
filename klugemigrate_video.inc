<?php

class Kluge1VideoMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('klugemigrate'));
    $this->description = t('kluge video migration');

      // this absolutely has to be the correct columns in the correct order,
      // the import pays no attention to the headings in the csv file
    $columns = array(
    	array('filmID', 'filmID'),
    	array('broadcast', 'broadcast_eng'),
    	array('date', 'date'),
    	array('duration', 'duration'),
     	array('time_start', 'time_start'),
    	array('time_end', 'time_end'),
		array('title', 'title_eng'),
    	array('title_deu', 'title_deu'),
    	array('subtitle', 'subtitle_eng'),
    	array('subtitle_deu', 'subtitle_deu'),
     	array('description', 'description_eng'),
   		array('description_deu', 'description_deu'),
    );

    $this->source = new MigrateSourceCSV(DRUPAL_ROOT . '/' . 
    	drupal_get_path('module', 'klugemigrate') . 
    	'/kluge_export_film.csv', $columns);
    
    
    $language = 'en';
    $text_format = 'filtered_html';
    $node_options = MigrateDestinationNode::options($language, $text_format);
    $this->destination = new MigrateDestinationNode('video', $node_options);
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'filmID' => array(
            'type' => 'int',
            'unsigned' => true,
            'not null' => true,
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );
 
    // To maintain node identities between the old and new systems (i.e., have
    // the same unique IDs), map the ID column from the old system to nid and
    // set is_new to TRUE.
    $this->addFieldMapping('nid', 'filmID')
         ->description(t('Preserve old film ID as nid in Drupal'));
    $this->addFieldMapping('is_new')
         ->defaultValue(TRUE);

	$this->addFieldMapping('field_filmid', 'filmID');
	$this->addFieldMapping('field_date', 'date');
	$this->addFieldMapping('field_length', 'duration');
	$this->addFieldMapping('title_field', 'title');
	$this->addFieldMapping('title', 'title');
	$this->addFieldMapping('field_subtitle', 'subtitle');
	$this->addFieldMapping('field_broadcast', 'broadcast');
	$this->addFieldMapping('body', 'description');
	//$this->addFieldMapping('field_film_start', 'time_start');
	//$this->addFieldMapping('field_film_end', 'time_end');
	$this->addFieldMapping('field_playtime', 'time_start');
	
	$this->addUnmigratedSources(array('time_end', 'title_deu', 'subtitle_deu', 'description_deu'));
   
  }
  
  public function prepare($node, stdClass $row) {
  	// clone the english and replace german text only
  	$node->title_field['de'] = $node->title_field['en'];	// entity translate & title modules
  	$node->title_field['de'][0]['value'] = $row->title_deu;
  	
  	$node->field_subtitle['de'] = $node->field_subtitle['en'];
  	$node->field_subtitle['de'][0]['value'] = $row->subtitle_deu;
  	
  	$node->field_broadcast['de'] = $node->field_broadcast['en'];
  	$node->field_broadcast['de'][0]['value'] = $row->broadcast_deu;
  	
  	$node->body['de'] = $node->body['en'];
  	$node->body['de'][0]['value'] = $row->description_deu;
  	
  	$node->field_playtime = _set_cck_date($node->field_playtime, $row->time_start, $row->time_end);
    }
}
    
function _set_cck_date($cck_date, $row_start_date, $row_end_date) {
	if (empty($row_start_date) && empty($row_end_date)) {
		return $cck_date;	// avoids 'All Day'
		}
  	if (empty($row_start_date)) {
  		$cck_date['und'][0]['value'] = '0000-1-1 00:00:00';
  		}
  	else {
  		$time = substr("0000".$row_start_date, -8);
  		$cck_date['und'][0]['value'] = '0000-1-1 ' . $time;
  		}
	if (empty($row_end_date)) {
		$cck_date['und'][0]['value2'] = $cck_date['und'][0]['value'];
		}
	else {
		$time = substr("0000".$row_end_date, -8);
		$cck_date['und'][0]['value2'] = '0000-1-1 ' . $time;
		}
 	return $cck_date;
  	}

?>