var policyTreeAjax = (function() {
    var getData = function() {
        /**
         * Get the json for jstree
         *
         * @return json
         * {"policiesTree":POLICIES_JSTREE_JSON}
         */
    alert("policyTree");
        // $.get(Routing.generate('app_xslpolicy_xslpolicytreedata'))
        // .done(function(data) {
        //     policyTree.setData(data.policiesTree);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    };

    var policyImport = function(form) {
        /**
         * Import a policy from an XML (the XML is provided as POST data from a form)
         *
         * @return json
         * {"policy":POLICY_JSTREE_JSON}
         */
        // $.ajax({
        //     type: form.attr('method'),
        //         url: Routing.generate('app_xslpolicy_xslpolicytreeimport'),
        //         data: new FormData(form[0]),
        //         processData: false,
        //         contentType: false
        // })
        // .done(function (data) {
        //     policyTree.policyImport(data.policy);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var policyCreate = function(policyNode, parentId = -1) {
        /**
         * Create a policy
         * @param int parentId policy ID in which the new policy will be created
         *
         * @return json
         * {"policy":POLICY_JSTREE_JSON}
         */
        // $.get(Routing.generate('app_xslpolicy_xslpolicytreecreate', {parentId: parentId}))
        // .done(function (data) {
        //     policyTree.policyCreate(data.policy, policyNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var policyEdit = function(form, policyNode) {
        /**
         * Edit a policy (POST data from a form)
         * @param int id policy ID of the policy to edit
         *
         * @return json
         * {"policy":POLICY_JSTREE_JSON}
         */
        // $.ajax({
        //     type: form.attr('method'),
        //         url: Routing.generate('app_xslpolicy_xslpolicytreeedit', {id: policyNode.data.policyId}),
        //         data: new FormData(form[0]),
        //         processData: false,
        //         contentType: false
        // })
        // .done(function (data) {
        //     policyTree.policyEdit(data.policy, policyNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var policyDelete = function(policyNode) {
        /**
         * Delete a policy
         * @param int id policy ID of the policy to duplicate
         *
         * @return json
         * {"policyId":ID}
         */
        // $.get(Routing.generate('app_xslpolicy_xslpolicytreedelete', {id: policyNode.data.policyId}))
        // .done(function (data) {
        //     policyTree.policyDelete(policyNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var policyExport = function(policyNode) {
        /**
        * Export XML of a policy
        * @param int id policy ID of the policy to export
        *
        * @return XML
        */
        // window.location = Routing.generate('app_xslpolicy_xslpolicytreeexport', {id: policyNode.data.policyId});
    }

    var policyDuplicate = function(policyNode, dstNode) {
        /**
         * Duplicate a policy
         * @param int id policy ID of the policy to duplicate
         * @param int dstPolicyId policy ID of the destination policy
         *
         * @return json
         * {"policy":POLICY_JSTREE_JSON}
         */
        // $.get(Routing.generate('app_xslpolicy_xslpolicytreeduplicate', {id: policyNode.data.policyId, dstPolicyId: policyTree.getPolicyId(dstNode)}))
        // .done(function (data) {
        //     policyTree.policyDuplicate(data.policy, dstNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var policyMove = function(policyNode, dstNode) {
        /**
         * Move a policy
         * @param int id policy ID of the policy to duplicate
         * @param int dstPolicyId policy ID of the destination policy
         *
         * @return json
         * {"policy":POLICY_JSTREE_JSON}
         */
        // $.get(Routing.generate('app_xslpolicy_xslpolicytreemove', {id: policyNode.data.policyId, dstPolicyId: policyTree.getPolicyId(dstNode)}))
        // .done(function (data) {
        //     policyTree.policyMove(data.policy, dstNode, policyNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var ruleCreate = function(policyNode) {
        /**
         * Add a rule to a policy
         * @param int policyId policy ID of the policy that will contain the rule
         *
         * @return json
         * {"rule":{"tracktype":TRACKTYPE, "field":FIELD, "id":RULE_ID, "name":NAME, "value":VALUE, "occurrence":OCCURENCE,  "ope":OPERATOR}}
         */
        // $.get(Routing.generate('app_xslpolicy_xslpolicytreerulecreate', {policyId: policyNode.data.policyId}))
        // .done(function (data) {
        //     policyTree.ruleCreate(data.rule, policyNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var ruleEdit = function(form, policyId, ruleNode) {
        /**
         * Edit a rule (POST data from a form)
         * @param int id rule ID of the rule to edit
         * @param int policyId policy ID of the policy that contain the rule
         *
         * @return json
         * {"rule":{"tracktype":TRACKTYPE, "field":FIELD, "id":RULE_ID, "name":NAME, "value":VALUE, "occurrence":OCCURENCE, "ope":OPERATOR}}
         */
        // $.ajax({
        //     type: form.attr('method'),
        //         url: Routing.generate('app_xslpolicy_xslpolicytreeruleedit', {id: ruleNode.data.ruleId, policyId: policyId}),
        //         data: new FormData(form[0]),
        //         processData: false,
        //         contentType: false
        // })
        // .done(function (data) {
        //     policyTree.ruleEdit(data.rule, ruleNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var ruleDelete = function(policyId, ruleNode) {
        /**
         * Delete a rule
         * @param int id rule ID of the rule to delete
         * @param int policyId policy ID of the policy that contain the rule
         *
         * @return json
         * {"id":RULE_ID}
         */
        // $.get(Routing.generate('app_xslpolicy_xslpolicytreeruledelete', {id: ruleNode.data.ruleId, policyId: policyId}))
        // .done(function (data) {
        //     policyTree.ruleDelete(ruleNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var ruleDuplicate = function(policyId, ruleNode, dstNode) {
        /**
         * Duplicate a rule
         * @param int id rule ID of the rule to duplicate
         * @param int policyId policy ID of the policy that contain the rule
         * @param int dstPolicyId policy ID of the destination policy
         *
         * @return json
         * {"rule":{"tracktype":TRACKTYPE, "field":FIELD, "id":RULE_ID, "name":NAME, "value":VALUE, "occurrence":OCCURENCE,  "ope":OPERATOR}}
         */
        // $.get(Routing.generate('app_xslpolicy_xslpolicytreeruleduplicate', {id: ruleNode.data.ruleId, policyId: policyId, dstPolicyId: dstNode.data.policyId}))
        // .done(function (data) {
        //     policyTree.ruleDuplicate(data.rule, dstNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var ruleMove = function(policyId, ruleNode, dstNode) {
        /**
         * Move a rule
         * @param int id rule ID of the rule to move
         * @param int policyId policy ID of the policy that contain the rule
         * @param int dstPolicyId policy ID of the destination policy
         *
         * @return json
         * {"rule":{"tracktype":TRACKTYPE, "field":FIELD, "id":RULE_ID, "name":NAME, "value":VALUE, "occurrence":OCCURENCE, "ope":OPERATOR}}
         */
        // $.get(Routing.generate('app_xslpolicy_xslpolicytreerulemove', {id: ruleNode.data.ruleId, policyId: policyId, dstPolicyId: policyTree.getPolicyId(dstNode)}))
        // .done(function (data) {
        //     policyTree.ruleMove(data.rule, dstNode, ruleNode);
        // })
        // .fail(function (jqXHR) {
        //     mcoMessage.fail(jqXHR);
        // })
    }

    var getFieldsList = function(trackType, field) {
        /**
         * Get list of fields for a trackType (POST : type and field)
         *
         * @return json
         */
        // $.post(Routing.generate('app_xslpolicy_xslpolicyrulefieldslist'), {type: trackType, field: field})
        // .done(function(data) {
        //     policyTreeRules.fieldsListOk(data, field)
        // })
        // .fail(function () {
        //     policyTreeRules.fieldsListError(field)
        // });
    }

    var getValuesList = function(trackType, field, value) {
        /**
         * Get list of values for a trackType and a field (POST : type, field and value)
         *
         * @return json
         */
        // $.post(Routing.generate('app_xslpolicy_xslpolicyrulevalueslist'), {type: trackType, field: field, value: value})
        // .done(function(data) {
        //     policyTreeRules.valuesListOk(data.values, value);
        // })
        // .fail(function () {
        //     policyTreeRules.valuesListError(value);
        // });
    }

    return {
        getData: getData,
        policyImport: policyImport,
        policyCreate: policyCreate,
        policyEdit: policyEdit,
        policyDelete: policyDelete,
        policyDuplicate: policyDuplicate,
        policyMove: policyMove,
        policyExport: policyExport,
        ruleCreate: ruleCreate,
        ruleEdit: ruleEdit,
        ruleDelete: ruleDelete,
        ruleDuplicate: ruleDuplicate,
        ruleMove: ruleMove,
        getFieldsList: getFieldsList,
        getValuesList: getValuesList,
    };
})();
