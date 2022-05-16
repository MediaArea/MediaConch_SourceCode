var policyTreeRulesMT = (function() {
    var display = function(node) {
        // Check if rule is part of a policy system
        var system = true;
        if ('u' == policyTree.getInstance().get_node(node.parent).type) {
            system = false;
        }

        // Enable/disable MI/MT switch button
        $('.ruleMediaTrace').prop('disabled', system);
        $('.ruleMediaInfo').prop('disabled', system);

        $('.ruleMediaInfoActions').addClass('hidden');
        $('.ruleMediaTraceActions').removeClass('hidden');
        $('.ruleMediaInfo').removeClass('active');
        $('.ruleMediaTrace').addClass('active');

        $('#xslPolicyRuleMt_title').val(node.text);

        if (policyTreeRules.isRuleMT(node)) {
            $('#xslPolicyRuleMt_field').val(node.data.field);
            if ('' != node.data.level && $('#xslPolicyRuleMt_level option[value="' + node.data.level + '"]').length) {
                $('#xslPolicyRuleMt_level option[value="' + node.data.level + '"]').prop('selected', true);
            }
            else {
                $('#xslPolicyRuleMt_level option[value=""]').prop('selected', true);
            }
            if ('' != node.data.ope && $('#xslPolicyRuleMt_validator option[value="' + node.data.ope + '"]').length) {
                $('#xslPolicyRuleMt_validator option[value="' + node.data.ope + '"]').prop('selected', true);
            }
            else {
                $('#xslPolicyRuleMt_validator option[value="exists"]').prop('selected', true);
            }
            $('#xslPolicyRuleMt_validator').trigger('change');
            $('#xslPolicyRuleMt_value').val(node.data.value);
        }
        else {
            $('#xslPolicyRuleMt_field').val('');
            $('#xslPolicyRuleMt_level option[value=""]').prop('selected', true);
            $('#xslPolicyRuleMt_validator option[value="exists"]').prop('selected', true);
            $('#xslPolicyRuleMt_validator').trigger('change');
            $('#xslPolicyRuleMt_value').val('');
        }

        if (system) {
            $('#xslPolicyRuleMt_title').prop('disabled', true);
            $('#xslPolicyRuleMt_field').prop('disabled', true);
            $('#xslPolicyRuleMt_level').prop('disabled', true);
            $('#xslPolicyRuleMt_validator').prop('disabled', true);
            $('#xslPolicyRuleMt_value').prop('disabled', true);
            $('#xslPolicyRuleMt_SaveRule').addClass('hidden');
            $('#xslPolicyRuleMt_DuplicateRule').addClass('hidden');
            $('#xslPolicyRuleMt_DeleteRule').addClass('hidden');
        }
        else {
            $('#xslPolicyRuleMt_title').prop('disabled', false);
            $('#xslPolicyRuleMt_field').prop('disabled', false);
            $('#xslPolicyRuleMt_level').prop('disabled', false);
            $('#xslPolicyRuleMt_validator').prop('disabled', false);
            $('#xslPolicyRuleMt_value').prop('disabled', false);
            $('#xslPolicyRuleMt_SaveRule').removeClass('hidden');
            $('#xslPolicyRuleMt_DuplicateRule').removeClass('hidden');
            $('#xslPolicyRuleMt_DeleteRule').removeClass('hidden');
        }

        $('#policyFix').affix('checkPosition');
    };

    var displayValueField = function(validator) {
        // If validator is empty do not display the value field
        if ('' == validator || 'exists' == validator || 'must not exist' == validator) {
            $('#xslPolicyRuleMt_value').parent().addClass('hidden');
        }
        else {
            $('#xslPolicyRuleMt_value').parent().removeClass('hidden');
        }
    }

    return {
        display: display,
        displayValueField: displayValueField,
    }
})();
