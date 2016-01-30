## Configuration

### File

By default, the configuration file will be read from the user's local data.

On Linux, it will be in `/home/$USER/.config/MediaConch.rc`.

On Windows, it will be in `C:\Users\$USER\AppData\Roaming\MediaConch\MediaConch.rc`.

On Mac OS, it will be in `/Users/$USER/Library/Preferences/MediaConch.rc`.

### Usage

The format of the configuration file is *JSON*.

It should be an array of entries with each entry corresponding to the option wanted.

#### Reconized entries

* **SQLite\_Path**: give the path where the database should be created, default is the data application path.
* **Database\_Enabled**: enable or not the database, default yes.
* **Use\_Daemon**: in client mode, do the processing by a daemon or not.
* **Daemon\_Address**: in client and daemon mode, give the address where the daemon should bind, default 0.0.0.0.
* **Daemon\_Port**: in client and daemon mode, give the port where the daemon should bind, default 80.
* **Scheduler\_Max\_Threads**: give the number of cores which process files.
* **UI\_Poll\_Request**: set the value of the timer to refresh the UI in millisecond, default is 5000. It must be more than 500 and less than 10000.


#### Example

```
[
    {"Use_Daemon": false}, # Do not use the daemon
    {"SQLite_Path": "/tmp"}, # Put the Database into the /tmp directory
    {"Scheduler_Max_Threads": 2} # Use 2 cores to analyze files
]
```

#### Note

Consider using Scheduler\_Max\_Threads with your computer's real core number. If you use too many, your computer could freeze.
