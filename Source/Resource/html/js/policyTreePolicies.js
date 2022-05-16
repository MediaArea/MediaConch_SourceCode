var policyTreePolicies = (function() {
    var manage = function(node) {
        $('.policyManage').removeClass('hidden');
        $('.policyEdit').addClass('hidden');
        $('.policyEditRule').addClass('hidden');
    }

    var edit = function(node) {
        $('#xslPolicyInfo_policyName').val(getName(node));
        $('#xslPolicyInfo_policyDescription').val(node.data.description);
        $('#xslPolicyInfo_policyTags').val(node.data.tags ? node.data.tags.join('\n') : '');
        $('#xslPolicyInfo_policyType option[value="' + node.data.type + '"]').prop('selected', true);
        if ('' != node.data.level && $('#xslPolicyInfo_policyLevel option[value="' + node.data.level + '"]').length) {
            $('#xslPolicyInfo_policyLevel option[value="' + node.data.level + '"]').prop('selected', true);
        }
        else {
            $('#xslPolicyInfo_policyLevel option[value=""]').prop('selected', true);
        }
        if ('s' == node.type || ('u' == node.type && !node.data.isEditable) ) {
            $('#policyDelete').removeClass('hidden');
            $('#policyRuleCreateContainer').addClass('hidden');
            $('#xslPolicyInfo_policyName').prop('disabled', true);
            $('#xslPolicyInfo_policyDescription').prop('disabled', true);
            $('#xslPolicyInfo_policyTags').prop('disabled', true);
            $('#xslPolicyInfo_policyType').prop('disabled', true);
            $('#xslPolicyInfo_policyLevel').prop('disabled', true);
            $('#xslPolicyInfo_SavePolicyInfo').addClass('hidden');
            $('.policyEditActions.policyEditUser').addClass('hidden');
            $('.policyEditActions.policyVisibility').addClass('hidden');
            // if ($('#xslPolicyInfo_policyVisibility').length) {
            //     $('#xslPolicyInfo_policyVisibility').prop('disabled', true);
            // }
            if ($('#xslPolicyInfo_policyLicense').length) {
                $('#xslPolicyInfo_policyLicense').prop('disabled', true);
            }
        }
        else {
            $('#policyDelete').removeClass('hidden');
            $('#policyRuleCreateContainer').removeClass('hidden');
            $('#xslPolicyInfo_policyName').prop('disabled', false);
            $('#xslPolicyInfo_policyDescription').prop('disabled', false);
            $('#xslPolicyInfo_policyTags').prop('disabled', false);
            $('#xslPolicyInfo_policyType').prop('disabled', false);
            $('#xslPolicyInfo_policyLevel').prop('disabled', false);
            $('#xslPolicyInfo_SavePolicyInfo').removeClass('hidden');
            $('.policyEditActions.policyEditUser').removeClass('hidden');
            $('.policyEditActions.policyVisibility').addClass('hidden');
            // if ($('#xslPolicyInfo_policyVisibility').length) {
            //     $('#xslPolicyInfo_policyVisibility').prop('disabled', false);
            // }
            if ($('#xslPolicyInfo_policyLicense').length) {
                $('#xslPolicyInfo_policyLicense').prop('disabled', false);
            }
        }

        if ('s' == node.type) {
            $('#policyDelete').addClass('hidden');
        }

        // Top level
        if ('u_p' == node.parent || 's_p' == node.parent) {
            $('#xslPolicyInfo_policyTopLevel').val(1);

            // License
            $('#xslPolicyInfo_policyLicense').parent().removeClass('hidden');
            if (undefined === node.data.license || !$('#xslPolicyInfo_policyLicense option[value="' + node.data.license + '"]').length) {
                // Select Other license by default if field is empty (old policy)
                $('#xslPolicyInfo_policyLicense option:last').prop('selected', true);
            }
            else {
                $('#xslPolicyInfo_policyLicense option[value="' + node.data.license + '"]').prop('selected', true);
            }

            // Visibility
            if ($('#xslPolicyInfo_policyVisibility').length) {
                $('#xslPolicyInfo_policyVisibility').parent().addClass('hidden');
                // $('#xslPolicyInfo_policyVisibility').parent().removeClass('hidden');
                // $('#xslPolicyInfo_policyVisibility option[value="' + (true == node.data.isPublic ? 1 : 0) + '"]').prop('selected', true);
            }
        }
        else {
            $('#xslPolicyInfo_policyTopLevel').val(0);

            // License
            $('#xslPolicyInfo_policyLicense').parent().addClass('hidden');

            // Visibility
            if ($('#xslPolicyInfo_policyVisibility').length) {
                $('#xslPolicyInfo_policyVisibility').parent().addClass('hidden');
            }
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
