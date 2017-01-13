<?php

class Kluge2SegmentMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('klugemigrate2'));
    $this->description = t('kluge segment migration');
    
    $this->dependencies = array('Kluge1Video');

      // this absolutely has to be the correct columns in the correct order,
      // the import pays no attention to the headings in the csv file
    $columns = array(
    	array('filmID', 'filmID'),
    	array('segmentID', 'segmentID'),
    	array('start_time', 'start_time'),
    	array('end_time', 'end_time'),
  		array('title_eng', 'title_eng'),
      	array('title_deu', 'title_deu'),
    );

    $this->source = new MigrateSourceCSV(DRUPAL_ROOT . '/' . 
    	drupal_get_path('module', 'klugemigrate') . 
    	'/kluge_export_segments.csv', $columns);
    
    
    $language = 'en';
    $text_format = 'filtered_html';
    $node_options = MigrateDestinationNode::options($language, $text_format);
    $this->destination = new MigrateDestinationNode('segment', $node_options);
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'segmentID' => array(
            'type' => 'int',
            'unsigned' => true,
            'not null' => true,
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );
    
    $this->addFieldMapping('field_seg_video', 'filmID')
    	->sourceMigration('Kluge1Video')
    	->defaultValue(1);

	$this->addFieldMapping('title_field', 'title_eng');
	$this->addFieldMapping('title', 'title_eng');
	//$this->addFieldMapping('field_seg_start', 'start_time');
	//$this->addFieldMapping('field_seg_end', 'end_time');
	$this->addFieldMapping('field_playtime', 'time_start');
   
  }
  
  public function prepare($node, stdClass $row) {
  	// clone the english and replace german text only
  	$node->title_field['de'] = $node->title_field['en'];	// entity translate & title modules
  	$node->title_field['de'][0]['value'] = $row->title_deu;
  	
  	// _set_cck_date is in klugemigrate_video.inc
  	$node->field_playtime = _set_cck_date($node->field_playtime, $row->start_time, $row->end_time);
    }
}

?>