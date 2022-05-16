var policyCell = (function() {
    var result;
    var init = function(table) {
        result = table;
    };

    var success = function(data, fileId) {
        var nodeCell = result.$('#result-' + fileId);
        var nodePolicy = $(result.cell(nodeCell, 2).node());
        var policyResultText = '<span class="policyResult">';
        if (data.valid) {
            nodePolicy.removeClass().addClass('success');
            if (data.warn) {
                policyResultText += '<span class="glyphicon glyphicon-warning-sign" style="color:gold" aria-hidden="true"></span><span class="hidden">P</span> '
            }
            else if (data.info) {
                policyResultText += '<span class=" 	glyphicon glyphicon-info-sign" style="color:blue" aria-hidden="true"></span><span class="hidden">P</span> '
            }
            else {
                policyResultText += '<span class="glyphicon glyphicon-ok text-success" aria-hidden="true"></span><span class="hidden">P</span> '
            }
        }
        else {
            nodePolicy.removeClass().addClass('danger');
            policyResultText += '<span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span><span class="hidden">F</span> ';
        }

        policyResultText += '<span title="' + nodeCell.data('policyName') + '">' + textUtils.truncate(nodeCell.data('policyName'), 28) + '</span>';
        policyResultText += '</span>';

        result.cell(nodeCell, 2).data('<div>' + policyResultText + '<p class="policyButton hidden"><a href="#" data-toggle="modal" data-target="#modalPolicy-' + fileId + '" title="View policy report"><span class="glyphicon glyphicon-eye-open policy-view" aria-hidden="true"></span></a><a href="#" class="policy-dld" title="Download policy report"><span class="glyphicon glyphicon-download" aria-hidden="true"></span></a></p></div>');

        policyModal(fileId);
    };

    var error = function(fileId) {
        var nodeCell = result.$('#result-' + fileId);
        var nodePolicy = $(result.cell(nodeCell, 2).node());
        nodePolicy.addClass('danger');
        result.cell(nodeCell, 2).data('<span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span><span class="hidden">F</span> Server Error')
    };

    var update = function(fileId, policyId) {
        removeModalIfExists(fileId);

        // Update cell if analysis of file is succeeded
        if ($(result.cell('#result-' + fileId, 5).node()).hasClass('success')) {
            if (policyId !== undefined && policyId !== "-1" && policyId !== -1 && result.$('#result-' + fileId).data('attachment') !== "true") {
                reset(fileId);
                addSpinnerToCell(fileId);

                checkerAjax.policyStatus(fileId, policyId);
            }
            else if (result.$('#result-' + fileId).data('attachment') == "true") {
                empty(fileId)
            } else {
                emptyWithModal(fileId)
            }
        }
    };

    var empty = function(fileId) {
        var nodePolicy = $(result.cell('#result-' + fileId, 2).node());
        nodePolicy.removeClass().addClass('info');
        result.cell('#result-' + fileId, 2).data('<div><span class="policyResult">N/A</span></div>');
    };

    var emptyWithModal = function(fileId) {
        var nodePolicy = $(result.cell('#result-' + fileId, 2).node());
        nodePolicy.removeClass().addClass('info');
        result.cell('#result-' + fileId, 2).data('<div><span class="policyResult">N/A</span><p class="policyButton hidden"><a href="#" data-toggle="modal" data-target="#modalPolicy-' + fileId + '" title="View policy report"><span class="glyphicon glyphicon-eye-open policy-view" aria-hidden="true"></span></a></p></div>');

        policyModal(fileId);
    };

    var policyModal = function(fileId) {
        var nodePolicy = $(result.cell('#result-' + fileId, 2).node());
        nodePolicy.find('.policy-view').on('click', function(e) {
            e.preventDefault();
            var nodeModal = result.$('#result-' + fileId);
            if (!$('#modalPolicy-' + fileId).length) {
                $('.result-container').append(' \
                <div id="modalPolicy-' + fileId + '" \ class="modal fade"> \
                    <div class="modal-dialog modal-lg"> \
                        <div class="modal-content"> \
                            <div class="modal-header"> \
                                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button> \
                                <h4 class="modal-title">Policy report</h4> \
                            </div> \
                            <div class="modal-header form-horizontal"> \
                                <div class="col-md-6"> \
                                    <div class="form-group"><label class="col-sm-2 control-label">Policy</label><div class="col-sm-10"><select id="modalPolicyPolicy-' + fileId + '"></select></div></div> \
                                </div> \
                                <div class="col-md-6"> \
                                    <div class="form-group"><label class="col-sm-2 control-label">Display</label><div class="col-sm-10"><select id="modalPolicyDisplay-' + fileId + '"></select></div></div> \
                                </div> \
                            </div> \
                            <div class="modal-body"></div> \
                            <div class="modal-footer"> \
                                <button type="button" class="btn btn-primary policy-dld" data-target="#modalPolicy-' + fileId + '">Download policy report</button> \
                                <button type="button" class="btn btn-default" data-dismiss="modal">Close</button> \
                            </div> \
                        </div> \
                    </div> \
                </div>');

                if (nodeModal.data('policy') !== undefined && nodeModal.data('policy') !== "-1" && nodeModal.data('policy') !== -1) {
                    addSpinnerToModal(fileId);
                    checkerAjax.policyReport(fileId, nodeModal.data('policy'), nodeModal.data('display'));
                }

                $('#modalPolicy-' + fileId + ' .policy-dld').on('click', function(e) {
                    e.preventDefault();
                    var modalDisplay = $('#modalPolicyDisplay-' + fileId).val();
                    var modalPolicy = $('#modalPolicyPolicy-' + fileId).val();
                    if (modalPolicy) {
                        checkerAjax.downloadPolicyReportUrl(fileId, modalPolicy, modalDisplay);
                    }
                });

                // Update report when display is changed
                var displayList = $('.tab-content .active .displayList').clone();
                displayList.attr('id', 'modalPolicyDisplay-' + fileId);
                displayList.find('option').prop('selected', false);
                displayList.find("option[value = '" + nodeModal.data('display') + "']").prop('selected', true);
                $('#modalPolicyDisplay-' + fileId).replaceWith(displayList);
                $('#modalPolicyDisplay-' + fileId).on('change', function(e) {
                    var modalDisplay = $('#modalPolicyDisplay-' + fileId).val();
                    var modalPolicy = $('#modalPolicyPolicy-' + fileId).val();
                    if (modalPolicy) {
                        addSpinnerToModal(fileId);
                        checkerAjax.policyReport(fileId, modalPolicy, modalDisplay);
                    }
                    else {
                        $('#modalPolicy-' + fileId + ' .modal-body').empty('');
                    }
                });

                // Update report when policy is changed
                var policyList = $('.tab-content .active .policyList').clone();
                policyList.attr('id', 'modalPolicyPolicy-' + fileId);
                policyList.find('option').prop('selected', false);
                policyList.find("option[value = '" + nodeModal.data('policy') + "']").prop('selected', true);
                $('#modalPolicyPolicy-' + fileId).replaceWith(policyList);
                $('#modalPolicyPolicy-' + fileId).on('change', function(e) {
                    var modalDisplay = $('#modalPolicyDisplay-' + fileId).val();
                    var modalPolicy = $('#modalPolicyPolicy-' + fileId).val();
                    if (modalPolicy) {
                        addSpinnerToModal(fileId);
                        checkerAjax.policyReport(fileId, modalPolicy, modalDisplay);
                    }
                    else {
                        $('#modalPolicy-' + fileId + ' .modal-body').empty('');
                    }
                });
            }
        });

        nodePolicy.find('.policy-dld').on('click', function(e) {
            e.preventDefault();
            var nodeDld = result.$('#result-' + fileId);
            checkerAjax.downloadPolicyReportUrl(fileId, nodeDld.data('policy'), nodeDld.data('display'));
        });
    };

    var addSpinnerToCell = function(fileId) {
        result.cell('#result-' + fileId, 2).data('<span class="spinner-cell"></span>');
    };

    var addSpinnerToModal = function(fileId) {
        $('#modalPolicy-' + fileId + ' .modal-body').html('<span class="spinner-modal"></span>');
    };

    // Display report in the modal
    var displayReport = function(fileId, dataReport) {
        if (dataReport.isHtmlReport) {
            $('#modalPolicy-' + fileId + ' .modal-body').html(dataReport.report);
        }
        else {
            $('#modalPolicy-' + fileId + ' .modal-body').html('<pre class="report-content">');
            $('#modalPolicy-' + fileId + ' .report-content').text(dataReport.report);
        }

        $('#modalPolicy-' + fileId + ' .policy-dld').prop('disabled', false);
    };

    // Display report error in the modal
    var displayReportError = function(fileId) {
        $('#modalPolicy-' + fileId + ' .modal-body').html('Error: the requested report can not be retrieved');

        $('#modalPolicy-' + fileId + ' .policy-dld').prop('disabled', true);
    };

    var reset = function(fileId) {
        $(result.cell('#result-' + fileId, 2).node()).removeClass();
        $(result.cell('#result-' + fileId, 2).node()).empty();
    };

    var removeModalIfExists = function(fileId) {
        if ($('#modalPolicy-' + fileId).length) {
            $('#modalPolicy-' + fileId).remove();
        }
    };

    return {
        init: init,
        success: success,
        error: error,
        update: update,
        empty: empty,
        emptyWithModal: emptyWithModal,
        addSpinnerToCell: addSpinnerToCell,
        displayReport: displayReport,
        displayReportError: displayReportError,
        reset: reset,
        removeModalIfExists: removeModalIfExists,
    };
})();
