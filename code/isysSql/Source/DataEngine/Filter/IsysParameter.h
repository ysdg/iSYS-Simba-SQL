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

struct SIsysPara
{
	std::vector<simba_wstring> tagNames;
	SBoundary<simba_wstring> timeLeft;
	SBoundary<simba_wstring> timeRight;
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
	bool IsTagOver();
	std::deque<simba_wstring> GetTags();
	simba_signed_native GetRowNum();

private:
	std::deque<simba_wstring> m_tbName;
	std::deque<simba_wstring> m_tags;
	std::vector<simba_wstring> m_columns;
	SBoundary<simba_wstring> m_timeLeft;
	SBoundary<simba_wstring> m_timeRight;
	DataType m_type;
	simba_signed_native m_rowNumNow;

	static CIsysParameter* m_pInstance;
};

inline void CIsysParameter::AddTbName(const simba_wstring& tbName)
{
	m_tbName.push_back(tbName);
}

inline void CIsysParameter::AddTag(const simba_wstring& tagName)
{
	m_tags.push_back(tagName);
	m_rowNumNow = 0;
}

inline simba_wstring CIsysParameter::GetFrontTag()
{
	if (IsTagOver())
	{
		return simba_wstring();
	}
	return m_tags.front();
}

inline bool CIsysParameter::NextTag()
{
	if (IsTagOver())
	{
		return false;
	}
	m_tags.pop_front();
	++m_rowNumNow;

	return !IsTagOver();
}

inline simba_signed_native CIsysParameter::GetRowNum()
{
	return m_rowNumNow;
}

inline bool CIsysParameter::IsTagOver()
{
	return m_tags.empty();
}

inline std::deque<simba_wstring> CIsysParameter::GetTags()
{
	return m_tags;
}

ISYS_SQL_NAMESPACE_END
