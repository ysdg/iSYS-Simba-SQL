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
#include <deque>

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
	void AddTag(const simba_wstring& tagName);
	simba_wstring GetFrontTag();
	bool NextTag();

private:
	std::deque<simba_wstring> m_tbName;
	std::deque<simba_wstring> m_tags;
	std::vector<simba_wstring> m_columns;
	SBoundary<simba_wstring> m_timeLeft;
	SBoundary<simba_wstring> m_timeRight;
	DataType m_type;
	static CIsysParameter* m_pInstance;
};

inline void CIsysParameter::AddTbName(const simba_wstring& tbName)
{
	m_tbName.push_back(tbName);
}

inline void CIsysParameter::AddTag(const simba_wstring& tagName)
{
	m_tags.push_back(tagName);
}

inline simba_wstring CIsysParameter::GetFrontTag()
{
	return m_tags.front();
}

inline bool CIsysParameter::NextTag()
{
	if (m_tags.empty())
	{
		return false;
	}
	m_tags.pop_front();
	return true;
}

ISYS_SQL_NAMESPACE_END
