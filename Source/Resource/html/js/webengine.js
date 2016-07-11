$(document).ready(function() {
    // Register Qt WebPage object
    new QWebChannel(qt.webChannelTransport, function (channel) {
        webpage = channel.objects.webpage;
    });
});
