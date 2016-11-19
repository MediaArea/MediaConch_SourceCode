var urlUtils = (function() {
    var baseUrl = 'http://mediaconch/app_dev.php/';

    var publicApi = function(path) {
        var url = baseUrl + 'api/public/v1/';

        if (undefined !== path) {
            url += path;
        }

        return url;
    }

    return {
        publicApi: publicApi,
    }
})();
