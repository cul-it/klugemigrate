<?php

class Kluge3TranscriptMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('klugemigrate3'));
    $this->description = t('kluge video migration');

    $this->dependencies = array('Kluge1Video');

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
    $this->destination = new MigrateDestinationNode('transcript', $node_options);
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
 
    
    $this->addFieldMapping('field_trans_video', 'filmID')
    	->sourceMigration('Kluge1Video')
    	->defaultValue(1);
	
	$this->addFieldMapping('field_filmid', 'filmID');
	$this->addFieldMapping('title', 'title');
	$this->addFieldMapping('title_field', 'title');
	$this->addFieldMapping('body', 'title'); // temporary content
  }
  
  public function prepare($node, stdClass $row) {
  	// clone the english and replace german text only
  	$node->title_field['de'] = $node->title_field['en'];	// entity translate & title modules
  	$node->title_field['de'][0]['value'] = $row->title_deu;
  	
  	$node->body['de'] = $node->body['en'];
  	$node->body['de'][0]['value'] = $this->getTranscript($row->filmID, 'de');
  	$node->body['en'][0]['value'] = $this->getTranscript($row->filmID, 'en');
    }
    
  public function getTranscript($film_no, $language) {
  	$path = "http://muller-kluge.library.cornell.edu/$language/video_transcript.php?f=$film_no";
  	$trans = htmlqp($path, 'div#transcript div#transcript')->innerHTML();
  	//$trans = html_entities($trans);
  	$trans = mb_convert_encoding($trans, "ISO-8859-1");
  	$trans = mb_convert_encoding($trans, "UTF-8");
  	return "$trans";
  	}
}

?>