## Daemon

### Usage

MediaConch uses the non-daemon mode by default. In your configuration, if you set Use_Daemon to true, it will try to connect using it.

By default, it will try to bind on "0.0.0.0", port 80.

You can change the address using the Daemon\_Address value and the port to bind using Daemon\_Port value.

Refer to Config.md for more explanation about the configuration.

When the daemon has been started, it will run jobs asynchronously in the background.

### GUI and CLI

You will have to configure the CLI or GUI to use the Daemon and specify which Daemon address/port to use. If the CLI/GUI is using the same as the daemon, there's nothing more to do.

### Parameters

* -h or --help: Show the help
* -v or --version: Show the Daemon version
* -c File or --configuration=File: Use the configuration file given by File
* --fork=yes: Default. Fork the process and run in background 
* --fork=no: Do not fork the process and run in forground
* -n: Shortcut alias for --fork=no
* --compression=[None/ZLib]: Default is ZLib. Use the algorithm or library given to compress (None means no compression)
* -cz: Shortcut alias for --compression=ZLib

## API

### Communication

The MediaConch daemon uses the protocol HTTP to communicate with the other clients.

It uses the REST API to pass the command.

See REST.md for more information about the API.

### New clients

If you have another need than the one provided by the CLI or GUI, feel free to implement a new one using this API.

If you do so, please, send us an email to let us know (https://mediaarea.net/MediaConch/contact.html).
