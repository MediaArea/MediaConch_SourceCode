var urlUtils = (function() {
    var baseUrl = 'https://mediaarea.net/MediaConchOnline/';

    var publicApi = function(path) {
        var url = baseUrl + 'api/public/v1/';

        if (undefined !== path) {
            url += path;
        }

        return url;
    }

    var protectedApi = function(path) {
        var url = baseUrl + 'api/protected/v1/';

        if (undefined !== path) {
            url += path;
        }

        return url;
    }

    return {
        publicApi: publicApi,
        protectedApi: protectedApi,
    }
})();
