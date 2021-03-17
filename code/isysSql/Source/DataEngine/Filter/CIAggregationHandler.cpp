#include "CIAggregationHandler.h"
#include "AELiteral.h"
#include "FilterResult.h"
#include "IsysTable.h"

using namespace ISYS::SQL;


CIAggregationHandler::CIAggregationHandler(
    Simba::Support::SharedPtr<CAbstractResultSet> in_table,
    QuickstartSettings* in_codeBaseSettingss,
    CIsysResult* result)
    : m_passDown(false)
    , m_table(in_table)
    , m_settings(in_codeBaseSettingss)
    , m_result(result)
{

}

//SharedPtr<DSIExtResultSet> CIAggregationHandler::Passdown(AEAggregate* in_node)
//{
//    if (!m_passDown)
//    {
//        // Return Null and let the engine perform the aggregation.
//        return SharedPtr<DSIExtResultSet>();
//    }
//    
//    return SharedPtr<DSIExtResultSet>();
//}

bool CIAggregationHandler::SetAggregateLiteral(
    SEAggrFunctionID in_aggrFnID,
    AELiteral* in_argExpr,
    bool in_withDistinct)
{
    // Count(*) is currently the only supported aggregate function.
    // Avoid compiler warnings.
    UNUSED(in_aggrFnID);
    UNUSED(in_argExpr);
    UNUSED(in_withDistinct);

    return false;
}


SharedPtr<Simba::SQLEngine::DSIExtResultSet> CIAggregationHandler::CreateResult()
{
    if (!m_passDown)
    {
        // Return Null and let the engine perform the aggregation.
        return SharedPtr<DSIExtResultSet>();
    }

    return SharedPtr<DSIExtResultSet>();
    // Return the aggregation result.
    // return SharedPtr<DSIExtResultSet>(new CFilterResult(m_table, m_filter, m_result));
}

bool CIAggregationHandler::SetAggregateColumn(
    Simba::SQLEngine::SEAggrFunctionID in_aggrFnID,
    simba_uint16 in_colIndex,
    bool in_withDistinct)
{
    auto para = m_result->GetPara();
    if (in_colIndex == static_cast<simba_uint16>(RtdHisColIndex::VALUE))
    {
        para->func = in_aggrFnID;
    }

    return false;
}

bool CIAggregationHandler::SetAggregateFn(Simba::SQLEngine::SEAggrFunctionID in_aggrFnID)
{
    return false;
}

bool CIAggregationHandler::SetGroupingColumn(simba_uint16 in_colIndex)
{
    return true;
}
