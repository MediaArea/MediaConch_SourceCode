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

    return {
        init: init,
        loadTree: loadTree
    };

})();
