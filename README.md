# cld: Extensible CLI for Docker

A cross-platform, lightweight, fast and extensible CLI for docker. **cld** is implemented entirely in C and LUA. The use of LUA to provide extensible interfaces allows users to add functionality to the CLI.

**cld** uses **clibdocker** (another library written in C/LUA) to connect to docker, making it possible to connect to docker without the docker client.