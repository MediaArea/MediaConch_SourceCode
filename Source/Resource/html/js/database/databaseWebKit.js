var databaseAjax = (function() {

    var init = function() {
    }

    var loadTree = function() {
        var res = webpage.checker_list();
        var data = JSON.parse(res);

        var success = 0;
        $.each(data.files, function(idx, val) {
            success++;
            databaseTable.addFile(val);
        });

        if (success > 0) {
            databaseTable.draw();
        }
        else if (data.error && data.error.length)
            mcoMessage.error(data.error);
    }

    var addFileChecker = function(id) {
        var res = webpage.add_file_to_checker_again(id);
        var data = JSON.parse(res);

        if (data.error && data.error.length)
            mcoMessage.error(data.error);
        else if (data.success && data.success.length)
            mcoMessage.success(data.success);
    }

    var removeFile = function(id) {
        var res = webpage.remove_file_from_db(id);
        var data = JSON.parse(res);

        if (data.error && data.error.length)
            mcoMessage.error(data.error);
    }

    var removeAllFiles = function() {
        var res = webpage.remove_all_files_from_db();
        var data = JSON.parse(res);

        if (data.error && data.error.length)
            mcoMessage.error(data.error);
    }

    return {
        init: init,
        loadTree: loadTree,
        addFileChecker: addFileChecker,
        removeFile: removeFile,
        removeAllFiles: removeAllFiles,
    };

})();
