#pragma once

#include "IsysCommon.h"
#include "DSIExtSimpleResultSet.h"

ISYS_SQL_NAMESPACE_BEGIN

enum CB_RESULT_TYPE
{
    CB_TABLE,
    CB_FILTER_RESULT,
    CB_JOIN_RESULT,
    CB_AGGREGATION_RESULT,
};

class CAbstractResultSet : public Simba::SQLEngine::DSIExtSimpleResultSet
{

};

ISYS_SQL_NAMESPACE_END