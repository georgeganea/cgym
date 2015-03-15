# Syntax #

`cgym list [DIRECTORY] server1`

> `get [-n N] file server1 server2 ... serverN`

## List mode ##

When started with list as the second argument, the cgym client will connect to **server1** and request information about the contents of **DIRECTORY**. If **DIRECTORY** is omitted, it will request information about the root directory (**/**).

## Get mode ##

When started using the get argument, the cgym client will try to download the contents of **file** from the specified servers (**server1**, **server2** ... **serverN**). It will split the transfer in **N** segments, or, if the **-n** option is omitted, will split the transfer by the number of servers given.

# Return codes #

The program returns zero on success, and non-zero on error.