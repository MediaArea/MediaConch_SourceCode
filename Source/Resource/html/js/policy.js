function initPage() {
    mcoMessage.init('#policyInfo div');
    policyTree.init();

    // Make buttons in policy rule form display inline
    // Duplicate button
    $('#xslPolicyRule_DuplicateRule').parent().addClass('xslPolicyRuleDuplicateButton');
    $('#xslPolicyRuleMt_DuplicateRule').parent().addClass('xslPolicyRuleDuplicateButton');
    // Save button
    $('#xslPolicyRule_SaveRule').parent().addClass('xslPolicyRuleSaveButton');
    $('#xslPolicyRuleMt_SaveRule').parent().addClass('xslPolicyRuleSaveButton');
    // Delete button
    $('#xslPolicyRule_DeleteRule').parent().addClass('xslPolicyRuleDeleteButton');
    $('#xslPolicyRuleMt_DeleteRule').parent().addClass('xslPolicyRuleDeleteButton');

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

    $('#xslPolicyRuleMt_validator').select2({
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

    // Policy rule MT edit form
    $('form[name="xslPolicyRuleMt"]').on('submit', function (e) {
        e.preventDefault();

        // Duplicate
        if ('xslPolicyRuleMt_DuplicateRule' == $('button[type=submit][clicked=true]').prop('id')) {
            policyTreeAjax.ruleDuplicate(policyTree.getParentPolicyId(), policyTree.getSelectedNode(), policyTree.getParentPolicy());
        }
        // Delete
        else if ('xslPolicyRuleMt_DeleteRule' == $('button[type=submit][clicked=true]').prop('id')) {
            policyTreeAjax.ruleDelete(policyTree.getParentPolicyId(), policyTree.getSelectedNode());
        }
        // Edit
        else {
            policyTreeAjax.ruleEdit($(this), policyTree.getParentPolicyId(), policyTree.getSelectedNode());
        }
    });

    // Policy rule duplicate
    $('#xslPolicyRule_DuplicateRule').on('click', function (e) {
        e.preventDefault();

        policyTreeAjax.ruleDuplicate(policyTree.getParentPolicyId(), policyTree.getSelectedNode(), policyTree.getParentPolicy());
    });

    // Policy rule MT duplicate
    $('#xslPolicyRuleMt_DuplicateRule').on('click', function (e) {
        e.preventDefault();

        policyTreeAjax.ruleDuplicate(policyTree.getParentPolicyId(), policyTree.getSelectedNode(), policyTree.getParentPolicy());
    });

    // Policy rule delete
    $('#xslPolicyRule_DeleteRule').on('click', function (e) {
        e.preventDefault();

        policyTreeAjax.ruleDelete(policyTree.getParentPolicyId(), policyTree.getSelectedNode());
    });

    // Policy rule MT delete
    $('#xslPolicyRuleMt_DeleteRule').on('click', function (e) {
        e.preventDefault();

        policyTreeAjax.ruleDelete(policyTree.getParentPolicyId(), policyTree.getSelectedNode());
    });

    // Policy rule edit form trackType select list
    $('#xslPolicyRule_trackType').on('change', function() {
        if ('undefined' === $('#xslPolicyRule_field').val()) {
            var field = null;
        }
        else {
            var field = $('#xslPolicyRule_field').val();
        }

        policyTreeRulesMI.loadFieldsList($('#xslPolicyRule_trackType').val(), field);
        policyTreeRulesMI.displayOccurenceField($('#xslPolicyRule_trackType').val());
    });

    // Policy rule edit form field select list
    $('#xslPolicyRule_field').on('change', function() {
        policyTreeRulesMI.loadValuesList($('#xslPolicyRule_trackType').val(), $('#xslPolicyRule_field').val(), $('#xslPolicyRule_value').val());
    });

    // Policy rule edit form validator select list
    $('#xslPolicyRule_validator').on('change', function() {
        policyTreeRulesMI.displayValueField($('#xslPolicyRule_validator').val());
    })

    // Policy rule MT edit form validator select list
    $('#xslPolicyRuleMt_validator').on('change', function() {
        policyTreeRulesMT.displayValueField($('#xslPolicyRuleMt_validator').val());
    })

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

    // MediaInfo rule
    $('.ruleMediaInfo').on('click', function () {
        policyTreeRulesMI.display(policyTree.getSelectedNode());
    });

    // MediaTrace rule
    $('.ruleMediaTrace').on('click', function () {
        policyTreeRulesMT.display(policyTree.getSelectedNode());
    });
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
