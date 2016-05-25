$(document).ready(function() {
    result = $('#result-table').DataTable({
        'order': [],
        'autoWidth': false,
        'columnDefs': [
            { 'orderable': true, targets: 0 },
            { 'orderable': true, 'searchable': false, targets: [1, 2, 5] },
            { 'orderable': false, 'searchable': false, 'width': '10%', targets: [3, 4] },
            { 'width': '15%', targets: [1, 5] },
            { 'width': '25%', targets: [0, 2] },
        ]
    });

    // Upload form
    $('#file form').on('submit', function (e) {
        e.preventDefault();
        webpage.on_file_upload_selected($('.tab-content .active .policyList').val(),
                                        $('.tab-content .active .displayList').val(),
                                        $('.tab-content .active .verbosityList').val());
    });

    // Online form
    $('#url form').on('submit', function (e) {
        e.preventDefault();
        webpage.on_file_online_selected($('#checkerOnline_file').val(),
                                        $('.tab-content .active .policyList').val(),
                                        $('.tab-content .active .displayList').val(),
                                        $('.tab-content .active .verbosityList').val());
    });

    // Repository form
    $('#repository form').on('submit', function (e) {
        e.preventDefault();
        webpage.on_file_repository_selected($('.tab-content .active .policyList').val(),
                                            $('.tab-content .active .displayList').val(),
                                            $('.tab-content .active .verbosityList').val());
    });

    // Remove all results blocks
    $('#checkerResultTitle .close').click(function () {
        result.clear().draw();

        webpage.close_all();
        // Remove close all button
        $(this).addClass('hidden');
    });

    // Apply policy to all
    $('#checkerApplyAll').html('<div class="applyAll form-horizontal"></div>');
    $('#checkerApplyAll').addClass('tab-pane');

    // Duplicate policy list
    policyList = $('.tab-content .active .policyList').clone();
    policyList.attr('id', 'applyAllPolicy');
    policyList.children('option:first').text('Choose a new policy to apply').prop('selected', true);
    $('#checkerApplyAll div.applyAll').append('<div class="col-md-12"><div class="form-group"><label class="pull-left control-label">Apply a policy to all results</label><div class="col-sm-4 policy">')
    $('#checkerApplyAll div.applyAll div.policy').html(policyList);

    $('#applyAllPolicy').on('change', function(e) {
        applyPolicyToAll();
        resetSelectList('applyAllPolicy');
    });
});

function updateFileOrAddFile(sourceName, fileName, fileId, values) {
    var formValues = JSON.parse(values);
    if (!result.$('tr.fileId-' + fileId).length) {
        addFile(sourceName, fileName, fileId, formValues);
    } else {
        updateFile(fileId, formValues)
    }
}

function updateFile(fileId, formValues) {
    node = result.$('#result-' + fileId);

    // Update policy if it has changed
    if (node.data('policy') != formValues.policy && (2 == node.data('tool') || undefined == node.data('tool'))) {
        node.data('policy', formValues.policy);
        node.data('policyName', formValues.policyText);

        updatePolicyCell(fileId, node.data('policy'));
    }

    // Update display if it has changed
    if (node.data('display') != formValues.display && (2 == node.data('tool') || undefined == node.data('tool'))) {
        node.data('display', formValues.display);

        removeImplemModalIfExists(fileId);
        removePolicyModalIfExists(fileId);
    }

    // Update verbosity if it has changed
    if (node.data('verbosity') != formValues.verbosity && (2 == node.data('tool') || undefined == node.data('tool'))) {
        node.data('verbosity', formValues.verbosity);

        removeImplemModalIfExists(fileId);
    }
}

