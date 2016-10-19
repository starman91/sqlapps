# SQLApps

This is a C++ dynamic ibrary program to expand capabilities in the Qt SQL library to facilitate better functionality, 
at least for my work.

## Getting Started
This is a C++ program so you will need to be able to compile this on your system. It is based on Linux but can be convertedd to
Windows or MacOS with a bit of work. The cross-platform Qt library supporting this will help. This also uses .pro files to describe 
the compilation and build the makefile. If you are not familiar with Qt .pro files please see Qt documentation, version 4.8.6

### Prerequisites
Known operationally good configuration:
Fedora 20 kernel 3.14.2-200.fc20.x86_64
At least Qt 4.8.x (untested on Qt5)
postgresql-devel-9.3.6-1.fc20.x86_64
fileapps libfileapps_rv1.so - found in same repo area for author

## Once Compiled
Once compiled you will need to place the library in a place accessible by any software wanting to access this library and 
also the SQL_API text file into an include directory that will be required by your software program. NOTE: you will need to change
the locations listed in the SQL_API library to correspond where you have located the header files.
