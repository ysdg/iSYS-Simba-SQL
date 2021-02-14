#include "IsysResult.h"
#include "Quickstart.h"
#include "Filter/IsysParameter.h"
#include "TDWTimestamp.h"
#include "IsysTable.h"

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
	if (m_rowNow < m_result.tagValues.values.size() - 1)
	{
		++m_rowNow;
		return true;
	}
	return false;
}


simba_wstring CIsysResult::GetTagColStr(simba_uint16 columnNum)
{
	simba_wstring valueStr;
	::TAGVALSTATE tagValue = m_result.tagValues.values[m_rowNow];
	::REALTAGDEF tagInfo = m_result.tagValues.infos[m_rowNow];
	switch (static_cast<TagColIndex>(columnNum))
	{
	case TagColIndex::NAME:				valueStr= simba_wstring(tagInfo.baseDef.szTagName); 									break;
	case TagColIndex::TRANSFER:			valueStr= simba_wstring(tagInfo.szTransferName); 										break;
	case TagColIndex::ITEM_NAME:		valueStr= simba_wstring(tagInfo.szItemID); 												break;
	case TagColIndex::SCAN:				valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.bScan); 						break;
	case TagColIndex::SCAN_SEC:			valueStr= NumberConverter::ConvertUInt64ToWString(tagInfo.dwScanSec); 					break;
	case TagColIndex::INTERFACE_FLAG:	valueStr= NumberConverter::ConvertUInt64ToWString(tagInfo.dwInterfaceFlag); 			break;
	case TagColIndex::DESCR:			valueStr= simba_wstring(tagInfo.baseDef.szTagDescr); 									break;
	case TagColIndex::PATH:				valueStr= simba_wstring(tagInfo.baseDef.szPath); 										break;
	case TagColIndex::UNIT:				valueStr= simba_wstring(tagInfo.baseDef.szUnit); 										break;
	case TagColIndex::DATA_TYPE:		valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.baseDef.vtDataType); 			break;
	case TagColIndex::READONLY:			valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.baseDef.bReadOnly); 			break;
	case TagColIndex::STORE:			valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.baseDef.bStore); 				break;
	case TagColIndex::POOL_LEN:			valueStr= NumberConverter::ConvertUInt64ToWString(tagInfo.baseDef.dwPoolLenth); 		break;
	case TagColIndex::HIHI_LIMIT:		valueStr= NumberConverter::ConvertDouble64ToWString(tagInfo.baseDef.dbHihi_limit); 		break;
	case TagColIndex::HI_LIMIT:			valueStr= NumberConverter::ConvertDouble64ToWString(tagInfo.baseDef.dbHi_limit); 		break;
	case TagColIndex::LOLO_LIMIT:		valueStr= NumberConverter::ConvertDouble64ToWString(tagInfo.baseDef.dbLolo_limit); 		break;
	case TagColIndex::LO_LIMIT:			valueStr= NumberConverter::ConvertDouble64ToWString(tagInfo.baseDef.dbLo_limit); 		break;
	case TagColIndex::ALARM_FLAG:		valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.baseDef.bAlarmFlag); 			break;
	case TagColIndex::ARCHIVE:			valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.baseDef.bArchive); 			break;
	case TagColIndex::COMPRESS:			valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.baseDef.bCompress); 			break;
	case TagColIndex::COMP_MAX:			valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.baseDef.wCompMax); 			break;
	case TagColIndex::COMP_MIN:			valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.baseDef.wCompMin); 			break;
	case TagColIndex::COMP_DEV:			valueStr= NumberConverter::ConvertDouble64ToWString(tagInfo.baseDef.dbCompDev); 		break;
	case TagColIndex::BUFFER_COUNT:		valueStr= NumberConverter::ConvertUInt64ToWString(tagInfo.baseDef.dwBufferCount); 		break;
	case TagColIndex::PRECISION:		valueStr= NumberConverter::ConvertUInt32ToWString(tagInfo.baseDef.wFloatPrecision); 	break;
	default:
		assert(false);
		break;
	}
	return valueStr;
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

