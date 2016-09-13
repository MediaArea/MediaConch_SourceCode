$(document).ready(function() {
    // Settings form
    $('#settings form').on('submit', function (e) {
        e.preventDefault();
        webpage.on_save_settings_selected($('#settings_policy').val(),
                                          $('#settings_display_selector').val(),
                                          $('#settings_verbosity_selector').val(),
                                          $('input[name=settings_save_report_path]:checked').val(),
                                          $('input[name=settings_load_files_path]:checked').val(),
                                          $('input[name=settings_save_policy_path]:checked').val(),
                                          $('input[name=settings_load_policy_path]:checked').val(),
                                          $('input[name=settings_save_display_path]:checked').val(),
                                          $('input[name=settings_load_display_path]:checked').val());
    });
});

function set_save_report_selected(path)
{
    if (!path.length)
        path = 'last';

    if (path === 'last')
    {
        $('input[name=settings_save_report_path]').filter('[value="last"]').attr('checked', true);
        path = "Previous path";
    }
    else
        $('input[name=settings_save_report_path]').filter('[value="selected"]').attr('checked', true);
    $('#settings_save_report_path_selected').text(path);
}

function set_load_files_selected(path)
{
    if (!path.length)
        path = 'last';

    if (path === 'last')
    {
        $('input[name=settings_load_files_path]').filter('[value="last"]').attr('checked', true);
        path = "Previous path";
    }
    else
        $('input[name=settings_load_files_path]').filter('[value="selected"]').attr('checked', true);
    $('#settings_load_files_path_selected').text(path);
}

function set_save_policy_selected(path)
{
    if (!path.length)
        path = 'last';

    if (path === 'last')
    {
        $('input[name=settings_save_policy_path]').filter('[value="last"]').attr('checked', true);
        path = "Previous path";
    }
    else
        $('input[name=settings_save_policy_path]').filter('[value="selected"]').attr('checked', true);
    $('#settings_save_policy_path_selected').text(path);
}

function set_load_policy_selected(path)
{
    if (!path.length)
        path = 'last';

    if (path === 'last')
    {
        $('input[name=settings_load_policy_path]').filter('[value="last"]').attr('checked', true);
        path = "Previous path";
    }
    else
        $('input[name=settings_load_policy_path]').filter('[value="selected"]').attr('checked', true);
    $('#settings_load_policy_path_selected').text(path);
}

function set_save_display_selected(path)
{
    if (!path.length)
        path = 'last';

    if (path === 'last')
    {
        $('input[name=settings_save_display_path]').filter('[value="last"]').attr('checked', true);
        path = "Previous path";
    }
    else
        $('input[name=settings_save_display_path]').filter('[value="selected"]').attr('checked', true);
    $('#settings_save_display_path_selected').text(path);
}

function set_load_display_selected(path)
{
    if (!path.length)
        path = 'last';

    if (path === 'last')
    {
        $('input[name=settings_load_display_path]').filter('[value="last"]').attr('checked', true);
        path = "Previous path";
    }
    else
        $('input[name=settings_load_display_path]').filter('[value="selected"]').attr('checked', true);
    $('#settings_load_display_path_selected').text(path);
}