function addFile(sourceName, fileName, fileId, formValues) {
    node = result.row.add( [ '<span title="' + sourceName + '">' + truncateString(fileName, 23) + '</span>', '', '', '', '', '<span class="status-text">In queue</span><button type="button" class="btn btn-link result-close" title="Close result"><span class="glyphicon glyphicon-trash" aria-hidden="true"></span></button><button type="button" class="btn btn-link hidden" title="Reload result"><span class="glyphicon glyphicon-refresh" aria-hidden="true"></span></button>' ] ).draw(false).node();

    // Add id
    resultId = 'result-' + fileId;
    $(node).prop('id', resultId);
    $(node).addClass('fileId-' + fileId);
    $(node).data('fileId', fileId);

    // Add policy, display and verbosity
    $(node).data('policy', formValues.policy);
    $(node).data('policyName', formValues.policyText);
    $(node).data('display', formValues.display);
    $(node).data('verbosity', formValues.verbosity);

    // Change status class
    $(result.cell(node, 5).node()).addClass('info');

    // Close button
    $(node).find('.result-close').click(node, function (e) {
        result.row(e.data).remove().draw(false);

        // Remove close all button
        if (1 == $('table.checker-results tbody tr').length && $('table.checker-results tbody tr .dataTables_empty').length) {
            $('#checkerResultTitle .close').addClass('hidden');
            $('#checkerApplyAll').addClass('hidden');
        };
    });

    // Waiting loop for analyze
    (function theLoop(resultId, time, i) {
        setTimeout(function () {
            if (!$(result.cell('#' + resultId, 5).node()).hasClass('success')) {
                    if (WEBMACHINE == "WEB_MACHINE_KIT") {
                        analyzed = webpage.file_is_analyzed(sourceName);
                        var data = JSON.parse(analyzed);
                        processCheckerStatusRequest(data, resultId, fileId)
                    }
                    else {
                        webpage.file_is_analyzed(sourceName, function (analyzed) {
                            var data = JSON.parse(analyzed);
                            processCheckerStatusRequest(data, resultId, fileId)
                        });
                    }
            }
            // If i > 0, keep going
            if ($(result.cell('#' + resultId, 5).node()).hasClass('info') && --i > 0) {
                // Call the loop again
                theLoop(resultId, time * 2, i);
            }
        }, time);
    })(resultId, 100, 15);

    if ($('#checkerResultTitle .close').hasClass('hidden')) {
        $('#checkerResultTitle .close').removeClass('hidden');
        $('#checkerApplyAll').removeClass('hidden');
    }
};

function processCheckerStatusRequest(data, resultId, fileId) {
    sourceName = $(result.cell('#' + resultId, 0).node()).find('span').attr('title');
    if (data.finish) {
        node = result.$('#' + resultId);
        // Report type
        node.data('tool', data.tool);

        // Implementation
        if (WEBMACHINE == "WEB_MACHINE_KIT") {
            valid = webpage.implementation_is_valid(sourceName);
            var implemData = {};
            implemData.valid = valid;
            implementationCell(implemData, resultId, fileId);
        } else {
            webpage.implementation_is_valid(sourceName, function (valid) {
                var implemData = {};
                implemData["valid"] = valid;
                implementationCell(implemData, resultId, fileId);
            });
        }

        // Policy
        if (2 == data.tool)
        {
            if (node.data('policy'))
            {
                if (WEBMACHINE == "WEB_MACHINE_KIT") {
                    valid = webpage.policy_is_valid(sourceName);
                    var policyData = {};
                    policyData["valid"] = valid;
                    policyCell(policyData, resultId, fileId);
                }
                else {
                    webpage.policy_is_valid(sourceName, function (valid) {
                        var policyData = {};
                        policyData["valid"] = valid;
                        policyCell(policyData, resultId, fileId);
                    });
                }
            }
            else {
                policyCellEmptyWithModal(resultId, fileId)
            }
        }
        else {
            policyCellEmptyWithoutModal(resultId)
        }

        // MediaInfo
        mediaInfoCell(resultId, fileId);

        // MediaTrace
        mediaTraceCell(resultId, fileId);

        // Status
        statusCell(resultId, fileId);

        //stop timer
        //i = 0;
    }
    else if (data.percent > 0) {
        $(result.cell('#' + resultId, 5).node()).find('.status-text').text('Analyzing');
    }
}

function statusCell(resultId, fileId) {
    nodeStatus = $(result.cell('#' + resultId, 5).node());
    nodeStatus.removeClass().addClass('success');
    nodeStatus.find('.status-text').text('Analyzed');
};

