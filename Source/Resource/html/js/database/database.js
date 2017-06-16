var database = (function() {

    var init = function() {
        mcoMessage.init('#databaseInfo div');
        databaseTable.init();
        databaseAjax.init();
    }

    var load = function() {
        databaseAjax.loadTree();

        // Remove all file blocks
        $('#checkerResults .close').on('click', function(e) {
            e.preventDefault();
            databaseAjax.removeAllFiles();
            databaseTable.clear();

            // Remove close all button
            $(this).addClass('hidden');
        });
    }

    return {
        init: init,
        load: load,
    };

})();

var databaseTable = (function() {
    var init = function() {
        result = $('#result-table').DataTable({
            order: [0, 'asc'],
            autoWidth: false,
            lengthMenu: [10, 50, 100, 1000],
            pageLength: 100,
            fixedHeader: {
                headerOffset: $('#mco-navbar').outerHeight(true)
            },
            columnDefs: [
                { orderable: true, targets: 0 },
                { orderable: false, searchable: false, targets: 1, width: '100px' },
            ]
        });

        resultRowHoverBinding();
    };

    // Add file to table
    var addFile = function(file) {
        var node = result.row.add( [ '<span class="contentAutoWidth" title="' + file.name + '">' + file.name + '</span>',
            '<span>&nbsp;</span><div class="statusButton hidden"> \
            <button type="button" class="btn btn-link add-checker" title="Add to checker"><span class="glyphicon glyphicon-plus" aria-hidden="true"></span></button> \
            <button type="button" class="btn btn-link result-close" title="Delete file from database"><span class="glyphicon glyphicon-trash" aria-hidden="true"></span></button></div>'
        ] ).node();
        var nodej = $(node);

        // Add id
        nodej.prop('id', 'result-' + file.id);
        nodej.addClass('fileId-' + file.id);
        nodej.data('fileId', file.id);

        $(result.cell(node, 0).node()).addClass('containerAutoWidth');
        $(result.cell(node, 1).node()).addClass('text-center');

        // Delete button
        nodej.find('.result-close').click(node, function(e) {
            var id = $(result.row(e.data).node()).data('fileId');
            databaseAjax.removeFile(id);
            result.row(e.data).remove().draw(false);

            // Remove delete all button
            if (1 == $('table.checker-results tbody tr').length && $('table.checker-results tbody tr .dataTables_empty').length) {
                $('#checkerResults .close').addClass('hidden');
            };
        });

        // Add to checker button
        nodej.find('.add-checker').click(node, function(e) {
            var id = $(result.row(e.data).node()).data('fileId');

            databaseAjax.addFileChecker(id);
        });

        // Display delete all button
        $('#checkerResults .close').removeClass('hidden');
    }

    // Redraw the table
    var draw = function() {
        result.draw(false);
    };

    // Clear the table
    var clear = function() {
        result.clear().draw();
    };


    // Display actions button on hover
    var resultRowHoverBinding = function() {
        result.on('draw.dt', function() {
            result.$('tr').off('mouseenter');
            result.$('tr').off('mouseleave');

            // Display buttons on checker result
            result.$('tr').on('mouseenter', function() {
                $(this).find('.statusButton').removeClass('hidden');
            });
            result.$('tr').on('mouseleave', function() {
                $(this).find('.statusButton').addClass('hidden');
            });
        });
    };

    return {
        init: init,
        addFile: addFile,
        draw: draw,
        clear: clear,
    };
})();

$(document).ready(function() {
    (function loop_init(time) {
        setTimeout(function() {
            if (webpage === undefined)
                loop_init(100);
            else
            {
                database.init();
                database.load();
            }
        }, time);
    })(0);
});
