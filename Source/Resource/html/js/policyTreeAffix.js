var policyTreeAffix = (function () {
    var node = $('#policyFix');

    var init = function() {
        // Right panel affix
        $('div.content').css('min-height', function () {
            return $('.policyRightCol').outerHeight(true);
        })
        node.css('width', function () {
            return $('.policyRightCol').outerWidth(true);
        })

        node.affix({
            offset: {
                top: function () {
                    return $('#mco-navbar').outerHeight(true)
                },
                bottom: function () {
                    return ($('footer').outerHeight(true))
                }
            }
        })

        bindings();
    }

    var bindings = function() {
        node.on('affixed.bs.affix', function() {
            $('.affix').css('position', 'fixed');
            node.css('margin-top', '-100px');
        })

        node.on('affixed-top.bs.affix', function() {
            $('.affix-top').css('position', 'relative');
            node.css('margin-top', 0);
        })

        node.on('affixed-bottom.bs.affix', function() {
            $('.affix-bottom').css('position', 'relative');
        })
    }

    return {
        init: init,
    }
})();
