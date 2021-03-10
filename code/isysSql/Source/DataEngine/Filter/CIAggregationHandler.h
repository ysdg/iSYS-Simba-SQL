#pragma once

#include "IsysCommon.h"
#include "IsysResult.h"
#include "Quickstart.h"
#include "DSIExtSimpleAggregationHandler.h"
#include "DSIExtResultSet.h"
#include "AEAggregate.h"
#include "Filter/AbStractResultSet.h"

ISYS_SQL_NAMESPACE_BEGIN

using Simba::SQLEngine::DSIExtResultSet;
using Simba::SQLEngine::AEAggregate;
using Simba::Quickstart::QuickstartSettings;
using Simba::SQLEngine::SEAggrFunctionID;
using Simba::SQLEngine::AELiteral;

class CIAggregationHandler : public Simba::SQLEngine::DSIExtSimpleAggregationHandler
{
public:
	CIAggregationHandler(
		Simba::Support::SharedPtr<CAbstractResultSet> in_table,
		QuickstartSettings* in_codeBaseSettingss,
		CIsysResult* result);

	// virtual SharedPtr<DSIExtResultSet> Passdown(AEAggregate* in_node);

	virtual bool SetAggregateLiteral(
		SEAggrFunctionID in_aggrFnID,
		AELiteral* in_argExpr,
		bool in_withDistinct) override;
	
	virtual SharedPtr<Simba::SQLEngine::DSIExtResultSet> CreateResult() override;

	virtual bool SetAggregateColumn(
		Simba::SQLEngine::SEAggrFunctionID in_aggrFnID,
		simba_uint16 in_colIndex,
		bool in_withDistinct) override;

	virtual bool SetAggregateFn(Simba::SQLEngine::SEAggrFunctionID in_aggrFnID) override;

	virtual bool SetGroupingColumn(simba_uint16 in_colIndex) override;
private:

private:
	bool m_passDown;
	QuickstartSettings* m_settings;
	SharedPtr<CAbstractResultSet> m_table;
	CIsysResult* m_result;
};

ISYS_SQL_NAMESPACE_END

