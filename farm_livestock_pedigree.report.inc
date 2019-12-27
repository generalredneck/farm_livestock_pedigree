<?php

/**
 * @file
 * Farm Livestock pedigree report page.
 */

/**
 * Asset Report view callback.
 *
 * @param FarmAsset $farm_asset
 *   The farm asset entity.
 *
 * @return
 *   Returns the page content.
 */

const MAX_PEDIGREE_DEPTH = 4;

function farm_livestock_pedigree_individual_report(FarmAsset $farm_asset) {
  $output = '';
  $pedigree = farm_livestock_pedigree_get_pedigree_data($farm_asset);

  $settings = array(
    'farm_livestock_pedigree' => array(
      'pedigree_data' => $pedigree,
    ),
  );

  $output = '<div class="farm-pedigree"></div>';
  // Add JS and CSS to build the graphs.
  drupal_add_js($settings, 'setting');
  drupal_add_js('//d3js.org/d3.v3.min.js', 'external');
  drupal_add_js(drupal_get_path('module', 'farm_livestock_pedigree') . '/farm_livestock_pedigree.js', array('scope' => 'footer'));
  drupal_add_css(drupal_get_path('module', 'farm_livestock_pedigree') . '/farm_livestock_pedigree.css');
  return $output;
}

function farm_livestock_pedigree_get_pedigree_data(FarmAsset $farm_asset, $level = 1) {
  $animal_wrapper = entity_metadata_wrapper('farm_asset', $farm_asset);
  $dob = $animal_wrapper->field_farm_date->value();
  $sex = $animal_wrapper->field_farm_animal_sex->value();
  $pedigree = array(
    'name' => $animal_wrapper->name->value(),
    'dob' => !empty($dob) ? format_date($dob, 'custom', 'm/d/Y') : 'unknown',
    'sex' => !empty($sex) ? $sex : 'unknown',
  );
  if (!empty($animal_wrapper->field_farm_animal_type)) {
    $pedigree['type'] = $animal_wrapper->field_farm_animal_type->name->value();
  }

  if ($level != MAX_PEDIGREE_DEPTH && isset($animal_wrapper->field_farm_parent)) {
    $pedigree['parents'] = array();
    foreach ($animal_wrapper->field_farm_parent as $parent) {
      $pedigree['parents'][] = farm_livestock_pedigree_get_pedigree_data($parent->value(), $level + 1);
    }
  }
  return $pedigree;
}
