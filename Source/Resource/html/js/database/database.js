var database = (function() {

    var init = function() {
        mcoMessage.init('#databaseInfo div');
        databaseAjax.init();
    }

    var load = function() {
        databaseAjax.loadTree();

        // Show delete all button
        if (accordionDatabase.childElementCount != 0)
            $('#databaseCloseAll').removeClass('hidden');

        // Remove all file blocks
        $('#databaseCloseAll').on('click', function(e) {
            e.preventDefault();
            databaseAjax.removeAllFiles();
            $('#accordionDatabase').remove();

            // Remove close all button
            $(this).addClass('hidden');
        });
    }

    var addFile = function(file) {
        $('#accordionDatabase').append(' \
            <div class="panel panel-default" id="database_' + file.id + '"> \
              <div class="panel-heading" role="tab" id="heading' + file.id + ' + "> \
                <h4 class="panel-title" role="button" data-toggle="collapse" data-parent="#accordionDatabase" href="#collapse' + file.id + '" aria-expanded="true" aria-controls="collapse' + file.id + '"> \
            ' + file.name + ' \
                </h4> \
              </div> \
              <div id="collapse' + file.id + '" class="panel-collapse collapse" role="tabpanel" aria-labelledby="heading' + file.id +'"> \
                <div class="panel-body"> \
                  <a id="database_file_remove' + file.id + '" href="#"><button class="btn-warning pull-right">Remove</button></a> \
                  <a id="database_file_add' + file.id + '" href="#"><button class="btn-warning pull-right">Add to checker</button></a> \
                </div> \
              </div> \
            </div>');
        $('#databaseCloseAll').removeClass('hidden');

        $('#database_file_remove' + file.id).on('click', function(e) {
            e.preventDefault();
            databaseAjax.removeFile(file.id);
            $('#database_' + file.id).remove();
            // Show delete all button
            if (accordionDatabase.childElementCount != 0)
                $('#databaseCloseAll').addClass('hidden');
        });

        $('#database_file_add' + file.id).on('click', function(e) {
            e.preventDefault();
            databaseAjax.addFileChecker(file.id);
        });

    }

    return {
        init: init,
        load: load,
        addFile: addFile,
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