function implementationCell(data, resultId, fileId) {
    nodeCell = result.$('#' + resultId);
    nodeImplem = $(result.cell(nodeCell, 1).node());
    if (data.valid) {
        nodeImplem.addClass('success');
        implemResultText = '<span class="glyphicon glyphicon-ok text-success" aria-hidden="true"></span> Valid'
    }
    else {
        nodeImplem.addClass('danger');
        implemResultText = '<span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span> Not valid';
    }

    result.cell(nodeCell, 1).data(implemResultText + '<p class="pull-right"><a href="#" data-toggle="modal" data-target="#modalConformance' + resultId + '" title="View implementation report"><span class="glyphicon glyphicon-eye-open implem-view" aria-hidden="true"></span></a><a href="#" class="implem-dld" data-target="#modalConformance' + resultId + '" title="Download implementation report"><span class="glyphicon glyphicon-download" aria-hidden="true"></span></a></p>');

    nodeImplem.find('.implem-view').on('click', function(e) {
        name = $(result.cell('#' + resultId, 0).node()).find('span').attr('title');
        e.preventDefault();
        nodeModal = result.$('#' + resultId);
        if (!$('#modalConformance' + resultId).length) {
            $('.result-container').append(' \
<div id="modalConformance' + resultId + '" \ class="modal fade"> \
<div class="modal-dialog modal-lg"> \
<div class="modal-content"> \
<div class="modal-header"> \
<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button> \
<h4 class="modal-title">Implementation report</h4> \
</div> \
<div class="modal-header form-horizontal"> \
<div class="col-md-6"> \
<div class="form-group"><label class="col-sm-2 control-label">Display</label><div class="col-sm-10"><select id="modalConformanceDisplay' + resultId + '"></select></div></div> \
</div> \
<div class="col-md-6"> \
<div class="form-group"><label class="col-sm-3 control-label">Verbosity</label><div class="col-sm-9"><select id="modalConformanceVerbosity' + resultId + '"></select></div></div> \
</div> \
</div> \
<div class="modal-body"></div> \
<div class="modal-footer"> \
<button type="button" class="btn btn-primary implem-dld" data-target="#modalConformance' + resultId + '">Download implementation report</button> \
<button type="button" class="btn btn-default" data-dismiss="modal">Close</button> \
</div> \
</div> \
</div> \
</div>');

            if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                data = webpage.on_fill_implementation_report(name, $(node).data('display'), $(node).data('verbosity'));
                displayReport('#modalConformance' + resultId, data);
            } else {
                webpage.on_fill_implementation_report(name, $(node).data('display'), $(node).data('verbosity'), function (data) {
                    displayReport('#modalConformance' + resultId, data);
                });
            }

            $('#modalConformance' + resultId + ' .implem-dld').on('click', function(e) {
                e.preventDefault();
                modalDisplay = $('#modalConformanceDisplay' + resultId).val();
                modalVerbosity = $('#modalConformanceVerbosity' + resultId).val();
                webpage.on_save_implementation_report(name, modalDisplay, modalVerbosity);
            });

            // Update report when display is changed
            displayList = $('.tab-content .active .displayList').clone();
            displayList.attr('id', 'modalConformanceDisplay' + resultId);
            displayList.find("option[value = '" + nodeModal.data('display') + "']").attr('selected', 'selected');
            $('#modalConformanceDisplay' + resultId).replaceWith(displayList);
            $('#modalConformanceDisplay' + resultId).on('change', function(e) {
                modalDisplay = $('#modalConformanceDisplay' + resultId).val();
                modalVerbosity = $('#modalConformanceVerbosity' + resultId).val();
                if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                    data = webpage.on_fill_implementation_report(name, modalDisplay, modalVerbosity);
                    displayReport('#modalConformance' + resultId, data);
                } else {
                    webpage.on_fill_implementation_report(name, modalDisplay, modalVerbosity, function (data) {
                        displayReport('#modalConformance' + resultId, data);
                    });
                }
            });

            // Update report when verbosity is changed
            verbosityList = $('.tab-content .active .verbosityList').clone();
            verbosityList.attr('id', 'modalConformanceVerbosity' + resultId);
            verbosityList.find("option[value = '" + nodeModal.data('verbosity') + "']").attr('selected', 'selected');
            $('#modalConformanceVerbosity' + resultId).replaceWith(verbosityList);
            $('#modalConformanceVerbosity' + resultId).on('change', function(e) {
                modalDisplay = $('#modalConformanceDisplay' + resultId).val();
                modalVerbosity = $('#modalConformanceVerbosity' + resultId).val();
                if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                    data = webpage.on_fill_implementation_report(name, modalDisplay, modalVerbosity);
                    displayReport('#modalConformance' + resultId, data);
                } else {
                    webpage.on_fill_implementation_report(name, modalDisplay, modalVerbosity, function (data) {
                        displayReport('#modalConformance' + resultId, data);
                    });
                }
            });

            if (2 != nodeModal.data('tool')) {
                $('#modalConformance' + resultId + ' .modal-header.form-horizontal').hide();
            }
        }
    });

    nodeImplem.find('.implem-dld').on('click', function(e) {
        e.preventDefault();
        nodeDld = result.$('#' + resultId);
        sourceName = $(result.cell('#' + resultId, 0).node()).find('span').attr('title');
        webpage.on_save_implementation_report(sourceName, $(node).data('display'), $(node).data('verbosity'));
    });
}

