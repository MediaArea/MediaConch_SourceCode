(function(root) {
    /**
     * Create MediaConch Object.
     * @constructor
     * @param {Object} [config]           Configuration options.
     * @param {string} [config.dataPath]  Use this path instead of the current directory for files loading.
     * @param {string} [config.verbosity] Checker report verbosity.
     */
    MediaConch = function(config) {
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
        //Implementation
        imsc1ImplementationModule = null;
        imsc1ImplementationChecker = null;
        checkImplementation = false;
        implementationReport = null;
        // Buffer
        fileBuffer = null;
        // Displays
        displayXsl = null;
        // MediaInfo
        mediainfo = null;
        madiainfoReport = null;
        mediainfoModule = null;
        mediainfoVersion = '';
        // PolicyChecker
        checkerModule = null;

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

        // Promisified MediaConch PolicyChecker
        loadPolicyCheckerModule = function() {
            return new Promise(function(resolve, reject) {
                try {
                    checkerModule = PolicyChecker({
                        'locateFile': function(path) {
                            return dataPath + '/' + path;
                        },
                        'postRun': function() {
                            if (checkerModule instanceof Promise) {
                                checkerModule.then(function(module) {
                                    checkerModule = module;
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
                    reject(new Error('Unable to load PolicyChecker module'));
                }
            });
        };

        // Promisified MediaConch IMSC1ImplementationChecker
        loadIMSC1ImplementationCheckerModule = function() {
            return new Promise(function(resolve, reject) {
                try {
                    imsc1ImplementationModule = IMSC1Plugin({
                        'locateFile': function(path) {
                            return dataPath + '/' + path;
                        },
                        'postRun': function() {
                            if (imsc1ImplementationModule instanceof Promise) {
                                imsc1ImplementationModule.then(function(module) {
                                    imsc1ImplementationModule = module;
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
                    reject(new Error('Unable to load IMSC1ImplementationChecker module'));
                }
            });
        };

        // Generate mediainfo reports in suitable format for mediaconch
        createMediaInfoReport = function() {
            return new Promise(function(resolve, reject) {
                const template = '<MediaArea xmlns="https://mediaarea.net/mediaarea">\n'
                               + '<creatingLibrary version="' + mediainfoVersion + '" url="https://mediaarea.net/MediaInfo">MediaInfoLib</creatingLibrary>\n'
                               + '<media ref="' + mediainfo.Get(mediainfoModule.Stream.General, 0, 'CompleteName') + '">\n'
                               + '<MediaInfo xmlns="https://mediaarea.net/mediainfo"/>\n'
                               + (trace ? '<MicroMediaTrace xmlns="https://mediaarea.net/micromediatrace"/>\n' : '')
                               + '</media>\n'
                               + '</MediaArea>\n';
                const parser = new DOMParser();
                try {
                    const report = parser.parseFromString(template, 'application/xml');

                    //TODO: Fix MMT bug and check if MMT is needed by policies
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

                    resolve(report);
                }
                catch(e) {
                    reject(new Error('Unable to create MediaInfo report'));
                }
            });
        };

        // Generate implementation report
        createImplementationReport = function(file, mixml) {
            return new Promise(function(resolve, reject) {
                if (!(file instanceof(File))) {
                    reject(new Error('Not a File object'));
                }

                const parser = new DOMParser();
                try {
                        if (mixml.documentElement.querySelectorAll("track[type='General'] > Format")[0] === null ||
                            mixml.documentElement.querySelectorAll("track[type='General'] > Format")[0].textContent !== 'TTML') {
                            resolve(null);
                        }

                        file.text().then(function(data) {
                        const temp = imsc1ImplementationChecker.validate_buffer(data, file.name);
                        const report = parser.parseFromString(temp, 'application/xml');

                        resolve(report);
                    });
                }
                catch(e) {
                    reject(new Error('Unable to create implementation report'));
                }
            });
        };

        // Apply policies and display to media info report
        createMediaConchReport = function() {
            return new Promise(function(resolve, reject) {
                try {
                    if ((mediainfoReport === null || policies.length === 0) && (!checkImplementation || implementationReport === null)) {
                        resolve([true, '<MediaArea xmlns="https://mediaarea.net/mediaarea"/>\n']);
                    }

                    var toReturn = '';
                    var outcome = true;
                    const policyXsltProc = new XSLTProcessor();
                    policyXsltProc.setParameter(null, 'compare', '');
                    policyXsltProc.setParameter(null, 'policy_verbosity', verbosity);
                    policyXsltProc.importStylesheet(policyTransformXmlXsl);

                    const xmlSerializer = new XMLSerializer();
                    const parser = new DOMParser();

                    if ( checkImplementation && implementationReport !== null)
                    {
                        if (implementationReport.documentElement.getElementsByTagName('implementationChecks')[0] === null ||
                            implementationReport.documentElement.getElementsByTagName('implementationChecks')[0].attributes.hasOwnProperty('fail_count') === false ||
                            implementationReport.documentElement.getElementsByTagName('implementationChecks')[0].attributes['fail_count'].value !== '0') {
                            outcome = false;
                        }

                        if (displayXsl !== null) {
                            let displayXsltProc = new XSLTProcessor();
                            displayXsltProc.importStylesheet(displayXsl);
                            report = displayXsltProc.transformToDocument(implementationReport);

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
                            let xml = xmlSerializer.serializeToString(implementationReport);
                            xml = prettifyXml(xml);
                            toReturn += xml;
                        }
                        toReturn += '\n';
                    }

                    const checker = new checkerModule.PolicyChecker();
                    policies.forEach(function(policy) {
                        checker.add_policy(xmlSerializer.serializeToString(policy.xml));
                    });

                    if(checker.full_parse()) {
                        const temp = checker.analyze(xmlSerializer.serializeToString(mediainfoReport), verbosity);
                        var report = parser.parseFromString(temp, 'application/xml');

                        if (report.documentElement.getElementsByTagName('policy')[0] === null ||
                            report.documentElement.getElementsByTagName('policy')[0].attributes.hasOwnProperty('outcome') === false ||
                            report.documentElement.getElementsByTagName('policy')[0].attributes['outcome'].value === 'fail') {
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
                    }
                    else {
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
                                report.documentElement.getElementsByTagName('policy')[0].attributes['outcome'].value === 'fail') {
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
                    }
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
    root.MediaConch = MediaConch;
    root.MediaConchPolicyChecker = MediaConch;

    /**
     * Enum for reports type.
     * @readonly
     * @enum {number}
     */
     MediaConch.prototype.ReportType = Object.freeze({
         MediaConch: 0,
         MediaInfo: 1
     });

    /**
     * Initialize MediaConch object.
     * @return {Promise} Promise of initialized object.
     */
    MediaConch.prototype.init = async function() {
        const results = await Promise.all([
            (await loadPolicyTransformXmlXsl()),
            (await loadMediaInfoModule()),
            (await loadPolicyCheckerModule())
        ]);

        mediainfoVersion = mediainfoModule.MediaInfo.Option_Static('Info_Version', '').replaceAll('MediaInfoLib - v', '');
        mediainfo = new mediainfoModule.MediaInfo();
        mediainfo.Option('Language', 'raw');
        mediainfo.Option('Complete', '1');

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
    MediaConch.prototype.addPolicy = function(policy) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
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
    MediaConch.prototype.removePolicy = function(id) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
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
    MediaConch.prototype.listPolicies = function(id) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
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
    MediaConch.prototype.getPolicy = function(id) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
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
    MediaConch.prototype.setDisplay = function(display) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
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
    MediaConch.prototype.getDisplay = function() {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
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
    MediaConch.prototype.setVerbosity = function(level) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        verbosity = level;
    };

    /**
     * Get the verbosity level.
     * @throws {Error}       Will throw an error if the object is not in ready state.
     * @returns {?string}    Policy content XML (null if not found).
     */
    MediaConch.prototype.getVerbosity = function() {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        return verbosity;
    };

    /**
     * Check the implementation for newly opened files.
     * @param   {boolean}  state.
     * @throws {Error}       Will throw an error if the object is not in ready state.
     */
    MediaConch.prototype.setCheckImplementation = function(state) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        checkImplementation = state;
    };

    /**
     * Get the implementation check state.
     * @throws {Error}       Will throw an error if the object is not in ready state.
     * @returns {?string}    Implementation check state.
     */
    MediaConch.prototype.getCheckImplementation = function() {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        return checkImplementation;
    };

    /**
     * Validate file against policies.
     * @param   {File}    file The file to validate.
     * @throws  {Error}        Will throw an error if the object is not in ready state or invalid File object.
     * @returns {Promise}      Promise of validation result as boolean.
     */
    MediaConch.prototype.checkFile = async function(file) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        if (!(file instanceof(File))) {
            throw new Error('Not a File object');
        }

        mediainfo.Option('Details', '1');
        mediainfo.Option("Trace_Format", "MICRO_XML");
        await new Promise(function(resolve, reject) {
            mediainfo.Open(file, function() {
                resolve();
            });
        });

        mediainfoReport = await createMediaInfoReport();
        mediainfo.Close();

        if (checkImplementation) {
            if (imsc1ImplementationModule === null) {
                await loadIMSC1ImplementationCheckerModule();
                imsc1ImplementationChecker = new imsc1ImplementationModule.IMSC1Plugin("/tmp/");
            }
            implementationReport = await createImplementationReport(file, mediainfoReport);
        }
        else
            implementationReport = null;

        return (await createMediaConchReport())[0];
    };

    /**
     * Validate file buffer against policies - init.
     * @param   {Size}         File size (-1 if not known).
     * @param   {Offset}       File offset of next chunk.
     * @param   {Name}         File name for the ref attribute.
     * @throws  {Error}        Will throw an error if the object is not in ready state.
     * @returns {Number}       MediaInfo_Open_Buffer_Init() result.
     */
    MediaConch.prototype.checkBufferInit = function(size, offset, name) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        mediainfo.Option('File_FileName', name);

        mediainfo.Option('Details', '1');
        mediainfo.Option("Trace_Format", "MICRO_XML");

        if (checkImplementation) {
            fileBuffer = new Uint8Array(0);
        }

        return mediainfo.Open_Buffer_Init(size, offset);
    };

    /**
     * Validate file buffer against policies - send buffer data.
     * @param   {Data}         8 bit data as ArrayBuffer.
     * @throws  {Error}        Will throw an error if the object is not in ready state.
     * @returns {Number}       MediaInfo_Open_Buffer_Continue() result.
     */
    MediaConch.prototype.checkBufferContinue = function(data) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        var ret = mediainfo.Open_Buffer_Continue(data);

        if (checkImplementation && fileBuffer !== null) {
            if (ret&0x01) {
                var type = mediainfo.Get(mediainfoModule.Stream.General, 0, 'Format');
                if (type === 'TTML') {
                    const buffer = new Uint8Array(fileBuffer.byteLength + data.byteLength);
                    buffer.set(fileBuffer, 0);
                    buffer.set(new Uint8Array(data), fileBuffer.byteLength);
                    fileBuffer = buffer;
                }
                else {
                    fileBuffer = null;
                }
            }
            else if (fileBuffer.length < 16777216) {
                    const buffer = new Uint8Array(fileBuffer.byteLength + data.byteLength);
                    buffer.set(fileBuffer, 0);
                    buffer.set(new Uint8Array(data), fileBuffer.byteLength);
                    fileBuffer = buffer;
            }
            else {
                fileBuffer = null;
            }
        }

        return ret;
    };

    /**
     * Validate file buffer against policies - get goto request.
     * @throws  {Error}        Will throw an error if the object is not in ready state.
     * @returns {Number}       MediaInfo_Open_Buffer_GoTo_Get() result.
     */
    MediaConch.prototype.checkBufferGoToGet = function() {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        return mediainfo.Open_Buffer_Continue_Goto_Get();
    };

    /**
     * Validate file buffer against policies - goto inform.
     * @param   {Size}         File size (-1 if not known).
     * @param   {Offset}       File offset of next chunk.
     * @throws  {Error}        Will throw an error if the object is not in ready state.
     * @returns {Number}       MediaInfo_Open_Buffer_Init() result.
     */
    MediaConch.prototype.checkBufferGoTo = function(size, offset) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        if (checkImplementation && fileBuffer) {
            fileBuffer = null;
        }

        return mediainfo.Open_Buffer_Init(size, offset);
    };

    /**
     * Validate file buffer against policies - finalize.
     * @throws  {Error}        Will throw an error if the object is not in ready state.
     * @returns {Number}       MediaInfo_Open_Buffer_Finalize() result.
     */
    MediaConch.prototype.checkBufferFinalize = function() {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        return mediainfo.Open_Buffer_Finalize();
    };

    /**
     * Validate file buffer against policies - get results.
     * @throws  {Error}        Will throw an error if the object is not in ready state.
     * @returns {Promise}      Promise of validation result as boolean.
     */
    MediaConch.prototype.checkBufferResult = async function() {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        mediainfoReport = await createMediaInfoReport();
        if (checkImplementation && fileBuffer) {
            if (imsc1ImplementationModule === null) {
                await loadIMSC1ImplementationCheckerModule();
                imsc1ImplementationChecker = new imsc1ImplementationModule.IMSC1Plugin("/tmp/");
            }

            const temp = imsc1ImplementationChecker.validate_buffer(fileBuffer, mediainfo.Get(mediainfoModule.Stream.General, 0, 'CompleteName'));
            implementationReport = new DOMParser().parseFromString(temp, 'application/xml');
        }
        else {
            implementationReport = null;
        }
        mediainfo.Close();

        return (await createMediaConchReport())[0];
    };

    /**
     * Get the report of the last checked file (e.g. after changing the verbosity or the display).
     * @param   {MediaConch.ReportType} requested report type(MediaInfo or MediaConch).
     * @throws  {Error} Will throw an error if the object is not in ready state.
     * @returns {Promise} Promise of mediainfo or mediaconch report as string.
     */
    MediaConch.prototype.getReport = async function(type=MediaConch.prototype.ReportType.MediaConch) {
        if (!ready) {
            throw new Error('MediaConch is not ready. Call init() function before use.');
        }

        if (type===MediaConch.prototype.ReportType.MediaInfo) {
            if (mediainfoReport !== null) {
                return Promise.resolve(prettifyXml(new XMLSerializer().serializeToString(mediainfoReport)));
            }
            else {
                return Promise.resolve(null);
            }
        }
        return (await createMediaConchReport())[1];
    };
}(this));
