# cld: Extensible CLI for Docker

A cross-platform, lightweight, fast and extensible CLI for docker. **cld** is 
implemented entirely in C and LUA. The use of LUA to provide extensible 
interfaces allows users to add functionality to the CLI.

**cld** uses:

* [**clibdocker** (a Docker client in C)](https://abhishekmishra.github.io/clibdocker) 
to connect to docker, making it possible to connect to docker without the 
officical docker client.
* [**ZClk**](https://abhishekmishra.github.io/zclk) to create the command line 
interface with commands, sub-commands, options, arguments etc.