function policyCell(data, resultId, fileId)
{
    nodeCell = result.$('#' + resultId);
    nodePolicy = $(result.cell(nodeCell, 2).node());
    policyResultText = '<span class="policyResult">';
    if (data.valid) {
        nodePolicy.removeClass().addClass('success');
        policyResultText += '<span class="glyphicon glyphicon-ok text-success" aria-hidden="true"></span> '
    }
    else {
        nodePolicy.removeClass().addClass('danger');
        policyResultText += '<span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span> ';
    }

    policyResultText += '<span title="' + nodeCell.data('policyName') + '">' + truncateString(nodeCell.data('policyName'), 16) + '</span>';
    policyResultText += '</span>';

    result.cell(nodeCell, 2).data(policyResultText + '<p class="pull-right"><a href="#" data-toggle="modal" data-target="#modalPolicy' + resultId + '" title="View policy report"><span class="glyphicon glyphicon-eye-open policy-view" aria-hidden="true"></span></a><a href="#" class="policy-dld" data-target="#modalPolicy' + resultId + '" title="Download policy report"><span class="glyphicon glyphicon-download" aria-hidden="true"></span></a></p>');

    policyModal(resultId, fileId);
}

function policyCellEmptyWithModal(resultId, fileId)
{
    nodePolicy = $(result.cell('#' + resultId, 2).node());
    nodePolicy.removeClass().addClass('info');
    result.cell('#' + resultId, 2).data('<span class="policyResult">N/A</span><p class="pull-right"><a href="#" data-toggle="modal" data-target="#modalPolicy' + resultId + '" title="View policy report"><span class="glyphicon glyphicon-eye-open policy-view" aria-hidden="true"></span></a></p>');

    policyModal(resultId, fileId);
}

function policyCellEmptyWithoutModal(resultId)
{
    nodePolicy = $(result.cell('#' + resultId, 2).node());
    nodePolicy.removeClass().addClass('info');
    result.cell('#' + resultId, 2).data('N/A');
}

