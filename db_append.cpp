//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		    		db_append.cpp
//
//  Handles data base array fields type appending 
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

#include <QScopedPointer>
#include <iostream>
#include "db_append.h"

DBAppend::DBAppend()
{
	return;
}


DBAppend::~DBAppend()
{
}


///////////////////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////////////////
bool DBAppend::appendDBArray (QSqlDatabase& targetDB, const QString tableName, const QString fieldName,
                                                  const QString newVal, const QStringList qualFields, QStringList qualVals)
{
	QScopedPointer<QSqlQuery> query (new QSqlQuery(targetDB) );
	QString fieldString = ( "\"" + fieldName + "\"");

 	//Build qualifying statement
	QString qualifierString;
	for (int i = 0; i < qualFields.size(); i++)
	{
		QString conjunction="";
		if (qualVals[i].contains("'") ) //Check for any ' in the value string. This can cause it to break, so it needs to be escaped
		{
			qualVals[i].replace("'", "\\'");
			conjunction = "=E'"	;				//Declares the following string has escape sequences
		}
		if (qualVals[i].contains("$") ) //Check for any ' in the value string. This can cause it to break, so it needs to be escaped
		{
			conjunction = "=E'"	;				//Declares the following string has escape sequences
		}
		else
		{
			conjunction = "='";					//special characters requiring an escape sequence
		}
		qualifierString.append("\"" + qualFields[i] + "\"" + conjunction + qualVals[i] + "'" );
		if ( (i < qualFields.size()-1) && (1<qualFields.size()) )
		{
			qualifierString.append(" AND ");
		}
	}


   //Prep that statement
   //example: UPDATE characterizations SET "dataFileLink"=array_append("dataFileLink",'path1/file3.txt') WHERE "characterizationID"=1;
	QString queryString = "UPDATE " + tableName + " SET " + fieldString + "=array_append(" + fieldString + ",'" + newVal + "') WHERE " + qualifierString + ";";
	bool success = query->prepare(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres update preparation failed for: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}

	//Perform the array update
	success = query->exec();
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres update failed: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";
		return false;
	}

	#ifdef DEBUG
//		std::cout << "Executed successfully: " << query->lastQuery().toAscii().constData() << "\n";
	#endif
	return true;
}

