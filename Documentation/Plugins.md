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
* **bin**:    Full path to the Command Line Interface
* **params**: CLI parameters to be given to DPF Manager CLI
* **isos**:   TIFF standarts to check

##### PreHook

* **id**:            Id of the plugin
* **name**:          Has to be *PreHook*
* **bin**:           Full path to the Command Line Interface of the command
* **formatting**:    Command line formatting (following the pattern "$VAR[ $VARXXX]*", $VAR can be $BIN, $INPUTPARAMS, $INPUTFILE, $OUTPUTPARAMS, $OUTPUTFILE, $PARAMS; default is "$BIN $INPUTPARAMS $INPUTFILE $OUTPUTPARAMS $OUTPUTFILE $PARAMS")
* **createFile**:    Need to be set to true, tell the plugin manager to create an output file
* **analyzeSource**: Analyze the generated file and the source file
* **outputDir**:     Directory where the created file will be put
* **outputExt**:     Extension of the File created
* **inputParams**:   CLI parameters to be given before the input file
* **outputParams**:  CLI parameters to be given before the output file
* **params**:        CLI parameters added after the command

##### FFmpeg

* **id**:            Id of the plugin
* **name**:          Has to be *FFmpeg*
* **bin**:           Full path to the Command Line Interface of ffmpeg
* **formatting**:    Command line formatting (following the pattern "$VAR[ $VARXXX]*", $VAR can be $BIN, $INPUTPARAMS, $INPUTFILE, $OUTPUTPARAMS, $OUTPUTFILE, $PARAMS; default is "$BIN $INPUTPARAMS $INPUTFILE $OUTPUTPARAMS $OUTPUTFILE $PARAMS")
* **createFile**:    Need to be set to true, tell the plugin manager to create an output file
* **analyzeSource**: Analyze the generated file and the source file
* **outputDir**:     File created by ffmpeg and used to be analyzed will be creted in this directory
* **outputExt**:     Extension of the File created by ffmpeg
* **inputParams**:   CLI parameters to be given to ffmpeg for the input file, the -i will be added by the program
* **outputParams**:  CLI parameters to be given to ffmpeg for the output file
* **params**:        CLI parameters added after the ffmpeg command

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
                "params": ["-s"]
                "isos": ["Baseline","Tiff/EP","Tiff/IT"]
            },
            {
                "id": "plugin3",
                "name": "PreHook",
                "bin": "$PATH_TO_CMD",
                "analyzeSource": false,
                "createFile": true,
                "outputDir": "/tmp",
                "outputExt": "mkv",
                "inputParams": ["--input"],
                "outputParams": [],
                "params": []
            },
            {
                "id": "plugin4",
                "name": "FFmpeg",
                "bin": "$PATH_TO_FFMPEG",
                "analyzeSource": false,
                "createFile": true,
                "outputDir": "/tmp",
                "outputExt": "mkv",
                "inputParams": ["-y"],
                "outputParams": ["-vcodec", "ffv1"],
                "params": []
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