function policyModal(resultId, fileId)
{
    nodePolicy = $(result.cell('#' + resultId, 2).node());
    nodePolicy.find('.policy-view').on('click', function(e) {
        e.preventDefault();
        nodeModal = result.$('#' + resultId);
        if (!$('#modalPolicy' + resultId).length) {
            $('.result-container').append(' \
<div id="modalPolicy' + resultId + '" \ class="modal fade"> \
<div class="modal-dialog modal-lg"> \
<div class="modal-content"> \
<div class="modal-header"> \
<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button> \
<h4 class="modal-title">Policy report</h4> \
</div> \
<div class="modal-header form-horizontal"> \
<div class="col-md-6"> \
<div class="form-group"><label class="col-sm-2 control-label">Policy</label><div class="col-sm-10"><select id="modalPolicyPolicy' + resultId + '"></select></div></div> \
</div> \
<div class="col-md-6"> \
<div class="form-group"><label class="col-sm-2 control-label">Display</label><div class="col-sm-10"><select id="modalPolicyDisplay' + resultId + '"></select></div></div> \
</div> \
</div> \
<div class="modal-body"></div> \
<div class="modal-footer"> \
<button type="button" class="btn btn-primary policy-dld" data-target="#modalPolicy' + resultId + '">Download policy report</button> \
<button type="button" class="btn btn-default" data-dismiss="modal">Close</button> \
</div> \
</div> \
</div> \
</div>');

            if (nodeModal.data('policy')) {
                sourceName = $(result.cell('#' + resultId, 0).node()).find('span').attr('title');
                if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                    data = webpage.on_fill_policy_report(sourceName, $(node).data('policy'), $(node).data('display'));
                    displayReport('#modalPolicy' + resultId, data);
                } else {
                    webpage.on_fill_policy_report(sourceName, $(node).data('policy'), $(node).data('display'), function (data) {
                        displayReport('#modalPolicy' + resultId, data);
                    });
                }
            }

            $('#modalPolicy' + resultId + ' .policy-dld').on('click', function(e) {
                e.preventDefault();
                modalDisplay = $('#modalPolicyDisplay' + resultId).val();
                modalPolicy = $('#modalPolicyPolicy' + resultId).val();
                if (modalPolicy)
                {
                    sourceName = $(result.cell('#' + resultId, 0).node()).find('span').attr('title');
                    webpage.on_save_policy_report(sourceName, modalPolicy, modalDisplay);
                }
            });

            // Update report when display is changed
            displayList = $('.tab-content .active .displayList').clone();
            displayList.attr('id', 'modalPolicyDisplay' + resultId);
            displayList.find("option[value = '" + nodeModal.data('display') + "']").attr('selected', 'selected');
            $('#modalPolicyDisplay' + resultId).replaceWith(displayList);
            $('#modalPolicyDisplay' + resultId).on('change', function(e) {
                modalDisplay = $('#modalPolicyDisplay' + resultId).val();
                modalPolicy = $('#modalPolicyPolicy' + resultId).val();
                if (modalPolicy) {
                    sourceName = $(result.cell('#' + resultId, 0).node()).find('span').attr('title');
                    if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                        data = webpage.on_fill_policy_report(sourceName, modalPolicy, modalDisplay);
                        displayReport('#modalPolicy' + resultId, data);
                    } else {
                        webpage.on_fill_policy_report(sourceName, modalPolicy, modalDisplay, function (data) {
                            displayReport('#modalPolicy' + resultId, data);
                        });
                    }
                }
                else {
                    $('#modalPolicy' + resultId + ' .modal-body').empty('');
                }
            });

            // Update report when policy is changed
            policyList = $('.tab-content .active .policyList').clone();
            policyList.attr('id', 'modalPolicyPolicy' + resultId);
            policyList.find("option[value = '" + nodeModal.data('policy') + "']").attr('selected', 'selected');
            $('#modalPolicyPolicy' + resultId).replaceWith(policyList);
            $('#modalPolicyPolicy' + resultId).on('change', function(e) {
                modalDisplay = $('#modalPolicyDisplay' + resultId).val();
                modalPolicy = $('#modalPolicyPolicy' + resultId).val();
                if (modalPolicy)
                {
                    sourceName = $(result.cell('#' + resultId, 0).node()).find('span').attr('title');
                    if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                        data = webpage.on_fill_policy_report(sourceName, modalPolicy, modalDisplay);
                        displayReport('#modalPolicy' + resultId, data);
                    } else {
                        webpage.on_fill_policy_report(sourceName, modalPolicy, modalDisplay, function (data) {
                            displayReport('#modalPolicy' + resultId, data);
                        });
                    }
                }
                else {
                    $('#modalPolicy' + resultId + ' .modal-body').empty('');
                }
            });
        }
    });

    nodePolicy.find('.policy-dld').on('click', function(e) {
        e.preventDefault();
        nodeDld = result.$('#' + resultId);
        sourceName = $(result.cell('#' + resultId, 0).node()).find('span').attr('title');
        webpage.on_save_policy_report(sourceName, $(node).data('policy'), $(node).data('display'));
    });
}

function removePolicyModalIfExists(fileId)
{
    if ($('#modalPolicyresult-' + fileId).length) {
        $('#modalPolicyresult-' + fileId).remove();
    }
}

function removeImplemModalIfExists(fileId)
{
    if ($('#modalConformanceresult-' + fileId).length) {
        $('#modalConformanceresult-' + fileId).remove();
    }
}

function updatePolicyCell(fileId, policyId) {
    removePolicyModalIfExists(fileId);

    // Update cell if analysis of file is succeeded
    if ($(result.cell('#result-' + fileId, 5).node()).hasClass('success')) {
        if (policyId.length && policyId >= 0) {
            resetPolicyCell(fileId);
            sourceName = $(result.cell('#result-' + fileId, 0).node()).find('span').attr('title');
            if (WEBMACHINE == "WEB_MACHINE_KIT") {
                valid = webpage.policy_is_valid(sourceName);
                var policyData = {};
                policyData["valid"] = valid;
                resultId = 'result-' + fileId;
                policyCell(policyData, resultId, fileId);
            }
            else {
                webpage.policy_is_valid(sourceName, function (valid) {
                    var policyData = {};
                    policyData["valid"] = valid;
                    resultId = 'result-' + fileId;
                    policyCell(policyData, resultId, fileId);
                });
            }
        }
        else {
            policyCellEmptyWithModal(resultId, fileId)
        }
    }
}

