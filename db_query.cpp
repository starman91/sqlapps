//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		    		db_query.cpp
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


#include <QScopedPointer>
#include <iostream>
#include "db_query.h"

DBQuery::DBQuery()
{
	m_queryText = "";
	return;
}


DBQuery::~DBQuery()
{
}

///////////////////////////////////////////////////////////////////////////////////////////
/// Performs the communication wit the database. Query/Insert commands
/// are passed in and results + headers are passed back if generated.
//////////////////////////////////////////////////////////////////////////////////////////
bool DBQuery::communeWithDB (QSqlDatabase& targetDB, const QString queryString, QStringList& returnHeaders, QStringList& returnRecords)
{
	QScopedPointer<QSqlQuery> query (new QSqlQuery(targetDB) );
	returnHeaders.clear();
	returnRecords.clear();
	bool success = query->exec(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		if (error.text().contains("does not exist") )
		{
			//No data returned for this user so this will fail. Do not return false
			std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
//			std::cerr << "No data returned for this query due to no table entry for this qualifier: " << queryString.toAscii().constData() << "\n";
			return true;
		}
		else
		{
        	std::cerr << "ERROR: Postgres query failed: " << queryString.toAscii().constData() << "\n";
			std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
			return false;
		}
	}

	int numColumns = query->record().count();

	//Gather up header
	for (int i=0; i<numColumns; i++)
	{
		returnHeaders << query->record().fieldName(i);
	}

	//Gather up records
	while (query->next())
	{
		QString tempRecord="";
		for (int i=0; i<numColumns; i++)
		{
			if (query->value(i).isNull() )
			{
				tempRecord.append("");
			}
			else
			{
				tempRecord.append(query->value(i).toString() );
			}

			if (i < (numColumns-1))
			{
				tempRecord.append(", ");
			}
		}
		returnRecords << tempRecord;
	}

	#ifdef DEBUG
//		std::cout << "SQL Query Returned:\n";
//		for (int i=0; i < numColumns; i++ )
//		{
//			if (i < (numColumns-1))
//				std::cout << returnHeaders[i].toAscii().constData() << ", ";
//			else
//				std::cout << returnHeaders[i].toAscii().constData();
//		}
//		std::cout << "\n";
//		for (int i=0; i < returnRecords.size(); i++ )
//		{
//			std::cout << returnRecords[i].toAscii().constData() << "\n";
//		}
	#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Performs the communication with the database. Using a basic open ended query
/// for select procedures
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DBQuery::queryDB (QSqlDatabase& targetDB, const QString tableName, const QStringList returnFields,
																const QStringList qualFields, QStringList qualVals, QStringList& returnRecords)
{
	QScopedPointer<QSqlQuery> query (new QSqlQuery(targetDB) );

	//Sanity check
	if (returnFields.size() < 0 )
	{
		std::cerr << "ERROR: No values listed to return DB_Ops::queryDB()\n";
		return false;
	}

	//Build return values statement
	QString retString;
	for (int i = 0; i < returnFields.size(); i++)
	{
		retString.append( "\"" + returnFields[i] + "\"" );
		if ( (i < returnFields.size()-1) && (1<returnFields.size()) )
		{
			retString.append(", ");
		}
	}

	//Sanity Check
	if (qualFields.size() != qualVals.size() )
	{
		std::cerr << "ERROR: Qualifier names is not same size as Qualifier values in DB_Ops::queryDB()\n";
		return false;
	}

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
	QString queryString = "SELECT " + retString + " FROM " + tableName + " WHERE " + qualifierString + ";";
	#ifdef DEBUG
		std::cout << queryString.toAscii().constData() << "\n";
	#endif
	bool success = query->prepare(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres select preparation failed for: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}


	//Perform the select
	success = query->exec();
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres select failed: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";
		return false;
	}

	//Gather up records
	int numColumns = query->record().count();
	while (query->next())
	{
		QString tempRecord="";
		for (int i=0; i<numColumns; i++)
		{
			if (i < (numColumns-1))
				tempRecord.append(query->value(i).toString() + ", ");
			else
				tempRecord.append(query->value(i).toString());
		}
		returnRecords << tempRecord;
	}

	#ifdef DEBUG
//		std::cout << query->lastQuery().toAscii().constData() << "\n";
//		if (0<returnRecords.size() )
//		{
//			std::cout << "SQL Query Returned:\n";
//			for (int i=0; i < returnRecords.size(); i++ )
//			{
//				std::cout << returnRecords[i].toAscii().constData() << "\n";
//			}
//		}
//		else
//		{
//			std::cout << "No data returned from query\n";
//		}
	#endif
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Performs the communication with the database. Using a basic open ended query
/// for select procedures
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DBQuery::queryJoinDB (QSqlDatabase& targetDB, const QString tableName, const QString joinTable, const QString usingParam,
																const QStringList returnFields, const QStringList qualFields, QStringList qualVals,
																QStringList& returnRecords)
{
	QScopedPointer<QSqlQuery> query (new QSqlQuery(targetDB) );

	//Sanity check
	if (returnFields.size() < 0 )
	{
		std::cerr << "ERROR: No values listed to return DB_Ops::queryDB()\n";
		return false;
	}

	//Build return values statement
	QString retString;
	for (int i = 0; i < returnFields.size(); i++)
	{
		retString.append( "\"" + returnFields[i] + "\"" );
		if ( (i < returnFields.size()-1) && (1<returnFields.size()) )
		{
			retString.append(", ");
		}
	}

	//Sanity Check
	if (qualFields.size() != qualVals.size() )
	{
		std::cerr << "ERROR: Qualifier names is not same size as Qualifier values in DB_Ops::queryDB()\n";
		return false;
	}

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
	QString queryString = "SELECT " + retString + " FROM " + tableName + " JOIN " + joinTable + " USING (\"" + usingParam +"\") WHERE " + qualifierString + ";";
	#ifdef DEBUG
//		std::cout << queryString.toAscii().constData() << "\n";
	#endif
	bool success = query->prepare(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres select preparation failed for: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}


	//Perform the select
	success = query->exec();
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres select failed: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";
		return false;
	}

	//Gather up records
	int numColumns = query->record().count();
	while (query->next())
	{
		QString tempRecord="";
		for (int i=0; i<numColumns; i++)
		{
			if (i < (numColumns-1))
				tempRecord.append(query->value(i).toString() + ", ");
			else
				tempRecord.append(query->value(i).toString());
		}
		returnRecords << tempRecord;
	}

	#ifdef DEBUG
//		std::cout << query->lastQuery().toAscii().constData() << "\n";
//		if (0<returnRecords.size() )
//		{
//			std::cout << "SQL Query Returned:\n";
//			for (int i=0; i < returnRecords.size(); i++ )
//			{
//				std::cout << returnRecords[i].toAscii().constData() << "\n";
//			}
//		}
//		else
//		{
//			std::cout << "No data returned from query\n";
//		}
	#endif
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Performs the communication with the database. Using a basic open ended query
/// for select procedures joining to several tables
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DBQuery::queryMultiJoinDB (QSqlDatabase& targetDB, const QString tableName, const QStringList joinTables, const QStringList usingParams,
																const QStringList returnFields, const QStringList qualFields, QStringList qualVals,
																QStringList& returnRecords)
{
	QScopedPointer<QSqlQuery> query (new QSqlQuery(targetDB) );

	//Sanity check
	if (returnFields.size() < 0 )
	{
		std::cerr << "ERROR: No values listed to return DB_Ops::queryDB()\n";
		return false;
	}

	//Build return values statement
	QString retString;
	for (int i = 0; i < returnFields.size(); i++)
	{
		if (returnFields[i].contains(".") )
		{
			QString newField = returnFields[i];
			int mark = newField.indexOf(".");
			retString.append( newField.insert(mark+1, "\"") + "\"");
		}
		else
		{
			retString.append( "\"" + returnFields[i] + "\"" );
		}

		if ( (i < returnFields.size()-1) && (1<returnFields.size()) )
		{
			retString.append(", ");
		}
	}

	//Sanity Check
	if (qualFields.size() != qualVals.size() )
	{
		std::cerr << "ERROR: Qualifier names is not same size as Qualifier values in DB_Ops::queryDB()\n";
		return false;
	}

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

		if (qualFields[i].contains(".") )
		{
			QString newField = qualFields[i];
			int mark = newField.indexOf(".");
			qualifierString.append( newField.insert(mark+1, "\"") + "\"" + conjunction + qualVals[i] + "'" );
		}
		else
		{
			qualifierString.append("\"" + qualFields[i] + "\"" + conjunction + qualVals[i] + "'" );
		}
		if ( (i < qualFields.size()-1) && (1<qualFields.size()) )
		{
			qualifierString.append(" AND ");
		}
	}


	//Prep that statement
	QString joining;
	if (joinTables.size() != usingParams.size())
	{
		std::cerr << "Multiple joining tables size does is not the same as the using parameter size. Stopping query in DBQuery::queryMultiJoinDB()\n";
		return false;
	}
	else
	{
		for (int i=0; i < joinTables.size(); i++)
		{
			joining.append (" JOIN " + joinTables[i] + " USING (\"" + usingParams[i] +"\")" );

		}
	}

	QString queryString = "SELECT " + retString + " FROM " + tableName + joining + " WHERE " + qualifierString + ";";
	#ifdef DEBUG
		std::cout << queryString.toAscii().constData() << "\n";
	#endif
	bool success = query->prepare(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres select preparation failed for: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}


	//Perform the select
	success = query->exec();
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres select failed: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";
		return false;
	}

	//Gather up records
	int numColumns = query->record().count();
	while (query->next())
	{
		QString tempRecord="";
		for (int i=0; i<numColumns; i++)
		{
			if (i < (numColumns-1))
				tempRecord.append(query->value(i).toString() + ", ");
			else
				tempRecord.append(query->value(i).toString());
		}
		returnRecords << tempRecord;
	}

	#ifdef DEBUG
//		std::cout << query->lastQuery().toAscii().constData() << "\n";
//		if (0<returnRecords.size() )
//		{
//			std::cout << "SQL Query Returned:\n";
//			for (int i=0; i < returnRecords.size(); i++ )
//			{
//				std::cout << returnRecords[i].toAscii().constData() << "\n";
//			}
//		}
//		else
//		{
//			std::cout << "No data returned from query\n";
//		}
	#endif
	return true;
}



