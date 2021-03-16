#pragma once
#include "IsysTable.h"
#include "IsysConn.h"
#include "Filter/IsysParameter.h"
#include <functional>

ISYS_SQL_NAMESPACE_BEGIN

enum class IsysBound : simba_uint32 {
	NONE = 0, 
	LEFT,
	RIGHT,
	BOTH
};

enum class SampleStrategy : simba_uint32 {
	BEFORE = 0,
	AFTER, 
	INTERPLOATE
};

class CIsysResult
{
public:
	CIsysResult(const simba_wstring& tableName, ISYS::SQL::CIsysConn* isysConn, ILogger* log);
	~CIsysResult();

	simba_wstring GetColStr(simba_uint16 columnNum);
	ILogger* GetLog();

	bool NextRow();
	bool IsOver();
	simba_uint32 GetRowNum();

	bool Read();

	void Reset();

	SIsysPara* GetPara();
private:
	using GetColStrFunc = std::function<simba_wstring(simba_uint16)>;
	simba_wstring GetTagColStr(simba_uint16 columnNum);
	simba_wstring GetRtdColStr(simba_uint16 columnNum);
	simba_wstring GetHisColStr(simba_uint16 columnNum);
	simba_wstring GetSampleColStr(simba_uint16 columnNum);
	
	using ReadDataFunc = std::function<bool()>;
	bool ReadLike();
	bool ReadDataFromIsys();
	bool ReadTagDataFromIsys();
	bool ReadRtdDataFromIsys();
	bool ReadHisDataFromIsys();
	bool ReadSampleDataFromIsys();
	
	bool SavePeriodHisData(::HTAG* tagIds, std::size_t len);
	bool SaveStampHisData(::HTAG* tagIds, std::size_t len);
	bool SaveHisData(::TAGVALSTATE* tagValues, DWORD resultLen, const ::REALTAGDEF& tag);
	bool SaveData(::TAGVALSTATE* tagValues, ::HRESULT* results, DWORD resultLen);

	static simba_wstring FileTime2Str(const FILETIME& ft);
	static FILETIME Str2FileTime(const simba_wstring& timeStr);
	simba_wstring TagValue2Str(const ::VARIANT& tagValue, const ::VARTYPE dataType);
	::HTAG* ConvertTagIds();
	bool ConvertHisPara(DWORD& boundStrategy, FILETIME& timeBegin, FILETIME& timeEnd);
	bool ConvertBoundStrategy(DWORD& boundStrategy);
	bool ConvertTimePeriod(FILETIME& timeBegin, FILETIME& timeEnd);

private:
	std::map<simba_wstring, GetColStrFunc> m_tbName2Get;
	std::map<simba_wstring, ReadDataFunc> m_tbName2Read;

	simba_wstring m_tableName;
	SIsysResult<STagResult> m_result;
	simba_uint32 m_rowNow;
	CIsysConn* m_isysConn;
	SIsysPara* m_isysPara;

	ILogger* m_log;
};


inline ILogger* CIsysResult::GetLog()
{
	return m_log;
}

inline ::HTAG* CIsysResult::ConvertTagIds()
{
	auto size = m_result.tagInfos.size();
	::HTAG* tagIds = (::HTAG*)malloc(size * sizeof(::HTAG));
	for (std::size_t i = 0; i < size; i++)
	{
		tagIds[i] = m_result.tagInfos[i].baseDef.hTag;
	}
	return tagIds;
}

inline SIsysPara* CIsysResult::GetPara()
{
	return m_isysPara;
}

inline bool CIsysResult::IsOver()
{
	return m_rowNow < m_result.tagValues.values.size();
}

inline simba_uint32 CIsysResult::GetRowNum()
{
	return m_rowNow;
}

ISYS_SQL_NAMESPACE_END