function resetPolicyCell(fileId) {
    $(result.cell('#result-' + fileId, 2).node()).removeClass();
    $(result.cell('#result-' + fileId, 2).node()).empty();
}

function mediaInfoCell(resultId, fileId)
{
    nodeMI = $(result.cell('#' + resultId, 3).node());
    nodeMI.addClass('text-center');
    result.cell('#' + resultId, 3).data('<a href="#" class="mi-view" data-toggle="modal" data-target="#modalInfo' + resultId + '" title="View MediaInfo report"><span class="glyphicon glyphicon-eye-open" aria-hidden="true"></span></a><a href="#" class="mi-dld" data-target="#infoXml' + resultId + '" data-save-name="' + resultId + '_MediaInfo.xml" title="Download MediaInfo report"><span class="glyphicon glyphicon-download" aria-hidden="true"></span></a>');

    nodeMI.find('.mi-view').on('click', function(e) {
        e.preventDefault();
        if (!$('#modalInfo' + resultId).length) {
            $('.result-container').append(' \
<div id="modalInfo' + resultId + '" \ class="modal fade"> \
<div class="modal-dialog modal-lg"> \
<div class="modal-content"> \
<div class="modal-header"> \
<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button> \
<h4 class="modal-title">MediaInfo report</h4> \
</div> \
<div class="modal-body"> \
<div class="row"> \
<div class="col-md-6"> \
<i class="glyphicon glyphicon-search"></i><input type="text" value="" class="jstreeSearch" id="infoSearch' + resultId + '" placeholder="Search" /> \
</div> \
<div class="col-md-12"> \
<div id="info' + resultId + '"></div> \
</div> \
</div> \
</div> \
<div class="modal-footer"> \
<button type="button" class="btn btn-primary mi-dld" data-target="#modalInfo' + resultId + '" data-save-name="' + resultId + '_MediaInfoReport.txt">Download MediaInfo report</button> \
<button type="button" class="btn btn-default" data-dismiss="modal">Close</button> \
</div> \
</div> \
</div> \
</div>');

            mediaInfoTree(resultId, fileId);

            $('#modalInfo' + resultId + ' .mi-dld').on('click', function(e) {
                e.preventDefault();
                webpage.on_save_mediainfo_report($(result.cell('#' + resultId, 0).node()).find('span').attr('title'));
            });
        }
    });

    nodeMI.find('.mi-dld').on('click', function(e) {
        e.preventDefault();
        webpage.on_save_mediainfo_report($(result.cell('#' + resultId, 0).node()).find('span').attr('title'));
    });
};

function mediaInfoTree(resultId, fileId)
{
    $('#info' + resultId).jstree({
        'core' : {
            'check_callback' : function (operation, node, parent, position, more) {
                if (operation === 'copy_node' || operation === 'move_node') {
                    return false; // disable copy and move
                }
                else {
                    return true;
                }
            },
            'multiple' : false,
            'dblclick_toggle' : false,
            'data' : function (obj, callback) {
                if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                    report = webpage.on_fill_mediainfo_report($(result.cell('#' + resultId, 0).node()).find('span').attr('title'));
                    callback.call(this, eval(report));
                } else {
                    webpage.on_fill_mediainfo_report($(result.cell('#' + resultId, 0).node()).find('span').attr('title'), function (report) {
                        callback.call(this, eval(report));
                    });
                }
            }
        },
        "plugins" : ['search', 'types', 'grid'],
        'types' : {
            'default' : {'icon' : 'glyphicon glyphicon-folder-open'},
            'block' : {'icon' : 'glyphicon glyphicon-folder-open'},
            'data' : {'icon' : 'glyphicon glyphicon-file'},
        },
        grid: {
            columns: [
                {header: "Key", tree: true},
                {header: "Value", value: "dataValue"},
            ],
            resizable: true,
        },
    });

    $('#info' + resultId).on('ready.jstree', function () {
        $(function () {
            var to = false;
            $('#infoSearch' + resultId).keyup(function () {
                if(to) { clearTimeout(to); }
                to = setTimeout(function () {
                    var v = $('#infoSearch' + resultId).val();
                    $('#info' + resultId).jstree(true).search(v);
                }, 250);
            });
        });
    });

    $('#info' + resultId).on('loaded.jstree', function (e, data) {
        data.instance.open_all();
    });

    $('#info' + resultId).on('select_node.jstree', function (e, data) {
        data.instance.toggle_node(data.node);
    });
}


