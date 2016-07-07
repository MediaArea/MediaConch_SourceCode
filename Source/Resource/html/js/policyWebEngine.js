function getPolicyTreeData() {
    webpage.get_policies_tree(function(data) {
        var policies = JSON.parse(data);
        displayTree(policies.policiesTree);
    });
}

function policyImportForm(form) {
    webpage.import_policy(function (data) {
    //     data: policyName, policyId, policyRules
        var imported = JSON.parse(data);
        if (!imported.error)
            policyImport(imported);
        else
            errorMessage(imported.error);
    });
}

function policyCreateForm(form) {
    // webpage.create_policy(form, function (data) {
    // //     data: policyName, policyId
    //     policyCreate(data);
    // });
}

function policyRuleForm(form, ruleNode, action, routeAction) {
    // webpage.rule_action(routeAction, policyId, ruleNode.data.Id, action, function (data) {
    //     ruleAction(data, ruleNode, action);
    // });
}

function policyDuplicateRequest(policyNode) {
    // webpage.duplicate_policy(policyNode, function (data) {
    // // data: policyName, policyId, policyRules
    //     policyDuplicate(data, policyNode);
    // });
}

function policyDeleteRequest(policyNode) {
    // webpage.delete_policy(policyNode, function (data) {
    // // data:
    //     policyDelete(data, policyNode);
    // });
}

function policyRuleCreateRequest(policyNode) {
    // webpage.create_rule(policyNode, function (data) {
    // // data: ...
    //     policyRuleCreate(data, policyNode);
    // });
}

function getFieldsList(trackType, field) {
    // webpage.get_fields_list(trackType, field, function (data) {
    // // data: [{key,value}]
    //     fieldsListOk(data, field);
    // });
}

function getValuesList(trackType, field, value) {
    // webpage.get_values_list(trackType, field, value, function (data) {
    // // data: [{key,value}]
    //     valuesListOk(data.values, value);
    // });
}
