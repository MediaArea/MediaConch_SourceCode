== Configuration ==

=== File ===

By default, the configuration file will be read from the user local data.

On Linux, it will be in ~/.config/MediaConch.rc.

On Windows, it will be in C:\Users\$USER_NAME\AppData\Roaming\MediaConch\MediaConch.rc

On Mac OS, it will be in ~/Library/Preferences/MediaConch.rc.

=== Usage ===

The format of the configuration file is JSON.

It should be an array of entries.

Each entry corresponding to the option wanted.

==== Reconized entry ====

* SQLite_Path: give the path where the database should be created, default is the data application path.

* Database_Enabled: enable or not the database, default yes.

* Use_Daemon: in client mode, do the processing by a daemon or not.

* Daemon_Address: in client and daemon mode, give the address where the daemon should bind, default 0.0.0.0.

* Daemon_Port: in client and daemon mode, give the port where the daemon should bind, default 80.

* Scheduler_Max_Threads: give the number of cores which process files.


==== Example ====

'''
[
    {"Use_Daemon": false}, # Do not use the daemon
    {"SQLite_Path": "/tmp"}, # Put the Database into the /tmp directory
    {"Scheduler_Max_Threads": 2} # Use 2 cores to analyze files
]
'''

==== Note ====

Consider using Scheduler_Max_Threads with you real cores number. If you use too much, your computer could freeze.