var policyTreePolicies = (function() {
    var manage = function(node) {
        $('.policyManage').removeClass('hidden');
        $('.policyEdit').addClass('hidden');
        $('.policyEditRule').addClass('hidden');
    }

    var edit = function(node) {
        $('#xslPolicyInfo_policyName').val(getName(node));
        $('#xslPolicyInfo_policyDescription').val(node.data.description);
        $('#xslPolicyInfo_policyType option[value="' + node.data.type + '"]').prop('selected', true);

        if ('s' == node.type || ('u' == node.type && !node.data.isEditable) ) {
            $('#policyDelete').removeClass('hidden');
            $('#policyRuleCreateContainer').addClass('hidden');
            $('#xslPolicyInfo_policyName').prop('disabled', true);
            $('#xslPolicyInfo_policyDescription').prop('disabled', true);
            $('#xslPolicyInfo_policyType').prop('disabled', true);
            $('#xslPolicyInfo_SavePolicyInfo').addClass('hidden');
            $('.policyEditActions.policyEditUser').addClass('hidden');
        }
        else {
            $('#policyDelete').removeClass('hidden');
            $('#policyRuleCreateContainer').removeClass('hidden');
            $('#xslPolicyInfo_policyName').prop('disabled', false);
            $('#xslPolicyInfo_policyDescription').prop('disabled', false);
            $('#xslPolicyInfo_policyType').prop('disabled', false);
            $('#xslPolicyInfo_SavePolicyInfo').removeClass('hidden');
            $('.policyEditActions.policyEditUser').removeClass('hidden');
        }

        if ('s' == node.type) {
            $('#policyDelete').addClass('hidden');
        }

        $('.policyManage').addClass('hidden');
        $('.policyEdit').removeClass('hidden');
        $('.policyEditRule').addClass('hidden');
    }

    // Remove (or|and) from policy name
    var getName = function(node) {
        if ('XSLT' == node.data.kind) {
            var regex = /(.*) \((or|and)\)$/i;

            if (null != regex.exec(node.text)) {
                return regex.exec(node.text)[1];
            }
        }

        return node.text;
    }

    return {
        manage: manage,
        edit: edit,
    }
})();
