# Syntax #

`cgymd DIRECTORY [PORT]`

# Options #

When executed, the cgymd program will try to bind to TCP port **PORT** and start accepting connections. if **PORT** is omitted, the default port (2496) is used.

It will use **DIRECTORY** as a base directory, and will not accept any requests outside this directory.

# Return codes #

The program returns zero on success, and non-zero on error.