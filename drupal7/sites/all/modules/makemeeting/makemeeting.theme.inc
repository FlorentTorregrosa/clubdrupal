<?php

/**
 * Returns HTML for an admin poll form for choices.
 */
function theme_makemeeting_choices($variables) {
  $form = $variables['form'];

  // Add appropriate CSS.
  drupal_add_css(drupal_get_path('module', 'makemeeting') . '/makemeeting.css');

  $rows = array();
  $headers = array(t('Date'));
  $headers_added = FALSE;

  foreach (element_children($form) as $key) {
    // Build the table rows.
    $row = array();
    $row[] = array(
      'data'  => drupal_render($form[$key]['chdate']) . drupal_render($form[$key]['chremove']),
      'class' => 'makemeeting-choice-date',
    );
    $delta = 0;
    foreach (element_children($form[$key]['chsuggestions']) as $key2) {
      $delta++;
      // Add sugesstion headers for the first line
      if (!$headers_added) {
        $headers[] = t('Suggestion %num', array('%num' => $delta));
      }
      $row[] = drupal_render($form[$key]['chsuggestions'][$key2]);
    }
    $headers_added = TRUE;
    $rows[] = $row;
  }

  $output = theme('table', array(
    'header'     => $headers,
    'rows'       => $rows,
    'attributes' => array('id' => 'poll-choice-table')
  ));
  $output .= drupal_render_children($form);
  return $output;
}


/**
 * Function used to render a single answer row as a form
 */
function theme_makemeeting_answer_row($vars) {
  $form = $vars['form'];
  $element = array(
    '#prefix'     => '<table><tr>',
    'first_cell'  => array(),
    'other_cells' => array('#markup' => ''),
    '#suffix'     => '</tr></table>',
  );

  // Render suggestions
  foreach (element_children($form['answers']) as $answer_key) {
    $element['other_cells']['#markup'] .= _theme_table_cell(array(
      'data'  => drupal_render($form['answers'][$answer_key]),
      'class' => $form['answers'][$answer_key]['#type'],
    ));
  }

  // Render name, submit and the rest of hidden children
  $first_cell = drupal_render($form['name']) . drupal_render($form['submit']);
  $first_cell .= drupal_render_children($form);
  $element['first_cell']['#markup'] = _theme_table_cell($first_cell);

  return drupal_render($element);
}


/**
 * Function used to render answers' table
 */
