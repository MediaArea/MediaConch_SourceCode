function getPolicyTreeData() {
    webpage.get_policies_tree(function(data) {
        var policies = JSON.parse(data);
        displayTree(policies.policiesTree);
    });
}

function policyImportForm(form) {
}

function policyCreateForm(form) {
}

function policyRuleForm(form, ruleNode, action, routeAction) {
}

function policyDuplicateRequest(policyNode) {
}

function policyDeleteRequest(policyNode) {
}

function policyRuleCreateRequest(policyNode) {
}

function getFieldsList(trackType, field) {
}

function getValuesList(trackType, field, value) {
}
