    $('#modalTrace{{ loop.index }}').on('show.bs.modal', function() {
        $('#trace{{ loop.index }}').jstree({
            'core' : {
                'check_callback' : function (operation, node, parent, position, more) {
                    if (operation === 'copy_node' || operation === 'move_node') {
                        return false;
                    }
                    else {
                        return true;
                    }
                },
                'multiple' : false,
                'dblclick_toggle' : false,
                'data' : {{ check.getTrace.jstree|raw }}
            },
            "plugins" : ['search', 'types', 'grid'],
            'types' : {
                'default' : {'icon' : 'glyphicon glyphicon-folder-open'},
                'block' : {'icon' : 'glyphicon glyphicon-folder-open'},
                'data' : {'icon' : 'glyphicon glyphicon-file'},
            },
            'grid' : {
                columns : [
                    {header: "Offset", value: 'offset'},
                    {header: "Key", tree: true},
                    {header: "Value",  value: 'dataValue'},
                ],
                resizable: true,
            },
        })
    });

    $('#trace{{ loop.index }}').on('ready.jstree', function () {
        $(function () {
            var to = false;
            $('#traceSearch{{ loop.index }}').keyup(function () {
                if(to) { clearTimeout(to); }
                to = setTimeout(function () {
                    var v = $('#traceSearch{{ loop.index }}').val();
                    $('#trace{{ loop.index }}').jstree(true).search(v);
                }, 250);
            });
        });
    });

    $('#trace{{ loop.index }}').on('loaded.jstree', function (e, data) {
        data.instance.get_container().find('li').each(function () {
            data.instance.open_node($(this));
        })
    });

    $('#trace{{ loop.index }}').on('select_node.jstree', function (e, data) {
        data.instance.toggle_node(data.node);
    });

    $('#modalInfo{{ loop.index }}').on('show.bs.modal', function() {
        $('#info{{ loop.index }}').jstree({
            'core' : {
                'check_callback' : function (operation, node, parent, position, more) {
                    if (operation === 'copy_node' || operation === 'move_node') {
                        return false;
                    }
                    else {
                        return true;
                    }
                },
                'multiple' : false,
                'dblclick_toggle' : false,
                'data' : {{ check.getInfo.jstree|raw }}
            },
            "plugins" : ['search', 'types', 'grid'],
            'types' : {
                'default' : {'icon' : 'glyphicon glyphicon-folder-open'},
                'block' : {'icon' : 'glyphicon glyphicon-folder-open'},
                'data' : {'icon' : 'glyphicon glyphicon-file'},
            },
            'grid' : {
                columns : [
                    {header: "Key", tree: true},
                    {header: "Value",  value: 'dataValue'},
                ],
                resizable: true,
            },
        })
    });

    $('#info{{ loop.index }}').on('ready.jstree', function () {
        $(function () {
            var to = false;
                    $('#infoSearch{{ loop.index }}').keyup(function () {
                        if(to) { clearTimeout(to); }
                        to = setTimeout(function () {
                            var v = $('#infoSearch{{ loop.index }}').val();
                            $('#info{{ loop.index }}').jstree(true).search(v);
                        }, 250);
                    });
                });
    });

    $('#info{{ loop.index }}').on('loaded.jstree', function (e, data) {
        data.instance.open_all();
    });

    $('#info{{ loop.index }}').on('select_node.jstree', function (e, data) {
        data.instance.toggle_node(data.node);
    });
