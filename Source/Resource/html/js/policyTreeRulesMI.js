var policyTreeRulesMI = (function() {
    var display = function(node) {
        // Check if rule is part of a policy system
        var system = true;
        if ('u' == policyTree.getInstance().get_node(node.parent).type) {
            system = false;
        }

        // Enable/disable MI/MT switch button
        $('.ruleMediaTrace').prop('disabled', system);
        $('.ruleMediaInfo').prop('disabled', system);

        $('.ruleMediaTraceActions').addClass('hidden');
        $('.ruleMediaInfoActions').removeClass('hidden');
        $('.ruleMediaTrace').removeClass('active');
        $('.ruleMediaInfo').addClass('active');

        $('#xslPolicyRule_title').val(node.text);

        if (policyTreeRules.isRuleMI(node)) {
            $('#xslPolicyRule_occurrence').val(node.data.occurrence);
            $('#xslPolicyRule_field option').remove();
            $('#xslPolicyRule_field').append('<option value="' + node.data.field + '" selected>' + node.data.field + '</option>');
            $('#xslPolicyRule_value option').remove();
            if (null != node.data.value) {
                $('#xslPolicyRule_value').append('<option value="' + node.data.value + '" selected>' + node.data.value + '</option>');
            }
            if ('' != node.data.level && $('#xslPolicyRule_level option[value="' + node.data.level + '"]').length) {
                $('#xslPolicyRule_level option[value="' + node.data.level + '"]').prop('selected', true);
            }
            else {
                $('#xslPolicyRule_level option[value=""]').prop('selected', true);
            }
            $('#xslPolicyRule_trackType option[value="' + node.data.tracktype + '"]').prop('selected', true);
            $('#xslPolicyRule_trackType').trigger('change');
            if ('' != node.data.ope && $('#xslPolicyRule_validator option[value="' + node.data.ope + '"]').length) {
                $('#xslPolicyRule_validator option[value="' + node.data.ope + '"]').prop('selected', true);
            }
            else {
                $('#xslPolicyRule_validator option[value="exists"]').prop('selected', true);
            }
            $('#xslPolicyRule_validator').trigger('change');
        }
        else {
            $('#xslPolicyRule_field option').remove();
            $('#xslPolicyRule_field option[value=""]').prop('selected', true);
            $('#xslPolicyRule_field').trigger('change');
            $('#xslPolicyRule_level option').remove();
            $('#xslPolicyRule_level option[value=""]').prop('selected', true);
            $('#xslPolicyRule_level').trigger('change');
            $('#xslPolicyRule_value option').remove();
            $('#xslPolicyRule_value option[value=""]').prop('selected', true);
            $('#xslPolicyRule_value').trigger('change');
            $('#xslPolicyRule_trackType option[value=""]').prop('selected', true);
            $('#xslPolicyRule_trackType').trigger('change');
            $('#xslPolicyRule_validator option[value="exists"]').prop('selected', true);
            $('#xslPolicyRule_validator').trigger('change');
        }

        if (system) {
            $('#xslPolicyRule_title').prop('disabled', true);
            $('#xslPolicyRule_trackType').prop('disabled', true);
            $('#xslPolicyRule_field').prop('disabled', true);
            $('#xslPolicyRule_level').prop('disabled', true);
            $('#xslPolicyRule_occurrence').prop('disabled', true);
            $('#xslPolicyRule_validator').prop('disabled', true);
            $('#xslPolicyRule_value').prop('disabled', true);
            $('#xslPolicyRule_SaveRule').addClass('hidden');
            $('#xslPolicyRule_DuplicateRule').addClass('hidden');
            $('#xslPolicyRule_DeleteRule').addClass('hidden');
        }
        else {
            $('#xslPolicyRule_title').prop('disabled', false);
            $('#xslPolicyRule_trackType').prop('disabled', false);
            $('#xslPolicyRule_field').prop('disabled', false);
            $('#xslPolicyRule_level').prop('disabled', false);
            displayOccurenceField($('#xslPolicyRule_trackType').val());
            $('#xslPolicyRule_validator').prop('disabled', false);
            $('#xslPolicyRule_value').prop('disabled', false);
            $('#xslPolicyRule_SaveRule').removeClass('hidden');
            $('#xslPolicyRule_DuplicateRule').removeClass('hidden');
            $('#xslPolicyRule_DeleteRule').removeClass('hidden');
        }

        $('#policyFix').affix('checkPosition');
    }

    var loadFieldsList = function(trackType, field) {
        policyTreeAjax.getFieldsList(trackType, field);

        if (field) {
            $('#xslPolicyRule_field').trigger('change');
        }
    }

    var fieldsListOk = function(fields, field) {
        $('#xslPolicyRule_field option').remove();
        $('#xslPolicyRule_field').append('<option value="">Choose a field</option>');
        $.each(fields, function(k, v) {
            $('#xslPolicyRule_field').append('<option value="' + v + '">' + v + '</option>');
        });

        if (field) {
            $('#xslPolicyRule_field option[value="' + field + '"]').prop('selected', true);
        }
    }

    var fieldsListError = function(field) {
        $('#xslPolicyRule_field').html('');
        $('#xslPolicyRule_field').append('<option value="">Choose a field</option>');

        if (field) {
            $('#xslPolicyRule_field').append('<option value="' + field + '" selected>' + field + '</option>');
        }
    }

    var loadValuesList = function(trackType, field, value) {
        if (trackType && field) {
            policyTreeAjax.getValuesList(trackType, field, value);

            $('#xslPolicyRule_value').trigger('change');
        }
    }

    var valuesListOk = function(values, value) {
        $('#xslPolicyRule_value option').remove();
        $.each(values, function(k, v) {
            $('#xslPolicyRule_value').append('<option value="' + v + '">' + v + '</option>');
        });

        if (value) {
            $('#xslPolicyRule_value option[value="' + value + '"]').prop('selected', true);
        }
        else {
            $('#xslPolicyRule_value').prepend('<option value="" selected></option>');
        }
    }

    var valuesListError = function(value) {
        $('#xslPolicyRule_value option').remove();
        if (value) {
            $('#xslPolicyRule_value').append('<option value="' + value + '" selected>' + value + '</option>');
        }
    }

    var displayValueField = function(validator) {
        // If validator is empty do not display the value field
        if ('' == validator || 'exists' == validator || 'must not exist' == validator) {
            $('#xslPolicyRule_value').parent().addClass('hidden');
        }
        else {
            $('#xslPolicyRule_value').parent().removeClass('hidden');
        }
    }

    var displayOccurenceField = function(trackType) {
        if ('General' == trackType) {
            $('#xslPolicyRule_occurrence').prop('disabled', true);
            $('#xslPolicyRule_occurrence').val('*');
        }
        else {
            $('#xslPolicyRule_occurrence').prop('disabled', false);
        }
    }

    return {
        display: display,
        fieldsListOk: fieldsListOk,
        fieldsListError: fieldsListError,
        valuesListOk: valuesListOk,
        valuesListError: valuesListError,
        loadFieldsList: loadFieldsList,
        displayOccurenceField: displayOccurenceField,
        loadValuesList: loadValuesList,
        displayValueField: displayValueField,
    }
})();
