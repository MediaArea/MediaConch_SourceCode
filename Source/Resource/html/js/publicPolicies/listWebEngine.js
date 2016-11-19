var publicPoliciesListAjax = (function() {
    var getList = function() {
        if (undefined === user.getMcoUserToken() || '' == user.getMcoUserToken()) {
            var apiAddress = urlUtils.publicApi("publicpolicies/list");
        }
        else {
            var apiAddress = urlUtils.protectedApi("publicpolicies/list");
        }

        /**
         * Get the json for jstree
         *
         * @return json
         * {"policiesTree":POLICIES_JSTREE_JSON}
         */
        $.get({
            url: apiAddress,
            dataType: "json",
            beforeSend: function(xhr) {
                if (undefined !== user.getMcoUserToken() && '' != user.getMcoUserToken()) {
                    xhr.setRequestHeader('X-apiKey', user.getMcoUserToken());
                }
            }
        }).done(function(data) {
            policyListSpinner.hide();
            publicPoliciesList.displayList(data.list);
        }).fail(function (jqXHR) {
            policyListSpinner.hide();
            mcoMessage.fail(jqXHR);
        });
    };

    var getApiPolicyUrl = function(policyId, policyUserId) {
        /**
         * Url of policy API
         * @param int policyId policy ID of the policy
         * @param int policyUserId user ID of the policy
         *
         * @return string
         * /api/public/v1/publicpolicies/policy/POLICYID/POLICYUSERID
         */
         return urlUtils.publicApi("publicpolicies/policy/" + policyId + "/" + policyUserId)
    };

    var policyExport = function(policyId, policyUserId) {
        /**
        * Export XML of a policy
        * @param int policyId policy ID of the policy to export
        * @param int policyUserId user ID of the policy to export
        *
        * @return XML
        */
        $.get({
            url: urlUtils.publicApi("publicpolicies/policy/export/" + policyId + "/" + policyUserId),
            dataType: "text",
            async: false
        }).done(function(data) {
            webpage.policy_export_data(data, function (res) {
                var json = JSON.parse(res);
                if (json.error)
                    mcoMessage.error(json.error);
            });
        }).fail(function (jqXHR) {
            mcoMessage.fail(jqXHR);
        });
    };

    var policyImport = function(policyId, policyUserId, button) {
        /**
        * Import a policy to user policies
        * @param int policyId policy ID of the policy to export
        * @param int policyUserId user ID of the policy to export
        *
        * @return json
        * {"policyId":ID}
        */
        $.get({
            url: urlUtils.publicApi("publicpolicies/policy/export/" + policyId + "/" + policyUserId),
            dataType: "text",
            async: false
        }).done(function(data) {
            webpage.policy_import_data(data, function (res) {
                var json = JSON.parse(res);
                if (json.error) {
                    importPolicy.error(button);
                }

                importPolicy.success(data.policyId, button);
            });
        }).fail(function (jqXHR) {
            importPolicy.error(button);
        });

    };

    var leave = function () {
    };

    return {
        getList: getList,
        getApiPolicyUrl: getApiPolicyUrl,
        policyExport: policyExport,
        policyImport: policyImport,
        leave: leave,
    };
})();
