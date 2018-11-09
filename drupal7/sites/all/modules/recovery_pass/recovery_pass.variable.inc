<?php
/**
 * @file
 *
 * Variable information for Recovery Password.
 */

/**
 * Implements hook_variable_info().
 */
function recovery_pass_variable_info($options) {
  $variables['recovery_pass_email_subject'] = array(
    'type' => 'string',
    'title' => t('Email Subject', array(), $options),
    'default' => _recovery_pass_mail_text('email_subject'),
    'description' => t('Recovery Password Email subject', array(), $options),
    'required' => TRUE,
    'localize' => TRUE,
    'group' => 'recovery_pass',
  );
  $variables['recovery_pass_email_text'] = array(
    'type' => 'text',
    'title' => t('Email Body', array(), $options),
    'default' => _recovery_pass_mail_text('email_text'),
    'description' => t('Recovery Password Email Body', array(), $options),
    'required' => TRUE,
    'localize' => TRUE,
    'group' => 'recovery_pass',
  );
  $variables['recovery_pass_old_pass_warning'] = array(
    'type' => 'text',
    'title' => t('Old Password Warning', array(), $options),
    'default' => _recovery_pass_mail_text('old_pass_warning'),
    'description' => t('Old Password Warning Message', array(), $options),
    'required' => TRUE,
    'localize' => TRUE,
    'group' => 'recovery_pass',
  );
  return $variables;
}

/**
 * Implements hook_variable_group_info().
 */
function recovery_pass_variable_group_info() {
  $groups['recovery_pass'] = array(
    'title' => t('Recovery Password'),
    'access' => 'administer users',
    'path' => 'admin/config/people/recovery-pass',
  );
  return $groups;
}
