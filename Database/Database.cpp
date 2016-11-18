// Database.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Database.h"

using namespace std;

extern sqlite3* ppDb;
extern sqlite3_stmt* ppStmt;

bool connectDatabase(string fileName)
{
	bool databaseNotExist = true;

	if (ifstream(fileName))
		databaseNotExist = false;


	ppDb = NULL;
	int result = sqlite3_open(fileName.c_str(), &ppDb);

	if (result != SQLITE_OK) {
		throw std::runtime_error("Can't open database");
		return false;
	}

	if (databaseNotExist) {
		return createTable();
	}

	return true;
}

bool createTable()
{
	query("CREATE TABLE Template ( short_code VARCHAR(50) NOT NULL, full_code NVARCHAR(5000) NOT NULL, description NVARCHAR(50) NOT NULL, PRIMARY KEY (short_code));", NULL);
	return true;
}

bool query(string query, sqlite3_callback callback)
{
	char* zErrMsg = NULL;
	char data[] = "";
	int result = sqlite3_exec(ppDb, query.c_str(), callback, (void*)data, &zErrMsg);

	std::ofstream f;
	f.open("log.txt");

	f << query;

	f.close();

	if (result != SQLITE_OK && zErrMsg) {
		throw std::runtime_error("Can't exec query");
		sqlite3_free(zErrMsg);
	}

	return true;
}

sqlite3_stmt* getStmt(string sql)
{
	sqlite3_stmt *stmt;
	sqlite3_prepare(ppDb, sql.c_str(), -1, &stmt, 0);

	return stmt;
}

bool insertTemplate(string short_code, string full_code, string description)
{
	char * query = "insert into Template (short_code, full_code, description) values (?, ?, ?);";

	auto stmt = getStmt(query);

	sqlite3_bind_text(stmt, 1, short_code.c_str(), short_code.length(), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, full_code.c_str(), full_code.length(), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, description.c_str(), description.length(), SQLITE_STATIC);

	auto result = sqlite3_step(stmt);

	if (result != SQLITE_DONE) {
		throw std::runtime_error("Can't exec query");
	}
	readDatabase();

	return true;
}

bool updateTemplate(string short_code, string full_code, string description)
{
	char * query = "update Template set full_code='?', description='?' where short_code='?';";

	auto stmt = getStmt(query);

	sqlite3_bind_text(stmt, 3, short_code.c_str(), short_code.length(), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 1, full_code.c_str(), full_code.length(), SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, description.c_str(), description.length(), SQLITE_STATIC);

	auto result = sqlite3_step(stmt);

	if (result != SQLITE_DONE) {
		throw std::runtime_error("Can't exec query");
	}

	return true;
}

bool deleteTemplate(string short_code) {
	std::string sql("delete from Template where short_code='");
	sql += short_code + "';";

	query(sql, NULL);

	readDatabase();
	return true;
}

//char *** getAllTemplate()
//{
//	if (listTemplate.size() == 0)
//		return NULL;
//
//	char*** result = new char**[listTemplate.size()];
//
//	for (int i = 0; i < listTemplate.size(); i++) {
//		result[i] = new char*[3];
//		result[i][0] = (char*)listTemplate[i].shortCode.c_str();
//		result[i][1] = (char*)listTemplate[i].fullCode.c_str();
//		result[i][2] = (char*)listTemplate[i].description.c_str();
//	}
//
//	return result;
//}

vector<vector<string>> getAllTemplate()
{
	vector<vector<string>> result;
	
	int numTemplate = listTemplate.size();
	result.resize(numTemplate);
	for (int i = 0; i < numTemplate; i++) 
	{
		result[i].push_back(listTemplate[i].shortCode);
		result[i].push_back(listTemplate[i].fullCode);
		result[i].push_back(listTemplate[i].description);
	}

	return result;
}

string searchTemplate(string short_code)
{
	
	for (int i = 0; i < listTemplate.size(); i++) {
		if (short_code == listTemplate[i].shortCode)
			return listTemplate[i].fullCode;
	}

	return "";
}

int getNumberTemplate()
{
	return listTemplate.size();
}

bool closeDatase()
{
	sqlite3_close(ppDb);
	return true;
}

int callbackReadData(void *NotUsed, int argc, char **argv, char **azColName) {
	std::string short_code(argv[0] ? argv[0] : "NULL");
	std::string full_code(argv[1] ? argv[1] : "NULL");
	std::string description(argv[2] ? argv[2] : "NULL");

	Data d;

	d.shortCode = short_code;
	d.fullCode = full_code;
	d.description = description;

	listTemplate.push_back(d);

	return 0;
}

void readDatabase()
{
	listTemplate.clear();
	query("select * from Template;", callbackReadData);
}
