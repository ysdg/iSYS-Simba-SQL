#include "IsysParameter.h"
#include <mutex>

using namespace ISYS::SQL;

CIsysParameter* CIsysParameter::m_pInstance = CIsysParameter::Instance();

CIsysParameter* CIsysParameter::Instance()
{
	static std::once_flag s_flag;
	std::call_once(s_flag, [&]() {
		m_pInstance = new CIsysParameter();
		});
	return m_pInstance;
}

CIsysParameter::CIsysParameter()
	: m_type(DataType::RTD)
	, m_rowNumNow(0)
{

}
