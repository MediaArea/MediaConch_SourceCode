var functionsUtils = (function() {
    // Based on https://stackoverflow.com/a/359910
    var executeFunctionByName = function(functionName, context /*, args */) {
      var args = [].slice.call(arguments).splice(2);
      var namespaces = functionName.split(".");
      var func = namespaces.pop();
      for(var i = 0; i < namespaces.length; i++) {
        context = context[namespaces[i]];
      }
      return context[func].apply(context, args);
    }
    return {
        executeFunctionByName: executeFunctionByName,
    }
})();
