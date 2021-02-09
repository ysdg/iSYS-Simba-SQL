#pragma  once 
#ifndef ISYS_CONN_H_
#define ISYS_CONN_H_

#include "DataStructure.h"
#include "DllFunDef.h"
#include "Simba_Windows.h"
#include "simba_wstring.h"

#define ISYS_SQL_NAMESPACE_BEGIN namespace ISYS{namespace SQL{
#define ISYS_SQL_NAMESPACE_END }}

ISYS_SQL_NAMESPACE_BEGIN

class CIsysConn
{
public:
	HANDLE conn = NULL;
	simba_wstring addr;
	simba_uint32 port;
	simba_wstring user;
	simba_wstring password;

	simba_wstring Dump();
};

inline simba_wstring Dump()
{

}

ISYS_SQL_NAMESPACE_END

#endif //ISYS_CONN_H_