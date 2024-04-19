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
            $('#xslPolicyRule_occurrence option').remove();
            $('#xslPolicyRule_sourceOccurrence option').remove();
            [
                {name: '', value: ''},
                {name: 'Any', value: 'any'},
                {name: 'All', value: 'all'},
                {name: '1', value: '1'},
                {name: '2', value: '2'},
                {name: '3', value: '3'},
                {name: '4', value: '4'},
                {name: '5', value: '5'},
                {name: '6', value: '6'},
                {name: '7', value: '7'},
                {name: '8', value: '8'},
                {name: '9', value: '9'},
                {name: '10', value: '10'},
                {name: '11', value: '11'},
                {name: '12', value: '12'},
                {name: '13', value: '13'},
                {name: '14', value: '14'},
                {name: '15', value: '15'},
                {name: '16', value: '16'}
            ].forEach(element => {
                $('#xslPolicyRule_occurrence').append('<option value="' + element.value + '">' + element.name + '</option>');
                $('#xslPolicyRule_sourceOccurrence').append('<option value="' + element.value + '">' + element.name + '</option>');
            });
            if ($('#xslPolicyRule_occurrence option[value="' + node.data.occurrence + '"]').length) {
                $('#xslPolicyRule_occurrence option[value="' + node.data.occurrence + '"]').prop('selected', true);
            }
            else {
                $('#xslPolicyRule_occurrence').append('<option value="' + node.data.occurrence + '" selected>' + node.data.occurrence + '</option>');
            }
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
            if (undefined !== node.data.source) {
                if ($('#xslPolicyRule_sourceOccurrence option[value="' + node.data.source.occurrence + '"]').length) {
                    $('#xslPolicyRule_sourceOccurrence option[value="' + node.data.source.occurrence + '"]').prop('selected', true);
                }
                else {
                    $('#xslPolicyRule_sourceOccurrence').append('<option value="' + node.data.source.occurrence + '" selected>' + node.data.source.occurrence + '</option>');
                }
                $('#xslPolicyRule_sourceField option').remove();
                $('#xslPolicyRule_sourceField').append('<option value="' + node.data.source.field + '" selected>' + node.data.source.field + '</option>');
                $('#xslPolicyRule_sourceTrackType option[value="' + node.data.source.tracktype + '"]').prop('selected', true);
                $('#xslPolicyRule_sourceTrackType').trigger('change');
                $('#xslPolicyRule_sourceOccurrence').parent().removeClass('hidden');
                $('#xslPolicyRule_sourceField').parent().removeClass('hidden');
                $('#xslPolicyRule_sourceTrackType').parent().removeClass('hidden');
                $('#xslPolicyRule_value').parent().addClass('hidden');
                $('#xslPolicyRule_sourceOccurrence').prop('required', 'required');
                $('#xslPolicyRule_sourceField').prop('required', 'required');
                $('#xslPolicyRule_sourceTrackType').prop('required', 'required');
                $('#xslPolicyRule_value').prop('required', false);
                $('#xslPolicyRule_valueType').prop('checked', true);
            }
            else {
                $('#xslPolicyRule_sourceOccurrence option[value=""]').prop('selected', true);
                $('#xslPolicyRule_sourceField option[value=""]').prop('selected', true);
                $('#xslPolicyRule_sourceTrackType option[value=""]').prop('selected', true);
                $('#xslPolicyRule_sourceTrackType').trigger('change');
                $('#xslPolicyRule_sourceOccurrence').parent().addClass('hidden');
                $('#xslPolicyRule_sourceField').parent().addClass('hidden');
                $('#xslPolicyRule_sourceTrackType').parent().addClass('hidden');
                $('#xslPolicyRule_value').parent().removeClass('hidden');
                $('#xslPolicyRule_sourceOccurrence').prop('required', false);
                $('#xslPolicyRule_sourceField').prop('required', false);
                $('#xslPolicyRule_sourceTrackType').prop('required', false);
                $('#xslPolicyRule_value').prop('required', 'required');
                $('#xslPolicyRule_valueType').prop('checked', false);
            }
        }
        else {
            $('#xslPolicyRule_occurrence option').remove();
            $('#xslPolicyRule_occurrence option[value=""]').prop('selected', true);
            $('#xslPolicyRule_occurrence').trigger('change');
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
            $('#xslPolicyRule_sourceOccurrence option').remove();
            $('#xslPolicyRule_sourceOccurrence option[value=""]').prop('selected', true);
            $('#xslPolicyRule_sourceOccurrence').trigger('change');
            $('#xslPolicyRule_sourceField option').remove();
            $('#xslPolicyRule_sourceField option[value=""]').prop('selected', true);
            $('#xslPolicyRule_sourceField').trigger('change');
            $('#xslPolicyRule_sourceTrackType option[value=""]').prop('selected', true);
            $('#xslPolicyRule_sourceTrackType').trigger('change');
        }

        if (system) {
            $('#xslPolicyRule_title').prop('disabled', true);
            $('#xslPolicyRule_trackType').prop('disabled', true);
            $('#xslPolicyRule_field').prop('disabled', true);
            $('#xslPolicyRule_level').prop('disabled', true);
            $('#xslPolicyRule_occurrence').prop('disabled', true);
            $('#xslPolicyRule_validator').prop('disabled', true);
            $('#xslPolicyRule_value').prop('disabled', true);
            $('#xslPolicyRule_valueType').prop('disabled', true);
            $('#xslPolicyRule_sourceOccurrence').prop('disabled', true);
            $('#xslPolicyRule_sourceField').prop('disabled', true);
            $('#xslPolicyRule_sourceTrackType').prop('disabled', true);
            $('#xslPolicyRule_SaveRule').parent().addClass('hidden');
            $('#xslPolicyRule_DuplicateRule').parent().addClass('hidden');
            $('#xslPolicyRule_DeleteRule').parent().addClass('hidden');
        }
        else {
            $('#xslPolicyRule_title').prop('disabled', false);
            $('#xslPolicyRule_trackType').prop('disabled', false);
            $('#xslPolicyRule_field').prop('disabled', false);
            $('#xslPolicyRule_level').prop('disabled', false);
            displayOccurrenceField($('#xslPolicyRule_trackType').val(), $('#xslPolicyRule_occurrence'));
            $('#xslPolicyRule_validator').prop('disabled', false);
            $('#xslPolicyRule_valueType').prop('disabled', false);
            displayOccurrenceField($('#xslPolicyRule_sourceTrackType').val(), $('#xslPolicyRule_sourceOccurrence'));
            $('#xslPolicyRule_sourceField').prop('disabled', false);
            $('#xslPolicyRule_sourceTrackType').prop('disabled', false);
            $('#xslPolicyRule_value').prop('disabled', false);
            $('#xslPolicyRule_SaveRule').parent().removeClass('hidden');
            $('#xslPolicyRule_DuplicateRule').parent().removeClass('hidden');
            $('#xslPolicyRule_DeleteRule').parent().removeClass('hidden');
        }

        $('#policyFix').affix('checkPosition');
    }

    var loadFieldsList = function(trackType, field) {
        var value = field === null ? '' : field.val();
        policyTreeAjax.getFieldsList(trackType, field, value);

        if (field !== null) {
            field.trigger('change');
        }
    }

    var fieldsListOk = function(fields, field, value) {
        if (field === null) {
            return;
        }

        field.find('option').remove();
        field.append('<option value="">Choose a field</option>');
        $.each(fields, function(k, v) {
            field.append('<option value="' + v + '">' + v + '</option>');
        });

        if (value) {
            field.find('option[value="' + value + '"]').prop('selected', true);
        }
    }

    var fieldsListError = function(field, value) {
        if (field === null) {
            return;
        }

        field.html('');
        field.append('<option value="">Choose a field</option>');

        if (value) {
            field.append('<option value="' + value + '" selected>' + value + '</option>');
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

    var displayOccurrenceField = function(trackType, field) {
        if ('General' == trackType) {
            field.prop('disabled', true);
            field.val('*');
        }
        else {
            field.prop('disabled', false);
        }
    }

    var displaySourceFields = function() {
        if ($('#xslPolicyRule_valueType').is(':checked')) {
            $('#xslPolicyRule_sourceOccurrence').parent().removeClass('hidden');
            $('#xslPolicyRule_sourceField').parent().removeClass('hidden');
            $('#xslPolicyRule_sourceTrackType').parent().removeClass('hidden');
            $('#xslPolicyRule_value').parent().addClass('hidden');
            $('#xslPolicyRule_sourceOccurrence').prop('required', 'required');
            $('#xslPolicyRule_sourceField').prop('required', 'required');
            $('#xslPolicyRule_sourceTrackType').prop('required', 'required');
            $('#xslPolicyRule_value').prop('required', false);
        }
        else {
            $('#xslPolicyRule_sourceOccurrence').parent().addClass('hidden');
            $('#xslPolicyRule_sourceField').parent().addClass('hidden');
            $('#xslPolicyRule_sourceTrackType').parent().addClass('hidden');
            $('#xslPolicyRule_value').parent().removeClass('hidden');
            $('#xslPolicyRule_sourceOccurrence').prop('required', false);
            $('#xslPolicyRule_sourceField').prop('required', false);
            $('#xslPolicyRule_sourceTrackType').prop('required', false);
            $('#xslPolicyRule_value').prop('required', 'required');
        }
    }

    return {
        display: display,
        fieldsListOk: fieldsListOk,
        fieldsListError: fieldsListError,
        valuesListOk: valuesListOk,
        valuesListError: valuesListError,
        loadFieldsList: loadFieldsList,
        displayOccurrenceField: displayOccurrenceField,
        displaySourceFields: displaySourceFields,
        loadValuesList: loadValuesList,
        displayValueField: displayValueField,
    }
})();
