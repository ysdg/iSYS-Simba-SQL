#pragma once

#include "AEOr.h"
#include "IsysCommon.h"
#include "IsysParameter.h"
#include "Quickstart.h"
#include "Filter/AbStractResultSet.h"
#include "DSIExtSimpleBooleanExprHandler.h"
#include "DSIExtColumnRef.h"
#include "IsysResult.h"

ISYS_SQL_NAMESPACE_BEGIN

class CFilterHandler : public Simba::SQLEngine::DSIExtSimpleBooleanExprHandler
{
public:
	CFilterHandler(
		Simba::Support::SharedPtr<CAbstractResultSet> in_table,
        Simba::Quickstart::QuickstartSettings* in_codeBaseSettingss, 
        CIsysResult* result);

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

    virtual bool PassdownLikePredicate(Simba::SQLEngine::AELikePredicate* in_node);

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

    template<typename paraT>
    bool Convert2Filter(
        Simba::SQLEngine::SEComparisonType compOp, 
        const Simba::SQLEngine::DSIExtColumnRef& colRef,
        const paraT& paraValue,
        bool& isLeftCol);

    template<typename paraT>
    bool ConvertPeriod2Filter(
        Simba::SQLEngine::SEComparisonType compOp,
        const Simba::SQLEngine::DSIExtColumnRef& colRef,
        const paraT& paraValue);

    template<typename AET>
    bool CheckNodeExpr(AET* in_node);

    template<typename AET, typename AEList>
    void GetLeftAndRightList(AET* in_node, AEList** left, AEList** right);

    bool IsTagNameCol(const Simba::SQLEngine::DSIExtColumnRef& colRef);

    template<typename AET, typename AEExpr>
    bool SortAENode(AET* in_node, AEExpr** columnExpr, AEExpr** literalExpr, bool& isLeftCol);

    template<typename AET, typename AEExpr>
    bool CheckExpr(AET* in_node, AEExpr* columnExpr, AEExpr* literalExpr);

private:

    Simba::Support::SharedPtr<CAbstractResultSet> m_table;

    simba_wstring m_filter;

    Simba::Quickstart::QuickstartSettings* m_codeBaseSettings;

    bool m_isPassedDown;

    ISYS::SQL::SIsysPara* m_isysPara;
    CIsysResult* m_result;
};

template<typename AET, typename AEList>
void CFilterHandler::GetLeftAndRightList(AET* in_node, AEList** left, AEList** right)
{
    *left = in_node->GetLeftOperand();
    *right = in_node->GetRightOperand();
}

template<typename AET, typename AEExpr>
bool CFilterHandler::SortAENode(AET* in_node, AEExpr** columnExpr, AEExpr** literalExpr, bool& isLeftCol)
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
            isLeftCol = true;
            *columnExpr = lExpr;
            *literalExpr = rExpr;
        }
        else
        {
            isLeftCol = false;
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

        //if (SE_COMP_EQ != compOp)
        //{
        //    return false;
        //}
        
    }

    return !IsNullIncluded(columnSqlType, paramSqlType, literalValue, compOp);
}

template<typename AET>
bool CFilterHandler::CheckNodeExpr(AET* in_node)
{
    AEValueExpr* columnExpr = nullptr;
    AEValueExpr* paraExpr = nullptr;
    bool isLeftCol = true;
    if (!SortAENode(in_node, &columnExpr, &paraExpr, isLeftCol))
    {
        return false;
    }

    if (!CheckExpr(in_node, columnExpr, paraExpr))
    {
        return false;
    };

    // Construct the filter string for input to CodeBase.
    //ConstructFilter(columnName, columnSqlType, paramSqlType, literalValue, compOp);

    DSIExtColumnRef colRef;
    GetTableColRef(columnExpr, colRef);
    if (SqlDataTypeUtilitiesSingleton::GetInstance()->IsIntegerType(paraExpr->GetMetadata()->GetSqlType()))
    {
        ConvertPeriod2Filter(
            in_node->GetComparisonOp(),
            colRef,
            NumberConverter::ConvertWStringToInt32(paraExpr->GetAsLiteral()->GetLiteralValue()));
    }
    else
    {
        Convert2Filter(
            in_node->GetComparisonOp(),
            colRef,
            paraExpr->GetAsLiteral()->GetLiteralValue(),
            isLeftCol);
    }

    m_isPassedDown = true;
    return true;
};

inline bool CFilterHandler::IsTagNameCol(const Simba::SQLEngine::DSIExtColumnRef& colRef)
{
    return (static_cast<simba_uint16>(RtdHisColIndex::TAG_NAME) == colRef.m_colIndex) ||
        static_cast<simba_uint16>(TagColIndex::NAME) == colRef.m_colIndex;
};

template<typename paraT>
bool CFilterHandler::ConvertPeriod2Filter(
    Simba::SQLEngine::SEComparisonType compOp,
    const Simba::SQLEngine::DSIExtColumnRef& colRef,
    const paraT& paraValue)
{
    if (static_cast<simba_uint16>(SampleColIndex::PERIOD) == colRef.m_colIndex)
    {
        if (Simba::SQLEngine::SE_COMP_EQ == compOp)
        {
            m_isysPara->period = paraValue;
        }
    }
    return true;
}

template<typename paraT>
bool CFilterHandler::Convert2Filter(
    Simba::SQLEngine::SEComparisonType compOp,
    const Simba::SQLEngine::DSIExtColumnRef& colRef,
    const paraT& paraValue,
    bool& isLeftCol)
{
    if (IsTagNameCol(colRef) && SE_COMP_EQ == compOp)
    {
        m_isysPara->tagNames.push_back(paraValue);
    }
    else if (static_cast<simba_uint16>(RtdHisColIndex::TIME_STAMP) == colRef.m_colIndex)
    {
        SBoundary<simba_wstring>* left = &m_isysPara->timeLeft;
        SBoundary<simba_wstring>* right = &m_isysPara->timeRight;
        if (!isLeftCol)
        {
            left = &m_isysPara->timeRight;
            right = &m_isysPara->timeLeft;
        }
        switch (compOp)
        {
        case Simba::SQLEngine::SE_COMP_EQ:
            m_isysPara->timeStamps.insert(paraValue);
            break;
        case Simba::SQLEngine::SE_COMP_NE:
            break;
        case Simba::SQLEngine::SE_COMP_GT:
            left->isContain = false;
            left->value = paraValue;
            break;
        case Simba::SQLEngine::SE_COMP_GE:
            left->isContain = true;
            left->value = paraValue;
            break;
        case Simba::SQLEngine::SE_COMP_LT:
            right->isContain = false;
            right->value = paraValue;
            break;
        case Simba::SQLEngine::SE_COMP_LE:
            right->isContain = true;
            right->value = paraValue;
            break;
        default:
            assert(false);
            break;
        }
    }

    return true;
}

ISYS_SQL_NAMESPACE_END