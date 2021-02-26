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
#include <set>

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
	std::set<simba_wstring> timeStamps;
};

enum class DataType : simba_uint32
{
	HIS = 0,
	RTD, 
	TAG,
};

ISYS_SQL_NAMESPACE_END
