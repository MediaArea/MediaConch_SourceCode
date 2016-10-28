var textUtils = (function() {
    // Based on https://stackoverflow.com/a/4835406
    var sanitizeHtml = function(str) {
        var map = {
            '&': '&amp;',
            '<': '&lt;',
            '>': '&gt;',
            '"': '&quot;',
            "'": '&#039;'
        };

        return str.replace(/[&<>"']/g, function(m) { return map[m]; });
    }

// Based on https://stackoverflow.com/a/2919363
    var nl2br = function(str) {
        var breakTag = '<br>';

        return (str + '').replace(/([^>\r\n]?)(\r\n|\n\r|\r|\n)/g, '$1'+ breakTag +'$2');
    }

    return {
        sanitizeHtml: sanitizeHtml,
        nl2br: nl2br,
    };
})();
