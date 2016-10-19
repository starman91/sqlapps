//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		    		db_update.cpp
//
//	Class to handle record updates to the database
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
#include "db_update.h"

DBUpdate::DBUpdate()
{
	return;
}


DBUpdate::~DBUpdate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////////////////////////
bool DBUpdate::updateToDB (QSqlDatabase& targetDB, const QString tableName, const QStringList setNames,
																				const QStringList setVals, const QStringList qualNames,
																				const QStringList qualVals)
{
	QScopedPointer<QSqlQuery> query (new QSqlQuery(targetDB) );
	QString queryString;
	bool ok;

	//Sanity Check
	if (setNames.size() != setVals.size() )
	{
		std::cerr << "ERROR: Set names is not same size as Set values in DB_Ops::updateToDB()\n";
		return false;
	}

	//Build qualifying statement
	QString setString;
	for (int i = 0; i < setNames.size(); i++)
	{
		setString.append("\""+setNames[i] + "\"=:s" + QString::number(i) );
		if ( (i < setNames.size()-1) && (1<setNames.size()) )
		{
			setString.append(", ");
		}
	}

	//Sanity Check
	if (qualNames.size() != qualVals.size() )
	{
		std::cerr << "ERROR: Qualifier names is not same size as Qualifier values in DB_Ops::updateToDB()\n";
		return false;
	}

	//Build qualifying statement
	QString qualifierString;
	for (int i = 0; i < qualNames.size(); i++)
	{
		qualifierString.append("\"" + qualNames[i] + "\"=:q" + QString::number(i) );
		if ( (i < qualNames.size()-1) && (1<qualNames.size()) )
		{
			qualifierString.append(" AND ");
		}
	}

	//Prep that statement
	queryString = "UPDATE " + tableName + " SET " + setString + " WHERE " + qualifierString + ";";
	#ifdef DEBUG
//		std::cout << queryString.toAscii().constData() << "\n";
	#endif
	bool success = query->prepare(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres update preparation failed for: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}

	//Bind the values for set
	for (int i=0; i < setVals.size(); i++)
	{
		QString targetBind = ":s"+QString::number(i);
		//test for type
		int tempInt = setVals[i].toInt(&ok);
		if (ok)
		{
			// This was an integer. Bind it
			query->bindValue(targetBind, tempInt);
		}
		else
		{
			double tempDouble = setVals[i].toDouble(&ok);
			if (ok)
			{
				//This was a double. Bind it
				query->bindValue(targetBind, tempDouble);
			}
			else
			{
                if (setVals[i].isEmpty() )
				{
                    QVariant x;      // Throw in a null value
					query->bindValue(targetBind, x);
				}
				 else
				 {
					//Its a string so bind it as is
					query->bindValue(targetBind, setVals[i]);
				 }
			}
		}
	}

	//Bind the values for qualifier
	for (int i=0; i < qualVals.size(); i++)
	{
		QString targetBind = ":q"+QString::number(i);
		//test for type
		int tempInt = qualVals[i].toInt(&ok);
		if (ok)
		{
			// This was an integer. Bind it
			query->bindValue(targetBind, tempInt);
		}
		else
		{
			double tempDouble = qualVals[i].toDouble(&ok);
			if (ok)
			{
				//This was a double. Bind it
				query->bindValue(targetBind, tempDouble);
			}
			else
			{
				//Its a string so bind it as is
				query->bindValue(targetBind, qualVals[i]);
			}
		}
	}

	//Perform the update
	success = query->exec();
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres update failed: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";
		return false;
	}
	return true;
}

