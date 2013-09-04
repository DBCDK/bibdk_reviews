<?php

/** Ajax submit callback. Returns rendered reviews
 * @param $form
 * @param $form_state
 * @return array
 */
function bibdk_reviews_article_ajax_trigger_form_submit($form, $form_state){
  $ids = $form['ajax_trigger']['#ids'];
  $entity = _bibdk_reviews_get_work_entity_from_manifestation_ids($ids);
  $ids = $entity->getArticleReviews();
  $commands = array(
    ajax_command_replace('#'.$form['#id'], bibdk_reviews_get_article_view($ids))
  );
  return array('#type' => 'ajax', '#commands' => $commands);
}



function bibdk_reviews_get_article_view($ids){
  if (!isset($ids) || !count($ids))
    return t('no reviews 1', array(), array('context' => 'bibdk_reviews'));
  $work = ting_openformat_get_manifestations($ids);

  $work = reset($work);
  $manifestations = $work->getManifestations();
  if (!count($manifestations)){
    return t('no reviews 2', array(), array('context' => 'bibdk_reviews'));
  }

  foreach($manifestations as $manifestation){
    $view []= bibdk_reviews_article_view($manifestation);
  }

  return drupal_render($view);

}

function bibdk_reviews_article_view(Manifestation $manifestation){

  if ($url = $manifestation->getInfomediaLink()) {
    $link = _ting_infomedia_get_link($manifestation->id, $url);
    $link = theme('ting_infomedia_button', array('ting_infomedia_button' => $link));
  } else if ( $url = $manifestation->getAccessInformation()){
    $link = _ting_openformat_parse_element($manifestation->getAccessInformation(),'.', TRUE, TRUE, t('litteratursiden_link', array(), array('context' => 'bibdk_reviews')));
  }

  $rating = $manifestation->getReviewRatings();

  $title = _ting_openformat_parse_element($manifestation->getTitle());
  $title_url = l($title, 'work/' . $manifestation->id);
  $creator = $manifestation->getCreator();
  $host_publication = $manifestation->getHostPublication();
  $publisher = isset($host_publication) ? $host_publication : $manifestation->getPublisher();
  return array(
    '#theme' => 'bibdk_review_article_review',
    '#title' => $title_url,
    '#creator' => t('reviewed by !creator', array('!creator' => _ting_openformat_parse_element($creator)), array('context' => 'bibdk_reviews')),
    '#publisher' => t('in !publisher', array('!publisher' => _ting_openformat_parse_element($publisher)), array('context' => 'bibdk_reviews')),
    '#link' => $link,
    '#rating' => bibdk_reviews_get_rating($rating),
  );

  return ting_openformat_manifestation_view($manifestation);
}



function bibdk_reviews_get_rating($rating){
  if (!isset($rating) || !strpos($rating, '/')){
    return;
  }

  $values = explode('/', $rating);

  $i = 0;

  while($i < $values[1]){
    $type = ($i < $values[0]) ? 'positive' : 'negative';
    $stars[] = array(
      '#type' => 'html_tag',
      '#tag' => 'span',
      '#value' => '*',
      '#attributes' => array(
        'class' => array('star', $type)
      ),
    );
    $i++;
  }

  return $stars;
}
