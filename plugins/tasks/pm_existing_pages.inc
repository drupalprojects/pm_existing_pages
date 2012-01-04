<?php

/**
 * @file
 * Handle the custom Page manager custom tasks.
 */

/**
 * Specialized implementation of hook_page_manager_task_tasks(). See api-task.html for
 * more information.
 */
function pm_existing_pages_pm_existing_pages_page_manager_tasks() {

  return array(
    // This is a 'page' task and will fall under the page admin UI
    'task type' => 'page',
    'title' => t('Existing page'),

    // There are multiple pages, let's override each of them
    // separately.
    'subtasks' => TRUE,
    'subtask callback' => 'pm_existing_pages_pm_existing_pages_subtask',
    'subtasks callback' => 'pm_existing_pages_pm_existing_pages_subtasks',

    // Menu hooks so that we can alter the node/%node menu entry to point to us.
    'hook menu alter' => 'pm_existing_pages_pm_existing_pages_menu_alter',

    // This is task uses 'context' handlers and must implement these to give the
    // handler data it needs.
    'handler type' => 'context',
  );
}

/**
 * Callback defined by pm_existing_pages_pm_existing_pages_page_manager_tasks().
 *
 * Override the page callbacks for custom tasks.
 */
function pm_existing_pages_pm_existing_pages_menu_alter(&$items, $task) {

  $custom_tasks = pm_existing_pages_get_existing_pages();
  foreach ($custom_tasks as $task_id => $task_info) {

    if (variable_get('pm_existing_pages_disabled_' . $task_id, TRUE)) {
      continue;
    }

    $paths = explode("\n", $task_info->paths);
    foreach ($paths as $path) {
      $path = trim($path);

      // Make sure we have a path.
      if (empty($path) || !isset($items[$path])) {
        drupal_set_message(t('%path has not been overridden because the menu item does not exist.', array('%path' => $path)));
        continue;
      }

      // Add more page arguments.
      $pm_args = array(
        'ti' => $task_id,
        'pc' => $items[$path]['page callback'],
        'f' => isset($items[$path]['file']) ? $items[$path]['file'] : '',
        'fp' => isset($items[$path]['file path']) ? $items[$path]['file path'] : '',
        'm' => isset($items[$path]['module']) ? $items[$path]['module'] : '',
      );
      $page_arguments = isset($items[$path]['page arguments']) ? $items[$path]['page arguments'] : array();
      $page_arguments[] = $pm_args;
      $items[$path]['page callback'] = 'pm_existing_pages_pm_existing_pages_page';
      $items[$path]['page arguments'] = $page_arguments;
      $items[$path]['file path'] = $task['path'];
      $items[$path]['file'] = $task['file'];
    }
  }
}

/**
 * Entry point for our overridden task page.
 */
function pm_existing_pages_pm_existing_pages_page() {
  $args = func_get_args();

  $pm_args = array_pop($args);
  $task_id = $pm_args['ti'];
  $function = $pm_args['pc'];
  $file = $pm_args['f'];
  $file_path = $pm_args['fp'];
  $module = $pm_args['m'];

  // Load task plugin.
  $task = page_manager_get_task('pm_existing_pages');
  $subtask = page_manager_get_task_subtask($task, $task_id);

  // Render through CTools.
  ctools_include('context');
  ctools_include('context-task-handler');
  $output = ctools_context_handler_render($task, $subtask, array(), array());
  if ($output !== FALSE) {
    return $output;
  }

  // Fallback.
  if ($file) {
    if (empty($file_path) && !empty($module)) {
      $file_path = drupal_get_path('module', $module);
    }
    require_once DRUPAL_ROOT . '/' . $file_path . '/' . $file;
  }
  return call_user_func_array($function, $args);
}

/**
 * Callback to enable/disable the page from the UI.
 */
function pm_existing_pages_pm_existing_pages_enable($cache, $status) {
  variable_set('pm_existing_pages_disabled_' . $cache->subtask_id, $status);

  // Set a global flag so that the menu routine knows it needs
  // to set a message if enabling cannot be done.
  if (!$status) {
    $GLOBALS['page_manager_enabling_search'] = TRUE;
  }
}

/**
 * Task callback to get all subtasks.
 */
function pm_existing_pages_pm_existing_pages_subtasks($task) {
  $return = array();

  $custom_tasks = pm_existing_pages_get_existing_pages();
  foreach ($custom_tasks as $task_id => $info) {
    $return[$task_id] = pm_existing_pages_pm_existing_pages_build_subtask($task, $task_id);
  }

  return $return;
}

/**
 * Callback to return a single subtask.
 */
function pm_existing_pages_pm_existing_pages_subtask($task, $subtask_id) {
  return pm_existing_pages_pm_existing_pages_build_subtask($task, $subtask_id);
}

/**
 * Build a subtask array for a given page.
 */
function pm_existing_pages_pm_existing_pages_build_subtask($task, $task_id) {
  $info = pm_existing_pages_get_existing_pages($task_id);
  $paths = explode("\n", $info->paths);
  $main_path = $paths[0];
  $subtask = array(
    'name' => $task_id,
    'admin title' => check_plain($info->label),
    'admin path' => check_plain($main_path),
    'admin description' => check_plain($info->label),
    'admin type' => t('Existing'),
    'row class' => empty($page->disabled) ? 'page-manager-enabled' : 'page-manager-disabled',
    'storage' => t('In code'),
    'disabled' => variable_get('pm_existing_pages_disabled_' . $task_id, TRUE),
    'enable callback' => 'pm_existing_pages_pm_existing_pages_enable',
  );

  return $subtask;
}