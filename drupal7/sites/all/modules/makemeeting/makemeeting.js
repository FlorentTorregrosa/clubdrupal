(function ($) {
  Drupal.behaviors.makemeetingHoverButtons = {
    attach: function (context) {
      var $trs = $('.makemeeting-table', context).find('tr.editable');
      $trs.removeClass('in').hover(function () {
        // This will hide all other tr
        $trs.filter(':not(.hovering)').removeClass('in');
        $(this).addClass('in hovering');
      }, function () {
        // Hide links after a while
        $(this).removeClass('hovering');
        setTimeout(function () {
          $trs.filter(':not(.hovering)').removeClass('in');
        }, 500);
      });
    }
  };

  Drupal.behaviors.makemeetingFormWidth = {
    attach: function (context) {
      if ($(context).is('.editing')) {
        $(context).next().find('td').each(function (index) {
          $('form td', context).eq(index).css('width', $(this).width() + 'px');
        });
      }
    }
  };
}(jQuery));
