#include "IsysResult.h"
#include "Quickstart.h"
#include "Filter/IsysParameter.h"
#include "TDWTimestamp.h"
#include "IsysTable.h"

using namespace Simba::Quickstart;
using namespace ISYS::SQL;

CIsysResult::CIsysResult(const simba_wstring& tableName, ISYS::SQL::CIsysConn* isysConn, ILogger* log)
	: m_tableName(tableName)
	, m_isysConn(isysConn)
	, m_rowNow(0)
	, m_log(log)
{
	m_isysPara = new SIsysPara();

	m_tbName2Get[simba_wstring("isys_tag")] = std::bind(&CIsysResult::GetTagColStr, this, std::placeholders::_1);
	m_tbName2Get[simba_wstring("isys_rtd")] = std::bind(&CIsysResult::GetRtdColStr, this, std::placeholders::_1);
	m_tbName2Get[simba_wstring("isys_his")] = std::bind(&CIsysResult::GetHisColStr, this, std::placeholders::_1);
	m_tbName2Get[simba_wstring("isys_sample")] = std::bind(&CIsysResult::GetSampleColStr, this, std::placeholders::_1);

	m_tbName2Read[simba_wstring("isys_tag")] = std::bind(&CIsysResult::ReadTagDataFromIsys, this);
	m_tbName2Read[simba_wstring("isys_rtd")] = std::bind(&CIsysResult::ReadRtdDataFromIsys, this);
	m_tbName2Read[simba_wstring("isys_his")] = std::bind(&CIsysResult::ReadHisDataFromIsys, this);
	m_tbName2Read[simba_wstring("isys_sample")] = std::bind(&CIsysResult::ReadSampleDataFromIsys, this);
}


void CIsysResult::Reset()
{
	m_rowNow = 0;
}

