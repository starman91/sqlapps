///////////////////////////////////////////////////////////////////////////////
//

//************************************************************
//                                 MAIN
/*! \mainpage
*
* \section intro_sec Introduction
* SQLApps Library \n
* \n
* This library allows other systems to communicate with a PostgreSQL database.
* many of the queries are built exclusively for PosgreSQL. However the functions can be
* overloaded for other types as long as the Qt library underneath has been built with the needed
* database modules (Oracle, MySql, etc)
*
* The library is a go between for communication with the database. The methods are built genericlly so no knowledge of the datbase or schemas is reuired by the library.
* That information must be passed in by the user to direct the communications. The methods are built to handle single or multiple queries and bulk uploads and downloads
*  from the database.
*
* The library also provides the method to establish the connection to the datbase and any disconnections.
*
*  the libray uses smart pointers and error handling with errors returned via std::cerr or through a log file interface (see fileApps.lib data for more information).
*\n \n
*  \section install_sec Installation
* Copy all files from the follwing directories to your local machine to run the program: \n
* src/xxx.cpp \n
* include/xxx.h \n
* bin/ (should be empty) \n
* obj/ (should be empty) \n
* config/ (should be empty) \n
* docs/ contains the latest code documentation for the system (HTML) \n
*
* \subsection step1 Step 1: Library Dependencies
* This software depends on the following libraries: \n
* 1. Qt (version 4.6.0 or later)
*      In addition Qt will need its $QTDIR environment variable set accordingly and also it must have been
* compiled with Postgres sub libraries enabled  \n
* 2. fileapps - home made library for access and writing of files (configs, logs, etc.) \n
*
* \subsection step2 Step 2: Setting up project file
* The project uses the sqlapps.pro file to define project parameters. Be sure to set
* library dependencies and include dependencies paths according to the system to be compiled
* on. Note that the file currently includes the correct library names and number for compilation to work.
* Note that the pddiluserdata lib file must compile and install first in order for this to be able to compile.
* However, locations may vary. \n
*
* In testing the main program is compiled in /data0/pdil/lib/sqlapps/trunk \n
* Library dependencies are located in /data0/pdil/lib \n
*
* \subsection step3 Step 3: Creating makefile
* The makefile is created by envoking the following command: \n
* <b>qmake -o Makefile sqlapps.pro </b>
*
* \subsection step4 Step 4: Making the program
* Build the program by evoking the following command from the installed root project directory \n
* <b>make clean </b> \n
* <b>make </b> \n
* The library "sqlapps.so" is now in the bin/ directory. \n
*
*/
//************************************************************
//////////////////////////////////////////////////////////////////////////////////


