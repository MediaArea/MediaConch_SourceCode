var mcoMessage = (function() {
    var node;

    var init = function(displayNode) {
        node = displayNode;
    }

    // Display success message
    var success = function(message) {
        $(node).html('<div class="alert alert-success alert-dismissible"><button type="button" class="close" data-dismiss="alert" aria-label="Close"><span aria-hidden="true">&times;</span></button>' + message + '</div>');
    }

    // Display error message
    var error = function(message) {
        $(node).html('<div class="alert alert-danger alert-dismissible"><button type="button" class="close" data-dismiss="alert" aria-label="Close"><span aria-hidden="true">&times;</span></button>' + message + '</div>');
    }

    // Handle fail ajax response
    var fail = function(jqXHR, otherNode) {
        // if (typeof jqXHR.responseJSON !== 'undefined') {
        //     if (jqXHR.responseJSON.hasOwnProperty('quota')) {
        //         if (null !== otherNode) {
        //             $(otherNode).html(jqXHR.responseJSON.quota);
        //         }
        //         else {
        //             $(node).html(jqXHR.responseJSON.quota);
        //         }
        //     }
        //     else if (jqXHR.responseJSON.hasOwnProperty('message')) {
        //         error(jqXHR.responseJSON.message);
        //     }
        //     else {
        //         error('An error has occured, please try again later');
        //     }
        // }
        // else {
            error('An error has occured, please try again later');
        // }
    }

    // Close message
    var close = function() {
        $(node).alert('close');
    }

    return {
        init: init,
        success: success,
        error: error,
        fail: fail,
        close: close,
    }
})();
