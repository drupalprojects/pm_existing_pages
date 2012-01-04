
/**
 * @file
 * Page manager existing pages - some overrides for autocomplete.
 */

(function ($) {

  Drupal.jsAC.prototype.select = function (node) {
    var $value = $(node).data('autocompleteValue');
    if ($value == 'close_pm_existing_pages_suggestions') {
      Drupal.jsAC.prototype.hidePopup('close_pm_existing_pages_suggestions');
    }
    else {
      $current = $('#edit-paths').val();
      if ($current.length > 0) {
        var $array = $current.split("\n");
      }
      else {
        $array = new Array;
      }
      if ($.inArray($value, $array) == -1) {
        $array.push($value);
      }
      $('#edit-paths').val($array.join("\n"));
    }
  };  
  
  Drupal.jsAC.prototype.hidePopup = function (keycode) {
    if (keycode == 'close_pm_existing_pages_suggestions') {
      this.popup = null;
      $('#autocomplete').fadeOut('fast', function () { $('#autocomplete').remove(); });    
      $(this.ariaLive).empty();
      this.selected = false;
    }
  };
  
})(jQuery);