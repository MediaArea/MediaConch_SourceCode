var checker = (function() {
    var initPage = function() {
        mcoMessage.init('#checkerInfo div');
        checkerTable.init();

        formBindings();
        applyPolicyToAll();
        bindings();
    }

    var formBindings = function() {
        // Upload form
        $('#file form').on('submit', function(e) {
            e.preventDefault();

            // Check file size
            if ( window.File )
            {
                var uploadFiles = $(this).find(':file');
            }

            checkerAjax.formRequest($(this), getDataFromForm($(this)), 'file');
        });

        // Online form
        $('#url form').on('submit', function(e) {
            e.preventDefault();
            checkerAjax.formRequest($(this), getDataFromForm($(this)), 'url');
        });

        // Repository form
        $('#repository form').on('submit', function(e) {
            e.preventDefault();
            checkerAjax.formRequest($(this), getDataFromForm($(this)), 'repository');
        });
    };

    // Get form data
    var getDataFromForm = function(form) {
        var formValues = {policy:form.find('.policyList').val(),
            policyText:form.find('.policyList option:selected').text(),
            display:form.find('.displayList').val(),
            verbosity:form.find('.verbosityList').val()
        };

        return formValues;
    };

    var applyPolicyToAll = function() {
        // Apply policy to all
        $('#checkerApplyAll').html('<div class="applyAll form-horizontal"></div>');
        $('#checkerApplyAll').addClass('tab-pane');

        var resetSelectList = function(listId) {
            $('#' + listId + ' option').prop('selected', false);
        };

        // Duplicate policy list
        var policyList = $('.tab-content .active .policyList').clone();
        policyList.attr('id', 'applyAllPolicy');
        policyList.children('option:first').text('Choose a new policy to apply');
        $('#checkerApplyAll div.applyAll').append('<div class="col-md-12"><div class="form-group"><label class="pull-left control-label">Apply a policy to all results</label><div class="col-sm-4 policy">')
        $('#checkerApplyAll div.applyAll div.policy').html(policyList);
        resetSelectList('applyAllPolicy');

        $('#applyAllPolicy').on('change', function(e) {
            checkerTable.applyPolicyToAll();
            resetSelectList('applyAllPolicy');
        });
    };

    var bindings = function() {
        // Scroll to the top of the results when page is changed
        $('#result-table').on('page.dt', function() {
            $('html, body').animate({
                scrollTop: $('#checkerResults').offset().top
            }, 200);
        });


        // Remove all results blocks
        $('#checkerResultTitle .close').click(function() {
            checkerTable.clear();
            checkerAjax.closeAll();

            // Remove close all button
            $(this).addClass('hidden');

            // Remove apply to all
            $('#checkerApplyAll').addClass('hidden');
        });
    };

    return {
        initPage: initPage,
        getDataFromForm: getDataFromForm,
    };
})();

$(document).ready(function() {
    (function loop_init(time) {
        setTimeout(function() {
            if (webpage === undefined)
                loop_init(100);
            else
            {
                checker.initPage();
            }
        }, time);
    })(0);
});
