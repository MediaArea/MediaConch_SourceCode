(function(root) {
    /**
     * Create MediaConchPolicyChecker Object.
     * @constructor
     * @param {Object} [config]           Configuration options.
     * @param {string} [config.dataPath]  Use this path instead of the current directory for files loading.
     * @param {string} [config.verbosity] Checker report verbosity.
     */
    MediaConchPolicyChecker = function(config) {
        // Internal
        ready = false;
        trace = false;
        lastId = -1;
        // Config
        dataPath = '.';
        verbosity = 0;
        //Policies
        policies = [];
        policyTransformXmlXsl = null;
        // Displays
        displayXsl = null;
        // MediaInfo
        mediainfo = null;
        madiainfoReport = null;
        mediainfoModule = null;
        mediainfoVersion = '';

        if (config) {
            if (config.dataPath) {
                dataPath = config.dataPath;
            }

            if (config.verbosity) {
                verbosity = config.verbosity;
            }
        }

        updatePrefixInPolicy = function(policy) {
            if(!(policy instanceof(XMLDocument))) {
                return '';
            }
            //TODO: avoid this step
            var temp = new XMLSerializer().serializeToString(policy);
            temp = temp.replaceAll('my:namespace', 'http://www.w3.org/1999/XSL/Transform')
                       .replaceAll('aliasxsl', 'xsl');
            return new DOMParser().parseFromString(temp, 'application/xml');
        };

        // Prettify xml since Firefox XSLT processor does not supports indent="yes"
        prettifyXml = function(report) {
            const prefix = '  ';
            var toReturn = '';
            var indent = '';

            report.split(/>\s*</).forEach(function(node) {
                if (node.match(/^\/\w/)) {
                    indent = indent.substring(prefix.length);
                }
                toReturn += indent + '<' + node + '>\n';
                if (node.match(/^<?\w[^>]*[^\/]$/)) {
                    indent += prefix;
                }
            });

           return toReturn.substring(1, toReturn.length - 2);
        };

        // Promisified MediaInfo loading
        loadMediaInfoModule = function() {
            return new Promise(function(resolve, reject) {
                try {
                    mediainfoModule = MediaInfoLib({
                        'locateFile': function(path) {
                            return dataPath + '/' + path;
                        },
                        'postRun': function() {
                            if (mediainfoModule instanceof Promise) {
                                mediainfoModule.then(function(module) {
                                    mediainfoModule = module;
                                    resolve();
                                });
                            }
                            else {
                                resolve();
                            }
                        }
                    });
                }
                catch(e) {
                    reject(new Error('Unable to load MediaInfo module'));
                }
            });
        };

        // Generate mediainfo reports in suitable format for mediaconch
        createMediaInfoReport = function(file) {
            return new Promise(function(resolve, reject) {
                if (!(file instanceof(File))) {
                    reject(new Error('Not a File object'));
                }

                const template = '<MediaArea xmlns="https://mediaarea.net/mediaarea">\n'
                               + '<creatingLibrary version="' + mediainfoVersion + '" url="https://mediaarea.net/MediaInfo">MediaInfoLib</creatingLibrary>\n'
                               + '<media ref="' + file.name + '">\n'
                               + '<MediaInfo xmlns="https://mediaarea.net/mediainfo"/>\n'
                               + (trace ? '<MicroMediaTrace xmlns="https://mediaarea.net/micromediatrace"/>\n' : '')
                               + '</media>\n'
                               + '</MediaArea>\n';
                const parser = new DOMParser();
                try {
                    const report = parser.parseFromString(template, 'application/xml');

                    //TODO: Fix MMT bug and check if MMT is needed by policies
                    mediainfo.Option('Details', '1');
                    mediainfo.Option("Trace_Format", "MICRO_XML");
                    mediainfo.Open(file, function() {
                        mediainfo.Option('Details', '0');
                        mediainfo.Option('Inform', 'MIXML');
                        const miTemp = mediainfo.Inform();

                        const miNode = report.documentElement
                                             .getElementsByTagName('MediaInfo')[0];

                        miNode.innerHTML = parser.parseFromString(miTemp, 'application/xml')
                                                 .documentElement
                                                 .getElementsByTagName('media')[0].innerHTML;

                        if (trace) {
                            mediainfo.Option('Details', '1');
                            mediainfo.Option('Inform', 'MICRO_XML');
                            mediainfo.Inform();
                            const mmtTemp = mediainfo.Inform();

                            const mmtNode = report.documentElement
                                                  .getElementsByTagName('MicroMediaTrace')[0];

                            mmtNode.innerHTML = parser.parseFromString(mmtTemp, 'application/xml')
                                                      .documentElement
                                                      .getElementsByTagName('media')[0].innerHTML;
                        }

                        mediainfo.Close();
                        resolve(report);
                        console.dir(report)
                    });
                }
                catch(e) {
                    reject(new Error('Unable to create MediaInfo report'));
                }
            });
        };

        // Apply policies and display to media info report
        createMediaConchReport = function() {
            return new Promise(function(resolve, reject) {
                try {
                    if (mediainfoReport === null || policies.length === 0) {
                        resolve([true, '<MediaArea xmlns="https://mediaarea.net/mediaarea"/>\n']);
                    }

                    var toReturn = '';
                    var outcome = true;
                    const policyXsltProc = new XSLTProcessor();
                    policyXsltProc.setParameter(null, 'compare', '');
                    policyXsltProc.setParameter(null, 'policy_verbosity', verbosity);
                    policyXsltProc.importStylesheet(policyTransformXmlXsl);

                    const xmlSerializer = new XMLSerializer();
                    policies.forEach(function(policy) {
                        if (toReturn.length > 0) {
                            toReturn += '\n\n';
                        }

                        let policyXsl = policyXsltProc.transformToDocument(policy.xml);
                        policyXsl = updatePrefixInPolicy(policyXsl);

                        let reportXsltProc = new XSLTProcessor();
                        reportXsltProc.importStylesheet(policyXsl);
                        var report = reportXsltProc.transformToDocument(mediainfoReport);

                        if (report.documentElement.getElementsByTagName('policy')[0] === null ||
                            report.documentElement.getElementsByTagName('policy')[0].attributes.hasOwnProperty('outcome') === false ||
                            report.documentElement.getElementsByTagName('policy')[0].attributes['outcome'].value !== 'pass') {
                            outcome = false;
                        }

                        if (displayXsl !== null) {
                            let displayXsltProc = new XSLTProcessor();
                            displayXsltProc.importStylesheet(displayXsl);
                            report = displayXsltProc.transformToDocument(report);

                            // Unpack report
                            if (displayXsl.documentElement.nodeName === 'xsl:stylesheet' &&
                                displayXsl.documentElement.getElementsByTagName('xsl:output')[0] &&
                                displayXsl.documentElement.getElementsByTagName('xsl:output')[0].attributes.hasOwnProperty('method') &&
                                displayXsl.documentElement.getElementsByTagName('xsl:output')[0].attributes['method'].value === 'text') {
                                if (report.documentElement.nodeName === 'html' &&
                                    report.documentElement.getElementsByTagName('body')[0] &&
                                    report.documentElement.getElementsByTagName('body')[0].getElementsByTagName('pre')[0]) {
                                    toReturn += report.documentElement.getElementsByTagName('body')[0].getElementsByTagName('pre')[0].innerText;
                                }
                                else if(report.documentElement.nodeName === 'transformiix:result') {
                                    toReturn += report.documentElement.textContent;
                                }
                            }
                            else { // Pretty-print xml
                                let xml = xmlSerializer.serializeToString(report);
                                xml = prettifyXml(xml);

                                toReturn += xml;
                            }
                        }
                        else { // Pretty-print xml
                            let xml = xmlSerializer.serializeToString(report);
                            xml = prettifyXml(xml);

                            toReturn += xml;
                        }
                    });
                    resolve([outcome, toReturn]);
                }
                catch(e) {
                    reject(e);
                }
            });
        };

        // Load xsl sheet for policies
        loadPolicyTransformXmlXsl = function() {
            return new Promise(function(resolve, reject) {
                const xsl = 'MediaConch-PolicyTransformXml.xsl'
                const xhr = new XMLHttpRequest();

                xhr.responseType = 'document';
                xhr.onload = function() {
                    if (this.status >= 200 && this.status <= 299 && xhr.response !== null) {
                        policyTransformXmlXsl = xhr.response;
                        resolve();
                    }
                    else {
                        reject(new Error('Unable to fetch ' + dataPath + '/'  + xsl + ', status code: ' + xhr.status + ' (' + xhr.statusText + ')'));
                    }
                };
                xhr.onerror = function() {
                    reject(new Error('Unable to fetch ' + dataPath + '/'  + xsl + ', status code: ' + xhr.status + ' (' + xhr.statusText + ')'));
                };

                xhr.open('GET', dataPath + '/' + xsl);
                xhr.send();
            });
        };
    };
    root.MediaConchPolicyChecker = MediaConchPolicyChecker;

    /**
     * Initialize MediaConchPolicyChecker object.
     * @return {Promise} Promise of initialized object.
     */
    MediaConchPolicyChecker.prototype.init = async function() {
        const results = await Promise.all([
            (await loadPolicyTransformXmlXsl()),
            (await loadMediaInfoModule())
        ]);

        mediainfoVersion = mediainfoModule.MediaInfo.Option_Static('Info_Version', '').replaceAll('MediaInfoLib - v', '');
        mediainfo = new mediainfoModule.MediaInfo();
        mediainfo.Option('ReadByHuman', '0');
        mediainfo.Option('Language', 'raw');
        mediainfo.Option('ParseSpeed', '0');

        //TODO: check for trace support

        ready = true;

        return this;
    };

    /**
     * Add a policy to the checker list.
     * @param  {string}  policy Policy XML.
     * @throws {Error}          Will throw an error if the object is not in ready state.
     * @return {?number}        Policy ID (null if the policy is not valid).
     */
    MediaConchPolicyChecker.prototype.addPolicy = function(policy) {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }

        const dom = new DOMParser().parseFromString(policy, 'application/xml');

        if (dom.documentElement.nodeName !== 'policy') {
            return null;
        }

        var name = '';
        if (dom.documentElement.attributes.hasOwnProperty('name'))
            name = dom.documentElement.attributes['name'].value;

        const id = ++lastId;

        policies.push({
            id: id,
            name: name,
            xml: dom
        });

        return id;
    };

    /**
     * Remove policy from the checker list.
     * @param  {number}  id Policy ID.
     * @throws {Error}      Will throw an error if the object is not in ready state.
     * @return {?number}    Removed policy ID (null if not found).
     */
    MediaConchPolicyChecker.prototype.removePolicy = function(id) {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }

        for (var i = 0; i < policies.length; ++i) {
            if (policies[i].id === id) {
                policies.splice(i, 1);
                return id;
            }
        }

        return null;
    };

    /**
     * List policies in the checker list.
     * @throws  {Error}                    Will throw an error if the object is not in ready state.
     * @returns {Object[]} policies        Policies list.
     * @returns {number}   policies[].id   Policy ID.
     * @returns {string}   policies[].name Policy name.
     */
    MediaConchPolicyChecker.prototype.listPolicies = function(id) {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }

        const toReturn = [];
        policies.forEach(function(policy) {
            toReturn.push({
                id: policy.id,
                name: policy.name
            });
        });

        return toReturn;
    };

    /**
     * Get the policy xml.
     * @param   {number}  id Policy ID.
     * @throws {Error}       Will throw an error if the object is not in ready state.
     * @returns {?string}    Policy content XML (null if not found).
     */
    MediaConchPolicyChecker.prototype.getPolicy = function(id) {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }

        for (var i = 0; i < policies.length; ++i) {
            if (policies[i].id === id) {
                return new XMLSerializer().serializeToString(policies[i].xml);
            }
        }

        return null;
    };

    /**
     * Set current display.
     * @param  {string} display Custom display xslt stylesheet or '' for raw xml report.
     * @throws {Error}          Will throw an error if the object is not in ready state or if the custom stylesheet is not valid.
     */
    MediaConchPolicyChecker.prototype.setDisplay = function(display) {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }

        if (display === '') {
            displayXsl = null;
        }
        else {
            const newDisplay = new DOMParser().parseFromString(display, 'application/xml');
            if (newDisplay.documentElement.nodeName === 'parsererror' || newDisplay.documentElement.nodeName === 'html') {
                throw new Error('Invalid display.');
            }
            displayXsl = newDisplay;
        }
    };

    /**
     * Get current display.
     * @throws {Error}    Will throw an error if the object is not in ready state.
     * @returns {?string} Current display sheet (null if none).
     */
    MediaConchPolicyChecker.prototype.getDisplay = function() {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }

        if(displayXsl === null)
            return null;

        return new XMLSerializer().serializeToString(displayXsl);
    };

    /**
     * Set the verbosity level.
     * @param  {number} level Verbosity level.
     * @throws {Error}        Will throw an error if the object is not in ready state.
     */
    MediaConchPolicyChecker.prototype.setVerbosity = function(level) {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }

        verbosity = level;
    };

    /**
     * Get the verbosity level.
     * @throws {Error}       Will throw an error if the object is not in ready state.
     * @returns {?string}    Policy content XML (null if not found).
     */
    MediaConchPolicyChecker.prototype.getVerbosity = function() {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }

        return verbosity;
    };

    /**
     * Validate file against policies.
     * @param   {File}    file The file to validate.
     * @throws  {Error}        Will throw an error if the object is not in ready state.
     * @returns {Promise}      Promise of validation result as boolean.
     */
    MediaConchPolicyChecker.prototype.checkFile = async function(file) {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }
        mediainfoReport = await createMediaInfoReport(file);
        return (await createMediaConchReport())[0];
    };

    /**
     * Get the validation report of the last checked file (e.g. after changing the verbosity or the display).
     * @throws  {Error} Will throw an error if the object is not in ready state.
     * @returns {Promise} Promise of validation report as string.
     */
    MediaConchPolicyChecker.prototype.getReport = async function() {
        if (!ready) {
            throw new Error('MediaConchPolicyChecker is not ready. Call init() function before use.');
        }

        return (await createMediaConchReport())[1];
    };
}(this));
