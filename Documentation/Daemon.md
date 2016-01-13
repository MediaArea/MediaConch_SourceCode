## Daemon

### Usage

MediaConch uses the non-daemon mode by default.
If you add in your configure the Use_Daemon to true, it will try to connect using it.

By default, it try to bind on "0.0.0.0", port 80.

You can change the address using the Daemon\_Address value and the port to bind using Daemon\_Port value.
Refer to Config.md for more explanation about the configuration.

Then, start it and let it do its job alone.

### GUI and CLI

You have to configure the CLI or GUI to use the Daemon and the Daemon address/port to use.
The good part, you can use the same as your daemon, nothing to add more.

Then, use them like you used to use them. 

### Parameter

* -h or --help: Show the help
* -v or --version: Show the Daemon version
* -c File or --configuration=File: Use the configuration file given by File
* --fork=yes: Default, Fork the process and run in background 
* --fork=no: Do not fork the process and run in forground
* -n: Same as --fork=no
* --compression=[None/ZLib]: Default ZLib, use the algorithm or library given to compress (None is no compression use)
* -cz: Same as --compression=ZLib

## API

### Communication

MediaConch daemon is using the protocol HTTP to communicate with the other clients.

It is using the REST API to pass the command.
See REST.md for more information about the API.

### New clients

If you have another need than the one provided by the CLI or GUI, feel free to implement a new one using this API.

If you do so, please, send us an email to let us know (https://mediaarea.net/MediaConch/contact.html).
