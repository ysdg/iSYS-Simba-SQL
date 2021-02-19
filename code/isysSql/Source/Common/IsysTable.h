/*************************************************
 * @Description: file content
 * @Author: yuanquan
 * @Email: yuanquan@supcon.com
 * @Date: 2021-02-19 16:47:25
 * @LastEditors: yuanquan
 * @LastEditTime: 2021-02-19 17:23:48
 * @copyright: Copyright (c) SUPCON
 *************************************************/
#include "IsysCommon.h"
#include "Quickstart.h"

ISYS_SQL_NAMESPACE_BEGIN

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

class CIsysTable
{
public:
	using Columns = std::vector<ColumnData>;
	static const Columns& GetColumns(const simba_wstring& tbName);

private:
	static std::map<simba_wstring, Columns> m_tbName2Cols;
	static Columns HisColumns;
	static Columns RtdColumns;
	static Columns TagColumns;
};


ISYS_SQL_NAMESPACE_END
