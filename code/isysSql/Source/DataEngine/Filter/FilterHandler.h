#pragma once

#include "IsysCommon.h"
#include "Quickstart.h"
#include "Filter/AbStractResultSet.h"
#include "DSIExtSimpleBooleanExprHandler.h"

ISYS_SQL_NAMESPACE_BEGIN

class CFilterHandler : public Simba::SQLEngine::DSIExtSimpleBooleanExprHandler
{
public:
	CFilterHandler(
		Simba::Support::SharedPtr<CAbstractResultSet> in_table,
        Simba::Quickstart::QuickstartSettings* in_codeBaseSettingss);

	virtual bool CanHandleMoreClauses();

	virtual Simba::Support::SharedPtr<Simba::SQLEngine::DSIExtResultSet> TakeResult();

protected:
    virtual bool PassdownComparison(Simba::SQLEngine::AEComparison* in_node);

    virtual bool PassdownSimpleComparison(
        Simba::SQLEngine::DSIExtColumnRef& in_leftExpr,
        Simba::SQLEngine::LiteralValue in_rightExpr,
        Simba::SQLEngine::SEComparisonType in_compOp);

    virtual bool PassdownSimpleComparison(
        Simba::SQLEngine::DSIExtColumnRef& in_leftExpr,
        Simba::SQLEngine::DSIExtColumnRef& in_rightExpr,
        Simba::SQLEngine::SEComparisonType in_compOp);

    virtual bool PassdownSimpleInPredicate(
        Simba::SQLEngine::DSIExtColumnRef& in_column,
        Simba::SQLEngine::LiteralVector& in_literals);

    virtual bool PassdownSimpleNullPredicate(
        Simba::SQLEngine::DSIExtColumnRef& in_column,
        bool in_isNull);

private:

    void ConstructFilter(
        simba_wstring in_columnName,
        simba_int16 in_columnSqlType,
        simba_int16 in_exprSqlType,
        const simba_wstring& in_exprValue,
        Simba::SQLEngine::SEComparisonType in_compOp);

    bool IsNullIncluded(
        simba_int16 in_columnSqlType,
        simba_int16 in_exprSqlType,
        const simba_wstring& in_exprValue,
        Simba::SQLEngine::SEComparisonType in_compOp);

    Simba::Support::SharedPtr<CAbstractResultSet> m_table;

    simba_wstring m_filter;

    Simba::Quickstart::QuickstartSettings* m_codeBaseSettings;

    bool m_isPassedDown;
};

ISYS_SQL_NAMESPACE_END