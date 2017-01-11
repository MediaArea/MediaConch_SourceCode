function initPage() {
    mcoMessage.init('#policyInfo div');
    policyTree.init();
    user.loadToken();

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
    policyEditHelp();
    policyRuleHelp();
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

    // Create policy from file form
    $('form[name="xslPolicyCreateFromFile"]').on('submit', function (e) {
        e.preventDefault();

        policyTreeAjax.policyCreateFromFile($(this));
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

    // User login for token form
    $('form[name="userLoginToken"]').on('submit', function(e) {
        e.preventDefault();

        user.loginForToken($(this), 'userConnectModal.formSuccess', 'userConnectModal.formError');
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

    // Push policy to public policies
    $('.policyPushPublic').on('click', function () {
        var token = user.getMcoUserToken();
        if ('' == token) {
            userConnectModal.setAction('pushPublic');
            userConnectModal.show();
        }
        else {
            user.pushPolicyToMCO(policyTree.getPolicyId(), 'public');
        }
    });

    // Push policy to public policies
    $('.policyPushPrivate').on('click', function () {
        var token = user.getMcoUserToken();
        if ('' == token) {
            userConnectModal.setAction('pushPrivate');
            userConnectModal.show();
        }
        else {
            user.pushPolicyToMCO(policyTree.getPolicyId(), 'private');
        }
    });

    // Import policy from MCO
    $('.userPolicies').on('click', function () {
        var token = user.getMcoUserToken();
        if ('' == token) {
            userConnectModal.setAction('userPolicyList');
            userConnectModal.show();
        }
        else {
            userPoliciesModal.show();
        }
    });

    // MediaInfo rule
    $('.ruleMediaInfo').on('click', function () {
        policyTreeRulesMI.display(policyTree.getSelectedNode());
    });

    // MediaTrace rule
    $('.ruleMediaTrace').on('click', function () {
        policyTreeRulesMT.display(policyTree.getSelectedNode());
    });

    // Reload page
    $('.reload-page').on('click', function () {
        location.reload();
    });
}

var popoverHelp = (function() {
    // Keep popover open while hover on the popover content
    var popoverManualBinding = function(elem) {
        elem.on('mouseenter', function () {
            var _this = this;
            $(this).popover('show');
            $(this).siblings('.popover').on('mouseleave', function () {
                $(_this).popover('hide');
            });
        }).on('mouseleave', function () {
            var _this = this;
            setTimeout(function () {
                if (!$('.popover:hover').length) {
                    $(_this).popover('hide');
                }
            }, 300);
        });
    };

    // Add help sign and bind popover
    var add = function(elem, content, title, elemClass) {
        if (undefined === title) {
            var title = 'Help';
        }

        if (undefined === elemClass) {
            var elemClass = elem + 'Help';
        }
        else {
            var elemClass = elemClass + 'Help';
        }

        $(elem).append('&nbsp;<span class="glyphicon glyphicon-info-sign ' + elemClass + '" aria-hidden="true"></span>');
        var popHelp = $('.' + elemClass).popover({title: title, content: content, placement: 'auto top', trigger: 'manual', html: true});
        popoverManualBinding(popHelp);
    };

    return {
        add: add,
    }
})();

function policyEditHelp() {
    // Add help
    var addHelp = function(elem, content, title) {
        popoverHelp.add('label[for="xslPolicyInfo_' + elem + '"]', content, title, elem);
    };

    // Name
    addHelp(
        'policyName',
        'Brief summary of policy purpose.',
        'Policy name'
    );

    // Description
    addHelp(
        'policyDescription',
        'Optional description of the intent of the created policy, or policy and nested rules.',
        'Policy description'
    );

    // Type
    addHelp(
        'policyType',
        'Clarifies how subsequent rules should exist in relation to each other: \
        <ul><li>AND: Each rule works independently from other rules in policy. One failing rule causes the policy to fail.</li> \
        <li>OR: All rules set within the policy work in relation to each other and only one rule passing is necessary for the policy to pass.</li></ul>',
        'Policy type'
    );

    // License
    addHelp(
        'policyLicense',
        '<ul><li>Creative Commons Zero (1.0 or later): I like public domain</li> \
        <li>Creative Commons Attribution (4.0 or later): I want it permissive</li> \
        <li>Creative Commons Attribution-ShareAlike (4.0 or later): I care about sharing improvements</li> \
        <li>Other: a license not in the predefined list</li></ul> \
        <footer>More information about how to choose a license for your policies may be found on <a href="https://creativecommons.org/share-your-work/" onclick="urlUtils.openInBrowser(event);">Creative Commons website</a></footer>',
        'Policy license');

    // Visibility
    addHelp(
        'policyVisibility',
        'Allow your policy to be public and visible on the public policies list: \
        <ul><li>Private: your policy won\'t be visible to others.</li> \
        <li>Public : your policy will be visible by others.</li></ul>',
        'Policy visibility'
    );

    // Push to mco
    popoverHelp.add(
        '.policyPush',
        'You can add your policy to MediaConchOnline. \
        <ul><li>Public: the policy is added to your policies list and also to the public policy page to share it with everyone</li> \
        <li>Private: the policy is added to your policies list but is only available for you</li></ul>',
        'Push policy',
        'policyPush'
    );
}

function policyRuleHelp() {
    // Add help
    var addHelp = function(elem, content, title) {
        popoverHelp.add('label[for="xslPolicyRule_' + elem + '"]', content, title, elem);
    };

    // Name
    addHelp('title', 'Brief summary of rule purpose.', 'Rule name');

    // trackType
    addHelp('trackType', 'Specifies which section of the file the rule should  target.', 'Track type');

    // Field
    addHelp('field', 'Specifies the field (within the track) that the rule should target.', 'Field');

    // Occurrence
    addHelp('occurrence', 'Optional field to specify which track should be tested. Leaving this field blank will test all tracks.', 'Occurrence');

    // Validator
    addHelp('validator', 'Applies the appropriate operator to the rule.', 'Validator');
}

var createPolicyFromFile = (function() {
    var addSpinner = function() {
        $('#xslPolicyCreateFromFile_CreatePolicyFromFile').after('<span class="spinner-policy"></span>');
    }

    var removeSpinner = function() {
        $('.spinner-policy').remove();
    }

    return {
        addSpinner: addSpinner,
        removeSpinner: removeSpinner,
    }
})();

$(document).ready(function () {
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
