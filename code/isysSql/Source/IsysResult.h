#pragma once
#include "IsysTable.h"
#include "IsysConn.h"
#include <functional>

ISYS_SQL_NAMESPACE_BEGIN

class CIsysResult
{
public:
	CIsysResult(const simba_wstring& tableName, ISYS::SQL::CIsysConn* isysConn);
	~CIsysResult();

	simba_wstring GetColStr(simba_uint16 columnNum);

	bool NextRow();

	template<typename TagsT>
	bool UpdateTags(TagsT tags);

	void Reset();
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

	simba_wstring FileTime2Str(const FILETIME& ft);
	simba_wstring TagValue2Str(const ::VARIANT& tagValue, const ::VARTYPE dataType);
	::HTAG* ConvertTagIds();

private:
	std::map<simba_wstring, GetColStrFunc> m_tbName2Get;
	std::map<simba_wstring, ReadDataFunc> m_tbName2Read;

	simba_wstring m_tableName;
	SIsysResult m_result;
	simba_uint32 m_rowNow;
	ISYS::SQL::CIsysConn* m_isysConn;
};

template<typename TagsT>
bool CIsysResult::UpdateTags(TagsT tags)
{
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

ISYS_SQL_NAMESPACE_END
