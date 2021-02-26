#pragma  once 
#ifndef ISYS_CONN_H_
#define ISYS_CONN_H_

#include "IsysCommon.h"

#include "DataStructure.h"
#include "DllFunDef.h"
#include "Simba_Windows.h"
#include "simba_wstring.h"
#include "NumberConverter.h"
#include "ILogger.h"

#define ISYS_SUCCESS(x)		((x)==S_OK)

ISYS_SQL_NAMESPACE_BEGIN

using Simba::Support::ILogger;

class CIsysConn
{
public:
	CIsysConn(ILogger* log);

	bool Connect(bool throwErr=true);
	void DisConnect();

	simba_wstring addr;
	simba_uint32 port;
	simba_wstring user;
	simba_wstring password;

	simba_wstring Dump();
	::HANDLE GetConn(bool throwErr=true);

private:
	void ErrorMsg(bool throwErr = true);

private:
	::HANDLE m_conn = NULL;
	ILogger* m_log;
};


inline simba_wstring CIsysConn::Dump()
{
	return
		"connnect"
		", addr: " + addr +
		", port: " + Simba::Support::NumberConverter::ConvertUInt32ToWString(port) +
		", user: " + user +
		", password: " + password;
};

ISYS_SQL_NAMESPACE_END

#endif //ISYS_CONN_H_