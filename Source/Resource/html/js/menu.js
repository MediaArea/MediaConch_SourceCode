var menu = (function() {
    var menu_change = function(e, name) {
        e.preventDefault();
        webpage.menu_link_checker(name);
    }

    return {
        menu_change: menu_change,
    }
})();
