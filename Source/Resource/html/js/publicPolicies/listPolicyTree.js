var policyTree = (function() {
    var instance = null;
    var policyId = null;
    var policyUserId = null;

    var init = function() {
        // Disable F2 shortcut (rename) in jstree, needs to be done before jstree initialization
        $('#policyTree').on('keydown.jstree', '.jstree-anchor', function(event) {
            if (113 == event.keyCode) {
                event.stopImmediatePropagation();

                return false;
            }
        });

        $('#policyTree').jstree({
            core: {
                check_callback: function (operation, node, parent, position, more) {
                    if (operation === 'copy_node' || operation === 'move_node') {
                        return false; // disable copy and move
                    }
                    else {
                        return true;
                    }
                },
                multiple: false,
                dblclick_toggle: true,
                data: {
                    url: function (node) {
                        return publicPoliciesListAjax.getApiPolicyUrl(policyId, policyUserId);
                    },
                    dataType: 'json',
                    success: function() {
                        $('#modalPolicy .modal-body .policyError').remove();
                        $('#policyTree').show();
                        $('#modalPolicy .btn-primary').prop('disabled', false);
                    },
                    error: function() {
                        $('#policyTree').hide();
                        $('#modalPolicy .modal-body').append('<div class="policyError">Error: the requested policy can not be retrieved</div>');
                        $('#modalPolicy .btn-primary').prop('disabled', true);
                    }
                }
            },
            plugins: ['search', 'types'],
            types: {
                default: {'icon' : 'glyphicon glyphicon-folder-open'},
                a: {icon : 'glyphicon glyphicon-folder-open'},
                u: {icon : 'glyphicon glyphicon-folder-open'},
                s: {icon : 'glyphicon glyphicon-folder-open'},
                up: {icon : 'glyphicon glyphicon-folder-open'},
                sp: {icon : 'glyphicon glyphicon-folder-open'},
                r: {icon : 'glyphicon glyphicon-file'},
            }
        })
        .on('refresh.jstree', function () {
            $(this).jstree('open_all');
        })
        .on('hover_node.jstree', function (event, data) {
            $('#' + data.node.id + '_anchor').popover({title: policyHoverTitle(data.node), content: policyHoverContent(data.node), placement: 'auto right', trigger: 'hover', html: true}).popover('show');
        })
        .on('ready.jstree', function () {
            $(this).jstree('open_all');

            var to = false;
            $('#policyTreeSearch').keyup(function () {
                if(to) { clearTimeout(to); }
                to = setTimeout(function () {
                    var v = $('#policyTreeSearch').val();
                    instance.search(v);
                }, 250);
            });
        });

        instance = $('#policyTree').jstree(true);
    }

    var refreshData = function(id, userId) {
        policyId = id;
        policyUserId = userId;

        if (null === instance) {
            init();
        }
        else {
            instance.refresh();
        }
    };

    var policyHoverTitle = function(node) {
        var data = '';
        if ('u' == node.type) {
            if ('' != node.data.description) {
                data = 'Policy';
            }
        }
        else if ('r' == node.type) {
            data = 'Rule';
        }

        return data;
    };

    var policyHoverContent = function(node) {
        var data = '';
        if ('u' == node.type) {
            if ('' != node.data.description) {
                data = textUtils.nl2br(textUtils.sanitizeHtml(node.data.description));;
            }
        }
        else if ('r' == node.type) {
            data += '<ul>';
            if (undefined == node.data.scope || '' == node.data.scope) {
                data += '<li>Scope: MediaInfo</li>';
            }
            else {
                data += '<li>Scope: MicroMediaTrace</li>';
            }

            if (undefined != node.data.tracktype) {
                data += '<li>TrackType: ' + node.data.tracktype + '</li>';
            }

            if (undefined != node.data.field) {
                data += '<li>Field: ' + node.data.field + '</li>';
            }

            if (undefined == node.data.occurrence || -1 == node.data.occurrence || '*' == node.data.occurrence) {
                data += '<li>Occurence: *</li>';
            }
            else {
                data += '<li>Occurence: ' + node.data.occurrence + '</li>';
            }

            if (undefined != node.data.ope) {
                data += '<li>Operator: ' + node.data.ope + '</li>';
            }

            if (undefined != node.data.value) {
                data += '<li>Value: ' + node.data.value + '</li>';
            }
            data += '</ul>';
        }

        return data;
    }

    return {
        init: init,
        refreshData: refreshData,
    };
})();
