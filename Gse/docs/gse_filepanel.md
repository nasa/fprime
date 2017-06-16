# File Panel Docs

This markdown describes the usage and implemenation of the GSE File Panel.

## Usage
----
#### Uplinking
- Select Folder containing desired file
- Specify destination path
- Click 'Upload'
#### Downlinking
- Speficy source path
- Specify destination path
- Click 'Request File'

## Implementation
----
### Classes:
#### [file_panel.py]
	- The 'View'
	- User can 'spawn' uplink processes
	- Spawning puts a subprocess command into the [file_uplink_client]'s queue.
	- [file_listener] handles the actual creation of uplink/downlink subprocesses
#### [file_listener.py]
    - The 'Controller'
    - Periodically executes 'update_task'.
    - 'update_task' creates and listens to subprocesses.
    - Updates [file_panel] with uplink/downlink status
#### [file_uplink_client.py]
    - Provides an interface for starting an uplink subprocess.
    - Also provides gse_api with uplink capability
#### [file_downlink_client.py]
    - Provides an interface for starting downlink subprocess
    - Also provides gse_api with downlink capability


#### Uplink Details
An uplink subprocess is created for each file uplink. [file_listener]
handles most communication between the subprocess and [file_panel].

To create an uplink subprocess [file_panel] calls:

`[file_uplink_client].spawn(host, port, src_path, dest_path)`

Spawn adds a subprocess command to a [file_uplink_client] queue. During the periodically called `[file_listener].update_task()`, [file_listener] calls `[file_uplink_client].startNext()`. This executes the next available subprocess command.

This process allows the GUI or API to queue multiple file uplinks.

When [file_listener] starts a subprocess, [file_listener] creates a monitor thread to monitor the subprocess status. This monitor thread reads tokens returned from the subprocess stdout. Tokens are parsed and data is returned to [file_listener] who updates [file_panel] with the data.

#### Downlink Details
A downlink subprocess is spawned during [file_panel] initalization. [file_listener] starts a monitor thread after the subprocess is started. The downlink monitor thread behaves as the uplink monitor thread except that it is continuously run.
