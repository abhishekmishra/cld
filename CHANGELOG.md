**0.1.0-alpha.0**  2023-01-22 Abhishek Mishra  <abhishekmishra3@gmail.com>

- This is the first ever official release of `cld`, although I've been
  working on it off and on for several years now. But development had stalled
  for more than a year and I picked it up again in late 2022.
- There are too many *changes* to enumerate.
- The `cld` program is a close equivalent of the docker cli program. I've 
  developed it alongwith the `clibdocker` program which provides a C API to
  the Docker Runtime API.
- To provide the command and sub-command structure of CLI program similar to
  the docker official client - a new library named `ZClk` was developed which
  provides an API to implement such a facility in C.
- Since this is a CLI program - it is better to look at the help files of
  the command runtime to understand its usage. Start with `cld --help`
