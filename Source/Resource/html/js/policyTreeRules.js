var policyTreeRules = (function() {
    var display = function(node) {
        // Display MI or MT rule
        if (isRuleMT(node)) {
            policyTreeRulesMT.display(node);
        }
        else {
            policyTreeRulesMI.display(node);
        }

        $('.policyManage').addClass('hidden');
        $('.policyEdit').addClass('hidden');
        $('.policyEditRule').removeClass('hidden');
    };

    var isRuleMI = function(node) {
        if ('' == node.data.scope) {
            return true;
        }

        return false;
    }

    var isRuleMT = function(node) {
        if ('mt' == node.data.scope) {
            return true;
        }

        return false;
    }

    return {
        display: display,
        isRuleMI: isRuleMI,
        isRuleMT: isRuleMT,
    }
})();
