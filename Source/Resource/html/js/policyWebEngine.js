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
function policyRuleFormEdit(form, policyNode, ruleNode) {
    var title = $(form).find("#xslPolicyRule_title").val();
    var editor_selected = $(form).find("#xslPolicyRule_editor_0").is(':checked');
    var type = $(form).find("#xslPolicyRule_trackType").val();
    var field = $(form).find("#xslPolicyRule_field").val();
    var occurrence = $(form).find("#xslPolicyRule_occurrence").val();

    if (occurrence === "*")
        occurrence = -1;
    occurrence = parseInt(occurrence);
    if (isNaN(occurrence))
        occurrence = 0;

    var validator = $(form).find("#xslPolicyRule_validator").val();
    var value = $(form).find("#xslPolicyRule_value").val();
    var text = $(form).find("#xslPolicyRule_valueFreeText").val();
    webpage.policy_rule_edit(policyNode.data.policyId, ruleNode.data.ruleId, title, editor_selected, type, field, occurrence, validator, value, text, function (data) {
        //data: rule
        var edited = JSON.parse(data);
        if (!edited.error)
            ruleEdit(edited, ruleNode);
        else
            errorMessage(edited.error);
    });
}

function policyRuleForm(form, policyNode, ruleNode, action, routeAction) {
    // routeAction is for MCO

    if (action === "duplicate")
    {
        webpage.policy_rule_duplicate(policyNode.data.policyId, ruleNode.data.ruleId, function (data) {
            //data: rule
            var duplicated = JSON.parse(data);
            if (!duplicated.error)
                ruleDuplicate(duplicated, ruleNode);
            else
                errorMessage(duplicated.error);
        });
    }
    else if (action === "delete")
    {
        webpage.policy_rule_delete(policyNode.data.policyId, ruleNode.data.ruleId, function (data) {
            //data: rule
            var deleted = JSON.parse(data);
            if (!deleted.error)
                ruleDelete(deleted, ruleNode);
            else
                errorMessage(deleted.error);
        });
    }
    else if (action === "edit")
    {
        policyRuleFormEdit(form, policyNode, ruleNode);
    }
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

function policyExportRequest(policyId) {
    webpage.export_policy(policyId, function (data) {
        var exported = JSON.parse(data);
        if (exported.error)
            errorMessage(exported.error);
    });
}

function policyDeleteRequest(policyNode) {
    if (!policyNode || !policyNode.data || !policyNode.data.policyId)
        return;

    webpage.delete_policy(policyNode.data.policyId, function (data) {
    // data:
        var deleted = JSON.parse(data);
        if (!deleted.error)
            policyDelete(deleted, policyNode);
        else
            errorMessage(deleted.error);
    });
}

function policyRuleCreateRequest(policyNode) {
    webpage.policy_rule_create(policyNode.data.policyId, function (data) {
        var created = JSON.parse(data);
        if (!created.error)
            policyRuleCreate(created.rule, policyNode);
        else
            errorMessage(created.error);
    });
}

function getFieldsList(trackType, field) {
    // // data: {values:[value,value]}
    webpage.get_fields_list(trackType, field, function (data) {
        var fields = JSON.parse(data);
        if (!fields.error)
            fieldsListOk(fields.fields, field);
        else
            fieldsListError(field);
    });
}

function getValuesList(trackType, field, value) {
    // // data: {values:[value,value,...]}
    webpage.get_values_list(trackType, field, value, function (data) {
        var values = JSON.parse(data);
        if (!values.error)
            valuesListOk(values.values, value);
        else
            valuesListError(value);
    });
}
