/*************************************************
 * @Description: file content
 * @Author: yuanquan
 * @Email: yuanquan@supcon.com
 * @Date: 2021-02-19 16:47:57
 * @LastEditors: yuanquan
 * @LastEditTime: 2021-02-19 17:32:30
 * @copyright: Copyright (c) SUPCON
 *************************************************/
#include "IsysTable.h"

using namespace ISYS::SQL;

CIsysTable::Columns CIsysTable::HisColumns{
	ColumnData{"tag_name", 		SQL_WVARCHAR},
	ColumnData{"time_stamp", 	SQL_TIMESTAMP},
	ColumnData{"quality", 		SQL_INTEGER},
	ColumnData{"alarm_state", 	SQL_INTEGER},
	ColumnData{"value", 		SQL_DOUBLE},
};

CIsysTable::Columns CIsysTable::RtdColumns = CIsysTable::HisColumns;

CIsysTable::Columns CIsysTable::TagColumns{
	ColumnData{"name", 					SQL_WVARCHAR},
	ColumnData{"transfer", 				SQL_WVARCHAR},
	ColumnData{"item_name", 			SQL_WVARCHAR},
	ColumnData{"scan", 					SQL_NUMERIC},
	ColumnData{"scan_sec", 				SQL_NUMERIC},
	ColumnData{"interface_flag", 		SQL_NUMERIC},
	ColumnData{"descr", 				SQL_WVARCHAR},
	ColumnData{"path", 					SQL_WVARCHAR},
	ColumnData{"unit", 					SQL_WVARCHAR},
	ColumnData{"data_type", 			SQL_NUMERIC},
	ColumnData{"readonly", 				SQL_NUMERIC},
	ColumnData{"store", 				SQL_NUMERIC},
	ColumnData{"pool_len", 				SQL_NUMERIC},
	ColumnData{"hihi_limit", 			SQL_DOUBLE},
	ColumnData{"hi_limit", 				SQL_DOUBLE},
	ColumnData{"lolo_limit", 			SQL_DOUBLE},
	ColumnData{"lo_limit", 				SQL_DOUBLE},
	ColumnData{"alarm_flag", 			SQL_NUMERIC},
	ColumnData{"archive", 				SQL_NUMERIC},
	ColumnData{"compress", 				SQL_NUMERIC},
	ColumnData{"comp_max", 				SQL_NUMERIC},
	ColumnData{"comp_min", 				SQL_NUMERIC},
	ColumnData{"comp_dev", 				SQL_DOUBLE},
	ColumnData{"buffer_count", 			SQL_NUMERIC},
	ColumnData{"precision", 			SQL_NUMERIC},
};

std::map<simba_wstring, CIsysTable::Columns> CIsysTable::m_tbName2Cols{
	{simba_wstring("isys_tag"), CIsysTable::TagColumns},
	{simba_wstring("isys_rtd"), CIsysTable::RtdColumns},
	{simba_wstring("isys_his"), CIsysTable::HisColumns},
};

const bool CIsysTable::IsContainTbName(const simba_wstring& tbName)
{
	auto pair = m_tbName2Cols.find(tbName);
	if (pair != m_tbName2Cols.end())
	{
		return true;
	}
	return false;
}

const CIsysTable::Columns& CIsysTable::GetColumns(const simba_wstring& tbName)
{
	if (IsContainTbName(tbName))
	{
		return m_tbName2Cols[tbName];
	}

	return Columns();
}