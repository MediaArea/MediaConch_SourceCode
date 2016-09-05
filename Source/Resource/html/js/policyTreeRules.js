var policyTreeRules = (function() {
    function display(node, system) {
        $('#xslPolicyRule_title').val(node.text);

        $('#xslPolicyRule_field option').remove();
        $('#xslPolicyRule_field').append('<option value="' + node.data.field + '" selected>' + node.data.field + '</option>');
        $('#xslPolicyRule_value option').remove();
        if (null != node.data.value) {
            $('#xslPolicyRule_value').append('<option value="' + node.data.value + '" selected>' + node.data.value + '</option>');
        }

        $('#xslPolicyRule_trackType option[value="' + node.data.tracktype + '"]').prop('selected', true);
        $('#xslPolicyRule_trackType').trigger('change');
        $('#xslPolicyRule_occurrence').val(-1 == node.data.occurrence ? '*' : node.data.occurrence);
        $('#xslPolicyRule_validator option[value="' + node.data.ope + '"]').prop('selected', true);
        $('#xslPolicyRule_validator').trigger('change');

        if (system) {
            $('#xslPolicyRule_title').prop('disabled', true);
            $('#xslPolicyRule_trackType').prop('disabled', true);
            $('#xslPolicyRule_field').prop('disabled', true);
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
            displayOccurenceField($('#xslPolicyRule_trackType').val());
            $('#xslPolicyRule_validator').prop('disabled', false);
            $('#xslPolicyRule_value').prop('disabled', false);
            $('#xslPolicyRule_SaveRule').removeClass('hidden');
            $('#xslPolicyRule_DuplicateRule').removeClass('hidden');
            $('#xslPolicyRule_DeleteRule').removeClass('hidden');
        }

        $('.policyManage').addClass('hidden');
        $('.policyEdit').addClass('hidden');
        $('.policyEditRule').removeClass('hidden');
    }

    function loadFieldsList(trackType, field) {
        policyTreeAjax.getFieldsList(trackType, field);

        if (field) {
            $('#xslPolicyRule_field').trigger('change');
        }
    }

    function fieldsListOk(fields, field) {
        $('#xslPolicyRule_field option').remove();
        $('#xslPolicyRule_field').append('<option value="">Choose a field</option>');
        $.each(fields, function(k, v) {
            $('#xslPolicyRule_field').append('<option value="' + k + '">' + v + '</option>');
        });

        if (field) {
            $('#xslPolicyRule_field option[value="' + field + '"]').prop('selected', true);
        }
    }

    function fieldsListError(field) {
        $('#xslPolicyRule_field').html('');
        $('#xslPolicyRule_field').append('<option value="">Choose a field</option>');

        if (field) {
            $('#xslPolicyRule_field').append('<option value="' + field + '" selected>' + field + '</option>');
        }
    }

    function loadValuesList(trackType, field, value) {
        if (trackType && field) {
            policyTreeAjax.getValuesList(trackType, field, value);

            $('#xslPolicyRule_value').trigger('change');
        }
    }

    function valuesListOk(values, value) {
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

    function valuesListError(value) {
        $('#xslPolicyRule_value option').remove();
        if (value) {
            $('#xslPolicyRule_value').append('<option value="' + value + '" selected>' + value + '</option>');
        }
    }

    function displayValueField(validator) {
        if ('exists' == validator || 'does_not_exist' == validator) {
            $('#xslPolicyRule_value').parent().addClass('hidden');
        }
        else {
            $('#xslPolicyRule_value').parent().removeClass('hidden');
        }
    }

    function displayOccurenceField(trackType) {
        if ('General' == trackType) {
            $('#xslPolicyRule_occurrence').prop('disabled', true);
            $('#xslPolicyRule_occurrence').val('*');
        }
        else {
            $('#xslPolicyRule_occurrence').prop('disabled', false);
        }
    }

    $('#xslPolicyRule_validator').on('change', function() {
        displayValueField($('#xslPolicyRule_validator').val());
    })

    $('#xslPolicyRule_trackType').on('change', function() {
        loadFieldsList($('#xslPolicyRule_trackType').val(), $('#xslPolicyRule_field').val());
        displayOccurenceField($('#xslPolicyRule_trackType').val());
    });

    $('#xslPolicyRule_field').on('change', function() {
        loadValuesList($('#xslPolicyRule_trackType').val(), $('#xslPolicyRule_field').val(), $('#xslPolicyRule_value').val());
    });

    return {
        display: display,
        fieldsListOk: fieldsListOk,
        fieldsListError: fieldsListError,
        valuesListOk: valuesListOk,
        valuesListError: valuesListError,
    }
})();
