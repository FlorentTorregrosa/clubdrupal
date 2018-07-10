<?php

/**
 * @file
 * This file is mostly about the field configuration.
 */

/**
 * Implements hook_field_info().
 */
function makemeeting_field_info() {
  return array(
    'makemeeting' => array(
      'label'             => t('Makemeeting form'),
      'description'       => t('This field stores user choices about some dates in the database.'),
      'default_widget'    => 'makemeeting_choices',
      'default_formatter' => 'makemeeting_answers',
    ),
  );
}

/**
 * Implements hook_field_widget_info().
 */
function makemeeting_field_widget_info() {
  return array(
    'makemeeting_choices' => array(
      'label'       => t('Makemeeting choices'),
      'field types' => array('makemeeting'),
      'behaviors'   => array(
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_settings_form().
 */
function makemeeting_field_widget_settings_form($field, $instance) {
  $widget = $instance['widget'];
  $settings = $widget['settings'];

  $form = array();
  if ($widget['type'] == 'makemeeting_choices') {
    $form['size'] = array(
      '#type'             => 'textfield',
      '#title'            => t('Number of dates'),
      '#default_value'    => isset($settings['size']) ? $settings['size'] : 1,
      '#element_validate' => array('element_validate_integer'),
      '#required'         => TRUE,
      '#size'             => 5,
      '#description'      => t('Default number of date choices to display. Must be positive or equal to 0.')
    );
  }

  return $form;
}

/**
 * Implements hook_field_widget_form().
 */
function makemeeting_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $element += array(
    '#type' => 'fieldset',
  );

  $item =& $items[$delta];

  // Simple options form elements about the poll
  $element['hidden'] = array(
    '#title'         => t('Hidden poll'),
    '#description'   => t('Confidential participation: only you and administrators can see the answers.'),
    '#type'          => 'checkbox',
    '#default_value' => isset($item['hidden']) ? $item['hidden'] : '',
  );
  $element['one_option'] = array(
    '#title'         => t('Participant can only choose one option'),
    '#description'   => t('By default all options are selectable. This setting limits the choice to one option per participant.'),
    '#type'          => 'checkbox',
    '#default_value' => isset($item['one_option']) ? $item['one_option'] : '',
  );
  $element['limit'] = array(
    '#title'            => t('Limit the number of participants per option'),
    '#description'      => t('Poll as registration form: As soon as the indicated limit has been reached, the respective option is no longer available. 0 for unlimited'),
    '#type'             => 'textfield',
    '#default_value'    => isset($item['limit']) ? $item['limit'] : 0,
    '#element_validate' => array('element_validate_integer'),
  );
  $element['yesnomaybe'] = array(
    '#title'         => t('Provide a "Maybe" option'),
    '#description'   => t('Provide a third "Maybe" option in case users may be available.'),
    '#type'          => 'checkbox',
    '#default_value' => isset($item['yesnomaybe']) ? $item['yesnomaybe'] : '',
    // Hide if one_option is checked
    '#states'        => array(
      'visible' => array(
        ':input[name$="[one_option]"]' => array('checked' => FALSE),
      ),
    ),
  );
  $element['closed'] = array(
    '#title'         => t('Closed'),
    '#description'   => t('Check this when you want to close the poll.'),
    '#type'          => 'checkbox',
    '#default_value' => isset($item['closed']) ? $item['closed'] : '',
  );

  // Now let's take care of the choices part
  // $process is a callback that we will attach to the wrapper in order
  // to add parents to the form elements. This is so we can store
  // nested values in the DB table with field storage API.
  $fieldset_info = element_info('fieldset');
  $process = array_merge($fieldset_info['#process'], array('_makemeeting_rebuild_parents'));
  $element['choices_wrapper'] = array(
    '#tree'    => FALSE,
    '#prefix'  => '<div class="clearfix" id="makemeeting-choices-wrapper">',
    '#suffix'  => '</div>',
    '#process' => $process,
  );

  // Container just for the poll choices.
  $element['choices_wrapper']['choices'] = array(
    '#prefix' => '<div id="makemeeting-choices">',
    '#suffix' => '</div>',
    '#theme'  => 'makemeeting_choices',
  );

  // Fill the item values with previously submitted values
  if (!empty($form_state['values'])) {
    $submitted_values = drupal_array_get_nested_value($form_state['values'], $element['#field_parents']);
    $submitted_values = $submitted_values[$element['#field_name']][$element['#language']][$element['#delta']];
    if (isset($submitted_values['choices'])) {
      $item['choices'] = $submitted_values['choices'];
    }
  }

  // Calculate the suggestion count
  if (empty($form_state['suggestion_count'])) {
    $form_state['suggestion_count'] = 1;
  }
  if (isset($item['choices'])) {
    foreach ($item['choices'] as $choice) {
      if ($form_state['suggestion_count'] < count($choice['chsuggestions'])) {
        $form_state['suggestion_count'] = count($choice['chsuggestions']);
      }
    }
  }

  // Determine choice count if not provided
  if (!isset($form_state['choice_count'])) {
    try {
      list($entity_id) = entity_extract_ids($element['#entity_type'], $element['#entity']);
    } catch (Exception $e) {
      // Fail silently if the field is not attached to any entity yet.
    }
    // If the entity isn't created yet, offer the default number of choices,
    // else the number of previous choices, or 0 if none
    if (!isset($entity_id)) {
      $widget = $instance['widget'];
      $settings = $widget['settings'];
      $form_state['choice_count'] = isset($settings['size']) ? $settings['size'] : 1;
    }
    else {
      $form_state['choice_count'] = isset($item['choices']) ? count($item['choices']) : 0;
    }
  }

  // Add the current choices to the form.
  $delta = 0;
  if (isset($item['choices']) && $form_state['choice_count']) {
    // Sort choices by their dates
    asort($item['choices']);
    $delta = count($item['choices']);
    foreach ($item['choices'] as $key => $choice) {
      $element['choices_wrapper']['choices'][$key] =
        _makemeeting_choice_form($key, $choice['chdate'], $choice['chsuggestions'], $form_state['suggestion_count']);
    }
  }

  // Add choices as required.
  if ($form_state['choice_count']) {
    // Add a day to the last option's timestamp for the new default date
    $last_option_timestamp = isset($choice['chdate']) ? _makemeeting_date_timestamp($choice['chdate']) : REQUEST_TIME;
    $existing_delta = $delta;
    for (; $delta < $form_state['choice_count'] ; $delta++) {
      $additional_stamp = strtotime('+1 day', $last_option_timestamp);
      // Build default date
      $default_date = array(
        'month' => date('n', $additional_stamp),
        'day'   => date('j', $additional_stamp),
        'year'  => date('Y', $additional_stamp)
      );
      $key = 'new:' . ($delta - $existing_delta);
      $element['choices_wrapper']['choices'][$key] =
        _makemeeting_choice_form($key, $default_date, NULL, $form_state['suggestion_count']);
      // Repeat
      $last_option_timestamp = $additional_stamp;
    }
  }

  // We prefix our buttons with 'makemeeting' to avoid conflicts
  // with other modules using Ajax-enabled buttons with the id 'more'.
  $default_submit = array(
    '#type'                    => 'submit',
    '#limit_validation_errors' => array(array('choices')),
    '#submit'                  => array('makemeeting_choices_submit'),
    '#ajax'                    => array(
      'callback' => 'makemeeting_choice_js',
      'wrapper'  => 'makemeeting-choices-wrapper',
      'effect'   => 'fade',
    ),
  );
  $element['choices_wrapper']['makemeeting_more_choices'] = $default_submit + array(
      '#value'      => t('More choices'),
      '#attributes' => array(
        'title' => t("If the amount of rows above isn't enough, click here to add more choices."),
      ),
      '#weight'     => 1,
    );
  $element['choices_wrapper']['makemeeting_more_suggestions'] = $default_submit + array(
      '#value'      => t('More suggestions'),
      '#attributes' => array(
        'title' => t("If the amount of boxes above isn't enough, click here to add more suggestions."),
      ),
      '#weight'     => 2,
    );
  if ($form_state['choice_count'] > 1) {
    $element['choices_wrapper']['makemeeting_copy_suggestions'] = $default_submit + array(
        '#value'      => t('Copy and paste first row'),
        '#attributes' => array(
          'title' => t("Copy the suggestions from the first to the other rows."),
        ),
        '#weight'     => 3,
      );
  }
  return $element;
}

/**
 * Helper function to construct a row of the widget form
 *
 * @param $key
 * @param string $value
 * @param array $suggestions
 * @param int $size
 * @return array
 */
function _makemeeting_choice_form($key, $value = '', $suggestions = array(), $size = 1) {
  $form = array(
    '#tree' => TRUE,
  );

  // We'll manually set the #parents property of these fields so that
  // their values appear in the $form_state['values']['choices'] array
  // They are to be updated later in #process as we don't know here the
  // hierarchy of the parents
  $form['chdate'] = array(
    '#type'          => 'date',
    '#title'         => t('Date'),
    '#title_display' => 'invisible',
    '#default_value' => $value,
    '#parents'       => array('choices', $key, 'chdate'),
  );
  $form['chremove'] = array(
    '#type'                    => 'submit',
    '#parents'                 => array(),
    '#submit'                  => array('makemeeting_choices_submit'),
    '#limit_validation_errors' => array(array('choices')),
    '#ajax'                    => array(
      'callback' => 'makemeeting_choice_js',
      'wrapper'  => 'makemeeting-choices',
      'effect'   => 'fade',
    ),
    '#value'                   => t('Remove'),
    // Assigning key as name to make each 'Remove' button unique
    '#name'                    => $key,
  );

  $form['chsuggestions'] = array(
    '#tree'    => TRUE,
    '#parents' => array('choices', $key, 'chsuggestions'),
  );

  for ($i = 0 ; $i < $size ; $i++) {
    $key2 = 'sugg:' . ($i);
    $form['chsuggestions'][$key2] = array(
      '#type'          => 'textfield',
      '#title_display' => 'invisible',
      '#default_value' => isset($suggestions[$key2]) ? $suggestions[$key2] : '',
      '#size'          => 5,
      '#maxlength'     => 255,
      '#parents'       => array('choices', $key, 'chsuggestions', $key2),
    );
  }
  return $form;
}

/**
 * Helper function to update the form elements parents. This is required
 * for the element to be saved properly by the Field API (see #process)
 *
 * @param $element
 * @return mixed
 */
function _makemeeting_rebuild_parents(&$element) {
  $parents = array_diff($element['#array_parents'], $element['#parents']);
  $elements = array(
    'makemeeting_more_choices',
    'makemeeting_more_suggestions',
    'makemeeting_copy_suggestions'
  );
  foreach ($elements as $e) {
    $element[$e]['#parents'] = $parents;
    if (isset($element[$e]['#limit_validation_errors'])) {
      $element[$e]['#limit_validation_errors'][0] = array_merge($parents,
        $element[$e]['#limit_validation_errors'][0]);
    }
  }
  $element['makemeeting_more_suggestions']['#parents'] = $parents;
  $element['makemeeting_copy_suggestions']['#parents'] = $parents;
  foreach (element_children($element['choices']) as $key1) {
    foreach (element_children($element['choices'][$key1]) as $key2) {
      $keys = element_children($element['choices'][$key1][$key2]);
      if (empty($keys)) {
        $element['choices'][$key1][$key2]['#parents'] = array_merge($parents,
          $element['choices'][$key1][$key2]['#parents']);
      }
      else {
        foreach ($keys as $key3) {
          $element['choices'][$key1][$key2][$key3]['#parents'] = array_merge($parents,
            $element['choices'][$key1][$key2][$key3]['#parents']);
        }
      }
    }
  }
  return $element;
}

/**
 * Implements hook_field_validate().
 */
function makemeeting_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  foreach ($items as $delta => $item) {
    if (isset($item['choices'])) {
      // Verify that we do not have duplicate dates
      $dates = array();
      foreach ($item['choices'] as $chid => $chvalue) {
        $date = implode('', $chvalue['chdate']);
        if (in_array($date, $dates)) {
          // Register an error with the choice id
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error'   => 'duplicate_dates',
            'message' => t('%name: you can\'t have duplicate dates.', array('%name' => $instance['label'])),
            'chid'    => $chid,
          );
        }
        else {
          $dates[] = $date;
        }
      }
    }
  }
}

/**
 * Implements hook_field_widget_error().
 */
function makemeeting_field_widget_error($element, $error) {
  // Show our error concerning duplicate dates here
  $choices = $element['choices_wrapper']['choices'];
  form_error($choices[$error['chid']]['chdate'], $error['message']);
}


/**
 * Implements hook_field_is_empty().
 */
function makemeeting_field_is_empty($item) {
  return empty($item['choices']);
}

/**
 * Implements hook_field_formatter_info().
 */
function makemeeting_field_formatter_info() {
  return array(
    'makemeeting_answers' => array(
      'label'       => t('Makemeeting answers'),
      'field types' => array('makemeeting'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 *
 * Build a renderable array for a field value.
 *
 * @param $entity_type
 *   The type of $entity.
 * @param $entity
 *   The entity being displayed.
 * @param $field
 *   The field structure.
 * @param $instance
 *   The field instance.
 * @param $langcode
 *   The language associated with $items.
 * @param $items
 *   Array of values for this field.
 * @param $display
 *   The display settings to use, as found in the 'display' entry of instance
 *   definitions. The array notably contains the following keys and values;
 *   - type: The name of the formatter to use.
 *   - settings: The array of formatter settings.
 *
 * @return array
 *  A renderable array for the $items, as an array of child elements keyed
 */
function makemeeting_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $content = array();
  foreach ($items as $delta => $item) {
    list($entity_id) = entity_extract_ids($entity_type, $entity);
    $instance += array(
      'entity_id' => $entity_id,
      'language'  => $langcode,
      'delta'     => $delta
    );
    // Define answers hidden status here as we have $field variable available
    $item['is_hidden'] = (bool) $item['hidden'];
    if ($item['is_hidden']) {
      $access = field_access('edit', $field, $entity_type, $entity);
      if ($entity_type == 'node') {
        $access = $access && node_access('update', $entity);
      }
      elseif (module_exists('entity')) {
        $access = $access && entity_access('edit', $entity_type, $entity);
      }
      $item['is_hidden'] = $item['is_hidden'] && !$access;
    }
    $content[] = drupal_get_form('makemeeting_answers_form_' . $instance['entity_id'], $item, $instance);
  }
  return $content;
}

/**
 * Implements hook_field_load().
 */
function makemeeting_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  // Unserialize array values
  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => $item) {
      if ($field['type'] == 'makemeeting' && is_string($items[$id][$delta]['choices'])) {
        $items[$id][$delta]['choices'] = unserialize($items[$id][$delta]['choices']);
      }
    }
  }
}

/**
 * Implements hook_field_presave().
 */
function makemeeting_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  if ($field['type'] == 'makemeeting') {
    foreach ($items as $delta => $item) {
      if (isset($item['choices'])) {
        // Sort the choices by date
        $choices = array_values($item['choices']);
        usort($choices, '_makemeeting_sort_choices');
        // Affect timestamp as keys
        $timestamped_choices = array();
        foreach ($choices as $choice) {
          $timestamped_choices[_makemeeting_date_timestamp($choice['chdate'])] = $choice;
        }
        // Serialize the whole array
        $items[$delta]['choices'] = serialize($timestamped_choices);
      }
    }
  }
}

