#pragma once

#include "IsysCommon.h"
#include "DSIExtPassdownInformation.h"

ISYS_SQL_NAMESPACE_BEGIN

class CPassdownInfo : public Simba::SQLEngine::DSIExtPassdownInformation
{
	virtual bool CanHandlePassdown(Simba::SQLEngine::AEBooleanExpr* in_node);

	virtual bool CanHandlePassdown(Simba::SQLEngine::AERelationalExpr* in_node);

	// virtual bool CanHandlePassdown(AERelationalExpr* in_node);
};

ISYS_SQL_NAMESPACE_END