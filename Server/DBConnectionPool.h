#pragma once
#include "DBConnection.h"

/*
	DBConnectionPool
*/

class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool					Connect(int connectionCount, const WCHAR* connectionString);
	void					Clear();

	DBConnection* Pop();
	void					Push(DBConnection* connection);

private:
	SQLHENV					_environment = SQL_NULL_HANDLE;
	std::vector<DBConnection*>	_connections;
};

