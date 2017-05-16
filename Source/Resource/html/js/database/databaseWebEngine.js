var databaseAjax = (function() {

    var init = function() {
    }

    var loadTree = function() {
        webpage.checker_list(function(res) {
            data = JSON.parse(res);

            $.each(data.files, function(idx, val) {
                database.addFile(val);
            });

            if (data.error && data.error.length)
                mcoMessage.error(data.error);
        });
    }

    var removeFile = function(id) {
        webpage.remove_file_from_db(id, function(res) {
            data = JSON.parse(res);

            if (data.error && data.error.length)
                mcoMessage.error(data.error);
        });
    }

    var removeAllFiles = function() {
        webpage.remove_all_files_from_db(function(res) {
            data = JSON.parse(res);

            if (data.error && data.error.length)
                mcoMessage.error(data.error);
        });
    }

    return {
        init: init,
        loadTree: loadTree,
        removeFile: removeFile,
        removeAllFiles: removeAllFiles,
    };

})();