FILETIME CIsysResult::Str2FileTime(const simba_wstring& timeStr)
{
	using Simba::Support::TDWTimestamp;
	TDWTimestamp timeStamp(timeStr);
	SYSTEMTIME systemTime;
	systemTime.wYear = timeStamp.Year;
	systemTime.wMonth = timeStamp.Month;
	systemTime.wDayOfWeek = 0;
	systemTime.wDay = timeStamp.Day;
	systemTime.wHour = timeStamp.Hour;
	systemTime.wMinute = timeStamp.Minute;
	systemTime.wSecond = timeStamp.Second;
	systemTime.wMilliseconds = 0;

	FILETIME fileTime{ 0 };
	SystemTimeToFileTime(&systemTime, &fileTime);
	return fileTime;
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
	::TAGVALSTATE tagValue = m_result.tagValues.values[m_rowNow];
	::REALTAGDEF tagInfo = m_result.tagValues.infos[m_rowNow];
	switch (static_cast<RtdHisColIndex>(columnNum))
	{
	case RtdHisColIndex::TAG_NAME:		valueStr = simba_wstring(tagInfo.baseDef.szTagName); break;
	case RtdHisColIndex::TIME_STAMP:	valueStr = FileTime2Str(tagValue.ftTimeStamp); break;
	case RtdHisColIndex::QUALITY:		valueStr = NumberConverter::ConvertUInt64ToWString(tagValue.wQuality);  break;
	case RtdHisColIndex::ALARM_STATE:	valueStr = NumberConverter::ConvertUInt64ToWString(tagValue.wAlarmState); break;
	case RtdHisColIndex::VALUE:			valueStr = TagValue2Str(tagValue.vEng_value, tagInfo.baseDef.vtDataType); break;
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
	auto size = m_result.tagInfos.size();
	m_result.tagValues.infos.reserve(size);
	m_result.tagValues.values.reserve(size);
	for (std::size_t i = 0; i < size; i++)
	{
		m_result.tagValues.infos.push_back(m_result.tagInfos[i]);
		m_result.tagValues.values.push_back({});
	}
	return true;
}

bool CIsysResult::ReadRtdDataFromIsys()
{
	auto size = m_result.tagInfos.size();
	::HTAG* tagIds = ConvertTagIds();
	
	::HRESULT* results = nullptr;
	::TAGVALSTATE* tagValues = nullptr;

	auto result = ::ReadTagsValue(m_isysConn->conn, size, tagIds, &results, &tagValues);
	if (!ISYS_SUCCESS(result))
	{
		QSTHROW2(QS_DATAENGINE_STATE, L"ReadTagValueError", m_tableName, NumberConverter::ConvertInt32ToWString(result));
	}

	m_result.tagValues.infos.reserve(size);
	m_result.tagValues.values.reserve(size);
	for (std::size_t i = 0; i < size; i++)
	{
		m_result.results.push_back(results[i]);
		m_result.tagValues.infos.push_back(m_result.tagInfos[i]);
		m_result.tagValues.values.push_back(tagValues[i]);
	}

	realloc(tagIds, size * sizeof(::HTAG));
	::CoTaskMemFree(results);
	::CoTaskMemFree(tagValues);
	return true;
}

bool CIsysResult::ConvertBoundStrategy(DWORD& boundStrategy)
{
	if (m_isysPara->timeLeft.isContain && m_isysPara->timeRight.isContain)
	{
		boundStrategy = static_cast<DWORD>(IsysBound::BOTH);
	}
	else if (m_isysPara->timeLeft.isContain)
	{
		boundStrategy = static_cast<DWORD>(IsysBound::LEFT);
	}
	else if (m_isysPara->timeRight.isContain)
	{
		boundStrategy = static_cast<DWORD>(IsysBound::RIGHT);
	}
	else
	{
		boundStrategy = static_cast<DWORD>(IsysBound::NONE);
	}

	return true;
}

bool CIsysResult::ConvertTimePeriod(FILETIME& timeBegin, FILETIME& timeEnd)
{
	timeBegin = Str2FileTime(m_isysPara->timeLeft.value);
	timeEnd = Str2FileTime(m_isysPara->timeRight.value);
	return true;
}

bool CIsysResult::ConvertHisPara(DWORD& boundStrategy, FILETIME& timeBegin, FILETIME& timeEnd)
{
	ConvertBoundStrategy(boundStrategy);
	ConvertTimePeriod(timeBegin, timeEnd);
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
	ConvertHisPara(boundStrategy, timeBegin, timeEnd);
	for (std::size_t i = 0; i < size; i++)
	{
		auto result = ::ReadTagDiskHisInTime(m_isysConn->conn, tagIds[i], timeBegin, timeEnd, boundStrategy, resultLen, &tagValues);
		if (!ISYS_SUCCESS(result))
		{
			QSTHROW2(QS_DATAENGINE_STATE, L"ReadTagValueError", m_tableName, NumberConverter::ConvertInt32ToWString(result));
		}
		for (std::size_t j = 0; j < resultLen; j++)
		{
			m_result.tagValues.infos.push_back(m_result.tagInfos[i]);
			m_result.tagValues.values.push_back(tagValues[j]);
		}
		::CoTaskMemFree(tagValues);
	}

	realloc(tagIds, size * sizeof(::HTAG));

	return true;
}

bool CIsysResult::Read()
{
	auto tags = m_isysPara->tagNames;
	auto size = tags.size();
	m_result.tagInfos.reserve(size);

	::HTAG* tagIds = (::HTAG*)malloc(size * sizeof(::HTAG));

	for (std::size_t i = 0; i < size; i++)
	{
		auto result = ::GetTagIDByName(m_isysConn->conn, tags[i].GetAsPlatformWString().c_str(), tagIds[i]);
	}

	::HRESULT* results = nullptr;
	::REALTAGDEF* tagInfos = nullptr;
	auto result = ::GetRealTagInfo(m_isysConn->conn, size, tagIds, &results, &tagInfos);

	for (std::size_t i = 0; i < size; i++)
	{
		m_result.tagInfos.push_back(tagInfos[i]);
	}

	realloc(tagIds, size * sizeof(::HTAG));
	::CoTaskMemFree(results);
	::CoTaskMemFree(tagInfos);

	return ReadDataFromIsys();
};
