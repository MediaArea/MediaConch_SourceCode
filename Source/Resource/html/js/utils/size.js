var sizeUtils = (function() {
    // Convert human readable size to bytes
    function humanToBytes(size) {
        var powers = {'k': 1, 'm': 2, 'g': 3, 't': 4};
        var regex = /(\d+(?:\.\d+)?)\s?(k|m|g|t)?b?/i;

        alert(size);
        var res = regex.exec(size);

        if (res[2] !== undefined) {
            return res[1] * Math.pow(1024, powers[res[2].toLowerCase()]);
        }
        else {
            return size;
        }
    }

    return {
        humanToBytes: humanToBytes,
    };
})();
