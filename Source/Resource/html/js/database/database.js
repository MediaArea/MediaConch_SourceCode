var database = (function() {

    mcoMessage.init('#databaseInfo div');

    var init = function() {
        databaseAjax.init();
    }

    var load = function() {
        databaseAjax.loadTree();
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
                </div> \
              </div> \
            </div>');

        $('#database_file_remove' + file.id).on('click', function(e) {
            e.preventDefault();
            webpage.remove_file_from_db(file.id);
            $('#database_' + file.id).remove();
        });;

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
