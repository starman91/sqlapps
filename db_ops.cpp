//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		    		db_ops.cpp
//
//	Base class for Handling basic operations for connection and access 
// to a PostgreSQL database
//
//              -------------------
// author 		: Robert R. White
// date         	: Wed Jan 23 2008
// copyright	: (C) 2008 by  NREL
// email			: robert.white@nrel.gov
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	  Copyright	(C):  2008 by Robert R. White and  NREL
//    This file is part of the SQLApps library.
//
//    SQLApps is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    SQLApps is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//    along with SQLApps.  If not, see <http://www.gnu.org/licenses/>.
//
//	 We also ask that you maintain this copyright block and that any publications
//   surrounding, attributed to, or linked to this file or entire software system are also 
//   credited to the author and institution of this copyright
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include "db_ops.h"

DB_Ops::DB_Ops(const QString hostname, const QString dbName, const QString uname, const QString pwd, const int port) 
{ 
	setDBName(dbName);
	setDBHostName( hostname);
	setDBUserName(uname);
	setDBPd(pwd);
	setDBPort(port);
	m_dbConnect = false;
	return;
}

DB_Ops::DB_Ops()
{
	return;
}

DB_Ops::~DB_Ops() 
{ 
	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Initializes values in the DB class. Used for situations where the DB_Ops class is built as an instance.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DB_Ops::Init(const QString hostname, const QString dbName, const QString uname, const QString pwd, const int port)
{
	setDBName(dbName);
	setDBHostName( hostname);
	setDBUserName(uname);
	setDBPd(pwd);
	setDBPort(port);
	m_dbConnect = false;	
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Establishes open connection to DB when class is initiated
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DB_Ops::kickStart()
{
	if (getDBName() == "Samples_Test")
	{
		std::cout << "!!! WARNING !!! This is currently running against a test database, " <<  getDBName().toAscii().constData() << "\n\n";
	}
	
	//check for existing connection
	if (!m_dbConnect)
	{
		db = QSqlDatabase::addDatabase("QPSQL", "DBOPS");
		db.setHostName(getDBHostName());
		db.setDatabaseName(getDBName());
		db.setUserName(getDBUserName());
		db.setPassword(getDBPd());
		db.setPort(getDBPort());

		m_dbConnect = db.open();
		if (!m_dbConnect)
		{
			QSqlError error =db.lastError();
			std::cerr << "ERROR: Postgres open db failed for " << getDBName().toAscii().constData() << ". Error: " << error.text().toAscii().constData() << ":\n";
			return false;
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
/// Establishes open connection to DB when class is initiated
/// This is PDIL specific
//////////////////////////////////////////////////////////////////////////////////////////
bool DB_Ops::kickStart(const QString dbMoniker)
{
	if (getDBName() == "Samples_Test")
	{
		std::cout << "!!! WARNING !!! This is currently running against a test database, " <<  getDBName().toAscii().constData() << "\n\n";
	}
	
	//check for existing connection
	if (!m_dbConnect)
	{
		db = QSqlDatabase::addDatabase("QPSQL", dbMoniker);
		db.setHostName(getDBHostName());
		db.setDatabaseName(getDBName());
		db.setUserName(getDBUserName());
		db.setPassword(getDBPd());
		db.setPort(getDBPort());

		m_dbConnect = db.open();
		if (!m_dbConnect)
		{
			QSqlError error =db.lastError();
			std::cerr << "ERROR: Postgres open db failed for " << getDBName().toAscii().constData() << ". Error: " << error.text().toAscii().constData() << ":\n";
			return false;
		}
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////
/// Closes database connection
///////////////////////////////////////////////////////////////////////////////////////////
void DB_Ops::closeDB()
{
	db.close();
	return;
}

