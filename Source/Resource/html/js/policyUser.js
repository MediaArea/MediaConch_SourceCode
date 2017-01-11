var userConnectModal = (function() {
    var show = function() {
        $('#userConnectModal').modal('show');
    }

    var hide = function() {
        $('#userConnectModal').modal('hide');
    }

    var formSuccess = function() {
        // Close modal
        hide();

        // Export policy to MCO
        if ('pushPublic' == getAction()) {
            user.pushPolicyToMCO(policyTree.getPolicyId(), 'public');
        }
        else if ('pushPrivate' == getAction()) {
            user.pushPolicyToMCO(policyTree.getPolicyId(), 'private');
        }
        else if ('userPolicyList' == getAction()) {
            userPoliciesModal.show();
        }
    }

    var formError = function(error) {
        $('.userConnectModalError').html('<div class="alert alert-danger alert-dismissible"> \
        <button type="button" class="close" data-dismiss="alert" aria-label="Close"><span aria-hidden="true">&times;</span></button> \
        Login fail, please check your login and password</div>');
    }

    var setAction = function(action) {
        $('#userConnectModal').data('action', action);
    }

    var getAction = function() {
        return $('#userConnectModal').data('action');
    }

    return {
        show: show,
        hide: hide,
        formSuccess: formSuccess,
        formError: formError,
        setAction: setAction,
    }
})();

var userPoliciesModal = (function() {
    var show = function() {
        $('#userPoliciesModal').modal('show');
        user.getPoliciesList();
    }

    var displayPoliciesList = function(list) {
        $('#userPoliciesList').empty();
        if (0 == list.length) {
            $('#userPoliciesList').html('<div class="col-md-12"><div class="alert alert-info" role="alert">You have no policies available on MediaConchOnline</div></div>');
        }
        else {
            $.each(list, function(index, policy) {
                $('#userPoliciesList').append(
'<div class="col-xs-12 policiesLineWrapper"> \
    <div class="policyBox policyBox-' + policy.id + '"> \
        <header> \
            <h4>' + policy.name + '</h4> \
            <span class="policyDescription">' + policy.description + '</span> \
        </header> \
        <div class="policyActions row text-center"> \
            <button type="button" class="btn btn-default userPolicyImport" data-policy-id="' + policy.id + '">Import this policy</button> \
        </div> \
    </div> \
</div>'
                );
            });
        }

        buttonBindings();
    }

    var policiesListError = function() {
        $('#userPoliciesList').html('<div class="col-md-12"><div class="alert alert-danger" role="alert">Error cannot load your policies from MediaConchOnline</div></div>');
    }

    var buttonBindings = function() {
        $('.userPolicyImport').on('click', function(event) {
            user.getPolicy($(this).data('policy-id'), $(this));
        })
    }

    var policyImportSuccess = function(button) {
        button.fadeOut(200).replaceWith('<div class="alert alert-success alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-ok text-success" aria-hidden="true"></span> Policy imported successfuly</div>');
    }

    var policyImportError = function(button) {
        button.fadeOut(200).replaceWith('<div class="alert alert-danger alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span> Error policy not imported</div>');
    }

    return {
        show: show,
        displayPoliciesList: displayPoliciesList,
        policiesListError: policiesListError,
        policyImportSuccess: policyImportSuccess,
        policyImportError: policyImportError,
    }
})();
