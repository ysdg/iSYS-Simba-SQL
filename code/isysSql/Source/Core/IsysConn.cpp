#include "IsysConn.h"
#include "Quickstart.h"

using namespace ISYS::SQL;
using namespace Simba::Quickstart;

CIsysConn::CIsysConn(ILogger* log)
	: m_log(log)
{

}

bool CIsysConn::Connect(bool throwErr)
{
	m_conn = ::Connect(addr.GetAsPlatformWString().c_str(), port);
	if (!m_conn)
	{
		ErrorMsg(throwErr);
		return false;
	}

	auto result = ::LogIn(
		m_conn, 
		user.GetAsPlatformWString().c_str(), 
		password.GetAsPlatformWString().c_str()
	);
	if (!ISYS_SUCCESS(result))
	{
		if (!throwErr) return false;
		HRESULT hr = GetLastException();
		QSTHROWGEN2("IsysConnectLoginError", NumberConverter::ConvertUInt32ToWString(hr), Dump());
	}
	return true;
}

void CIsysConn::DisConnect()
{
	bool isConnected = false;
	::IsConnected(m_conn, isConnected);
	if (isConnected)
	{
		::LogOut(m_conn);
		::DisConnect(m_conn);
	}
}

void CIsysConn::ErrorMsg(bool throwErr)
{
	HRESULT hr = ::GetLastException();
	if (throwErr)
	{
		QSTHROWGEN2("IsyscConnectError", NumberConverter::ConvertUInt32ToWString(hr), Dump());
	}
	ISYS_LOG_ERROR(
		m_log,
		Dump() + simba_wstring("Connet to isys error") + NumberConverter::ConvertInt32ToWString(hr)
	);
}

::HANDLE CIsysConn::GetConn(bool throwErr)
{
	bool isConnected = false;
	::IsConnected(m_conn, isConnected);
	if (isConnected)
	{
		return m_conn;
	}
	else
	{
		ErrorMsg(throwErr);
	}
}
