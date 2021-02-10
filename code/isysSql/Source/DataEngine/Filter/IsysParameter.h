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
	RTD = 1
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

	std::vector<simba_wstring> tagsName;
	std::vector<simba_wstring> columns;

	SBoundary<simba_wstring> timeLeft;
	SBoundary<simba_wstring> timeRight;

	bool IsHis();

private:
	DataType m_type;
	static CIsysParameter* m_pInstance;
};

inline bool CIsysParameter::IsHis()
{
	return m_type == DataType::HIS;
}

ISYS_SQL_NAMESPACE_END
