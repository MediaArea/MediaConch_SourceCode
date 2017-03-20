var statusCell = (function() {
    var result;
    var init = function(table) {
        result = table;
    };

    var success = function(fileId) {
        var nodeStatus = $(result.cell('#result-' + fileId, 5).node());
        nodeStatus.removeClass('info danger checkInProgress').addClass('success');
        nodeStatus.find('.statusResult').html('<span class="glyphicon glyphicon-ok text-success" aria-hidden="true"></span><span class="status-text"> Analyzed</span>');
        nodeStatus.find('.result-reload').removeClass('hidden');
    };

    var inProgress = function(fileId, status) {
        var nodeStatus = $(result.cell('#result-' + fileId, 5).node());
        nodeStatus.addClass('checkInProgress');
        if (undefined == status.tool || 2 != status.tool || 100 == status.percent) {
            nodeStatus.find('.statusResult').html('<span class="spinner-status"></span>');
        }
        else {
            nodeStatus.find('.statusResult').html('<span class="spinner-status"></span>&nbsp;' + Math.round(status.percent) + '%');
        }
    };

    var error = function(fileId) {
        var nodeStatus = $(result.cell('#result-' + fileId, 5).node());
        nodeStatus.removeClass('info danger checkInProgress').addClass('danger');
        nodeStatus.find('.statusResult').html('<span class="glyphicon glyphicon-remove text-danger" aria-hidden="true"></span> Error');
        nodeStatus.find('.result-reload').removeClass('hidden');
    };

    var reset = function(fileId) {
        var nodeStatus = $(result.cell('#result-' + fileId, 5).node());
        nodeStatus.removeClass().addClass('statusCell info');
        nodeStatus.find('.statusButton').addClass('hidden').parent().removeClass('text-center');
        nodeStatus.find('.statusResult').removeClass('hidden').html('In queue');
        nodeStatus.find('.result-reload').addClass('hidden');
    };

    return {
        init: init,
        success: success,
        inProgress: inProgress,
        error: error,
        reset: reset,
    };
})();
