<?php
/**
 * Generate Transcript from Caption Files
 */

if ($entity->type != 'transcript') return;
// use lower case in this array!!!
$known_speakers = array('müller', 'kluge', 'intertitle', 'running text', 'tafel', 'textband', 'rihm',
  'castro', 'narrator', 'first cuban', 'second cuban', 'erzählerin', 'texttafel',
  '1. kubaner', '2. kubaner', 'adolph hitler', 'abspann', 'actor', 'bentzien', 'erzählerstimme',
  'film clip', 'filmausschnitt', 'günter gaus', 'gaus', 'hans bentzien', 'hindenburg',
  'hitler', 'muller', 'music', 'musik', 'schauspieler', 'text', 'wolfgang rihm', 'titel',
  'paul von hindenburg', 'der vater', 'friedrich ii', 'title', 'kluge and müller', 'sprecherin',
  'nurse', 'soldier', 'krankenschwester', 'kluge und müller', 'alexander kluge', 'heiner müller',
  'gautam dasgupta', 'dasgupta', 'dasgupta (via dolmetscherin)', 'chorus on stage',
  'chorus', 'actor on stage', 'heiner goebbels', 'goebbels', 'sänger',
  'mark lammert', 'lammert',
  'kluttig', 'the count', 'jourdheuil', 'achilles', 'sandrina',
  'song', 'boy', 'frau', 'mann', 'man', 'woman', 'voice-over', 'thomas heise', 'heise',
  'sign', 'graffiti', 'mann 1', 'mann 2', 'young man 1', 'young man 2',
  'young woman 1', 'young woman 2', 'young man', 'group song', 'voice-over (young woman)',
  'junge', 'voice-over', 'junger mann 1', 'junger mann 2', 'mädchen 1', 'mädchen 2',
  'gesang gruppe', 'plakat', 'voice-over (mädchen)',
  'poster', 'junger mann',
  'rodberg (via translator)', 'rodberg (via dolmetscherin)',
  'gorbachev (via translator)', 'gorbatschow (via dolmetscher)',
  );
$mylang = array('en','de');
foreach ($mylang as $lang) {
  // find the file
  if (isset($entity->field_captions[$lang][0]['fid'])) {
    $fid = $entity->field_captions[$lang][0]['fid'];
    $loaded = file_load($fid);
    $realpath = drupal_realpath($loaded->uri);
    //dsm($realpath);
    $lines = file($realpath, FILE_IGNORE_NEW_LINES);
    $output = array();
    $state = 'number';
    foreach ($lines as $line) {
      switch ($state) {
        case 'number':
          if (preg_match('/^[0-9]+$/', $line) === 1) {
            $state = 'timing';
          }
          else {
            // allow multiple empty lines
            $line = trim($line);
            $state = empty($line) ? 'number' : 'error 1';
          }
          break;
        case 'timing':
          $state = (preg_match('/\d{2}:\d{2}:\d{2},\d{3} --> \d{2}:\d{2}:\d{2},\d{3}/', $line) === 1) ? 'first line' : 'error 2';
          break;
        case 'first line':
          $state = 'another line';
          $matches = array();
          if (preg_match('/^([^:]*): (.*)$/', $line, $matches) === 1) {
            $speaker = strtolower($matches[1]);
            if (in_array($speaker, $known_speakers)) {
              $output[] = array('speaker' => $matches[1]);
              $output[] = array('text' => trim($matches[2]));
              break;
            }
            else {
              dsm("unknown speaker: $speaker");
            }
          }
          // intentional fall through
        case 'another line':
          $trimmed = trim($line);
          if (empty($trimmed)) {
            $state = 'number';
          }
          else {
            $output[] = array('text' => $trimmed);
          }
          break;
        default:
          dsm (array('error trying to build transcript: ', $state));
          return;
        }
      }

    $value = array();
    $value[] = '<dl>' . PHP_EOL;
    $first_speach = TRUE;
    foreach ($output as $line) {
      if (isset($line['text'])) {
         $value[] = $line['text'];
      }
      elseif (isset($line['speaker'])) {
        if ($first_speach) {
          $value[] = PHP_EOL . '<dt>' . $line['speaker'] . '</dt><dd>';
          $first_speach = FALSE;
        }
        else {
          $value[] = '</dd>' . PHP_EOL . '<dt>' . $line['speaker'] . '</dt><dd>';
        }
      }
    }
    $value[] = '</dd>' . PHP_EOL . '</dl>';
    $entity->field_transcript_html[$lang][0]['value'] = implode(' ', $value);
    $entity->field_transcript_html[$lang][0]['format'] = 'filtered_html';
  }
}
node_save($entity);
return;

?>
