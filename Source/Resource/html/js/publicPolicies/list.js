function initPage() {
    mcoMessage.init('.displayMessages');
    modalBinding();

    // Resize policy header when screen is resized
    $(window).resize(function () {
        publicPoliciesList.policyDescriptionHeight();
    });
}

function modalBinding() {
    $('#modalPolicy').on('show.bs.modal', function (event) {
        var policyElem = $(event.relatedTarget).parent().parent().parent();

        policyTree.refreshData(policyElem.data('policy-id'), policyElem.data('user-id'));

        // Set policyId and userId in modal
        $(this).data('policy-id', policyElem.data('policy-id')).data('user-id', policyElem.data('user-id'));
    });

    $('.modalPolicyExport').on('click', function (event) {
        var policyElem = $('#modalPolicy');

        publicPoliciesListAjax.policyExport(policyElem.data('policy-id'), policyElem.data('user-id'));
    });

    $('.modalPolicyImport').on('click', function (event) {
        var policyElem = $('#modalPolicy');

        publicPoliciesListAjax.policyImport(policyElem.data('policy-id'), policyElem.data('user-id'), $(this));
    });
}

var publicPoliciesList = (function() {
    var displayList = function(list) {
        $.each(list, function(index, policy) {
            if (undefined === policy.license || '' == policy.license) {
                policy.license = 'Other';
            }

            $('#publicPoliciesList').append(
'<div class="col-xs-12 col-sm-6"> \
    <div class="policyBox" data-policy-id="' + policy.id + '" data-user-id="' + policy.user.id + '"> \
        <header> \
            <h4><a href="#" data-toggle="modal" data-target="#modalPolicy" title="View policy">' + policy.name + '</a></h4> \
            <span class="policyDescription">' + policy.description + '</span> \
        </header> \
        <p class="policyInfos"> \
            <span class="policyAuthor">Maintainer: ' + policy.user.name + '</span> \
            <span class="policyLicense">License: ' + policy.license + '</span> \
        </p> \
        <div class="policyActions"> \
            <button type="button" class="btn btn-default policyImport">Add to my policies</button> \
            <button type="button" class="btn btn-default pull-right policyExport">Export</button> \
        </div> \
    </div> \
</div>'
            );
        });

        policyDescriptionHeight();
        buttonBindings();
    };

    // Adapt height of policy header to have buttons on the same line
    var policyDescriptionHeight = function() {
        var previousElem;
        // Apply only on medium screen and up
        if ($(window).width() >= 768) {
            $('.policyBox header').each(function(index, elem) {
                if (index % 2) {
                    if (previousElem.clientHeight > elem.clientHeight) {
                        $(elem).height(previousElem.clientHeight);
                    }
                    else if (elem.clientHeight > previousElem.clientHeight) {
                        $(previousElem).height(elem.clientHeight);
                    }
                }
                else {
                    previousElem = elem;
                }
            });
        }
        else {
            $('.policyBox header').each(function(index, elem) {
                $(elem).height('auto');
            });
        }
    };

    var buttonBindings = function() {
        $('.policyExport').on('click', function (event) {
            var policyElem = $(this).parent().parent();

            publicPoliciesListAjax.policyExport(policyElem.data('policy-id'), policyElem.data('user-id'));
        });

        $('.policyImport').on('click', function (event) {
            var policyElem = $(this).parent().parent();

            publicPoliciesListAjax.policyImport(policyElem.data('policy-id'), policyElem.data('user-id'), $(this));
        });
    }

    return {
        displayList: displayList,
        policyDescriptionHeight: policyDescriptionHeight,
    };
})();

var importPolicy = (function() {
    var success = function(data, button) {
        button.fadeOut(200).replaceWith('<div class="alert alert-success alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-ok text-success" aria-hidden="true"></span> <a href="' + Routing.generate('app_xslpolicy_xslpolicytree') + '" target="_blank" title="View the new policy" class="alert-link">Policy</a> successfuly added</div>');
    }

    var error = function(button) {
        button.fadeOut(200).replaceWith('<div class="alert alert-danger alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span> Error policy not created</div>');
    }

    return {
        success: success,
        error: error,
    };
})();

var policyListSpinner = (function() {
    var show = function() {
        $('.publicPoliciesListSpinner').removeClass('hidden');
    };
    var hide = function() {
        $('.publicPoliciesListSpinner').addClass('hidden');
    };

    return {
        show: show,
        hide: hide,
    };
})();

var timer_for_ready = null;
$(document).ready(function () {
    (function loop_init(time) {
        timer_for_ready = setTimeout(function() {
            if (webpage === undefined)
                loop_init(100);
            else
            {
                timer_for_ready = null;
                initPage();
                publicPoliciesListAjax.getList();
            }
        }, time);
    })(0);
});


$(window).unload(function() {
    if (timer_for_ready)
        clearTimeout(timer_for_ready);
    publicPoliciesListAjax.leave();
});
