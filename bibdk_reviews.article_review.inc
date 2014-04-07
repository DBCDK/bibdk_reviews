<?php

/** Ajax submit callback. Returns rendered reviews
 * @param $form
 * @param $form_state
 * @return array
 */
function bibdk_reviews_article_ajax_trigger_form_submit($form, $form_state) {
  $ids = $form['ajax_trigger']['#ids'];
  $entity = _bibdk_reviews_get_work_entity_from_manifestation_ids($ids);
  $ids = $entity->getArticleReviews();
  $commands = array(
    ajax_command_replace('#' . $form['#id'], bibdk_reviews_get_view_for_articles($ids))
  );

  return array('#type' => 'ajax', '#commands' => $commands);
}


/** Get views for a number of articles. Takes an array of article ids as argument. Returns rendered html
 * @param $ids
 * @return null|string
 */
function bibdk_reviews_get_view_for_articles($ids) {
  $no_reviews = array(
    '#theme' => 'worktabs_no_content',
    '#no_content' => t('no reviews', array(), array('context' => 'bibdk_reviews')),
  );
  $no_reviews = drupal_render($no_reviews);

  if (!isset($ids) || !count($ids)) {
    return $no_reviews;
  }
  $works = ting_openformat_get_manifestations($ids);

  /* @var $work BibdkWork */
  $work = reset($works);
  $manifestations = $work->getManifestations();
  if (!count($manifestations)) {
    return $no_reviews;
  }

  foreach ($manifestations as $manifestation) {
    $view [] = bibdk_reviews_article_view($manifestation);
  }

  return drupal_render($view);

}

/**
 * Get render array for a single article
 *
 * @param Manifestation $manifestation
 * @return array
 */
function bibdk_reviews_article_view($manifestation) {
  if ($url = $manifestation->getInfomediaLink()) {
    $link = _ting_infomedia_get_link($manifestation->id, $url);
    $link = theme('ting_infomedia_button', array('ting_infomedia_button' => $link));
  }
  else if ($url = $manifestation->getAccessInformation()) {
    $request_method = $manifestation->getRequestMethod();
    $text_string = isset($request_method) ? 'review_link_text_' . $request_method : 'review_link_text';
    $link = _ting_openformat_parse_element($manifestation->getAccessInformation(), '.', TRUE, TRUE, '<span class="icon icon-left icon-darkgrey-infomedia">&nbsp;</span>' . t($text_string, array(), array('context' => 'bibdk_reviews')));
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
    '#link' => isset($link) ? $link : '',
    '#rating' => bibdk_reviews_get_rating($rating),
  );

  return ting_openformat_manifestation_view($manifestation);
}


/** Create rating render array
 * @param $rating
 * @return array
 */
function bibdk_reviews_get_rating($rating) {
  if (!isset($rating) || !strpos($rating, '/')) {
    return;
  }

  $values = explode('/', $rating);

  $i = 0;

  while ($i < $values[1]) {
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
