# libtap

**libtap** is a library that supports writing unit tests for C and C++ programs.
Test output is compatible with the Test Anything Protocol (TAP).
The first version of this libary was included with the FreeBSD operating system.
An enhanced version, **libtap2**, was made available on **sourceforge.com**.
Version 2 was then incorporated into the **sxe** project on **github.com**.
This github **libtap** project is a stand-alone version of the library that can be built with the **baker** build program.

## Building With Make

Command | Description
------- | -----------
make    | Build the library
make test | Build and test the library
make package | Build the library and create a .deb package in the output directory

## Building With Baker

To build on Linux, clone the **baker** repository, then run: `../baker/baker -t`

## Installation

To install libtap, run: sudo dpkg -i output/output/libtap_2.01_amd64.deb

## Use

See the included tap.3 manual page. A formatted version of the manual page can be found here: [libtap Version 2 Programmer's Manual](https://github.com/jimbelton/libtap/wiki/libtap-Version-2-Programmer%27s-Manual)

