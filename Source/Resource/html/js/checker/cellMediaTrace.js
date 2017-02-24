var mediaTraceCell = (function() {
    var result;
    var init = function(table) {
        result = table;
    };

    var success = function(fileId) {
        nodeMT = $(result.cell('#result-' + fileId, 4).node());
        nodeMT.addClass('text-center');
        result.cell('#result-' + fileId, 4).data('<a href="#" class="mt-view" data-toggle="modal" data-target="#modalTrace-' + fileId + '" title="View MediaTrace report"><span class="glyphicon glyphicon-eye-open" aria-hidden="true"></span></a><a href="#" class="mt-dld" title="Download MediaTrace report"><span class="glyphicon glyphicon-download" aria-hidden="true"></span></a>');

        nodeMT.find('.mt-view').on('click', function(e) {
            e.preventDefault();
            if (!$('#modalTrace-' + fileId).length) {
                $('.result-container').append(' \
                <div id="modalTrace-' + fileId + '" \ class="modal fade"> \
                    <div class="modal-dialog modal-lg"> \
                        <div class="modal-content"> \
                            <div class="modal-header"> \
                                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button> \
                                <h4 class="modal-title">MediaTrace report</h4> \
                            </div> \
                            <div class="modal-body"> \
                                <div class="row"> \
                                    <div class="col-md-6"> \
                                        <i class="glyphicon glyphicon-search"></i><input type="text" value="" class="jstreeSearch" id="traceSearch-' + fileId + '" placeholder="Search" /> \
                                    </div> \
                                    <div class="col-md-12"> \
                                        <div id="trace-' + fileId + '"></div> \
                                    </div> \
                                </div> \
                            </div> \
                            <div class="modal-footer"> \
                                <button type="button" class="btn btn-primary mt-dld" data-target="#modalTrace-' + fileId + '">Download MediaTrace report</button> \
                                <button type="button" class="btn btn-default" data-dismiss="modal">Close</button> \
                            </div> \
                        </div> \
                    </div> \
                </div>');

                displayTree(fileId);

                $('#modalTrace-' + fileId + ' .mt-dld').on('click', function(e) {
                    e.preventDefault();
                    checkerAjax.downloadReportUrl(fileId, 'mt');
                });
            }
        });

        nodeMT.find('.mt-dld').on('click', function(e) {
            e.preventDefault();
            checkerAjax.downloadReportUrl(fileId, 'mt');
        });
    };

    var displayTree = function(fileId) {
        // Disable F2 shortcut (rename) in jstree, needs to be done before jstree initialization
        $('#trace-' + fileId).on('keydown.jstree', '.jstree-anchor', function(event) {
            if (113 == event.keyCode) {
                event.stopImmediatePropagation();

                return false;
            }
        });

        $('#trace-' + fileId).jstree({
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
                        report = webpage.on_fill_mediatrace_report(fileId);
                        callback.call(this, eval(report));
                    } else {
                        webpage.on_fill_mediatrace_report(fileId, function (report) {
                            callback.call(this, eval(report));
                        });
                    }
                }
            },
            plugins: ['search', 'types', 'grid'],
            types: {
                default: {icon: 'glyphicon glyphicon-folder-open'},
                block: {icon: 'glyphicon glyphicon-folder-open'},
                data: {icon: 'glyphicon glyphicon-file'},
            },
            grid: {
                columns: [
                    {header: 'Offset', value: 'offset'},
                    {header: 'Key', tree: true},
                    {header: 'Value', value: 'dataValue'},
                ],
                resizable: true,
            },
        });

        $('#trace-' + fileId).on('ready.jstree', function() {
            var to = false;
            $('#traceSearch-' + fileId).keyup(function() {
                if(to) { clearTimeout(to); }
                to = setTimeout(function() {
                    var v = $('#traceSearch-' + fileId).val();
                    $('#trace-' + fileId).jstree(true).search(v);
                }, 250);
            });
        });

        $('#trace-' + fileId).on('loaded.jstree', function(e, data) {
            data.instance.get_container().find('li').each(function() {
                data.instance.open_node($(this));
            })
        });

        $('#trace-' + fileId).on('select_node.jstree', function(e, data) {
            data.instance.toggle_node(data.node);
        });
    };

    var reset = function(fileId) {
        $(result.cell('#result-' + fileId, 4).node()).empty();
    };

    var removeModalIfExists = function(fileId) {
        if ($('#modalTrace-' + fileId).length) {
            $('#modalTrace-' + fileId).remove();
        }
    };

    return {
        init: init,
        success: success,
        reset: reset,
        removeModalIfExists: removeModalIfExists,
    };
})();
