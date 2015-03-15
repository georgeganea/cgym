# Introduction #

The client and server communicate over a TCP connection. The default port for the connection is 2496. The server answers requests it receives on the connections until the connection is closed (either directly, or by issuing the `QUIT` command).

# Protocol specification #

## Handshake ##

When the server receives a new connection, it accepts and identifies itself by sending:

`CGYM\r\n`

## Client commands ##

The client can ask the server to perform 4 actions:
  * LIST 

&lt;directory&gt;

 - list the contents of the **directory**
  * SIZE 

&lt;file&gt;

 - get information about a **file** (size, MD5, name)
  * GET 

&lt;start&gt;

 

&lt;stop&gt;

 

&lt;file&gt;

 - get contents of **file**, starting at byte **start** (inclusive) and ending at byte **stop** (exclusive)

## LIST ##

The `LIST` command gets information about the contents of a specific directory from the server.

Format

`LIST path\r\n`

Example:

`LIST /\r\n` - used to get information about the contents of the document root (`/`)

`LIST /downloads\r\n` - used to get information about the contents of the `downloads` directory

### LIST replies ###

The server can answer the request in the following ways:

**Success**

The directory exists and the server has permissions to read its contents
The reply will be:

`OK\r\n`

Followed by the listing of the directory's contents, one entry per line, in the following **format**:

`type no_bytes md5 path\r\n`

where:

  * **type** can be the character **d** for directories, or **-** for files
  * **no\_bytes** is the number of bytes the file occupies, or the character **-** for directories
  * **md5** is the MD5 hash value of the file, or the character **-** for directories
  * **path** is the complete path of the file (e.g. _/downloads/ubuntu-7.10-desktop-i386.iso_)

After the last entry, an empty line (`\r\n`) will be sent to mark the end of the directory listing.

Examples:

Client sends:

`LIST /downloads\r\n`

Server replies with:

`OK\r\n`

`d - - /downloads/private/\r\n`

`- 729608192 d2334dbba7313e9abc8c7c072d2af09c /downloads/ubuntu-7.10-desktop-i386.iso\r\n`

`- 730691584 61c87943a92bc7bf519da4e2555d6e86 /downloads/ubuntu-7.10-desktop-amd64.iso\r\n`

`\r\n`

Meaning there are 1 directory (_private/_) and 2 files in _/downloads_.

**Error**

The directory contents cannot be accessed. The reply will be

`ERR\r\n`

## SIZE ##

The `SIZE` command gives information about a directory entry. It uses the same format as the LIST entry (see above)

Format:

`SIZE path\r\n`

### SIZE replies ###

On **success**, the server will reply with

`OK\r\n`

followed by the entry information (in the format specified in the `LIST` description).

Example:

Client sends:

`SIZE /downloads/ubuntu-7.10-desktop-i386.iso\r\n`

Server replies with:

`OK\r\n`

`- 729608192 d2334dbba7313e9abc8c7c072d2af09c /downloads/ubuntu-7.10-desktop-i386.iso\r\n`

meaning the file _ubuntu-7.10-desktop-i386.iso_ does exist in the _/downloads_ directory, is _729608192_ bytes long, and its MD5 hash value is _d2334dbba7313e9abc8c7c072d2af09c_.

On **error**, the server will reply with

`ERR\r\n`

## GET ##

The GET command initiates the transfer of a selected file from the server to the client.
Format:

`GET start stop path\r\n`

The server will attempt to send the contents of the file in **path**, starting from byte **start** (inclusive), up to byte **stop** (exclusive).

### GET replies ###

On **success**, the server will reply with

`OK\r\n`

and then start sending the content of the file (**stop** - **start** bytes in total). After sending the last byte, it will send

`\r\n`

, used by the client to match the end of the segment.

On **error**, the server will reply with

`ERR\r\n`

Errors can occur for several reasons:
  * the file does not exist
  * the server does not have permissions to read the file
  * the requested number of bytes cannot be sent (the range is not in the file)

## QUIT ##

The `QUIT` command terminates the connection between client and server.

Format:

`QUIT\r\n`

### QUIT replies ###

There is no reply, the server closes the connection immediately.