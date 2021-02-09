#include "OperationHandlerFactory.h"

#include "DSIExtSimpleAggregationHandler.h"
#include "IBooleanExprHandler.h"
#include "Filter/FilterHandler.h"

using namespace Simba::Quickstart;
using namespace Simba::SQLEngine;
using namespace ISYS::SQL;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
COperationHandlerFactory::COperationHandlerFactory(QuickstartSettings* in_Settings) :
    m_settings(in_Settings)
{
    assert(in_Settings);
}

AutoPtr<IAggregationHandler> COperationHandlerFactory::CreateAggregationHandler(
    SharedPtr<Simba::SQLEngine::DSIExtResultSet> in_table)
{
    //SharedPtr<CBAbstractResultSet> temp(static_cast<CBAbstractResultSet*> (in_table.Get()));

    //return AutoPtr<IAggregationHandler>(new CBAggregationHandler(temp, m_codeBaseSettings));
    return AutoPtr<IAggregationHandler>(nullptr);
}

AutoPtr<Simba::SQLEngine::IBooleanExprHandler> COperationHandlerFactory::CreateFilterHandler(
    SharedPtr<Simba::SQLEngine::DSIExtResultSet> in_table)
{
    SharedPtr<CAbstractResultSet> temp(static_cast<CAbstractResultSet*> (in_table.Get()));

    return AutoPtr<IBooleanExprHandler>(new CFilterHandler(temp, m_settings));
}