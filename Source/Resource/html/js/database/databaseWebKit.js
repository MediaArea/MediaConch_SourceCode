var databaseAjax = (function() {

    var init = function() {
    }

    var loadTree = function() {
        res = webpage.checker_list();
        data = JSON.parse(res);

        $.each(data.files, function(idx, val) {
            database.addFile(val);
        });

        if (data.error && data.error.length)
            mcoMessage.error(data.error);
    }

    var addFileChecker = function(id) {
        res = webpage.add_file_to_checker_again(id);
        data = JSON.parse(res);

        if (data.error && data.error.length)
            mcoMessage.error(data.error);
        else if (data.success && data.success.length)
            mcoMessage.success(data.success);
    }

    var removeFile = function(id) {
        res = webpage.remove_file_from_db(id);
        data = JSON.parse(res);

        if (data.error && data.error.length)
            mcoMessage.error(data.error);
    }

    var removeAllFiles = function() {
        res = webpage.remove_all_files_from_db();
        data = JSON.parse(res);

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
