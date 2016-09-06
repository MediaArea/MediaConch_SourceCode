function initPage() {
    mcoMessage.init('#policyInfo div');
    policyTree.init();

    // Make buttons in policy rule form display inline
    // Duplicate button
    $('#xslPolicyRule_DuplicateRule').parent().addClass('xslPolicyRuleDuplicateButton');
    // Save button
    $('#xslPolicyRule_SaveRule').parent().addClass('xslPolicyRuleSaveButton');
    // Delete button
    $('#xslPolicyRule_DeleteRule').parent().addClass('xslPolicyRuleDeleteButton');

    policyTreeAffix.init();
    formBindings();
    buttonBindings();
    setSelect2Plugin();
}

function setSelect2Plugin() {
    // Use select2 jquery plugin
    $('#xslPolicyRule_trackType').select2({
        theme: 'bootstrap',
        width: '100%',
        minimumResultsForSearch: Infinity
    });

    $('#xslPolicyRule_validator').select2({
        theme: 'bootstrap',
        width: '100%',
        minimumResultsForSearch: Infinity
    });

    $('#xslPolicyRule_field').select2({
        tags: true,
        theme: 'bootstrap',
        width: '100%'
    });

    // Replace input text by select
    $('#xslPolicyRule_value').replaceWith('<select id="' + $('#xslPolicyRule_value').prop('id') + '"  name="' + $('#xslPolicyRule_value').prop('name') + '"class="' + $('#xslPolicyRule_value').prop('class') + '">')
    $('#xslPolicyRule_value').select2({
        tags: true,
        theme: 'bootstrap',
        width: '100%'
    });
}

function formBindings() {
    // Import policy form
    $('form[name="xslPolicyImport"]').on('submit', function (e) {
        e.preventDefault();

        policyTreeAjax.policyImport($(this));
    });

    // Policy edit form
    $('form[name="xslPolicyInfo"]').on('submit', function (e) {
        e.preventDefault();

        policyTreeAjax.policyEdit($(this), policyTree.getSelectedNode());
    });

    // Policy rule edit form
    $('form[name="xslPolicyRule"]').on('submit', function (e) {
        e.preventDefault();

        // Duplicate
        if ('xslPolicyRule_DuplicateRule' == $('button[type=submit][clicked=true]').prop('id')) {
            policyTreeAjax.ruleDuplicate(policyTree.getParentPolicyId(), policyTree.getSelectedNode(), policyTree.getParentPolicy());
        }
        // Delete
        else if ('xslPolicyRule_DeleteRule' == $('button[type=submit][clicked=true]').prop('id')) {
            policyTreeAjax.ruleDelete(policyTree.getParentPolicyId(), policyTree.getSelectedNode());
        }
        // Edit
        else {
            policyTreeAjax.ruleEdit($(this), policyTree.getParentPolicyId(), policyTree.getSelectedNode());
        }
    });

    // Multiple form button click
    $('form[name="xslPolicyRule"] button[type=submit]').on('click', function() {
        $('form[name="xslPolicyRule"] button[type=submit]').removeAttr('clicked');
        $(this).attr('clicked', true);
    });
}

function buttonBindings() {
    $('#policyDuplicate').on('click', function() {
        if ('s' == policyTree.getSelectedNode().type) {
            policyTreeAjax.policyDuplicate(policyTree.getSelectedNode(), policyTree.getInstance().get_node('u_p'));
        }
        else {
            policyTreeAjax.policyDuplicate(policyTree.getSelectedNode(), policyTree.getParentPolicy());
        }
    })

    $('#policyExport').on('click', function() {
        policyTreeAjax.policyExport(policyTree.getSelectedNode());
    })

    $('#policyDelete').on('click', function() {
        policyTreeAjax.policyDelete(policyTree.getSelectedNode());
    })

    $('#policyRuleCreate').on('click', function() {
        policyTreeAjax.ruleCreate(policyTree.getSelectedNode());
    })

    // Create policy
    $('.policyCreate').on('click', function () {
        policyTreeAjax.policyCreate(policyTree.getSelectedNode(), policyTree.getPolicyId());
    });


    // Create policy rules
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
}

function displayValueField(validator) {
    if ('exists' == validator || 'does_not_exist' == validator) {
        $('#xslPolicyRule_value').parent().addClass('hidden');
    }
    else {
        $('#xslPolicyRule_value').parent().removeClass('hidden');
    }
}

function loadFieldsList(trackType, field) {
    policyTreeAjax.getFieldsList(trackType, field);

    if (field) {
        $('#xslPolicyRule_field').trigger('change');
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

function loadValuesList(trackType, field, value) {
    if (trackType && field) {
        policyTreeAjax.getValuesList(trackType, field, value);

        $('#xslPolicyRule_value').trigger('change');
    }
}

$(document).ready(function() {
    (function loop_init(time) {
        setTimeout(function() {
            if (webpage === undefined)
                loop_init(100);
            else
            {
                initPage();
                policyTreeAjax.getData();
            }
        }, time);
    })(0);
});
