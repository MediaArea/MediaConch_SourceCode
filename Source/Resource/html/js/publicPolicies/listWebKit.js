var publicPoliciesListAjax = (function() {
    var address = "https://mediaarea.net/MediaConchOnline/api/public/v1/publicpolicies/";
    var get_cb = null;

    var getList = function() {
        /**
         * Get the json for jstree
         *
         * @return json
         * {"policiesTree":POLICIES_JSTREE_JSON}
         */
        var cb = $.get({
            url: address + "list",
            dataType: "json"
        }).done(function(data) {
            publicPoliciesListAjax.setGetCb(null);
            policyListSpinner.hide();
            publicPoliciesList.displayList(data.list);
        }).fail(function (jqXHR) {
            publicPoliciesListAjax.setGetCb(null);
            policyListSpinner.hide();
            mcoMessage.fail(jqXHR);
        });
        publicPoliciesListAjax.setGetCb(cb);
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
            var res = webpage.policy_export_data(data);
            var json = JSON.parse(res);
            if (json.error)
                mcoMessage.error(json.error);
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
            var res = webpage.policy_import_data(data);
            var json = JSON.parse(res);
            if (json.error)
                mcoMessage.error(json.error);
        }).fail(function (jqXHR) {
            mcoMessage.fail(jqXHR);
        });

    };

    var setGetCb = function(cb) {
        get_cb = cb;
    };

    var getGetCb = function(cb) {
        return get_cb;
    };

    var leave = function() {
        if (get_cb)
        {
            get_cb.abort();
            setGetCb(null);
        }
    };

    return {
        getList: getList,
        getApiPolicyUrl: getApiPolicyUrl,
        policyExport: policyExport,
        policyImport: policyImport,
        getGetCb: getGetCb,
        setGetCb: setGetCb,
        leave: leave
    };
})();
