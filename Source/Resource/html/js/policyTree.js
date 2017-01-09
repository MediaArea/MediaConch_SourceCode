var policyTree = (function() {
    var instance;

    var init = function() {
        // Disable F2 shortcut (rename) in jstree, needs to be done before jstree initialization
        $('#policiesTree').on('keydown.jstree', '.jstree-anchor', function(event) {
            if (113 == event.keyCode) {
                event.stopImmediatePropagation();

                return false;
            }
        });

        $('#policiesTree').jstree({
            core: {
                check_callback: function (operation, node, parent, position, more) {
                    if ('move_node' === operation || 'copy_node' === operation) {
                        // Do not allow drop between elements
                        if (more && more.dnd && 'i' !== more.pos) {
                            return false;
                        }

                        // Do not allow "cut" on self node
                        if ('move_node' === operation && node.parent == parent.id) {
                            return false;
                        }

                        // Allow droping a rule only into a policy
                        if ('r' === node.type && 'u' != parent.type) {
                            return false;
                        }

                        // For system policy and system rule force copy instead of move
                        if ('s' === node.type || ('r' === node.type && 's' == instance.get_node(node.parent).type)) {
                            instance.settings.dnd.always_copy = true;
                        }
                        else {
                            instance.settings.dnd.always_copy = false;
                        }

                        // Allow drop only on user policy
                        if ('u' === parent.type || 'up' === parent.type) {
                            return true;
                        }

                        return false;
                    }
                    else {
                        return true;
                    }
                },
                multiple: false,
                dblclick_toggle: true,
            },
            plugins: ['search', 'types', 'contextmenu', 'dnd'],
            types: {
                default: {'icon' : 'glyphicon glyphicon-folder-open'},
                a: {icon : 'glyphicon glyphicon-folder-open'},
                u: {icon : 'glyphicon glyphicon-folder-open'},
                s: {icon : 'glyphicon glyphicon-folder-open'},
                up: {icon : 'glyphicon glyphicon-folder-open'},
                sp: {icon : 'glyphicon glyphicon-folder-open'},
                r: {icon : 'glyphicon glyphicon-file'},
            },
            contextmenu : {
                items : function(node) {
                    switch (node.type) {
                        case 'u':
                            return userPolicyContextmenuItems(node);
                            break;
                        case 'up':
                            return userPoliciesContextmenuItems(node);
                            break;
                        case 's':
                            return systemPolicyContextmenuItems(node);
                            break;
                        case 'r':
                            return rulesContextmenuItems(node);
                            break;
                    }
                }
            },
            dnd: {
                inside_pos: 'last',
                use_html5: false
            }
        });

        instance = $('#policiesTree').jstree(true);
        searchBinding();
        rightPanelBinding();
        copyNodeBinding();
        moveNodeBinding();
    }

    var userPoliciesContextmenuItems = function(node) {
        return {
            addPolicy: {
                label: 'Add a policy',
                action: function() {
                    policyTreeAjax.policyCreate(node, -1);
                }
            },
            ccp: {
                separator_before: true,
                label: 'Edit',
                action: false,
                submenu: {
                    pastePolicy: {
                        label: 'Paste',
                        _disabled: function (data) {
                            return !can_paste(node, instance.get_buffer());
                        },
                        action: function (data) {
                            var obj = instance.get_node(data.reference),
                                buffer = instance.get_buffer();

                            if ('copy_node' == buffer.mode) {
                                policyTreeAjax.policyDuplicate(buffer.node[0], obj);
                            }
                            else if ('move_node' == buffer.mode) {
                                policyTreeAjax.policyMove(buffer.node[0], obj);
                            }
                        }
                    }
                }
            }
        }
    }

    var userPolicyContextmenuItems = function(node) {
        return {
            addPolicy: {
                label: 'Add a policy',
                action: function() {
                    policyTreeAjax.policyCreate(node, node.data.policyId);
                }
            },
            addRule: {
                label: 'Add a rule',
                action: function() {
                    policyTreeAjax.ruleCreate(node);
                }
            },
            deletePolicy: {
                label: 'Delete',
                action: function() {
                    policyTreeAjax.policyDelete(node);
                }
            },
            exportPolicy: {
                label: 'Export',
                action: function() {
                    policyTreeAjax.policyExport(node);
                }
            },
            ccp: {
                separator_before: true,
                label: 'Edit',
                action: false,
                submenu: {
                    cutPolicy: {
                        label: 'Cut',
                        action: function (data) {
                            var obj = instance.get_node(data.reference);
                            if(instance.is_selected(obj)) {
                                instance.cut(instance.get_top_selected());
                            }
                            else {
                                instance.cut(obj);
                            }
                        }
                    },
                    copyPolicy: {
                        label: 'Copy',
                        action: function (data) {
                            var obj = instance.get_node(data.reference);
                            if(instance.is_selected(obj)) {
                                instance.copy(instance.get_top_selected());
                            }
                            else {
                                instance.copy(obj);
                            }
                        }
                    },
                    pastePolicy: {
                        label: 'Paste',
                        _disabled: function (data) {
                            return !instance.can_paste();
                        },
                        action: function (data) {
                            var obj = instance.get_node(data.reference),
                                buffer = instance.get_buffer();

                            if ('copy_node' == buffer.mode) {
                                if ('r' == buffer.node[0].type) {
                                    policyTreeAjax.ruleDuplicate(getParentPolicyId(buffer.node[0]), buffer.node[0], obj);
                                }
                                else {
                                    policyTreeAjax.policyDuplicate(buffer.node[0], obj);
                                }
                            }
                            else if ('move_node' == buffer.mode) {
                                if ('r' == buffer.node[0].type) {
                                    policyTreeAjax.ruleMove(getParentPolicyId(buffer.node[0]), buffer.node[0], obj)
                                }
                                else {
                                    policyTreeAjax.policyMove(buffer.node[0], obj);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    var systemPolicyContextmenuItems = function(node) {
        return {
            exportPolicy: {
                label: 'Export',
                action: function() {
                    policyTreeAjax.policyExport(node);
                }
            },
            ccp: {
                separator_before: true,
                label: 'Edit',
                action: false,
                submenu: {
                    copyPolicy: {
                        label: 'Copy',
                        action: function (data) {
                            var obj = instance.get_node(data.reference);
                            if(instance.is_selected(obj)) {
                                instance.copy(instance.get_top_selected());
                            }
                            else {
                                instance.copy(obj);
                            }
                        }
                    },
                }
            }
        }
    }

    var rulesContextmenuItems = function(node) {
        if ('u' == getParentPolicy().type) {
            return {
                deleteRule: {
                    label: 'Delete',
                    action: function() {
                        var parentId = instance.get_node(node.parent).data.policyId;
                        policyTreeAjax.ruleDelete(parentId, node);
                    }
                },
                ccp: {
                    separator_before: true,
                    label: 'Edit',
                    action: false,
                    submenu: {
                        cutPolicy: {
                            label: 'Cut',
                            action: function (data) {
                                var obj = instance.get_node(data.reference);
                                if(instance.is_selected(obj)) {
                                    instance.cut(instance.get_top_selected());
                                }
                                else {
                                    instance.cut(obj);
                                }
                            }
                        },
                        copyRule: {
                            label: 'Copy',
                            action: function (data) {
                                var obj = instance.get_node(data.reference);
                                if(instance.is_selected(obj)) {
                                    instance.copy(instance.get_top_selected());
                                }
                                else {
                                    instance.copy(obj);
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            return {
                copyRule: {
                    label: 'Copy',
                    action: function (data) {
                        var obj = instance.get_node(data.reference);
                        if(instance.is_selected(obj)) {
                            instance.copy(instance.get_top_selected());
                        }
                        else {
                            instance.copy(obj);
                        }
                    }
                }
            }
        }
    }

    var can_paste = function(node, buffer) {
        if (buffer.mode === false || buffer.node === false) {
            return false;
        }

        if ('up' == node.type && 'r' == buffer.node[0].type) {
            return false;
        }

        return true;
    }

    // Search
    var searchBinding = function() {
        instance.get_container().on('ready.jstree', function () {
            var to = false;
            $('#policiesTreeSearch').keyup(function () {
                if(to) { clearTimeout(to); }
                to = setTimeout(function () {
                    var v = $('#policiesTreeSearch').val();
                    instance.search(v);
                }, 250);
            });
        });
    }

    // Right panel update when a node is selected
    var rightPanelBinding = function () {
        instance.get_container().on('select_node.jstree', function (e, data) {
            instance.open_node(data.node);
            mcoMessage.close();
            switch (data.node.type) {
                case 'r':
                    policyTreeRules.display(data.node);
                    break;
                case 'up':
                case 'sp':
                    policyTreePolicies.manage(data.node);
                    break;
                case 'u':
                case 's':
                    policyTreePolicies.edit(data.node);
                    break;
            }

            $('#policyFix').affix('checkPosition');
        });
    }

    var copyNodeBinding = function() {
        instance.get_container().on('copy_node.jstree', function (e, data) {
            if ('u' == data.node.type || 's' == data.node.type) {
                instance.delete_node(data.node);
                policyTreeAjax.policyDuplicate(data.original, getParentPolicy(data.node));
            }
            else if ('r' == data.node.type) {
                instance.delete_node(data.node);
                policyTreeAjax.ruleDuplicate(getParentPolicyId(data.original), data.original, getParentPolicy(data.node));
            }
        });
    }

    var moveNodeBinding = function() {
        instance.get_container().on('move_node.jstree', function (e, data) {
            if ('u' == data.node.type) {
                instance.delete_node(data.node);
                policyTreeAjax.policyMove(data.node, getParentPolicy(data.node));
            }
            else if ('r' == data.node.type) {
                instance.delete_node(data.node);
                policyTreeAjax.ruleMove(instance.get_node(data.old_parent).data.policyId, data.node, getParentPolicy(data.node));
            }
        });
    }

    var getInstance = function() {
        return instance;
    }

    var setData = function(treeData) {
        instance.settings.core.data = treeData;
        instance.refresh(false, true);
    };

    var getSelectedNode = function() {
        var selected = instance.get_selected(true);

        return selected[0];
    }

    var getTopLevelPolicyId = function(selected) {
        if (undefined === selected) {
            var selected = getSelectedNode();
        }
        var policyId;

        if ('u' == selected.type || ('r' == selected.type && 'u' == instance.get_node(selected.parent).type)) {
            do {
                policyId = selected.data.policyId;
                selected = instance.get_node(selected.parent);
            }
            while (selected.id != 'u_p');
        }
        else if ('s' == selected.type || ('r' == selected.type && 's' == instance.get_node(selected.parent).type)) {
            do {
                policyId = selected.data.policyId;
                selected = instance.get_node(selected.parent);
            }
            while (selected.id != 's_p');
        }
        else {
            policyId = -1;
        }

        return policyId;
    }

    var getPolicyId = function(policy) {
        if (undefined === policy) {
            var policy = getSelectedNode();
        }

        if (typeof policy != 'undefined' && typeof policy.data != 'undefined' && null !== policy.data && policy.data.hasOwnProperty('policyId')) {
            return policy.data.policyId;
        }

        return -1;
    }

    var getParentPolicy = function(policy) {
        if (undefined === policy) {
            var policy = getSelectedNode();
        }

        return instance.get_node(policy.parent);
    }

    var getParentPolicyId = function(policy) {
        if (undefined === policy) {
            var policy = getSelectedNode();
        }

        return getPolicyId(getParentPolicy(policy));
    }

    var policyImport = function(policy) {
        var policyNodeId = instance.create_node('u_p', policy);
        instance.deselect_node(instance.get_selected(), true);
        instance.select_node(policyNodeId);
        mcoMessage.success('Policy imported successfuly');
    }

    var policyCreateFromFile = function(policy) {
        var policyNodeId = instance.create_node('u_p', policy);
        instance.deselect_node(instance.get_selected(), true);
        instance.select_node(policyNodeId);
        mcoMessage.success('Policy created successfuly');
    }

    var policyCreate = function(policy, selectedPolicy) {
        // Force CC-BY-SA-4.0+ license by default
        policy.data.license = 'CC-BY-SA-4.0+';

        if ('s_p' == selectedPolicy.id) {
            var policyNodeId = instance.create_node('u_p', policy);
        }
        else {
            var policyNodeId = instance.create_node(selectedPolicy, policy);
        }
        instance.deselect_node(instance.get_selected(), true);
        instance.select_node(policyNodeId);
        mcoMessage.success('Policy created successfuly');
    }

    var policyEdit = function(policy, selectedPolicy) {
        instance.rename_node(selectedPolicy, policy.text);
        selectedPolicy.data.description = policy.data.description;
        selectedPolicy.data.license = policy.data.license;
        selectedPolicy.data.type = policy.data.type;
        if (undefined !== policy.data.isPublic) {
            selectedPolicy.data.isPublic = policy.data.isPublic;
        }
        mcoMessage.success('Policy info changed successfuly');
    }

    var policyDelete = function(selectedPolicy) {
        var parent = instance.get_parent(selectedPolicy);
        instance.delete_node(selectedPolicy.id);
        instance.select_node(parent, true);
        mcoMessage.success('Policy successfuly removed');
    }

    var policyDuplicate = function(policy, dstPolicy) {
        var policyNodeId = instance.create_node(dstPolicy.id, policy);
        instance.deselect_all();
        instance.select_node(policyNodeId);
        mcoMessage.success('Policy successfuly duplicated');
    }

    var policyMove = function(policy, dstPolicy, origPolicy) {
        instance.delete_node(origPolicy);
        var policyNodeId = instance.create_node(dstPolicy.id, policy);
        instance.deselect_all();
        instance.select_node(policyNodeId);
        mcoMessage.success('Policy successfuly moved');
    }

    var ruleCreate = function(rule, selectedPolicy) {
        var ruleNodeId = instance.create_node(selectedPolicy, dataRuleToJstree(rule));
        instance.deselect_node(selectedPolicy, true);
        instance.select_node(ruleNodeId);
        mcoMessage.success('Policy rule successfully created');
    }

    var ruleEdit = function(rule, selectedRule) {
        instance.rename_node(selectedRule, (undefined == rule.name) ? '' : rule.name);
        selectedRule.data.tracktype = (undefined == rule.tracktype) ? '' : rule.tracktype;
        selectedRule.data.field = (undefined == rule.field) ? '' : rule.field;
        selectedRule.data.occurrence = (undefined == rule.occurrence || -1 == rule.occurrence || '*' == rule.occurrence) ? '' : rule.occurrence;
        selectedRule.data.ope = (undefined == rule.ope) ? '' : rule.ope;
        selectedRule.data.value = (undefined == rule.value) ? '' : rule.value;
        selectedRule.data.scope = (undefined == rule.scope) ? '' : rule.scope;
        mcoMessage.success('Rule successfuly edited');
    }

    var ruleDelete = function(selectedRule) {
        var parent = instance.get_node(selectedRule.parent);
        instance.deselect_node(selectedRule, true);
        instance.select_node(parent, true);
        instance.delete_node(selectedRule);
        mcoMessage.success('Policy rule successfully deleted');
    }

    var ruleDuplicate = function(rule, dstPolicy) {
        var ruleNodeId = instance.create_node(dstPolicy, dataRuleToJstree(rule));
        instance.deselect_all();
        instance.select_node(ruleNodeId);
        mcoMessage.success('Policy rule successfully duplicated');
    }

    var ruleMove = function(rule, dstPolicy, origRule) {
        instance.delete_node(origRule);
        var ruleNodeId = instance.create_node(dstPolicy, dataRuleToJstree(rule));
        instance.deselect_all();
        instance.select_node(ruleNodeId);
        mcoMessage.success('Policy rule successfully moved');
    }

    var dataRuleToJstree = function(rule) {
        return {
            text: (undefined == rule.name) ? '' : rule.name,
            type: 'r',
            data: {
                ruleId: rule.id,
                tracktype: (undefined == rule.tracktype) ? '' : rule.tracktype,
                field: (undefined == rule.field) ? '' : rule.field,
                occurrence: (undefined == rule.occurrence || -1 == rule.occurrence || '*' == rule.occurrence) ? '' : rule.occurrence,
                ope: (undefined == rule.ope) ? '' : rule.ope,
                value: (undefined == rule.value) ? '' : rule.value,
                scope: (undefined == rule.scope) ? '' : rule.scope,
            }
        }
    }

    return {
        init: init,
        getInstance: getInstance,
        getSelectedNode: getSelectedNode,
        getTopLevelPolicyId: getTopLevelPolicyId,
        getPolicyId: getPolicyId,
        getParentPolicy: getParentPolicy,
        getParentPolicyId: getParentPolicyId,
        setData: setData,
        policyImport: policyImport,
        policyCreateFromFile: policyCreateFromFile,
        policyCreate: policyCreate,
        policyEdit: policyEdit,
        policyDelete: policyDelete,
        policyDuplicate: policyDuplicate,
        policyMove: policyMove,
        ruleCreate: ruleCreate,
        ruleEdit: ruleEdit,
        ruleDelete: ruleDelete,
        ruleDuplicate: ruleDuplicate,
        ruleMove: ruleMove,
    };
})();