/**
 * Helper function to sort choice dates
 */
function _makemeeting_sort_choices($a, $b) {
  if ($a['chdate']['year'] == $b['chdate']['year']) {
    if ($a['chdate']['month'] == $b['chdate']['month']) {
      if ($a['chdate']['day'] == $b['chdate']['day']) {
        return 0;
      }
      else {
        return (int) $a['chdate']['day'] < (int) $b['chdate']['day'] ? -1 : 1;
      }
    }
    else {
      return (int) $a['chdate']['month'] < (int) $b['chdate']['month'] ? -1 : 1;
    }
  }
  else {
    return (int) $a['chdate']['year'] < (int) $b['chdate']['year'] ? -1 : 1;
  }
}


/**
 * Submit handler to add more choices or suggestions to a poll form,
 * or to copy the first row of suggestions to the others
 *
 * This handler is run regardless of whether JS is enabled or not. It makes
 * changes to the form state. If the button was clicked with JS disabled, then
 * the page is reloaded with the complete rebuilt form. If the button was
 * clicked with JS enabled, then ajax_form_callback() calls makemeeting_choice_js() to
 * return just the changed part of the form.
 */
function makemeeting_choices_submit($form, &$form_state) {
  $clicked_button = $form_state['clicked_button'];

  // Get the element
  $element =& drupal_array_get_nested_value($form_state['input'], $clicked_button['#parents']);

  // If clicked button is 'Remove', unset corresponding choice
  if ($clicked_button['#value'] == t('Remove')) {
    unset($element['choices'][$clicked_button['#name']]);
    $form_state['choice_count'] = count($element['choices']);
  }

  // Affect timestamp as key to each choice
  $choices = array();
  if (isset($element['choices'])) {
    foreach ($element['choices'] as $choice) {
      $choices['choices'][_makemeeting_date_timestamp($choice['chdate'])] = $choice;
    }
  }

  // Handle other operations
  if (!empty($form_state['values']['op'])) {
    switch ($form_state['values']['op']) {
      // Add 1 more choice to the form.
      case t('More choices'):
        // Increment choice count
        $form_state['choice_count'] = count($element['choices']) + 1;
        break;

      // Add 1 more suggestion to the form.
      case t('More suggestions'):
        $form_state['suggestion_count']++;
        break;

      // Copy first row suggestions to the other rows.
      case t('Copy and paste first row');
        $first = reset($choices['choices']);
        foreach (array_keys($choices['choices']) as $key) {
          $choices['choices'][$key]['chsuggestions'] = $first['chsuggestions'];
        }
        break;
    }
  }

  // Replace submitted values by our own work
  drupal_array_set_nested_value($form_state['input'], $clicked_button['#parents'], $choices);
  drupal_array_set_nested_value($form_state['values'], $clicked_button['#parents'], $choices);

  // Require the widget form to rebuild the choices
  // with the values in $form_state
  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax callback in response to new choices being added to the form.
 *
 * This returns the new page content to replace the page content made obsolete
 * by the form submission.
 */
function makemeeting_choice_js($form, $form_state) {
  // Get the element from the parents of the clicked button
  $element = drupal_array_get_nested_value($form, $form_state['clicked_button']['#parents']);
  return $element['choices_wrapper'];
}
