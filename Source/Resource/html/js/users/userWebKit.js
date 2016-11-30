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

    var pushPolicyToPublicPoliciesOnMCO = function(policy) {
        $.post({
            url: urlUtils.protectedApi('publicpolicies/import'),
            data: {
                xml: webpage.policy_get_xml(policy)
            },
            beforeSend: function(xhr) {
                xhr.setRequestHeader('X-apiKey', getMcoUserToken());
            }
        }).done(function(data) {
            mcoMessage.success('Successfuly added to public policies list');
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

    return {
        loadToken: loadToken,
        getMcoUserToken: getMcoUserToken,
        loginForToken: loginForToken,
        pushPolicyToPublicPoliciesOnMCO: pushPolicyToPublicPoliciesOnMCO,
        policyUnpublish: policyUnpublish,
    };
})();
