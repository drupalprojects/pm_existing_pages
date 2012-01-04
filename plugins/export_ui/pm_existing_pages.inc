<?php

/**
 * @file
 * Page manager existing pages Export UI.
 */

$plugin = array(
  'schema' => 'page_manager_existing_pages',
  'access' => 'use page manager',

  'menu' => array(
    'menu prefix' => 'admin/structure/pages',
    'menu item' => 'existing-pages',
    'menu title' => 'Existing pages',
    'menu description' => 'Administer existing pages.',
  ),

  'title singular' => t('existing page'),
  'title singular proper' => t('Existing task'),
  'title plural' => t('existing pages'),
  'title plural proper' => t('Existing pages'),

  'form' => array(
    'settings' => 'pm_existing_pages_ctools_export_ui_form',
    'validate' => 'pm_existing_pages_ctools_export_ui_form_validate',
    'submit' => 'pm_existing_pages_ctools_export_ui_form_submit',
  ),
);

/**
 * CTools export ui form callback for existing pages.
 */
function pm_existing_pages_ctools_export_ui_form(&$form, &$form_state) {

  $form['#attached']['js'][] = drupal_get_path('module', 'pm_existing_pages') . '/js/pm_existing_pages.js';
  $form['#attached']['css'][] = drupal_get_path('module', 'pm_existing_pages') . '/css/pm_existing_pages.css';

  $form['info']['name']['#maxlength'] = 64;
  $form['info']['name']['#type'] = 'machine_name';
  $form['info']['name']['#machine_name'] = array(
    'exists' => 'pm_existing_pages_task_unique',
    'source' => array('label'),
  );

  $form['label'] = array(
    '#id' => 'edit-label',
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => $form_state['item']->label,
  );

  $form['paths'] = array(
    '#type' => 'textarea',
    '#title' => t('Paths'),
    '#description' => t('Add all paths to override per line. The first path will be used as the main path in the administration overview of page manager. Note that sometimes, you need to replace % depending on the load arguments.'),
    '#default_value' => $form_state['item']->paths,
    '#required' => TRUE,
  );

  $form['path_helper'] = array(
    '#type' => 'textfield',
    '#title' => t('Search paths'),
    '#description' => t('Start typing to find existing menu router items. Clicking on a path will add this to the "Paths" textarea above.'),
    '#autocomplete_path' => 'admin/structure/existing-pages/find',
    '#size' => 120,
  );
}

/**
 * Return whether a task machine name is unique.
 */
function pm_existing_pages_task_unique($name) {
  return pm_existing_pages_get_existing_pages($name);
}

/**
 * Menu callback to search for menu router items.
 */
function pm_existing_pages_find_paths($search = '') {

  $matches = array();

  if (strlen($search) > 2) {
    // Reconvert the '/'.
    $search = str_replace(chr(27), '/', $search);

    // Select max 10 paths.
    $records = db_select('menu_router', 'mr')
      ->fields('mr', array('path', 'title', 'page_callback', 'load_functions'))
      ->condition('page_callback', 'page_manager_page_execute', '!=')
      ->condition('page_callback', 'pm_existing_pages_pm_existing_pages_page', '!=')
      ->condition('path', '%' . $search . '%', 'LIKE')
      ->extend('PagerDefault')
      ->limit(10)
      ->execute();

    foreach ($records as $record) {
      $matches[$record->path] = '<div class="pm-path"><strong>'. t('Path') . '</strong>: ' . $record->path . '</div><div class="pm-callback"><strong>' . t('Page callback') . ':</strong> ' . $record->page_callback . '</div><div class="clearfix" />';
    }
  }

  if (!empty($matches)) {
    $matches['close_pm_existing_pages_suggestions'] = '<div>' . t('Close popup') . '</div>';
  }

  drupal_json_output($matches);
}

/**
 * CTools export UI validate callback for existing pages.
 */
function pm_existing_pages_ctools_export_ui_form_validate(&$form, &$form_state) {
  $error = FALSE;
  $paths = explode("\n", $form_state['values']['paths']);
  foreach ($paths as $path) {
    $path = trim($path);
    if (empty($path)) {
      $error = TRUE;
      break;
    }
  }
  if ($error) {
    form_set_error('paths', t('There is an empty path, please correct this.'));
  }
}

/**
 * CTools export UI submit callback for existing pages.
 */
function pm_existing_pages_ctools_export_ui_form_submit(&$form, &$form_state) {}

