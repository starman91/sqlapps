//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		    		db_insert.cpp  
//
//	Class to mange record inserts to database
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
#include "db_insert.h"
#include "errno.h"

DBInsert::DBInsert()
{
	return;
}


DBInsert::~DBInsert()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Performs an general insert into a defined tablename (as passed in)
/// The insert is performed as a mass transaction using bound variables
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DBInsert::sacrificeToDB (QSqlDatabase& targetDB, const QString tableName, const QStringList fieldList, const QStringList strValues)
{
	QScopedPointer<QSqlQuery> query (new QSqlQuery(targetDB) );
	QString queryString;
	QString fields = "(";
	QString placeholders = "(";

	//Sanity Check
	if (fieldList.size() != strValues.size() )
	{
		std::cerr << "ERROR: Field list size is not the same as the number of values passed into DB_Ops::sacrificeToDB()\n";
		return false;
	}

	//Build up message variables
	for (int i=0; i < fieldList.size(); i++)
	{
		fields.append("\"" + fieldList[i] + "\"");
		placeholders.append ("?");
		if ( i <  fieldList.size()-1 )
		{
			fields.append(", ");
			placeholders.append(", ");
		}
		else
		{
			fields.append(")");
			placeholders.append(")");
		}
	}

	queryString = "INSERT INTO " + tableName + " " + fields + " VALUES " + placeholders + ";";
	#ifdef DEBUG
//		std::cout << queryString.toAscii().constData() << "\n";
	#endif
	bool success = query->prepare(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres insert preparation failed for: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}

	for (int i=0; i < fieldList.size(); i++)
	{
		//test for type
		bool ok;
		int tempInt = strValues[i].toInt(&ok);
		if (ok)
		{
			// This was an integer. Bind it
			query->addBindValue(tempInt);
		}
		else
		{
			double tempDouble = strValues[i].toDouble(&ok);
			if (ok)
			{
				//This was a double. Bind it
				query->addBindValue(tempDouble);
			}
			else
			{
				//Its a string so bind it as is
				query->addBindValue(strValues[i]);
			}
		}
	}

	success = query->exec();
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres insert failed: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// overloaded class member of above, that returns PK of record created
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DBInsert::sacrificeToDB (QSqlDatabase& targetDB, const QString tableName, const QStringList fieldList, QStringList strValues, const QString pkName, int& retPK)
{
	QScopedPointer<QSqlQuery> query (new QSqlQuery(targetDB) );
	QString queryString;
	QString fields = "(";
	QString placeholders = "(";

	//Sanity Check
	if (fieldList.size() != strValues.size() )
	{
		std::cerr << "ERROR: Field list size is not the same as the number of values passed into DB_Ops::sacrificeToDB()\n";
		return false;
	}

	//Build up message variables
	for (int i=0; i < fieldList.size(); i++)
	{
		fields.append("\"" + fieldList[i] + "\"");
		placeholders.append ("?");
		if ( i <  fieldList.size()-1 )
		{
			fields.append(", ");
			placeholders.append(", ");
		}
		else
		{
			fields.append(")");
			placeholders.append(")");
		}
	}

	queryString = "INSERT INTO " + tableName + " " + fields + " VALUES " + placeholders + ";";
	#ifdef DEBUG
		std::cout << queryString.toAscii().constData() << "\n";
	#endif
	bool success = query->prepare(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres insert preparation failed for: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}

	for (int i=0; i < fieldList.size(); i++)
	{
		//test for type
		bool ok;
		int tempInt = strValues[i].toInt(&ok);
		if (ok)
		{
			// This was an integer. Bind it
			query->addBindValue(tempInt);
		}
		else
		{
			double tempDouble = strValues[i].toDouble(&ok);
			if (ok)
			{
				//This was a double. Bind it
				query->addBindValue(tempDouble);
			}
			else
			{
				//Its a string so bind it as is
				query->addBindValue(strValues[i]);
			}
		}
	}

	success = query->exec();
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres insert failed: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}

	QString queryFields;
	for (int i=0; i < fieldList.size(); i++)
	{
		QString conjunction="";
		if (strValues[i].contains("'") ) //Check for any ' in the value string. This can cause it to break, so it needs to be escaped
		{
			strValues[i].replace("'", "\\'");
			conjunction = "=E'"	;				//Declares the following string has escape sequences
		}
		if (strValues[i].contains("$") ) //Check for any ' in the value string. This can cause it to break, so it needs to be escaped
		{
			conjunction = "=E'"	;				//Declares the following string has escape sequences
		}
		else
		{
			conjunction = "='";					//special characters requiring an escape sequence
		}
		queryFields.append( "\"" + fieldList[i] + "\"" + conjunction + strValues[i] + "'");
		if ( i <  fieldList.size()-1 )
		{
			queryFields.append(" AND ");
		}
	}
	queryString = "SELECT \"" + pkName + "\" FROM " + tableName + " WHERE " + queryFields +";";
	#ifdef DEBUG
//		std::cout << queryString.toAscii().constData() << "\n";
	#endif
	query->exec(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres mass load failed for: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";;
		return false;
	}
	if (query->last() )
	{
		retPK = query->value(0).toInt();
	}
	else
	{
		std::cerr << "ERROR; Unable to find PK on insert  DB_Ops::sacrificeToDB ()\n";
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
/// Performs an general insert into a defined tablename (as passed in)
/// The insert is performed as a mass transaction using bound variables
////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DBInsert::massSacrificeToDB (QSqlDatabase& targetDB, const QString tableName, const QStringList fieldList, const int targetPK, const std::vector<double> values)
{
	QScopedPointer<QSqlQuery> query (new QSqlQuery(targetDB) );

	QString queryString;
	QString tmpFilename = "/tmp/tmp_vals_file.csv";
	//If copy command is to be run the file has to sit local to the machine with the database. If not adjust commands across net.
	if ( (targetDB.hostName() != "localhost") && (targetDB.hostName() != QHostInfo::localHostName() )  )
	{
		//If there is an old one there, get rid of it.
		QString rmCommand = "ssh " + targetDB.hostName().remove(".nrel.gov") + " \"rm -f ." + tmpFilename + "\"";
		int success = system(rmCommand.toAscii().constData());
		//Check for error
		if (success != 0)
		{	//Bad rm
			std::cerr << "ERROR: Unable to remove tmp file from host " << targetDB.hostName().toAscii().constData() << ".  errno = " << errno << "\n";
		}
	}
	else  //Local operation
	{
		//If there is an old one there, get rid of it.
		if (QFile::exists(tmpFilename) )
			QFile::remove(tmpFilename);
	}

	//Make a file to store temp data and open it.
	QScopedPointer<QFile> tmpFile (new QFile(tmpFilename) );

	if (!tmpFile->open(QIODevice::WriteOnly))
	{
		std::cerr << "Error creating file, /tmp/tmp_vals_file.txt to write\n";
		std::cerr << "Error detail: " << tmpFile->error() << "\n";
		return false;
	}

	//Load temp file with values data and the associated PK
	QTextStream outStream(tmpFile.data() );

	for (unsigned int i=0; i < values.size(); i++)
	{
		QString tmpString = QString::number(targetPK) + "," + QString::number(values[i],'e',16) + "\n";
		outStream << tmpString;
	}
	tmpFile->close();

    //Prep fields for COPY
	QString fields = "(";
	//Sanity Check
	if (fieldList.size() <=0 )
	{
		std::cerr << "ERROR: no Field List values passed into DB_Ops::massSacrificeToDB()\n";
		tmpFile->remove();
		return false;
	}

	//Build up message variables
	for (int i=0; i < fieldList.size(); i++)
	{
		fields.append("\"" + fieldList[i] + "\"");
		if ( i <  fieldList.size()-1 )
		{
			fields.append(", ");
		}
		else
		{
			fields.append(")");
		}
	}

	//Copy file to remote host, if not running locally
	if ( (targetDB.hostName() != "localhost") && (targetDB.hostName() != QHostInfo::localHostName() ) )
	{
		QString scpCommand = "scp -p " + tmpFilename + " " + targetDB.hostName().remove(".nrel.gov") + ":" + tmpFilename;
		int success = system(scpCommand.toAscii().constData());
		//Check for error
		if (success != 0)
		{	//If file did not transfer you have to bail
			std::cerr << "ERROR: Unable to copy tmp file to host " << targetDB.hostName().toAscii().constData() << ".  errno = " << errno << "\n";
	        return false;
		}
	}

	//Create SQL Command
	queryString = "COPY " + tableName + " " + fields + " FROM \'" + tmpFilename + "\' CSV;";

	//Execute command
	bool success = query->exec(queryString);
	if (!success)
	{
		QSqlError error = query->lastError();
		std::cerr << "ERROR: Postgres insert preparation failed for: " << queryString.toAscii().constData() << "\n";
		std::cerr << "Error message: " << error.text().toAscii().constData() << "\n";
		tmpFile->remove();
		return false;
	}

	tmpFile->remove();
	return true;
}