CIsysResult::~CIsysResult()
{
	Reset();
	delete m_isysPara;
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
	case VT_R4:		valueStr = NumberConverter::ConvertDouble64ToWString(tagValue.dblVal);	break;
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

simba_wstring CIsysResult::GetSampleColStr(simba_uint16 columnNum)
{
	if (static_cast<simba_uint16>(SampleColIndex::PERIOD) == columnNum)
	{
		return NumberConverter::ConvertUInt32ToWString(m_isysPara->period);
	}
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

	auto result = ::ReadTagsValue(m_isysConn->GetConn(), size, tagIds, &results, &tagValues);
	if (!ISYS_SUCCESS(result))
	{
		QSTHROW2(QS_DATAENGINE_STATE, L"ReadTagValueError", m_tableName, NumberConverter::ConvertInt32ToWString(result));
	}

	SaveData(tagValues, results, size);
	::CoTaskMemFree(tagValues);
	::CoTaskMemFree(results);

	realloc(tagIds, size * sizeof(::HTAG));
	return true;
}

bool CIsysResult::ReadSampleDataFromIsys()
{
	auto size = m_result.tagInfos.size();
	::HTAG* tagIds = ConvertTagIds();
	
	FILETIME timeBegin;
	FILETIME timeEnd;
	DWORD boundStrategy;
	DWORD resultLen = 0;
	ConvertHisPara(boundStrategy, timeBegin, timeEnd);

	::TAGVALSTATE* tagValues = nullptr;
	for (std::size_t i=0; i < size; i++)
	{
		auto result = ::ReadTagHisSampling(
			m_isysConn->GetConn(),
			tagIds[i],
			timeBegin,
			timeEnd,
			m_isysPara->period,
			static_cast<DWORD>(SampleStrategy::INTERPLOATE),
			0,
			resultLen,
			&tagValues
		);
		SaveHisData(tagValues, resultLen, m_result.tagInfos[i]);
		if (!ISYS_SUCCESS(result))
		{
			ISYS_LOG_WARNING(m_log, simba_wstring("Read his data error: [" + NumberConverter::ConvertUInt32ToWString(result) + "], tag name: ") + simba_wstring(m_result.tagInfos[i].baseDef.szTagName) + simba_wstring(", time in: ") + m_isysPara->timeLeft.value + simba_wstring(", ") + m_isysPara->timeRight.value);
		}
		::CoTaskMemFree(tagValues);
	}

	realloc(tagIds, size * sizeof(::HTAG));
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

bool CIsysResult::SaveHisData(::TAGVALSTATE* tagValues, DWORD resultLen, const ::REALTAGDEF& tag)
{
	m_result.tagValues.infos.reserve(m_result.tagValues.infos.size() + resultLen);
	m_result.tagValues.values.reserve(m_result.tagValues.infos.size() + resultLen);
	for (std::size_t j = 0; j < resultLen; j++)
	{
		m_result.tagValues.infos.push_back(tag);
		m_result.tagValues.values.push_back(tagValues[j]);
	}

	return true;
}

bool CIsysResult::SaveData(::TAGVALSTATE* tagValues, ::HRESULT* results, DWORD resultLen)
{
	bool result = true;
	m_result.tagValues.infos.reserve(m_result.tagValues.infos.size() + resultLen);
	m_result.tagValues.values.reserve(m_result.tagValues.infos.size() + resultLen);
	for (std::size_t i = 0; i < resultLen; i++)
	{
		if (ISYS_SUCCESS(results[i]))
		{
			m_result.tagValues.infos.push_back(m_result.tagInfos[i]);
			m_result.tagValues.values.push_back(tagValues[i]);
		}
		else
		{
			ISYS_LOG_WARNING(m_log, simba_wstring("Tag: ") + m_result.tagInfos[i].baseDef.szTagName + simba_wstring(" data error!"));
			result = false;
		}
	}
	return result;
}

bool CIsysResult::SavePeriodHisData(::HTAG* tagIds, std::size_t len)
{
	::TAGVALSTATE* tagValues = nullptr;
	::VARIANT tagFuncValue;

	FILETIME timeBegin;
	FILETIME timeEnd;
	DWORD boundStrategy;
	DWORD resultLen = 0;
	ConvertHisPara(boundStrategy, timeBegin, timeEnd);
	for (const auto& tag : m_result.tagInfos)
	{
		::HRESULT result = S_OK;
		switch (m_isysPara->func)
		{
		case SEAggrFunctionID::SE_FUNCT_AVG: 
			resultLen = 1;
			result = ::ReadAvgDiskHisInTime(m_isysConn->GetConn(), tag.baseDef.hTag, timeBegin, timeEnd, boundStrategy, &tagFuncValue);
			break;
		case SEAggrFunctionID::SE_FUNCT_MAX: 
			resultLen = 1;
			result = ::ReadMaxDiskHisInTime(m_isysConn->GetConn(), tag.baseDef.hTag, timeBegin, timeEnd, boundStrategy, &tagFuncValue);
			break;
		case SEAggrFunctionID::SE_FUNCT_MIN: 
			resultLen = 1;
			result = ::ReadMinDiskHisInTime(m_isysConn->GetConn(), tag.baseDef.hTag, timeBegin, timeEnd, boundStrategy, &tagFuncValue);
			break;
		case SEAggrFunctionID::SE_FUNCT_SUM: 
			resultLen = 1;
			result = ::ReadSumDiskHisInTime(m_isysConn->GetConn(), tag.baseDef.hTag, timeBegin, timeEnd, boundStrategy, &tagFuncValue);
			break;
		default:
			result = ::ReadTagHisInTime(m_isysConn->GetConn(), tag.baseDef.hTag, timeBegin, timeEnd, boundStrategy, resultLen, &tagValues);
			break;
		}
		
		if (!ISYS_SUCCESS(result))
		{
			ISYS_LOG_WARNING(m_log, simba_wstring("Read his data error: [" + NumberConverter::ConvertUInt32ToWString(result) + "], tag name: ") + simba_wstring(tag.baseDef.szTagName) + simba_wstring(", time in: ") + m_isysPara->timeLeft.value + simba_wstring(", ") + m_isysPara->timeRight.value);
		}
		else
		{
			if (m_isysPara->func == SEAggrFunctionID::SE_FUNCT_AVG ||
				m_isysPara->func == SEAggrFunctionID::SE_FUNCT_MAX ||
				m_isysPara->func == SEAggrFunctionID::SE_FUNCT_MIN ||
				m_isysPara->func == SEAggrFunctionID::SE_FUNCT_SUM)
			{
				::TAGVALSTATE value;
				value.vEng_value = tagFuncValue;
				value.hTag = tag.baseDef.hTag;
				SaveHisData(&value, resultLen, tag);
			}
			else
			{
				SaveHisData(tagValues, resultLen, tag);
				::CoTaskMemFree(tagValues);
			}
		}
	}
	return true;
}

bool CIsysResult::SaveStampHisData(::HTAG* tagIds, std::size_t len)
{
	::TAGVALSTATE* tagValues = nullptr;
	for (const auto& timeStamp : m_isysPara->timeStamps)
	{
		::HRESULT* results = nullptr;
		auto result = ::ReadTagsHisAtTime(
			m_isysConn->GetConn(),
			len,
			tagIds,
			Str2FileTime(timeStamp),
			static_cast<DWORD>(SampleStrategy::INTERPLOATE),
			0,
			&results,
			&tagValues
		);
		if (!ISYS_SUCCESS(result))
		{
			ISYS_LOG_WARNING(m_log, simba_wstring("Read his data error, time: ") + timeStamp);
		}
		else
		{
			SaveData(tagValues, results, len);
			::CoTaskMemFree(tagValues);
			::CoTaskMemFree(results);
		}
	}
	return true;
}

bool CIsysResult::ReadHisDataFromIsys()
{
	auto size = m_result.tagInfos.size();
	::HTAG* tagIds = ConvertTagIds();
	
	SavePeriodHisData(tagIds, size);
	SaveStampHisData(tagIds, size);

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
		auto result = ::GetTagIDByName(m_isysConn->GetConn(), tags[i].GetAsPlatformWString().c_str(), tagIds[i]);
		if (!ISYS_SUCCESS(result))
		{
			ISYS_LOG_WARNING(
				m_log,
				simba_wstring("Tag: ") + tags[i] + simba_wstring(" is not existed")
			);
		}
	}

	::HRESULT* results = nullptr;
	::REALTAGDEF* tagInfos = nullptr;
	auto result = ::GetRealTagInfo(m_isysConn->GetConn(), size, tagIds, &results, &tagInfos);

	for (std::size_t i = 0; i < size; i++)
	{
		if (ISYS_SUCCESS(results[i]))
		{
			m_result.tagInfos.push_back(tagInfos[i]);
		}
		else
		{
			ISYS_LOG_DEBUG(
				m_log, 
				simba_wstring("Tag: ") + simba_wstring(tagInfos[i].baseDef.szTagName) + simba_wstring(" is not existed")
			);
		}
	}

	realloc(tagIds, size * sizeof(::HTAG));
	::CoTaskMemFree(results);
	::CoTaskMemFree(tagInfos);

	return ReadDataFromIsys();
};
