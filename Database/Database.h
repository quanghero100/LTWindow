#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
using namespace std;

#define EXPORT  __declspec(dllexport)

struct Data {
	std::string shortCode, fullCode, description;
};

extern sqlite3* ppDb;
extern sqlite3_stmt* ppStmt;
extern std::vector<Data> listTemplate;

bool connectDatabase(string fileName);
bool createTable();
bool query(string query, sqlite3_callback callback);
bool closeDatase();
void readDatabase();
EXPORT bool insertTemplate(string short_code, string full_code, string description);
EXPORT bool updateTemplate(string short_code, string full_code, string description);
EXPORT bool deleteTemplate(string short_code);
EXPORT vector<vector<string>> getAllTemplate();
EXPORT string searchTemplate(string short_code);
EXPORT int getNumberTemplate();