#include "OperationHandlerFactory.h"

#include "DSIExtSimpleAggregationHandler.h"
#include "IBooleanExprHandler.h"
#include "Filter/FilterHandler.h"
#include "Filter/CIAggregationHandler.h"

using namespace Simba::Quickstart;
using namespace Simba::SQLEngine;
using namespace ISYS::SQL;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
COperationHandlerFactory::COperationHandlerFactory(QuickstartSettings* in_Settings, CIsysResult* result) 
    : m_settings(in_Settings)
    , m_result(result)
{
    assert(in_Settings);
}

AutoPtr<IAggregationHandler> COperationHandlerFactory::CreateAggregationHandler(
    SharedPtr<Simba::SQLEngine::DSIExtResultSet> in_table)
{
    SharedPtr<CAbstractResultSet> temp(static_cast<CAbstractResultSet*> (in_table.Get()));

    return AutoPtr<IAggregationHandler>(new CIAggregationHandler(temp, m_settings, m_result));
}

AutoPtr<Simba::SQLEngine::IBooleanExprHandler> COperationHandlerFactory::CreateFilterHandler(
    SharedPtr<Simba::SQLEngine::DSIExtResultSet> in_table)
{
    SharedPtr<CAbstractResultSet> temp(static_cast<CAbstractResultSet*> (in_table.Get()));

    return AutoPtr<IBooleanExprHandler>(new CFilterHandler(temp, m_settings, m_result));
}