function theme_makemeeting_answers($vars) {
  global $user;
  $form = $vars['form'];
  $item = $form['#item'];

  // Add appropriate CSS.
  drupal_add_css(drupal_get_path('module', 'makemeeting') . '/makemeeting.css');

  // Initialize variables
  $header = $day_cell = $row_form = $tree = $rows = array();

  // First construct tree of suggestions for headers
  foreach ($item['choices'] as $choice) {
    $chdate = _makemeeting_date_timestamp($choice['chdate']);
    $month = format_date($chdate, 'custom', 'F Y');
    $day = format_date($chdate, 'custom', 'D. j');
    $count = 0;
    foreach ($choice['chsuggestions'] as $text) {
      if (!$text && isset($tree[$month][$day]) && $count) {
        continue;
      }
      elseif (!$text) {
        $text = "-";
      }
      $tree[$month][$day][] = $text;
      $count++;
    }
  }

  // Now construct headers based on tree
  $headers = $row_days = $row_suggestions = array('');
  // Add month-related classes
  $months_total = count($tree);
  $months_count = 0;
  foreach ($tree as $month => $days) {
    $header['colspan'] = 0;
    $header['data'] = $month;

    $header['class'] = array('date-month');
    _makemeeting_define_class_suffix($header, $months_count, $months_total, 'date-month');
    $months_count++;

    // Add day-related classes
    $days_total = count($days);
    $days_count = 0;
    foreach ($days as $day => $suggestions) {
      $day_cell['colspan'] = 0;
      $day_cell['data'] = $day;

      $day_cell['class'] = array('date');
      _makemeeting_define_class_suffix($day_cell, $days_count, $days_total, 'date');
      $days_count++;

      // Add suggestion-related classes
      $suggestions_total = count($suggestions);
      $suggestion_count = 0;
      foreach ($suggestions as $text) {
        $suggestion = array('data' => $text, 'class' => array('suggestion'));
        _makemeeting_define_class_suffix($suggestion, $suggestion_count, $suggestions_total, 'suggestion');
        $suggestion_count++;

        $row_suggestions[] = $suggestion;

        // Set colspan
        $header['colspan']++;
        $day_cell['colspan']++;
      }
      $row_days[] = $day_cell;
    }
    $headers[] = $header;
  }
  array_push($rows, $row_days, $row_suggestions);

  // Initialize totals labels
  $totals = array();
  $options = _makemeeting_options($item['yesnomaybe']);
  foreach ($options as $key => $label) {
    $totals[$key]['table'] = array(
      array(
        'data'  => t('Totals ' . $label),
        'class' => 'total-title'
      )
    );
    foreach ($item['choices'] as $choice) {
      $count = 0;
      foreach ($choice['chsuggestions'] as $text) {
        if ($text || (!$text && !$count)) {
          // Initialize totals count
          $totals[$key]['count'][] = 0;
        }
        $count++;
      }
    }
  }

  // Display already submitted answers
  $select = db_select('makemeeting_answers', 'ma')
    ->fields('ma');
  foreach (array(
             'field_name',
             'entity_type',
             'deleted',
             'entity_id',
             'language',
             'delta'
           ) as $info) {
    $select->condition($info, $form[$info]['#value']);
  }
  // Filter out answer being edited
  if (!empty($form['answer_edited'])) {
    $select->condition('answer_id', $form['answer_edited']['#value']->answer_id, '!=');
  }
  $results = $select->execute()
                    ->fetchAllAssoc('answer_id');
  $uids = array();
  foreach ($results as $result) {
    $uids[] = $result->uid;
    if (!$item['is_hidden']) {
      $row = array();
      // Display name of the person that has answered
      $cell = _makemeeting_render_cell_name($result->uid, $result->name, $result->answer_id);
      $row[] = drupal_render($cell);

      // Display his/her answers
      $answers = unserialize($result->value);
      foreach (element_children($form['answers']) as $answer_key) {
        $row_count = count($row);

        // Determine choice based on choice type
        if ($item['one_option']) {
          $key = $answers == $answer_key ? MAKEMEETING_YES : MAKEMEETING_NO;
        }
        else {
          $key = isset($answers[$answer_key]) ? intval($answers[$answer_key]) : FALSE;
        }
        $label = isset($options[$key]) ? $options[$key] : FALSE;

        if ($key !== FALSE && $label !== FALSE) {
          // Add corresponding classes for the answer
          $classes = array('answer', 'answer-' . $key);
          foreach (array(
                     'suggestion-first',
                     'suggestion-medium',
                     'suggestion-last'
                   ) as $class) {
            if (in_array($class, $row_suggestions[$row_count]['class'])) {
              $classes[] = str_replace('suggestion', 'answer', $class);
            }
          }

          // Store answer (will an background image according to class)
          $row[] = array(
            'data'  => '<span>' . $label . '</span>',
            'class' => $classes,
          );

          // Increment key answer counter
          $totals[$key]['count'][$row_count - 1]++;
        }
        else {
          $row[] = t('No answer');
        }
      }
      $rows[] = array(
        'data'  => $row,
        'id'    => 'answer-' . $result->answer_id,
        'class' => array('in' . (empty($cell['#editable']) ? '' : ' editable')),
      );
    }
  }

  if (!$item['is_hidden']) {
    // Add participant count
    $rows[1][0] = format_plural(count($results), '@count participant', '@count participants');
  }

  // Display answer form if ... not limited,
  $limited = $item['limit'] > 0 && $item['limit'] * count($row_suggestions) - 1 <= count($results);
  // ... not closed,
  $closed = $item['closed'] || $limited;
  $submitted = user_is_logged_in() && in_array($user->uid, array_values($uids));
  // ... user has access and has not already submitted
  $show_form = user_access(MAKEMEETING_ANSWER_PERM) && !$closed && !$submitted;
  if ($show_form) {
    $row_form[] = drupal_render($form['name']);
    foreach (element_children($form['answers']) as $answer_key) {
      $row_form[] = array(
        'data'  => drupal_render($form['answers'][$answer_key]),
        'class' => $form['answers'][$answer_key]['#type'],
      );
    }
    $rows[] = array(
      'data'  => $row_form,
      'class' => array('answer-form-row'),
    );
  }

  // Get total maxima
  $totals_max = array();
  foreach ($totals as $key => &$table) {
    if (empty($table['count'])) {
      $table['count'][] = 0;
    }
    $totals_max[$key] = max($table['count']);
  }
  // Display totals
  foreach ($options as $key => $label) {
    foreach ($totals[$key]['count'] as $choice => $count) {
      $classes = array('total-count', 'total-' . $key);
      if ($count) {
        // Indicate first / last suggestion item
        foreach (array(
                   'suggestion-first',
                   'suggestion-medium',
                   'suggestion-last'
                 ) as $class) {
          if (in_array($class, $row_suggestions[$choice + 1]['class'])) {
            $classes[] = str_replace('suggestion', 'total', $class);
          }
        }
      }
      // Indicate max count
      if (!empty($totals_max[$key]) && $count == $totals_max[$key]) {
        $classes[] = 'total-max';
      }
      $totals[$key]['table'][] = array('data' => $count, 'class' => $classes);
    }
    $classes = array('total-row');

    // Change labels for two choices
    if (!$item['yesnomaybe']) {
      $totals[$key]['table'][0]['data'] = t('Totals');
    }
    // Do not display other totals for two choices
    if ($item['yesnomaybe'] || !$item['yesnomaybe'] && $key == MAKEMEETING_YES) {
      $classes[] = 'total-row-' . ($key ? ($key == MAKEMEETING_YES ? 'last' : 'medium') : 'first');
      $rows[] = array('data' => $totals[$key]['table'], 'class' => $classes);
    }
  }

  // Apply classes to the two first rows
  $rows[0] = array('data' => $rows[0], 'class' => array('dates'));
  $rows[1] = array('data' => $rows[1], 'class' => array('suggestions'));

  $suggestion = array(
    '#theme'      => 'table',
    '#header'     => $headers,
    '#rows'       => $rows,
    '#attributes' => array('class' => array('makemeeting-table')),
  );
  if ($show_form) {
    return drupal_render($suggestion) . drupal_render_children($form);
  }
  return drupal_render($suggestion);
}

