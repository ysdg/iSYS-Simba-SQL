/*************************************************
 * @Description: file content
 * @Author: yuanquan
 * @Email: yuanquan@supcon.com
 * @Date: 2021-02-19 16:47:25
 * @LastEditors: yuanquan
 * @LastEditTime: 2021-02-14 14:52:22
 * @copyright: Copyright (c) SUPCON
 *************************************************/
#pragma once
#include "IsysCommon.h"
#include "IsysConn.h"
#include "Quickstart.h"

ISYS_SQL_NAMESPACE_BEGIN

enum class RtdHisColIndex : simba_uint16 {
	TAG_NAME = 0,
	TIME_STAMP,
	QUALITY,
	ALARM_STATE,
	VALUE
};

enum class TagColIndex : simba_uint16 {
	NAME = 0,
	TRANSFER, 
	ITEM_NAME, 
	SCAN, 
	SCAN_SEC, 
	INTERFACE_FLAG, 
	DESCR, 
	PATH, 
	UNIT, 
	DATA_TYPE, 
	READONLY, 
	STORE, 
	POOL_LEN, 
	HIHI_LIMIT, 
	HI_LIMIT, 
	LOLO_LIMIT, 
	LO_LIMIT, 
	ALARM_FLAG, 
	ARCHIVE, 
	COMPRESS, 
	COMP_MAX, 
	COMP_MIN, 
	COMP_DEV, 
	BUFFER_COUNT, 
	PRECISION, 
};

enum class SampleColIndex : simba_uint16 {
	TAG_NAME = 0,
	TIME_STAMP,
	QUALITY,
	ALARM_STATE,
	VALUE,
	PERIOD
};

struct ColumnData
{
	simba_wstring name;
	simba_int16 type = SQL_INTEGER;
	bool isUnsigned = false;
	simba_uint32* size = nullptr;

	ColumnData(const simba_wstring& inName, const simba_int16& inType): name(inName), type(inType)
	{

	}
};

struct STagResult {
	std::vector<::REALTAGDEF> infos;
	std::vector<::TAGVALSTATE> values;
};

template<typename T>
struct SIsysResult
{
	std::vector<::REALTAGDEF> tagInfos;
	std::vector<::HRESULT> results;
	T tagValues;
};

class CIsysTable
{
public:
	using Columns = std::vector<ColumnData>;
	static const Columns& GetColumns(const simba_wstring& tbName);
	static const bool IsContainTbName(const simba_wstring& tbName);
	static const bool IsTagTb(simba_wstring tbName);

private:
	static std::map<simba_wstring, Columns> m_tbName2Cols;
	static Columns HisColumns;
	static Columns RtdColumns;
	static Columns TagColumns;
	static Columns SampleColumns;
};


inline const bool CIsysTable::IsTagTb(simba_wstring tbName)
{
	return tbName.ToLower() == simba_wstring("isys_tag");
}

ISYS_SQL_NAMESPACE_END
