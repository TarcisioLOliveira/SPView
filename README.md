# SPView - a frontend for [SolidPrep](https://github.com/TarcisioLOliveira/SolidPrep)

SPView is library + program for offloading results from finite element analysis. 
It is a separate application, but it also comes with a library for easily 
launching and communicating with the SPView process.

Currently uses Gmsh as a frontend for simplicity, but the idea is to develop
a custom one.

Due to how Boost handles pipe streams, this program currently only works with
POSIX-compliant operating systems (e.g. Linux), but making it work on Windows
should not require too many modifications.

## Dependencies
- [Boost](https://www.boost.org/) (Process, ASIO, Filesystem, Thread)
- [Gmsh](https://gmsh.info/)