/**
 * Render the cell containing the submitter's name with
 * a possible option to delete the answer
 */
function _makemeeting_render_cell_name($uid, $name, $answer_id) {
  global $user;
  if ($uid) {
    $account = user_load($uid);
    $username = theme('username', array('account' => user_load($account->uid)));
  }
  else {
    $username = $name;
  }
  if (!user_access(MAKEMEETING_DELETE_PERM) && !user_access(MAKEMEETING_EDIT_PERM) && (user_is_anonymous() || $uid != $user->uid)) {
    return array(
      '#markup' => $username,
    );
  }
  $images_path = drupal_get_path('module', 'makemeeting') . '/images/';
  $cell[] = array(
    '#prefix'     => '<div class="answer-edit">',
    'delete-link' => array(
      '#type'    => 'link',
      '#title'   => theme_image(array(
        'path'       => $images_path . 'trash.png',
        'attributes' => array('title' => t('Delete answer'))
      )),
      '#href'    => 'makemeeting/delete-answer/' . $answer_id,
      '#options' => array(
        'query'      => drupal_get_destination(),
        'html'       => TRUE,
        'attributes' => array('rel' => 'nofollow'),
      ),
      '#access'  => user_access(MAKEMEETING_DELETE_PERM) || $uid == $user->uid,
    ),
    'edit-link'   => array(
      '#type'    => 'link',
      '#title'   => theme_image(array(
        'path'       => $images_path . 'pen.png',
        'attributes' => array('title' => t('Edit answer'))
      )),
      // Note the /nojs portion of the href - if javascript is enabled,
      // this part will be stripped from the path before it is called.
      '#href'    => 'makemeeting/edit-answer/' . $answer_id . '/nojs/',
      '#options' => array(
        'query'      => drupal_get_destination(),
        'html'       => TRUE,
        'attributes' => array('rel' => 'nofollow'),
      ),
      '#ajax'    => array(
        'wrapper' => 'answer-' . $answer_id,
        'method'  => 'html',
      ),
      '#access'  => user_access(MAKEMEETING_EDIT_PERM) || $uid == $user->uid,
    ),
    '#suffix'     => '</div>',
    '#attached'   => array(
      'js' => array(
        drupal_get_path('module', 'makemeeting') . '/makemeeting.js',
      )
    ),
  );
  $cell[] = array('#markup' => $username);
  return array(
    '#markup'   => '<div class="answer-editable">' . drupal_render($cell) . '</div>',
    '#editable' => TRUE,
  );
}

/**
 * Helper function to define classes based on a count and a total
 *
 * @param $element
 * @param $count
 * @param $total
 * @param string $prefix
 */
function _makemeeting_define_class_suffix(&$element, $count, $total, $prefix = '') {
  if ($prefix) {
    $prefix .= '-';
  }
  if ($count == 0) {
    $element['class'][] = $prefix . 'first';
  }
  elseif ($count == ($total - 1)) {
    $element['class'][] = $prefix . 'last';
  }
  else {
    $element['class'][] = $prefix . 'medium';
  }
}
