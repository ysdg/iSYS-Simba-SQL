/*************************************************
 * @Description: file content
 * @Author: yuanquan
 * @Email: yuanquan@supcon.com
 * @Date: 2021-02-19 09:31:57
 * @LastEditors: yuanquan
 * @LastEditTime: 2021-02-19 17:55:48
 * @copyright: Copyright (c) SUPCON
 *************************************************/
#pragma once

#include "IsysCommon.h"
#include "Quickstart.h"

ISYS_SQL_NAMESPACE_BEGIN

template<typename T>
struct SBoundary
{
	T value;
	bool isContain = false;
};

enum class DataType : simba_uint32
{
	HIS = 0,
	RTD, 
	TAG,
};

enum class Column: simba_uint32
{

};

class CIsysParameter
{
private:
	CIsysParameter();

public:
	static CIsysParameter* Instance();
	void AddTbName(const simba_wstring& tbName);

private:
	std::vector<simba_wstring> m_tbName;
	std::vector<simba_wstring> m_columns;
	std::vector<simba_wstring> m_tags;
	SBoundary<simba_wstring> m_timeLeft;
	SBoundary<simba_wstring> m_timeRight;
	DataType m_type;
	static CIsysParameter* m_pInstance;
};

inline void CIsysParameter::AddTbName(const simba_wstring& tbName)
{
	m_tbName.push_back(tbName);
}

ISYS_SQL_NAMESPACE_END
