#include "Filter/CPassdownInfo.h"

using namespace ISYS::SQL;

bool CPassdownInfo::CanHandlePassdown(Simba::SQLEngine::AEBooleanExpr* in_node)
{
	return true;
}

bool CPassdownInfo::CanHandlePassdown(Simba::SQLEngine::AERelationalExpr* in_node)
{
	return true;
}