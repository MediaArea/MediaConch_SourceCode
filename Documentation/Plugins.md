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

* **name**: Has to be *VeraPDF*.
* **format**: Format given by MediaInfoLib to be used by the plugin. Here, it should be *PDF*.
* **bin**: Full path to the Command Line Interface.
* **params**: CLI parameters to be given to Vera PDF CLI.

##### DPFManager

* **name**: Has to be *DPFManager*.
* **format**: Format given by MediaInfoLib to be used by the plugin. Here, it should be *TIFF*.
* **bin**: Full path to the Command Line Interface.
* **params**: CLI parameters to be given to DPF Manager CLI.

##### FFmpeg

* **name**:         Has to be *FFmpeg*.
* **bin**:          Full path to the Command Line Interface of ffmpeg.
* **outputDir**:    File created by ffmpeg and used to be analyzed will be creted in this directory.
* **outputExt**:    Extension of the File created by ffmpeg.
* **inputParams**:  CLI parameters to be given to ffmpeg for the input file, the -i will be added by the program.
* **outputParams**: CLI parameters to be given to ffmpeg for the output file.
* **params**:       CLI parameters added after the ffmpeg command.

### Example

```
    {"Plugins":
        [
            {
                "name": "VeraPDF",
                "format": "PDF",
                "bin": "$PATH_TO_VERA_PDF",
                "params": ["--format", "xml"]
            },
            {
                "name": "DPFManager",
                "format": "TIFF",
                "bin": "$PATH_TO_DPF_MANAGER",
                "params": ["-s"]
            },
            {
                "name": "FFmpeg",
                "bin": "$PATH_TO_FFMPEG",
                "outputDir": "/tmp",
                "outputExt": "mkv",
                "inputParams": ["-y", "-v", "-8"],
                "outputParams": ["-vcodec", "ffv1"]
                "params": []
            }
        ]
    }
```

### Note

* Due to a bug in Linux with the CLI of DPF Manager, consider to use the parameter *-s*.
* Right now, only XML is possible for output, so, consider to use *--format xml* with Vera PDF.
