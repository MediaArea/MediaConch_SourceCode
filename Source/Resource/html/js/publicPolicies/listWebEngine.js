var publicPoliciesListAjax = (function() {
    var address = "https://mediaarea.net/MediaConchOnline/api/public/v1/publicpolicies/";
    var getList = function() {
        /**
         * Get the json for jstree
         *
         * @return json
         * {"policiesTree":POLICIES_JSTREE_JSON}
         */
        $.get({
            url: address + "list",
            dataType: "json",
            async: false
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
        return address + "policy/" + policyId + "/" + policyUserId;
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
            url: address + "policy/export/" + policyId + "/" + policyUserId,
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
            url: address + "policy/export/" + policyId + "/" + policyUserId,
            dataType: "text",
            async: false
        }).done(function(data) {
            webpage.policy_import_data(data, function (res) {
                var json = JSON.parse(res);
                if (json.error)
                    mcoMessage.error(json.error);
            });
        }).fail(function (jqXHR) {
            mcoMessage.fail(jqXHR);
        });

    };

    return {
        getList: getList,
        getApiPolicyUrl: getApiPolicyUrl,
        policyExport: policyExport,
        policyImport: policyImport,
    };
})();
