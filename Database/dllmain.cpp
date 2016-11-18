// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

sqlite3* ppDb = NULL;
sqlite3_stmt* ppStmt;
std::vector<Data> listTemplate;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		if (ppDb == NULL) {
			connectDatabase("data.db");
		}

		readDatabase();
	}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		if (ppDb != NULL) {
			closeDatase();
		}
		break;
	}
	return TRUE;
}

