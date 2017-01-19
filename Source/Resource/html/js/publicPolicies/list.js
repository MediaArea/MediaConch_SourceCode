function initPage() {
    user.loadToken();
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

        // Add to my policies or unpublish button
        if (true === policyElem.data('allow-edit')) {
            $('.modalPolicyImport').hide();
            $('.alert-modal-create-policy').hide();
            $('.modalPolicyUnpublish').show();
        }
        else {
            $('.modalPolicyUnpublish').hide();
            $('.modalPolicyImport').show();
        }
    });

    $('.modalPolicyExport').on('click', function (event) {
        var policyElem = $('#modalPolicy');

        publicPoliciesListAjax.policyExport(policyElem.data('policy-id'), policyElem.data('user-id'));
    });

    $('.modalPolicyImport').on('click', function (event) {
        var policyElem = $('#modalPolicy');

        publicPoliciesListAjax.policyImport(policyElem.data('policy-id'), policyElem.data('user-id'), $(this));
    });

    $('.modalPolicyUnpublish').on('click', function (event) {
        var policyElem = $('#modalPolicy');

        user.policyUnpublish(policyElem.data('policy-id'), $(this));
    });
}

var publicPoliciesList = (function() {
    var displayList = function(list) {
        $.each(list, function(index, policy) {
            if (undefined === policy.license || '' == policy.license) {
                policy.license = 'Other';
            }

            // Start wrapper line
            if (0 == index % 2) {
                $('#publicPoliciesList').append('<div class="clearfix">');
            }

            // Add to my policies or unpublish button
            if (true === policy.allowEdit) {
                var userButton = '<button type="button" class="btn btn-default policyUnpublish" title="Remove my policy from the public policies list">Unpublish</button>'
            }
            else {
                var userButton = '<button type="button" class="btn btn-default policyImport">Add to my policies</button>';
            }

            $('#publicPoliciesList').append(
'<div class="col-xs-12 col-sm-6"> \
    <div class="policyBox policyBox-' + policy.id + '" data-policy-id="' + policy.id + '" data-user-id="' + policy.user.id + '" data-allow-edit="' + policy.allowEdit + '"> \
        <header> \
            <h4><a href="#" data-toggle="modal" data-target="#modalPolicy" title="View policy">' + policy.name + '</a></h4> \
            <span class="policyDescription">' + policy.description + '</span> \
        </header> \
        <p class="policyInfos"> \
            <span class="policyAuthor">Maintainer: ' + policy.user.name + '</span> \
            <span class="policyLicense">License: ' + policy.license + '</span> \
        </p> \
        <div class="policyActions row text-left"> \
            ' + userButton + ' \
            <button type="button" class="btn btn-default policyExport pull-right">Export</button> \
        </div> \
    </div> \
</div>'
            );

            // End wrapper line
            if (1 == index % 2) {
                $('#publicPoliciesList').append('</div>');
            }
            else {
                if (list.length == (index + 1)) {
                    $('#publicPoliciesList').append('</div>');
                }
            }
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

        $('.policyUnpublish').on('click', function (event) {
            var policyElem = $(this).parent().parent();

            user.policyUnpublish(policyElem.data('policy-id'), $(this));
        });
    }

    return {
        displayList: displayList,
        policyDescriptionHeight: policyDescriptionHeight,
    };
})();

var importPolicy = (function() {
    var success = function(data, button) {
        button.fadeOut(200).replaceWith('<div class="alert alert-success alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-ok text-success" aria-hidden="true"></span> Policy successfuly added</div>');
    }

    var error = function(button) {
        button.fadeOut(200).replaceWith('<div class="alert alert-danger alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span> Error policy not created</div>');
    }

    return {
        success: success,
        error: error,
    };
})();

var unpublishPolicy = (function () {
    var success = function(button) {
        // Disable button and links in policyBox and modal
        if (button.hasClass('policyUnpublish')) {
            button.parent().find('button').prop('disabled', true);
            button.parent().parent().addClass('policyBoxDisabled').find('h4 a').on('click', function(e) {
                e.preventDefault();
                e.stopPropagation();
            });
        }
        else {
            button.parent().find('button.btn-primary').prop('disabled', true);
            var policyBoxId = $('#modalPolicy').data('policy-id');
            $('.policyBox-' + policyBoxId).addClass('policyBoxDisabled').find('button').prop('disabled', true);
            $('.policyBox-' + policyBoxId).find('h4 a').on('click', function(e) {
                e.preventDefault();
                e.stopPropagation();
            });

        }
       button.hide().after('<div class="alert alert-success alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-ok text-success" aria-hidden="true"></span> Policy successfuly unpublished</div>');
    }

    var error = function(button) {
        button.hide().after('<div class="alert alert-danger alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span> Error policy not unpublished</div>');
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
                setTimeout(function() {
                    publicPoliciesListAjax.getList();

                }, 100)
            }
        }, time);
    })(0);
});


$(window).unload(function() {
    if (timer_for_ready)
        clearTimeout(timer_for_ready);
    publicPoliciesListAjax.leave();
});
