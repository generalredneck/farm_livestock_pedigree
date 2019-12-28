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

function farm_livestock_pedigree_individual_report(FarmAsset $farm_asset) {
  $output = '';
  $pedigree = farm_livestock_pedigree_get_pedigree_ancestor_data($farm_asset);
  $pedigree['_children'] = farm_livestock_pedigree_get_pedigree_children_data($farm_asset);

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

function farm_livestock_pedigree_get_pedigree_ancestor_data(FarmAsset $farm_asset,$parent_id = 0, $level = 1) {
  $animal_wrapper = entity_metadata_wrapper('farm_asset', $farm_asset);
  $dob = $animal_wrapper->field_farm_date->value();
  $sex = $animal_wrapper->field_farm_animal_sex->value();
  $pedigree = array(
    'name' => $animal_wrapper->name->value(),
    'id' => $parent_id . '_' . $animal_wrapper->getIdentifier(),
    'dob' => !empty($dob) ? format_date($dob, 'custom', 'm/d/Y') : 'unknown',
    'sex' => !empty($sex) ? $sex : 'unknown',
  );
  if (!empty($animal_wrapper->field_farm_animal_type)) {
    $pedigree['type'] = $animal_wrapper->field_farm_animal_type->name->value();
  }

  if (isset($animal_wrapper->field_farm_parent)) {
    $pedigree['_parents'] = array();
    foreach ($animal_wrapper->field_farm_parent as $parent) {
      $pedigree['_parents'][] = farm_livestock_pedigree_get_pedigree_ancestor_data($parent->value(), $pedigree['id'], $level + 1);
    }
  }
  return $pedigree;
}

function farm_livestock_pedigree_get_pedigree_children_data(FarmAsset $farm_asset,$level = 1) {
  $children = array();
  $animal_wrapper = entity_metadata_wrapper('farm_asset', $farm_asset);
  $query = new EntityFieldQuery();
  $results = $query->entityCondition('entity_type', 'farm_asset')
    ->entityCondition('bundle', 'animal')
    ->fieldCondition('field_farm_parent', 'target_id', $animal_wrapper->getIdentifier(), '=')
    ->propertyCondition('archived', '0')
    ->execute();
  if (!empty($results) && !empty($results['farm_asset'])) {
    foreach ($results['farm_asset'] as $row) {
      $child_wrapper = entity_metadata_wrapper('farm_asset', $row->id);
      $dob = $child_wrapper->field_farm_date->value();
      $sex = $child_wrapper->field_farm_animal_sex->value();
      $child = array(
        'name' => $child_wrapper->name->value(),
        'id' => $animal_wrapper->getIdentifier() . '_' . $child_wrapper->getIdentifier(),
        'dob' => !empty($dob) ? format_date($dob, 'custom', 'm/d/Y') : 'unknown',
        'sex' => !empty($sex) ? $sex : 'unknown',
      );
      if (!empty($child_wrapper->field_farm_animal_type)) {
        $child['type'] = $child_wrapper->field_farm_animal_type->name->value();
      }
      $child['_children'] = farm_livestock_pedigree_get_pedigree_children_data($child_wrapper->value(), $level + 1);
      $children[] = $child;
    }
  }
  return $children;
}
