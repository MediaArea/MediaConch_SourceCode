$(document).ready(function() {
    global_id = 0;
    $('#displayImport').on('submit', function(e) {
        e.preventDefault();

        var name = $('#displayImport_displayName').val();
        if (WEBMACHINE == "WEB_MACHINE_KIT") {
            var value = webpage.display_add_file(name);
            if (value.length)
                addUserDisplay(value);
        } else {
            webpage.display_add_file(name, function (value) {
                if (value.length)
                    addUserDisplay(value);
            });
        }
        $('#displayImport_displayName').val('');
        $("#displayImport_displayFile").val('');
    });
});

function addUserDisplay(name) {
    var name_id = ++global_id;

    $('#accordion').append(' \
<div class="panel panel-default" id="display_' + name_id + '"> \
<div class="panel-heading" role="tab" id="heading' + name_id + ' + "> \
<h4 class="panel-title" role="button" data-toggle="collapse" data-parent="#accordion" href="#collapse' + name_id + '" aria-expanded="true" aria-controls="collapse' + name_id + '"> \
' + name + ' \
</h4> \
</div> \
<div id="collapse' + name_id + '" class="panel-collapse collapse" role="tabpanel" aria-labelledby="heading' + name_id +'"> \
<div class="panel-body"> \
<a id="display_user_delete' + name_id + '" href="#"><button class="btn-danger pull-right">Delete</button></a> \
<a id="display_user_export' + name_id + '" href="#"><button class="btn-warning pull-right">Export</button></a> \
</div> \
</div> \
</div>');

    $('#display_user_delete' + name_id).on('click', function(e) {
        e.preventDefault();
        webpage.display_delete_id(name);
        $('#display_' + name_id).remove();
    });;

    $('#display_user_export' + name_id).on('click', function(e) {
        e.preventDefault();
        webpage.display_export_id(name);
    });;
};

function addSystemDisplay(name) {
    var name_id = ++global_id;

    $('#accordionSystem').append(' \
<div class="panel panel-default"> \
<div class="panel-heading" role="tab" id="heading' + name_id + ' + "> \
<h4 class="panel-title" role="button" data-toggle="collapse" data-parent="#accordionSystem" href="#collapse' + name_id + '" aria-expanded="true" aria-controls="collapse' + name_id + '"> \
' + name + ' \
</h4> \
</div> \
<div id="collapse' + name_id + '" class="panel-collapse collapse" role="tabpanel" aria-labelledby="heading' + name_id +'"> \
<div class="panel-body"> \
<a id="display_system_export' + name_id + '" href="#"><button class="btn-warning pull-right">Export</button></a> \
</div> \
</div> \
</div>');

    $('#display_system_export' + name_id).on('click', function(e) {
        e.preventDefault();
        webpage.display_export_id(name);
    });;
};
