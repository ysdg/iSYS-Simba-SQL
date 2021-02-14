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

class CIsysResult
{
public:
	CIsysResult(const simba_wstring& tableName, ISYS::SQL::CIsysConn* isysConn);
	~CIsysResult();

	simba_wstring GetColStr(simba_uint16 columnNum);

	bool NextRow();
	bool IsOver();
	simba_uint32 GetRowNum();

	bool Read();

	void Reset();

	void SetPara(SIsysPara* para);
private:
	using GetColStrFunc = std::function<simba_wstring(simba_uint16)>;
	simba_wstring GetTagColStr(simba_uint16 columnNum);
	simba_wstring GetRtdColStr(simba_uint16 columnNum);
	simba_wstring GetHisColStr(simba_uint16 columnNum);
	
	using ReadDataFunc = std::function<bool()>;
	bool ReadDataFromIsys();
	bool ReadTagDataFromIsys();
	bool ReadRtdDataFromIsys();
	bool ReadHisDataFromIsys();

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
};


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

inline void CIsysResult::SetPara(SIsysPara* para)
{
	m_isysPara = para;
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
