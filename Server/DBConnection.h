#pragma once

#include "../Common.h"
#include <sql.h>
#include <sqlext.h>

/*
	DBConnection
*/

class DBConnection
{
public:
	bool			Connect(SQLHENV henv, const WCHAR* connectionString);
	void			Clear();

	bool			Execute(const WCHAR* query);
	bool			Fetch();
	int			GetRowCount();
	void			Unbind();

public:

	bool			BindParam(int paramIndex, int* value, SQLLEN* index)
	{
		return BindParam(paramIndex, SQL_C_LONG, SQL_INTEGER, sizeof(int), value, index);
	}
	bool			BindParam(int paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index)
	{
		return BindParam(paramIndex, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), value, index);
	}
	bool			BindParam(int paramIndex, const WCHAR* str, SQLLEN* index)
	{
		SQLULEN size = static_cast<SQLULEN>((::wcslen(str) + 1) * 2);
		*index = SQL_NTSL;

		if (size > 4000)
			return BindParam(paramIndex, SQL_C_WCHAR, SQL_WLONGVARCHAR, size, (SQLPOINTER)str, index);
		else
			return BindParam(paramIndex, SQL_C_WCHAR, SQL_WVARCHAR, size, (SQLPOINTER)str, index);
	}
	bool			BindParam(int paramIndex, float* value, SQLLEN* index)
	{
		return BindParam(paramIndex, SQL_C_FLOAT, SQL_REAL, 0, value, index);
	}

	bool			BindCol(int columnIndex, int* value, SQLLEN* index);
	bool			BindCol(int columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool			BindCol(int columnIndex, WCHAR* str, int size, SQLLEN* index);
	bool			BindCol(int columnIndex, float* value, SQLLEN* index);

private:
	//			인자를 넘겨줄때 사용
	bool			BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index);
	bool			BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void			HandleError(SQLRETURN ret);

private:
	SQLHDBC			_connection = SQL_NULL_HANDLE;
	SQLHSTMT		_statement = SQL_NULL_HANDLE;
};

