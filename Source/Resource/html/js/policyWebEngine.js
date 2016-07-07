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
    var name = $(form).find("#xslPolicyCreate_policyName").val();
    var description = $(form).find("#xslPolicyCreate_policyDescription").val();

    if (name === undefined)
        return;

    if (description === undefined)
        description = "";

    webpage.create_policy(name, description, function (data) {
    //     data: policyName, policyId
        var created = JSON.parse(data);
        if (!created.error)
            policyCreate(created);
        else
            errorMessage(created.error);
    });
}

function policyRuleForm(form, ruleNode, action, routeAction) {
    // webpage.rule_action(routeAction, policyId, ruleNode.data.Id, action, function (data) {
    //     ruleAction(data, ruleNode, action);
    // });
}

function policyDuplicateRequest(policyNode) {
    webpage.duplicate_policy(policyNode.data.policyId, function (data) {
    // data: policyName, policyId, policyRules
        var duplicated = JSON.parse(data);
        if (!duplicated.error)
            policyDuplicate(duplicated, policyNode);
        else
            errorMessage(duplicated.error);
    });
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
