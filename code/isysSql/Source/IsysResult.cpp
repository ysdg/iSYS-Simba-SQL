#include "IsysResult.h"
#include "Quickstart.h"
#include "Filter/IsysParameter.h"

using namespace Simba::Quickstart;
using namespace ISYS::SQL;

CIsysResult::CIsysResult(const simba_wstring& tableName, ISYS::SQL::CIsysConn* isysConn)
	: m_tableName(tableName)
	, m_isysConn(isysConn)
	, m_rowNow(0)
{
	m_tbName2Get[simba_wstring("isys_tag")] = std::bind(&CIsysResult::GetTagColStr, this, std::placeholders::_1);
	m_tbName2Get[simba_wstring("isys_rtd")] = std::bind(&CIsysResult::GetRtdColStr, this, std::placeholders::_1);
	m_tbName2Get[simba_wstring("isys_his")] = std::bind(&CIsysResult::GetHisColStr, this, std::placeholders::_1);

	m_tbName2Read[simba_wstring("isys_tag")] = std::bind(&CIsysResult::ReadTagDataFromIsys, this);
	m_tbName2Read[simba_wstring("isys_rtd")] = std::bind(&CIsysResult::ReadRtdDataFromIsys, this);
	m_tbName2Read[simba_wstring("isys_his")] = std::bind(&CIsysResult::ReadHisDataFromIsys, this);
}


void CIsysResult::Reset()
{
	m_rowNow = 0;
	m_result.Release();
}

CIsysResult::~CIsysResult()
{
	Reset();
}

simba_wstring CIsysResult::GetColStr(simba_uint16 columnNum)
{
	const auto pair = m_tbName2Get.find(m_tableName);
	if (pair == m_tbName2Get.end())
	{
		return simba_wstring();
	}
	return pair->second(columnNum);
}

bool CIsysResult::NextRow()
{
	if (m_rowNow < m_result.tagValues.size() - 1)
	{
		++m_rowNow;
		return true;
	}
	return false;
}


simba_wstring CIsysResult::GetTagColStr(simba_uint16 columnNum)
{
	return simba_wstring();
}

simba_wstring CIsysResult::FileTime2Str(const FILETIME& ft)
{
	SYSTEMTIME systemTime;
	FileTimeToSystemTime(&ft, &systemTime);
	WCHAR szDate[128];
	WCHAR szTime[128];
	GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &systemTime, TEXT("yyyy-MM-dd"), szDate, 12);
	GetTimeFormat(LOCALE_SYSTEM_DEFAULT, NULL, &systemTime, TEXT("HH:mm:ss"), szTime, 10);
	return simba_wstring(szDate) + simba_wstring(" ") + simba_wstring(szTime);
}

simba_wstring CIsysResult::TagValue2Str(const ::VARIANT& tagValue, const ::VARTYPE dataType)
{
	simba_wstring valueStr;
	switch (dataType)
	{
	case VT_I2:		valueStr = NumberConverter::ConvertInt16ToWString(tagValue.intVal);		break;
	case VT_UI2:	valueStr = NumberConverter::ConvertUInt16ToWString(tagValue.uintVal);	break;
	case VT_I4:		valueStr = NumberConverter::ConvertInt32ToWString(tagValue.llVal);		break;
	case VT_UI4:	valueStr = NumberConverter::ConvertUInt32ToWString(tagValue.ullVal);	break;
	case VT_I8:		valueStr = NumberConverter::ConvertInt64ToWString(tagValue.llVal);		break;
	case VT_UI8:	valueStr = NumberConverter::ConvertUInt64ToWString(tagValue.ullVal);	break;
	case VT_BOOL:	valueStr = NumberConverter::ConvertDouble64ToWString(tagValue.dblVal);	break;
	case VT_R4:		valueStr = NumberConverter::ConvertDouble64ToWString(tagValue.fltVal);	break;
	case VT_R8:		valueStr = NumberConverter::ConvertDouble64ToWString(tagValue.dblVal);	break;
	default:
		assert(false);
		break;
	}
	return valueStr;
}

simba_wstring CIsysResult::GetRtdColStr(simba_uint16 columnNum)
{
	simba_wstring valueStr;
	TAGVALSTATE tagValue = m_result.tagValues[m_rowNow];
	switch (columnNum)
	{
	case 0: valueStr = simba_wstring(m_result.tagInfos[m_rowNow].baseDef.szTagName); break;
	case 1: valueStr = FileTime2Str(tagValue.ftTimeStamp); break;
	case 2: valueStr = NumberConverter::ConvertUInt64ToWString(tagValue.wQuality);  break;
	case 3: valueStr = NumberConverter::ConvertUInt64ToWString(tagValue.wAlarmState); break;
	case 4: valueStr = TagValue2Str(tagValue.vEng_value, m_result.tagInfos[m_rowNow].baseDef.vtDataType); break;
	default:
		assert(false);
		break;
	}
	return valueStr;
}

simba_wstring CIsysResult::GetHisColStr(simba_uint16 columnNum)
{
	return GetRtdColStr(columnNum);
}


bool CIsysResult::ReadDataFromIsys()
{
	const auto pair = m_tbName2Read.find(m_tableName);
	if (pair == m_tbName2Read.end())
	{
		return false;
	}
	return pair->second();
}

bool CIsysResult::ReadTagDataFromIsys()
{
	return false;
}

bool CIsysResult::ReadRtdDataFromIsys()
{
	auto size = m_result.tagInfos.size();
	::HTAG* tagIds = ConvertTagIds();
	
	::HRESULT* results = nullptr;
	::TAGVALSTATE* tagValues = nullptr;

	auto result = ::ReadTagsValue(m_isysConn->conn, size, tagIds, &results, &tagValues);
	for (std::size_t i = 0; i < size; i++)
	{
		m_result.results.push_back(results[i]);
		m_result.tagValues.push_back(tagValues[i]);
	}

	if (!ISYS_SUCCESS(result))
	{
		QSTHROW2(QS_DATAENGINE_STATE, L"ReadTagValueError", m_tableName, NumberConverter::ConvertInt32ToWString(result));
	}

	realloc(tagIds, size * sizeof(::HTAG));
	::CoTaskMemFree(results);
	::CoTaskMemFree(tagValues);
	return true;
}

bool CIsysResult::ReadHisDataFromIsys()
{
	auto size = m_result.tagInfos.size();
	::HTAG* tagIds = ConvertTagIds();

	::TAGVALSTATE* tagValues = nullptr;

	FILETIME timeBegin;
	FILETIME timeEnd;
	DWORD boundStrategy;
	DWORD resultLen = 0;
	for (std::size_t i = 0; i < size; i++)
	{
		auto result = ::ReadTagDiskHisInTime(m_isysConn->conn, tagIds[i], timeBegin, timeEnd, boundStrategy, resultLen, &tagValues);
		if (!ISYS_SUCCESS(result))
		{
			QSTHROW2(QS_DATAENGINE_STATE, L"ReadTagValueError", m_tableName, NumberConverter::ConvertInt32ToWString(result));
		}
		for (std::size_t j = 0; j < resultLen; j++)
		{
			m_result.tagValues.push_back(tagValues[j]);
		}
		::CoTaskMemFree(tagValues);
	}

	realloc(tagIds, size * sizeof(::HTAG));

	return true;
}
