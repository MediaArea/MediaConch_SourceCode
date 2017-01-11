var policyTreeAjax = (function() {
    var getData = function() {
        /**
         * Get the json for jstree
         *
         * @return json
         * {"policiesTree":POLICIES_JSTREE_JSON}
         */
        res = webpage.get_policies_tree();
        data = JSON.parse(res);
        if (!data.error)
            policyTree.setData(data.policiesTree);
        else
            mcoMessage.error(data.error);
    };

    var policyImport = function(form) {
        /**
         * Import a policy from an XML (the XML is provided as POST data from a form)
         *
         * Call policy_import and policy_get
         *
         * @return json
         * {"policy":POLICY_JSTREE_JSON}
         */
        res = webpage.import_policy();
        data = JSON.parse(res);
        $("#xslPolicyImport_policyFile").val("");
        if (!data.error)
            policyTree.policyImport(data.policyTree);
        else
            mcoMessage.error(data.error);
    }

    var policyDropped = function(data)
    {
        // data = JSON.parse(res);
        if (!data.error)
            policyTree.policyImport(data.policyTree);
        else
            mcoMessage.error(data.error);
    }

    var policyCreateFromFile = function(form) {
        createPolicyFromFile.addSpinner();
        /**
         * Create a policy from a file (the file is provided as POST data from a form)
         *
         * @return json
         * {"policy":POLICY_JSTREE_JSON}
         */
         res = webpage.policy_from_file();
         data = JSON.parse(res);
         $("#xslPolicyCreateFromFile_file").val("");
         if (data.error) {
             mcoMessage.error(data.error);
             createPolicyFromFile.removeSpinner();
         }
    }

    var policyCreateFromFileCallback = function(policyId) {
        res = webpage.policy_get_jstree(policyId);
        data = JSON.parse(res);
        if (!data.error) {
            policyTree.policyCreateFromFile(data.policyTree);
            createPolicyFromFile.removeSpinner();
        }
        else {
            mcoMessage.error(data.error);
            createPolicyFromFile.removeSpinner();
        }
    }

    var policyCreate = function(policyNode, parentId) {
        /**
         * Create a policy
         * @param int parentId policy ID in which the new policy will be created
         *
         * Call xslt_policy_create and policy_get
         *
         * @return json
         * {"policy":POLICY_JSTREE_JSON}
         */
        res = webpage.xslt_policy_create(parentId);
        data = JSON.parse(res);
        if (!data.error)
            policyTree.policyCreate(data.policyTree, policyNode);
        else
            mcoMessage.error(data.error);
    }

    var policyEdit = function(form, policyNode) {
        /**
         * Edit a policy (POST data from a form)
         * @param int id policy ID of the policy to edit
         *
         * @return json
         * {"policy":POLICY_JSTREE_JSON}
         */
        var name = $("#xslPolicyInfo_policyName").val();
        if (name === null)
            name = "";

        var description = $("#xslPolicyInfo_policyDescription").val();
        if (description === null)
            description = "";

        var type = $("#xslPolicyInfo_policyType").val();
        if (type === null)
            type = "";

        var license = $("#xslPolicyInfo_policyLicense").val();
        if (license === null || license === "Other")
            license = "";

        var visibility = $("#xslPolicyInfo_policyVisibility").val();
        if (visibility === null)
            visibility = "";

        res = webpage.policy_edit(policyNode.data.policyId, name, description, license, type, visibility);
        data = JSON.parse(res);
        if (!data.error)
            policyTree.policyEdit(data.policyTree, policyNode);
        else
            mcoMessage.error(data.error);
    }

    var policyDelete = function(policyNode) {
        /**
         * Delete a policy
         * @param int id policy ID of the policy to duplicate
         *
         * @return json
         * {"policyId":ID}
         */
        res = webpage.policy_remove(policyNode.data.policyId);
        data = JSON.parse(res);
        if (!data.error)
            policyTree.policyDelete(policyNode);
        else
            mcoMessage.error(data.error);
    }

    var policyExport = function(policyNode) {
        /**
        * Export XML of a policy
        * @param int id policy ID of the policy to export
        *
        * @return XML
        */
        res = webpage.policy_export(policyNode.data.policyId);
        data = JSON.parse(res);
        if (data.error)
            mcoMessage.error(data.error);
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
        res = webpage.policy_duplicate(policyNode.data.policyId, policyTree.getPolicyId(dstNode));
        data = JSON.parse(res);
        if (!data.error)
            policyTree.policyDuplicate(data.policyTree, dstNode);
        else
            mcoMessage.error(data.error);
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
        res = webpage.policy_move(policyNode.data.policyId, policyTree.getPolicyId(dstNode));
        data = JSON.parse(res);
        if (!data.error)
            policyTree.policyMove(data.policyTree, dstNode, policyNode);
        else
            mcoMessage.error(data.error);
    }

    var ruleCreate = function(policyNode) {
        /**
         * Add a rule to a policy
         * @param int policyId policy ID of the policy that will contain the rule
         *
         * @return json
         * {"rule":{"tracktype":TRACKTYPE, "field":FIELD, "id":RULE_ID, "name":NAME, "value":VALUE, "occurrence":OCCURENCE,  "ope":OPERATOR}}
         */
        res = webpage.xslt_policy_rule_create(policyNode.data.policyId);
        data = JSON.parse(res);
        if (!data.error)
            policyTree.ruleCreate(data.rule, policyNode);
        else
            mcoMessage.error(data.error);
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

        var scope = "";
        var name = "";
        var tracktype = "";
        var field = "";
        var occurrence = -1;
        var ope = "";
        var value = "";

        if ($(".ruleMediaTrace").hasClass("active"))
            scope = "mmt";

        if (scope === "mmt")
        {
            name = $("#xslPolicyRuleMt_title").val();
            if (name === null)
                name = "";

            field = $("#xslPolicyRuleMt_field").val();
            if (field === null)
                field = "";

            ope = $("#xslPolicyRuleMt_validator").val();
            if (ope === null)
                ope = "";

            value = $("#xslPolicyRuleMt_value").val();
            if (value === null)
                value = "";
        }
        else
        {
            name = $("#xslPolicyRule_title").val();
            if (name === null)
                name = "";

            tracktype = $("#xslPolicyRule_trackType").val();
            if (tracktype === null)
                tracktype = "";

            field = $("#xslPolicyRule_field").val();
            if (field === null)
                field = "";

            occurrence = $("#xslPolicyRule_occurrence").val();
            if (occurrence === null || occurrence === "*" || occurrence === "")
                occurrence = -1;

            ope = $("#xslPolicyRule_validator").val();
            if (ope === null)
                ope = "";

            value = $("#xslPolicyRule_value").val();
            if (value === null)
                value = "";
        }

        res = webpage.xslt_policy_rule_edit(ruleNode.data.ruleId, policyId, name, tracktype, field, occurrence, ope, value, scope);
        data = JSON.parse(res);
        if (!data.error)
            policyTree.ruleEdit(data.rule, ruleNode);
        else
            mcoMessage.error(data.error);
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
        res = webpage.xslt_policy_rule_delete(policyId, ruleNode.data.ruleId);
        data = JSON.parse(res);
        if (!data.error)
            policyTree.ruleDelete(ruleNode);
        else
            mcoMessage.error(data.error);
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

        res = webpage.xslt_policy_rule_duplicate(policyId, ruleNode.data.ruleId, dstNode.data.policyId);
        data = JSON.parse(res);
        if (!data.error)
            policyTree.ruleDuplicate(data.rule, dstNode);
        else
            mcoMessage.error(data.error);
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

        res = webpage.xslt_policy_rule_move(policyId, ruleNode.data.ruleId, policyTree.getPolicyId(dstNode));
        data = JSON.parse(res);
        if (!data.error)
            policyTree.ruleMove(data.rule, dstNode, ruleNode);
        else
            mcoMessage.error(data.error);
    }

    var getFieldsList = function(trackType, field) {
        /**
         * Get list of fields for a trackType (POST : type and field)
         *
         * @return json
         */
        if (trackType === null)
            trackType = "";
        if (field === null)
            field = "";

        res = webpage.get_fields_list(trackType, field);
        data = JSON.parse(res);
        if (!data.error)
            policyTreeRulesMI.fieldsListOk(data.fields, field)
        else
            policyTreeRulesMI.fieldsListError(field);
    }

    var getValuesList = function(trackType, field, value) {
        /**
         * Get list of values for a trackType and a field (POST : type, field and value)
         *
         * @return json
         */
        if (trackType === null)
            trackType = "";
        if (field === null)
            field = "";
        if (value === null)
            value = "";

        res = webpage.get_values_list(trackType, field, value);
        data = JSON.parse(res);
        if (!data.error)
            policyTreeRulesMI.valuesListOk(data.values, value);
        else
            policyTreeRulesMI.valuesListError(value);
    }

    return {
        getData: getData,
        policyImport: policyImport,
        policyDropped: policyDropped,
        policyCreateFromFile: policyCreateFromFile,
        policyCreateFromFileCallback: policyCreateFromFileCallback,
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
