#pragma once

#include "AEOr.h"
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

    virtual bool PassdownOr(Simba::SQLEngine::AEOr* in_node);

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

    template<typename AET>
    bool CheckNodeExpr(AET* in_node);

    template<typename AET, typename AEList>
    void GetLeftAndRightList(AET* in_node, AEList** left, AEList** right);

    //template<>
    //void GetLeftAndRightList<Simba::SQLEngine::AENode, Simba::SQLEngine::AEValueList>(  
    //    Simba::SQLEngine::AENode* in_node, 
    //    Simba::SQLEngine::AEValueList* left, 
    //    Simba::SQLEngine::AEValueList* right);

    template<typename AET, typename AEExpr>
    bool SortAENode(AET* in_node, AEExpr** columnExpr, AEExpr** literalExpr);

    template<typename AET, typename AEExpr>
    bool CheckExpr(AET* in_node, AEExpr* columnExpr, AEExpr* literalExpr);

private:

    Simba::Support::SharedPtr<CAbstractResultSet> m_table;

    simba_wstring m_filter;

    Simba::Quickstart::QuickstartSettings* m_codeBaseSettings;

    bool m_isPassedDown;
};

template<typename AET, typename AEList>
void CFilterHandler::GetLeftAndRightList(AET* in_node, AEList** left, AEList** right)
{
    *left = in_node->GetLeftOperand();
    *right = in_node->GetRightOperand();
}

//template<>
//void CFilterHandler::GetLeftAndRightList<Simba::SQLEngine::AENode, Simba::SQLEngine::AEValueList>(
//    Simba::SQLEngine::AENode* in_node, 
//    Simba::SQLEngine::AEValueList* left,
//    Simba::SQLEngine::AEValueList* right)
//{
//    Simba::SQLEngine::AENode* leftExpr = in_node->GetChild(0);
//    Simba::SQLEngine::AENode* rightExpr = in_node->GetChild(1);
//
//    left = leftExpr->GetAsValueList();
//    right = rightExpr->GetAsValueList();
//}

template<typename AET, typename AEExpr>
bool CFilterHandler::SortAENode(AET* in_node, AEExpr** columnExpr, AEExpr** literalExpr)
{
    AEValueList* left = nullptr;
    AEValueList* right = nullptr;

    GetLeftAndRightList(in_node, &left, &right);

    if ((1 != left->GetChildCount()) ||
        (1 != right->GetChildCount()))
    {
        return false;
    }

    AEExpr* lExpr = left->GetChild(0);
    AEExpr* rExpr = right->GetChild(0);

    //  Only handle two cases: 
    //   - <column_reference> <compOp> <parameter>
    //   - <parameter> <compOp> <column_reference>
    if ((AE_NT_VX_COLUMN == lExpr->GetNodeType()) == (AE_NT_VX_COLUMN == rExpr->GetNodeType()))
    {
        return false;
    }
    else
    {
        if (AE_NT_VX_COLUMN == lExpr->GetNodeType())
        {
            *columnExpr = lExpr;
            *literalExpr = rExpr;
        }
        else
        {
            *columnExpr = rExpr;
            *literalExpr = lExpr;
        }
    }
    return true;
}

template<typename AET, typename AEExpr>
bool CFilterHandler::CheckExpr(AET* in_node, AEExpr* columnExpr, AEExpr* literalExpr)
{
    // Get the column reference information.
    DSIExtColumnRef colRef;
    if (!GetTableColRef(columnExpr, colRef))
    {
        // Column not found.
        return false;
    }

    // Get information about the column in the table that the filter is applied to.
    IColumns* tableColumns = colRef.m_table->GetSelectColumns();
    assert(tableColumns);
    CColumnHolder filterColumn(tableColumns->GetColumn(colRef.m_colIndex));
    SqlTypeMetadata* columnMetadata = filterColumn.GetMetadata();

    simba_int16 columnSqlType = columnMetadata->GetSqlType();
    simba_int16 paramSqlType = literalExpr->GetMetadata()->GetSqlType();
    simba_wstring literalValue = literalExpr->GetAsLiteral()->GetLiteralValue();
    SEComparisonType compOp = in_node->GetComparisonOp();

    // Supported parameter types: Integer, Decimal, Character String, and Date
    // (Time comparison is not supported by DBASE expressions)
    SqlDataTypeUtilities* dataTypeUtils = SqlDataTypeUtilitiesSingleton::GetInstance();
    if (dataTypeUtils->IsIntegerType(paramSqlType) ||
        dataTypeUtils->IsExactNumericType(paramSqlType) ||
        dataTypeUtils->IsApproximateNumericType(paramSqlType) ||
        ((SQL_TYPE_TIME != columnSqlType) &&
            (dataTypeUtils->IsCharacterType(paramSqlType) || dataTypeUtils->IsWideCharacterType(paramSqlType))))
    {
        if (literalValue.IsNull())
        {
            return false;
        }

        if (SE_COMP_EQ != compOp)
        {
            return false;
        }
        
    }

    return !IsNullIncluded(columnSqlType, paramSqlType, literalValue, compOp);
}

template<typename AET>
bool CFilterHandler::CheckNodeExpr(AET* in_node)
{
    AEValueExpr* columnExpr = nullptr;
    AEValueExpr* literalExpr = nullptr;
    if (!SortAENode(in_node, &columnExpr, &literalExpr))
    {
        return false;
    }

    if (!CheckExpr(in_node, columnExpr, literalExpr))
    {
        return false;
    };

    // Construct the filter string for input to CodeBase.
    //ConstructFilter(columnName, columnSqlType, paramSqlType, literalValue, compOp);

    CIsysParameter::Instance()->AddTag(literalExpr->GetAsLiteral()->GetLiteralValue());
    m_isPassedDown = true;
    return true;
}

ISYS_SQL_NAMESPACE_END