function mediaTraceCell(resultId, fileId)
{
    nodeMT = $(result.cell('#' + resultId, 4).node());
    nodeMT.addClass('text-center');
    result.cell('#' + resultId, 4).data('<a href="#" class="mt-view" data-toggle="modal" data-target="#modalTrace' + resultId + '" title="View MediaTrace report"><span class="glyphicon glyphicon-eye-open" aria-hidden="true"></span></a><a href="#" class="mt-dld" data-target="#traceXml' + resultId + '" data-save-name="' + resultId + '_MediaTrace.xml" title="Download MediaTrace report"><span class="glyphicon glyphicon-download" aria-hidden="true"></span></a>');

    nodeMT.find('.mt-view').on('click', function(e) {
        e.preventDefault();
        if (!$('#modalTrace' + resultId).length) {
            $('.result-container').append(' \
<div id="modalTrace' + resultId + '" \ class="modal fade"> \
<div class="modal-dialog modal-lg"> \
<div class="modal-content"> \
<div class="modal-header"> \
<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button> \
<h4 class="modal-title">MediaTrace report</h4> \
</div> \
<div class="modal-body"> \
<div class="row"> \
<div class="col-md-6"> \
<i class="glyphicon glyphicon-search"></i><input type="text" value="" class="jstreeSearch" id="traceSearch' + resultId + '" placeholder="Search" /> \
</div> \
<div class="col-md-12"> \
<div id="trace' + resultId + '"></div> \
</div> \
</div> \
</div> \
<div class="modal-footer"> \
<button type="button" class="btn btn-primary mt-dld" data-target="#modalTrace' + resultId + '" data-save-name="' + resultId + '_MediaTraceReport.txt">Download MediaTrace report</button> \
<button type="button" class="btn btn-default" data-dismiss="modal">Close</button> \
</div> \
</div> \
</div> \
</div>');

            mediaTraceTree(resultId, fileId);

            $('#modalTrace' + resultId + ' .mt-dld').on('click', function(e) {
                e.preventDefault();
                webpage.on_save_mediatrace_report($(result.cell('#' + resultId, 0).node()).find('span').attr('title'));
            });
        }
    });

    nodeMT.find('.mt-dld').on('click', function(e) {
        e.preventDefault();
        webpage.on_save_mediatrace_report($(result.cell('#' + resultId, 0).node()).find('span').attr('title'));
    });
};

function mediaTraceTree(resultId, fileId)
{
    $('#trace' + resultId).jstree({
        'core' : {
            'check_callback' : function (operation, node, parent, position, more) {
                if (operation === 'copy_node' || operation === 'move_node') {
                    return false; // disable copy and move
                }
                else {
                    return true;
                }
            },
            'multiple' : false,
            'dblclick_toggle' : false,
            'data' : function (obj, callback) {
                if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                    report = webpage.on_fill_mediatrace_report($(result.cell('#' + resultId, 0).node()).find('span').attr('title'));
                    callback.call(this, eval(report));
                } else {
                    webpage.on_fill_mediatrace_report($(result.cell('#' + resultId, 0).node()).find('span').attr('title'), function (report) {
                        callback.call(this, eval(report));
                    });
                }
            }
        },
        "plugins" : ['search', 'types', 'grid'],
        'types' : {
            'default' : {'icon' : 'glyphicon glyphicon-folder-open'},
            'block' : {'icon' : 'glyphicon glyphicon-folder-open'},
            'data' : {'icon' : 'glyphicon glyphicon-file'},
        },
        grid: {
            columns: [
                {header: "Offset", value: "offset"},
                {header: "Key", tree: true},
                {header: "Value", value: "dataValue"},
            ],
            resizable: true,
        },
    });

    $('#trace' + resultId).on('ready.jstree', function () {
        $(function () {
            var to = false;
            $('#traceSearch' + resultId).keyup(function () {
                if(to) { clearTimeout(to); }
                to = setTimeout(function () {
                    var v = $('#traceSearch' + resultId).val();
                    $('#trace' + resultId).jstree(true).search(v);
                }, 250);
            });
        });
    });

    $('#trace' + resultId).on('loaded.jstree', function (e, data) {
        data.instance.get_container().find('li').each(function () {
            data.instance.open_node($(this));
        })
            });

    $('#trace' + resultId).on('select_node.jstree', function (e, data) {
        data.instance.toggle_node(data.node);
    });
}

