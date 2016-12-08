var user = (function() {
    var mcoUserToken;

    var setMcoUserToken = function(token) {
        mcoUserToken = token;
    }

    var getMcoUserToken = function() {
        return mcoUserToken;
    }

    var loadToken = function() {
        setMcoUserToken(webpage.get_mco_token());
    }

    var loginForToken = function(form, callbackSuccess, callbackError) {
        var username = form[0][0].value;
        var password = form[0][1].value;
        var version = webpage.get_version();

        $.post({
            url: urlUtils.publicApi('login/ckeck'),
            data: {
                username: username,
                password: password,
                app: 'mc-gui',
                version: version
            }
        }).done(function(data) {
            setMcoUserToken(data.key);
            webpage.save_mco_token(data.key);
            if (undefined !== callbackSuccess) {
                functionsUtils.executeFunctionByName(callbackSuccess, window);
            }
        }).fail(function (jqXHR) {
            if (undefined !== callbackError) {
                functionsUtils.executeFunctionByName(callbackError, window, jqXHR);
            }
        });
    };

    var pushPolicyToMCO = function(policy, visibility) {
        $.post({
            url: urlUtils.protectedApi('policies/import'),
            data: {
                xml: webpage.policy_get_xml(policy),
                visibility: visibility
            },
            beforeSend: function(xhr) {
                xhr.setRequestHeader('X-apiKey', getMcoUserToken());
            }
        }).done(function(data) {
            mcoMessage.success('Successfuly added to MediaConchOnline');
        }).fail(function (jqXHR) {
            var data = JSON.parse(jqXHR.responseText);
            if (undefined !== data.message) {
                mcoMessage.error(data.message);
            }
            else {
                mcoMessage.error('Error');
            }
        });
    }

    var policyUnpublish = function(policyId, button) {
        /**
        * Unpublish a policy
        * @param int id policy ID of the policy to unpublish
        *
        * @return json
        * {"policyId":ID}
        */
        $.ajax({
            url: urlUtils.protectedApi('publicpolicies/unpublish/' + policyId),
            method: 'PUT',
            beforeSend: function(xhr) {
                xhr.setRequestHeader('X-apiKey', getMcoUserToken());
            }
        })
        .done(function(data) {
            unpublishPolicy.success(button);
        })
        .fail(function (jqXHR) {
            unpublishPolicy.error(button);
        })
    };

    getPoliciesList = function() {
        /**
         * User policies list
         *
         * @return json
         */
        $.get({
            url: urlUtils.protectedApi('userpolicies/list'),
            beforeSend: function(xhr) {
                xhr.setRequestHeader('X-apiKey', getMcoUserToken());
            }
        })
        .done(function(data) {
            userPoliciesModal.displayPoliciesList(data.list);
        })
        .fail(function (jqXHR) {
            userPoliciesModal.policiesListError();
        })
    };

    getPolicy = function(policyId, button) {
        $.get({
            url: urlUtils.protectedApi('userpolicies/policy/' + policyId),
            dataType: 'text',
            beforeSend: function(xhr) {
                xhr.setRequestHeader('X-apiKey', getMcoUserToken());
            }
        })
        .done(function(data) {
            // Save policy
            var res = webpage.policy_import_data(data);
            var json = JSON.parse(res);
            if (json.error) {
                userPoliciesModal.policyImportError(button);
            }
            else {
                // Get policy to display in jstree
                var res = webpage.policy_get_jstree(json.policyId);
                var json = JSON.parse(res);
                if (json.error) {
                    userPoliciesModal.policyImportError(button);
                }
                else {
                    userPoliciesModal.policyImportSuccess(button);
                    policyTree.policyImport(json.policyTree);
                }
            }
        })
        .fail(function (jqXHR) {
            userPoliciesModal.policyImportError(button);
        })
    };

    return {
        loadToken: loadToken,
        getMcoUserToken: getMcoUserToken,
        loginForToken: loginForToken,
        pushPolicyToMCO: pushPolicyToMCO,
        policyUnpublish: policyUnpublish,
        getPoliciesList: getPoliciesList,
        getPolicy: getPolicy,
    };
})();
