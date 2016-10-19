//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		    		db_ops.h
//
//	Handles basic operations for connection and access to database
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


#ifndef DB_OPS_H
#define DB_OPS_H

#include <QtSql>
#include "libpq-fe.h"

#include "db_query.h"
#include "db_insert.h"
#include "db_update.h"
#include "db_append.h"
#include "db_delete.h"

/// Main Class for preparing and intializing database connection and herding  communications with the database
class DB_Ops : public DBQuery, public DBInsert, public DBUpdate, public DBAppend, public DBDelete
{
	public:
		DB_Ops();
		DB_Ops(const QString, const QString, const QString, const QString, const int port = 5432); 
		~DB_Ops();
		void Init(const QString hostname, const QString dbName, const QString uname, const QString pwd, const int port);
		bool kickStart();
		bool kickStart(const QString);
		void closeDB(void);

		inline void setDBName (const QString text)		{ m_dbName = text;	};
		inline QString getDBName (void) const	{ return m_dbName;	}

		inline void setDBHostName (const QString text)		{ m_dbHostName = text;	};
		inline QString getDBHostName (void) const	{ return m_dbHostName;	}

		inline void setDBUserName (const QString text)		{ m_dbUserName = text;	};
		inline QString getDBUserName (void) const	{ return m_dbUserName;	}

		inline void setDBPd (const QString text)		{ m_dbPd = text;	};
		inline QString getDBPd (void) const	{ return m_dbPd;	}

		inline void setDBPort (const int value)		{ m_dbPort = value;	};
		inline int getDBPort (void) const	{ return m_dbPort;	}

		QSqlDatabase db;					/// Actual db object.

	private:
		QString m_dbName;				// DB name;
		QString m_dbHostName;		// DB Host holding db site
		QString m_dbUserName;		// DB user name
		QString m_dbPd;					// DB access password
		int m_dbPort;						// DB access port, defaults to 5432	

		bool m_dbConnect;				// Flag indicating db is connected.

};

#endif

