<?php

/**
 * @file
 * Plugin to handle an existing pages content.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Existing page'),
  'single' => TRUE,
  'defaults' => array(
    'task_id',
  ),
  'category' => t('Existing pages'),
  'all contexts' => TRUE,
);

/**
 * Output function for the custom task page.
 */
function pm_existing_pages_pm_existing_pages_content_type_render($subtype, $conf, $panel_args) {

  $task = pm_existing_pages_get_existing_pages($conf['task_id']);
  if (!$task) {
    return;
  }

  $menu_item = menu_get_item();
  if (!empty($menu_item['page_arguments'])) {
    $args = $menu_item['page_arguments'];
  }

  if (empty($args)) {
    return;
  }

  $pm_bonus_args = array_pop($args);
  $function = $pm_bonus_args['pc'];
  $file = $pm_bonus_args['f'];
  $file_path = $pm_bonus_args['fp'];
  $module = $pm_bonus_args['m'];

  if ($file) {
    if (empty($file_path) && !empty($module)) {
      $file_path = drupal_get_path('module', $module);
    }
    require_once DRUPAL_ROOT . '/' . $file_path . '/' . $file;
  }
  $content = call_user_func_array($function, $args);

  $block = new stdClass();
  $block->module = 'pm_existing_pages';
  $block->title = check_plain($task->label);
  $block->content = $content;

  return $block;
}

/**
 * Select the task/page.
 */
function pm_existing_pages_pm_existing_pages_content_type_edit_form($form, &$form_state) {
  $options = array();

  $custom_tasks = pm_existing_pages_get_existing_pages();
  foreach ($custom_tasks as $task_id => $info) {
    $options[$task_id] = $info->label;
  }

  $form['task_id'] = array(
    '#required' => TRUE,
    '#title' => t('Existing page'),
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $form_state['conf']['task_id'],
  );

  return $form;
}

/**
 * Save the task selection.
 */
function pm_existing_pages_pm_existing_pages_content_type_edit_form_submit($form, &$form_state) {
  $form_state['conf']['task_id'] = $form_state['values']['task_id'];
}

/**
 * Returns the administrative title for the Apache Solr search page.
 */
function pm_existing_pages_pm_existing_pages_content_type_admin_title($subtype, $conf) {
  return t('Existing page');
}

/**
 * Display the administrative information for a content pane.
 */
function pm_existing_pages_pm_existing_pages_content_type_admin_info($subtype, $conf) {
  return t('This pane will render on front end.');
}
