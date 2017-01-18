## Plugins

### File

The configuration of the plugins can be added in the general configuration of the CLI/GUI/Server.
See Config.md for more precision.

Also, it is also possible to give the file using the command line (CLI and Server only).


### Usage

The format used by this structure is *JSON*.

It should be a struct named *Plugins* which contains an array of plugin.


### Reconized entries

#### Format plugin

##### VeraPDF

* **id**:     Id of the plugin
* **name**:   Has to be *VeraPDF*
* **format**: Format given by MediaInfoLib to be used by the plugin. Here, it should be *PDF*
* **bin**:    Full path to the Command Line Interface
* **params**: CLI parameters to be given to Vera PDF CLI

##### DPFManager

* **id**:     Id of the plugin
* **name**:   Has to be *DPFManager*
* **format**: Format given by MediaInfoLib to be used by the plugin. Here, it should be *TIFF*
* **bin**:    Full path to the Command Line Interface (version 3.1)
* **params**: CLI parameters to be given to DPF Manager CLI (need at least: ["check", "-s", "-f", "xml"])
* **isos**:   TIFF standarts to check (could be: "TIAProfileChecker", "TiffITP1ProfileChecker", "TiffITP2ProfileChecker", "TiffITProfileChecker", "TIFF_Baseline_Core_6_0", "TIFF_Baseline_Extended_6_0", "TIFF_EP")

##### PreHook

###### PreHookOutputs

* **name**:          Name of the output, used in the formatting
* **createFile**:    If set to true, make the plugin manager to create an output file
* **analyze**:       Analyze the generated file, default set to true
* **outputDir**:     Directory where the created file will be put
* **outputExt**:     Extension of the File created
* **outputParams**:  CLI parameters to be given for the output file

###### Parameters

* **id**:            Id of the plugin (mandatory to be set)
* **name**:          Must be *PreHook*
* **bin**:           Full path to the Command Line Interface of the command
* **formatting**:    Command line formatting (following the pattern "$VAR[ $VARXXX]*", $VAR can be $BIN, $INPUTPARAMS, $INPUTFILE, $OUTPUTPARAMS, $OUTPUTFILE, $PARAMS; default is "$BIN $INPUTPARAMS $INPUTFILE $OUTPUTPARAMS $OUTPUTFILE $PARAMS"). In case of multiple outputs, $OUTPUTPARAMS and $OUTPUTFILE must be identified using ":$OUTPUTNAME" ($OUTPUTNAME is the name of the output)
* **inputParams**:   CLI parameters to be given before the input file
* **params**:        CLI parameters added after the command
* **outputs**:       List of PreHookOutputs

##### LogFile

* **id**:    Id of the plugin
* **name**:  Has to be *LogFile*
* **file**:  File name where log are wrote
* **level**: verbosity of the log (can be "nothing", "debug", "warning", "error"), default is only error.

### Example

```
    {"Plugins":
        [
            {
                "id": "plugin1",
                "name": "VeraPDF",
                "format": "PDF",
                "bin": "$PATH_TO_VERA_PDF",
                "params": ["--format", "mrr"]
            },
            {
                "id": "plugin2",
                "name": "DPFManager",
                "format": "TIFF",
                "bin": "$PATH_TO_DPF_MANAGER",
                "isos": ["TiffITProfileChecker", "TIFF_Baseline_Core_6_0", "TIFF_EP"],
                "params": ["check", "-s", "-f", "xml"]
            },
            {
                "id": "plugin3",
                "name": "PreHook",
                "bin": "$PATH_TO_CMD",
                "formatting": "$BIN $INPUTPARAMS $OUTPUTPARAMS:demo $OUTPUTFILE:demo $INPUTFILE",
                "inputParams": ["--input"],
                "params": [],
                "outputs": [
                    "name": "demo",
                    "createFile": true,
                    "analyze": true,
                    "outputDir": "/tmp",
                    "outputExt": "mkv",
                    "outputParams": []
                ]
            },
            {
                "id": "ffmpeg_example",
                "name": "PreHook",
                "bin": "$PATH_TO_FFMPEG",
                "formatting": "$BIN $INPUTPARAMS -i $INPUTFILE $OUTPUTPARAMS $OUTPUTFILE", #only one outputs, no need of the name
                "inputParams": ["-y"],
                "params": [],
                "outputs": [
                    "name": "demo",
                    "createFile": true,
                    "analyze": true,
                    "outputDir": "/tmp",
                    "outputExt": "mkv",
                    "outputParams": ["-vcodec", "ffv1"]
                ]
            },
            {
                "id": "plugin5",
                "name": "LogFile",
                "file": "/tmp/mediaconch.log",
                "level": "error"
           }
        ]
    }
```

### Note

* Due to a bug in Linux with the CLI of DPF Manager, consider to use the parameter *-s*.
* Right now, only XML is possible for output, so, consider to use *--format xml* with Vera PDF.
