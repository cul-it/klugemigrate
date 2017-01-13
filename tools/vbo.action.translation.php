<?php
/**
 * Add German Entity Translation to Existing Node
 */

//dsm($entity);
if (!in_array($entity->type, array('video','transcript','segment'))) return;
if (isset($entity->translations->data['de'])) {
  dsm('already translated: ' . $entity->nid);
  return;
}
$handler = entity_translation_get_handler('node', $entity, TRUE);
$translation = array(
  'translate' => 0,
  'status' => 1,
  'language' => 'de',
  'source' => 'en',
);
$handler->setTranslation($translation, $entity);
node_save($entity);
//dsm($entity);
return;

?>
