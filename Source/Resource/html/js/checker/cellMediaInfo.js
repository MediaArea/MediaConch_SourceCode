var mediaInfoCell = (function() {
    var result;
    var init = function(table) {
        result = table;
    };

    var success = function(fileId) {
        var nodeMI = $(result.cell('#result-' + fileId, 3).node());
        nodeMI.addClass('text-center');
        result.cell('#result-' + fileId, 3).data('<a href="#" class="mi-view" data-toggle="modal" data-target="#modalInfo-' + fileId + '" title="View MediaInfo report"><span class="glyphicon glyphicon-eye-open" aria-hidden="true"></span></a><a href="#" class="mi-dld" title="Download MediaInfo report"><span class="glyphicon glyphicon-download" aria-hidden="true"></span></a>');

        nodeMI.find('.mi-view').on('click', function(e) {
            e.preventDefault();
            if (!$('#modalInfo-' + fileId).length) {
                $('.result-container').append(' \
                <div id="modalInfo-' + fileId + '" \ class="modal fade"> \
                    <div class="modal-dialog modal-lg"> \
                        <div class="modal-content"> \
                        <div class="modal-header"> \
                            <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button> \
                            <h4 class="modal-title">MediaInfo report</h4> \
                        </div> \
                        <div class="modal-body"> \
                            <div class="row"> \
                                <div class="col-md-6"> \
                                    <i class="glyphicon glyphicon-search"></i><input type="text" value="" class="jstreeSearch" id="infoSearch-' + fileId + '" placeholder="Search" /> \
                                </div> \
                                <div class="col-md-12"> \
                                    <div id="info-' + fileId + '"></div> \
                                </div> \
                            </div> \
                        </div> \
                            <div class="modal-footer"> \
                                <button type="button" class="btn btn-warning mi-create-report">Create policy from MediaInfo report</button> \
                                <button type="button" class="btn btn-primary mi-dld">Download MediaInfo report</button> \
                                <button type="button" class="btn btn-default" data-dismiss="modal">Close</button> \
                            </div> \
                        </div> \
                    </div> \
                </div>');

                displayTree(fileId);

                $('#modalInfo-' + fileId + ' .mi-dld').on('click', function(e) {
                    e.preventDefault();
                    checkerAjax.downloadReportUrl(fileId, 'mi');
                });

                $('#modalInfo-' + fileId + ' .mi-create-report').on('click', function(e) {
                    e.preventDefault();
                    checkerAjax.createPolicyFromFileId(fileId);
                });
            }
        });

        nodeMI.find('.mi-dld').on('click', function(e) {
            e.preventDefault();
            checkerAjax.downloadReportUrl(fileId, 'mi');
        });
    };

    var displayTree = function(fileId) {
        // Disable F2 shortcut (rename) in jstree, needs to be done before jstree initialization
        $('#info-' + fileId).on('keydown.jstree', '.jstree-anchor', function(event) {
            if (113 == event.keyCode) {
                event.stopImmediatePropagation();

                return false;
            }
        });

        $('#info-' + fileId).jstree({
            core: {
                check_callback: function(operation, node, parent, position, more) {
                    if (operation === 'copy_node' || operation === 'move_node') {
                        return false; // disable copy and move
                    }
                    else {
                        return true;
                    }
                },
                multiple: false,
                dblclick_toggle: false,
                data : function (obj, callback) {
                    if (WEBMACHINE === 'WEB_MACHINE_KIT') {
                        report = webpage.on_fill_mediainfo_report(fileId);
                        callback.call(this, eval(report));
                    } else {
                        webpage.on_fill_mediainfo_report(fileId, function (report) {
                            callback.call(this, eval(report));
                        });
                    }
                }
            },
            plugins: ['search', 'types', 'grid'],
            types: {
                default: {icon: 'glyphicon glyphicon-folder-open'},
                block: {icon: 'glyphicon glyphicon-folder-open'},
                data:  {icon: 'glyphicon glyphicon-file'},
            },
            grid: {
                columns:[
                    {header: 'Key', tree: true},
                    {header: 'Value', value: 'dataValue'},
                ],
                resizable: true,
            },
        });

        $('#info-' + fileId).on('ready.jstree', function() {
            var to = false;
            $('#infoSearch-' + fileId).keyup(function() {
                if(to) { clearTimeout(to); }
                to = setTimeout(function() {
                    var v = $('#infoSearch-' + fileId).val();
                    $('#info-' + fileId).jstree(true).search(v);
                }, 250);
            });
        });

        $('#info-' + fileId).on('loaded.jstree', function(e, data) {
            data.instance.open_all();
        });

        $('#info-' + fileId).on('select_node.jstree', function(e, data) {
            data.instance.toggle_node(data.node);
        });
    };

    var createPolicySuccess = function(createPolicy, fileId) {
        if (createPolicy.result) {
            $('#modalInfo-' + fileId + ' .mi-create-report').fadeOut(200).replaceWith('<div class="alert alert-success alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-ok text-success" aria-hidden="true"></span>Policy successfuly created</div>');

            // Add new policy to all select lists
            $('.policyList .userPolicy').each(function() {
                if ($(this).hasClass('hidden'))
                    $(this).removeClass('hidden');
                $(this).append('<option value="' + createPolicy.policyId + '">' + createPolicy.policyName + '</option>');
            });
        }
        else {
            $('#modalInfo-' + fileId + ' .mi-create-report').fadeOut(200).replaceWith('<div class="alert alert-danger alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span> Error policy not created</div>');
        }
    }

    var createPolicyError = function(fileId) {
        $('#modalInfo-' + fileId + ' .mi-create-report').fadeOut(200).replaceWith('<div class="alert alert-danger alert-modal-create-policy" role="alert"><span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span> Error policy not created</div>');
    };

    var reset = function(fileId) {
        $(result.cell('#result-' + fileId, 3).node()).empty();
    };

    var removeModalIfExists = function(fileId) {
        if ($('#modalInfo-' + fileId).length) {
            $('#modalInfo-' + fileId).remove();
        }
    };

    return {
        init: init,
        success: success,
        createPolicySuccess: createPolicySuccess,
        createPolicyError: createPolicyError,
        reset: reset,
        removeModalIfExists: removeModalIfExists,
    };
})();
