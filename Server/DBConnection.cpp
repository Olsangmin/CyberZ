
#include "DBConnection.h"

/*
	DBConnection
*/


bool DBConnection::Connect(SQLHENV henv, const WCHAR* connectionString)
{
	if (::SQLAllocHandle(SQL_HANDLE_DBC, henv, &_connection) != SQL_SUCCESS)
		return false;

	WCHAR stringBuffer[MAX_PATH] = { 0 };
	::wcscpy_s(stringBuffer, connectionString);

	WCHAR resultString[MAX_PATH] = { 0 };
	SQLSMALLINT resultStringLen = 0;

	SQLRETURN ret = ::SQLDriverConnectW(
		_connection,
		NULL,
		reinterpret_cast<SQLWCHAR*>(stringBuffer),
		_countof(stringBuffer),
		reinterpret_cast<SQLWCHAR*>(resultString),
		_countof(resultString),
		&resultStringLen,
		SQL_DRIVER_NOPROMPT
	);

	if (::SQLAllocHandle(SQL_HANDLE_STMT, _connection, &_statement) != SQL_SUCCESS)
		return false;

	return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

void DBConnection::Clear()
{
	if (_connection != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, _connection);
		_connection = SQL_NULL_HANDLE;
	}

	if (_statement != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, _statement);
		_statement = SQL_NULL_HANDLE;
	}
}

bool DBConnection::Execute(const WCHAR* query)
{
	SQLRETURN ret = ::SQLExecDirectW(_statement, (SQLWCHAR*)query, SQL_NTSL);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		return true;

	HandleError(ret);
	return false;
}

bool DBConnection::Fetch()
{
	SQLRETURN ret = ::SQLFetch(_statement);

	switch (ret)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		return true;
	case SQL_NO_DATA:
		return false;
	case SQL_ERROR:
		HandleError(ret);
		return false;
	default:
		return true;
	}
}

int DBConnection::GetRowCount()
{
	SQLLEN count = 0;
	SQLRETURN ret = ::SQLRowCount(_statement, OUT & count);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		return static_cast<int>(count);

	return -1;
}

void DBConnection::Unbind()
{
	::SQLFreeStmt(_statement, SQL_UNBIND);
	::SQLFreeStmt(_statement, SQL_RESET_PARAMS);
	::SQLFreeStmt(_statement, SQL_CLOSE);
}

bool DBConnection::BindCol(int columnIndex, int* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_LONG, sizeof(int), value, index);
}

bool DBConnection::BindCol(int columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_TYPE_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), value, index);
}

bool DBConnection::BindCol(int columnIndex, WCHAR* str, int size, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_WCHAR, size, str, index);
}

bool DBConnection::BindCol(int columnIndex, float* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_FLOAT, sizeof(float), value, index);
}

bool DBConnection::BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index)
{
	SQLRETURN ret = ::SQLBindParameter(_statement, paramIndex, SQL_PARAM_INPUT, cType, sqlType, len, 0, ptr, 0, index);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

bool DBConnection::BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index)
{
	SQLRETURN ret = ::SQLBindCol(_statement, columnIndex, cType, value, len, index);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

void DBConnection::HandleError(SQLRETURN ret)
{
	if (ret == SQL_SUCCESS)
		return;

	SQLSMALLINT index = 1;
	SQLWCHAR sqlState[MAX_PATH] = { 0 };
	SQLINTEGER nativeErr = 0;
	SQLWCHAR errMsg[MAX_PATH] = { 0 };
	SQLSMALLINT msgLen = 0;
	SQLRETURN errorRet = 0;

	while (true)
	{
		errorRet = ::SQLGetDiagRecW(
			SQL_HANDLE_STMT,
			_statement,
			index,
			sqlState,
			&nativeErr,
			errMsg,
			_countof(errMsg),
			&msgLen
		);

		if (errorRet == SQL_NO_DATA)
			break;

		if (errorRet != SQL_SUCCESS && errorRet != SQL_SUCCESS_WITH_INFO)
			break;

		// TODO : Log
		std::wcout.imbue(std::locale("kor"));
		std::wcout << errMsg << std::endl;

		index++;
	}
}
