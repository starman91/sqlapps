//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		    		db_query.h
//
// Class to handle table queries to the database
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

#ifndef DB_QUERY_H
#define DB_QUERY_H

#include <QtSql>
#include <QtCore/QStringList>

#include "libpq-fe.h"

/// Class for perfoming queries to the DB
class DBQuery
{
	public:
		DBQuery();
		~DBQuery();
		static bool communeWithDB (QSqlDatabase&, const QString, QStringList&, QStringList&);
		bool queryDB (QSqlDatabase&, const QString, const QStringList, const QStringList, const QStringList, QStringList&);
		bool queryJoinDB (QSqlDatabase&, const QString, const QString, const QString, const QStringList, const QStringList, const QStringList, QStringList&);
		bool queryMultiJoinDB (QSqlDatabase&, const QString, const QStringList, const QStringList, const QStringList, const QStringList, const QStringList, QStringList&);

		inline void setQueryText (const QString text)		{ m_queryText = text;	};
		inline QString getQueryText (void)	const	{ return m_queryText;	};

	private:
		QString m_queryText;
};

#endif // DB_QUERY_H