function applyPolicyToAll()
{
    var policy = $('#applyAllPolicy').val();
    var policyText = $('#applyAllPolicy option:selected').text();
    result.$('tr').each(function () {
        var node = result.$('#' + $(this).prop('id'));

        if (2 == node.data('tool') || undefined == node.data('tool')) {
            if (node.data('policy') != policy) {
                // Update policy
                node.data('policy', policy);
                node.data('policyName', policyText);
                sourceName = $(result.cell('#' + $(this).prop('id'), 0).node()).find('span').attr('title');
                if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                    fileId = webpage.change_policy_for_file(sourceName, policy, node.data('fileId'));
                    updatePolicyCell(fileId, policy);
                } else {
                    webpage.change_policy_for_file(sourceName, policy, node.data('fileId'), function(fileId) {
                        updatePolicyCell(fileId, policy);
                    });
                }
            }
        }
    });
}

function resetSelectList(listId)
{
    $('#' + listId + ' option').removeAttr('selected');
}

function truncateString(str, length)
{
    return str.length > length ? str.substring(0, length) + '&hellip;' : str
}

// Display report in the modal
function displayReport(elemId, dataReport)
{
    if (WEBMACHINE === 'WEB_MACHINE_KIT') {
        if (webpage.report_is_html(dataReport)) {
            $(elemId + ' .modal-body').html(dataReport);
        }
        else {
            $(elemId + ' .modal-body').html('<pre class="report-content">');
            $(elemId + ' .report-content').text(dataReport);
        }
    } else {
        webpage.report_is_html(dataReport, function(is_html) {
            if (is_html) {
                $(elemId + ' .modal-body').html(dataReport);
            }
            else {
                $(elemId + ' .modal-body').html('<pre class="report-content">');
                $(elemId + ' .report-content').text(dataReport);
            }
        });
    }
}

    // // Alert user when they leave checker page with results
    // $('.nav.navbar a').each(function() {
    //     // Exclude some links
    //     if (!$(this).hasClass('no-close-alert')) {
    //         $(this).click(function (e) {
    //             // Check if there is close all button
    //             if (0 < $('table.checker-results tbody tr').length && !$('table.checker-results tbody tr .dataTables_empty').length) {
    //                 choice = confirm('Are you sure ?\nAll analysis results will be discarded!');
    //                 if (choice == false) {
    //                     e.preventDefault();
    //                 }
    //             };
    //         });
    //     };
    // });

    // // Display success message
    // function successMessage(message) {
    //     $('#checkerInfo div').replaceWith('<div class="alert alert-success">' + message + '</div>');
    //     $('#checkerInfo div').delay(5000).fadeOut();
    // }

    // // Display error message
    // function errorMessage(message) {
    //     $('#checkerInfo div').replaceWith('<div class="alert alert-danger">' + message + '</div>')
    //     $('#checkerInfo div').delay(10000).fadeOut();
    // }

    // // Handle fail ajax response
    // function failResponse(jqXHR, formType) {
    //     if (typeof jqXHR.responseJSON !== 'undefined') {
    //         if (jqXHR.responseJSON.hasOwnProperty('quota')) {
    //             $('#' + formType).html(jqXHR.responseJSON.quota);
    //         }
    //         else if ('file' == formType) {
    //             uploadFiles = $('#file form :file');
    //             errorMessage('The file is too big (max ' + uploadFiles.data('file-max-size')  + ')');
    //         }
    //         else if (jqXHR.responseJSON.hasOwnProperty('message')) {
    //             errorMessage(jqXHR.responseJSON.message);
    //         }
    //         else {
    //             errorMessage('An error has occured, please try again later');
    //         }
    //     }
    //     else {
    //         if ('file' == formType && 400 == jqXHR.status) {
    //             uploadFiles = $('#file form :file');
    //             errorMessage('The file is too big (max ' + uploadFiles.data('file-max-size')  + ')');
    //         }
    //         else {
    //             errorMessage('An error has occured, please try again later');
    //         }
    //     }
    // }
