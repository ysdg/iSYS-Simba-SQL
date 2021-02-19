#include "Quickstart.h"
#include "FilterHandler.h"
#include "AbStractResultSet.h"
#include "FilterResult.h"
#include "IsysParameter.h"

#include "AEComparison.h"
#include "AELiteral.h"
#include "AEParameter.h"

#include "DSIExtColumnRef.h"
#include "DSIExtResultSet.h"
#include "IColumns.h"
#include "NumberConverter.h"
#include "SqlDataTypeUtilities.h"
#include "SqlTypeMetadata.h"

using namespace Simba::Quickstart;
using namespace Simba::SQLEngine;
using namespace ISYS::SQL;
using namespace Simba::DSI;

// Helpers =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring GetParameterValue(AEParameter* in_parameter)
{
    simba_uint32 dataLength = 0;
    const void* data = in_parameter->GetInputData(dataLength);

    if ((0 == dataLength) && (NULL == data))
    {
        // Return a "null" value.
        return simba_wstring();
    }

    // Use the TDW type to avoid ambiguities about sign.
    switch (in_parameter->GetMetadata()->GetTDWType())
    {
    case TDW_SQL_CHAR:
    case TDW_SQL_VARCHAR:
    case TDW_SQL_LONGVARCHAR:
    case TDW_SQL_WCHAR:
    case TDW_SQL_WVARCHAR:
    case TDW_SQL_WLONGVARCHAR:
    {
        return simba_wstring(
            reinterpret_cast<const simba_byte*>(data),
            dataLength,
            in_parameter->GetMetadata()->GetEncoding());
    }

    case TDW_SQL_NUMERIC:
    case TDW_SQL_DECIMAL:
    {
        return reinterpret_cast<const TDWExactNumericType*>(data)->ToString();
    }

    case TDW_SQL_FLOAT:
    case TDW_SQL_DOUBLE:
    {
        return NumberConverter::ConvertDouble64ToWString(*reinterpret_cast<const simba_double64*>(data));
    }

    case TDW_SQL_REAL:
    {
        return NumberConverter::ConvertDouble32ToWString(*reinterpret_cast<const simba_double32*>(data));
    }

    case TDW_SQL_STINYINT:
    {
        return NumberConverter::ConvertToWString(*reinterpret_cast<const simba_int8*>(data));
    }

    case TDW_SQL_UTINYINT:
    {
        return NumberConverter::ConvertToWString(*reinterpret_cast<const simba_uint8*>(data));
    }

    case TDW_SQL_USMALLINT:
    {
        return NumberConverter::ConvertToWString(*reinterpret_cast<const simba_uint16*>(data));
    }

    case TDW_SQL_SSMALLINT:
    {
        return NumberConverter::ConvertToWString(*reinterpret_cast<const simba_int16*>(data));
    }

    case TDW_SQL_UINTEGER:
    {
        return NumberConverter::ConvertToWString(*reinterpret_cast<const simba_uint32*>(data));
    }

    case TDW_SQL_SINTEGER:
    {
        return NumberConverter::ConvertToWString(*reinterpret_cast<const simba_int32*>(data));
    }

    case TDW_SQL_UBIGINT:
    {
        return NumberConverter::ConvertToWString(*reinterpret_cast<const simba_uint64*>(data));
    }

    case TDW_SQL_SBIGINT:
    {
        return NumberConverter::ConvertToWString(*reinterpret_cast<const simba_int64*>(data));
    }

    default:
    {
        // This shouldn't be hit due to checks before to determine what types of parameters
        // can be passed down.
        SETHROW_INVALID_ARG();
    }
    }
}

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
CFilterHandler::CFilterHandler(
    SharedPtr<CAbstractResultSet> in_table,
    QuickstartSettings* in_codeBaseSettings)
    :
    m_table(in_table),
    m_codeBaseSettings(in_codeBaseSettings),
    m_isPassedDown(false)
{
    assert(!in_table.IsNull());
    assert(in_codeBaseSettings);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFilterHandler::CanHandleMoreClauses()
{
    // Always returns true since any incrementally adding more filter clauses is supported.
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
SharedPtr<DSIExtResultSet> CFilterHandler::TakeResult()
{
    if (!m_isPassedDown)
    {
        // Return NULL and let the engine do the filtering.
        return SharedPtr<DSIExtResultSet>();
    }
    // Return filter result.
    return SharedPtr<DSIExtResultSet>(new CFilterResult(m_table, m_filter));
}

// Protected =======================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFilterHandler::PassdownComparison(AEComparison* in_node)
{
    // If true, then there is an attempt to process the filter. This implementation only supports 
    // CBTable.
    // if (m_codeBaseSettings->m_passDownFlag && (m_table->GetTableType() == CB_TABLE))
    {
        //// Try the default case first.
        //if (DSIExtSimpleBooleanExprHandler::PassdownComparison(in_node))
        //{
        //    return true;
        //}

        // Try and handle parameters here.
        AEValueList* lOperand = in_node->GetLeftOperand();
        AEValueList* rOperand = in_node->GetRightOperand();

        if ((1 != lOperand->GetChildCount()) ||
            (1 != rOperand->GetChildCount()))
        {
            return false;
        }

        AEValueExpr* lExpr = lOperand->GetChild(0);
        AEValueExpr* rExpr = rOperand->GetChild(0);

        //  Only handle two cases: 
        //   - <column_reference> <compOp> <parameter>
        //   - <parameter> <compOp> <column_reference>
        if ((AE_NT_VX_COLUMN != lExpr->GetNodeType()) ||
            (AE_NT_VX_PARAMETER != rExpr->GetNodeType()))
        {
            if ((AE_NT_VX_COLUMN == rExpr->GetNodeType()) &&
                (AE_NT_VX_PARAMETER == lExpr->GetNodeType()))
            {
                // Swap the pointers, so the expressions are always in the form of
                // <column_reference> <compOp> <parameter>.
                std::swap(lExpr, rExpr);
            }
            else
            {
                return false;
            }
        }

        // Get the column reference information.
        DSIExtColumnRef colRef;
        if (!GetTableColRef(lExpr, colRef))
        {
            // Column not found.
            return false;
        }

        simba_wstring lname, rname;
        lExpr->GetName(lname);
        rExpr->GetName(rname);
        SEComparisonType compOp = in_node->GetComparisonOp();

        // Get information about the column in the table that the filter is applied to.
        IColumns* tableColumns = colRef.m_table->GetSelectColumns();
        assert(tableColumns);
         //CBColumnHolder filterColumn(tableColumns->GetColumn(colRef.m_colIndex));
         //SqlTypeMetadata* columnMetadata = filterColumn.GetMetadata();
         //simba_int16 columnSqlType = columnMetadata->GetSqlType();
        simba_int16 columnSqlType = 1;

        // Get the literal type of RHS of comparison expression.
        simba_int16 paramSqlType = rExpr->GetMetadata()->GetSqlType();

        // Supported parameter types: Integer, Decimal, Character String, and Date
        // (Time comparison is not supported by DBASE expressions)
        SqlDataTypeUtilities* dataTypeUtils = SqlDataTypeUtilitiesSingleton::GetInstance();
        if (dataTypeUtils->IsIntegerType(paramSqlType) ||
            dataTypeUtils->IsExactNumericType(paramSqlType) ||
            dataTypeUtils->IsApproximateNumericType(paramSqlType) ||
            ((SQL_TYPE_TIME != columnSqlType) &&
                (dataTypeUtils->IsCharacterType(paramSqlType) || dataTypeUtils->IsWideCharacterType(paramSqlType))))
        {
            // Get the parameter value.
            simba_wstring paramValue = GetParameterValue(rExpr->GetAsParameter());

            if (paramValue.IsNull())
            {
                // Can't handle NULL parameters.
                return false;
            }

            // Get the column name and comparison type.
            simba_wstring columnName;
            //filterColumn.GetName(columnName);
            SEComparisonType compOp = in_node->GetComparisonOp();

            // Construct the filter string for input to CodeBase.
            ConstructFilter(columnName, columnSqlType, paramSqlType, paramValue, compOp);

            // Setting passdown flag so the filter result set is returned.
            m_isPassedDown = true;

            // Determine if filter result might include NULL records, because
            // CodeBase doesn't recognize NULL, and treats it as '0' or 'false'.
            return !IsNullIncluded(columnSqlType, paramSqlType, paramValue, compOp);
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFilterHandler::PassdownSimpleComparison(
    DSIExtColumnRef& in_leftExpr,
    LiteralValue in_rightExpr,
    SEComparisonType in_compOp)
{
    assert(in_rightExpr.first);
    assert(!in_leftExpr.m_table.IsNull());
    SE_CHK_INVALID_ARG(in_leftExpr.m_table.Get() != m_table.Get());

    // If true, then there is an attempt to process the filter. This implementation only supports 
    // CBTable.
    //if (m_codeBaseSettings->m_passDownFlag && (m_table->GetTableType() == CB_TABLE))
    {
        // Get information about the column in the table that the filter is applied to.
        IColumns* tableColumns = in_leftExpr.m_table->GetSelectColumns();
        assert(tableColumns);
        //CBColumnHolder filterColumn(tableColumns->GetColumn(in_leftExpr.m_colIndex));
        //SqlTypeMetadata* columnMetadata = filterColumn.GetMetadata();
        //simba_int16 columnSqlType = columnMetadata->GetSqlType();
        simba_int16 columnSqlType = 1;

        // Get the literal type of RHS of comparison expression.
        PSLiteralType exprLiteralType = in_rightExpr.first->GetLiteralType();

        // Supported literal types: Unsigned Integer, Decimal, Character String, and Date
        // (Time comparison is not supported by DBASE expressions)
        if ((exprLiteralType == PS_LITERAL_APPROXNUM) ||
            (exprLiteralType == PS_LITERAL_USINT) ||
            (exprLiteralType == PS_LITERAL_DECIMAL) ||
            ((exprLiteralType == PS_LITERAL_CHARSTR) && (SQL_TYPE_TIME != columnSqlType)))
        {
            // Get the column name.
            simba_wstring columnName;
            //filterColumn.GetName(columnName);

            // Get the literal and prepend it with '-' if it's been negated.
            simba_wstring literalVal;
            if (in_rightExpr.second)
            {
                literalVal = "-" + in_rightExpr.first->GetLiteralValue();
            }
            else
            {
                literalVal = in_rightExpr.first->GetLiteralValue();
            }

            // Construct the filter string for input to CodeBase.
            ConstructFilter(
                columnName,
                columnSqlType,
                in_rightExpr.first->GetMetadata()->GetSqlType(),
                literalVal,
                in_compOp);

            // Setting passdown flag so the filter result set is returned.
            m_isPassedDown = true;

            // Determine if filter result might include NULL records, because
            // CodeBase doesn't recognize NULL, and treats it as '0' or 'false'.
            return !IsNullIncluded(
                columnSqlType,
                in_rightExpr.first->GetMetadata()->GetSqlType(),
                literalVal,
                in_compOp);
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFilterHandler::PassdownSimpleComparison(
    DSIExtColumnRef& in_leftExpr,
    DSIExtColumnRef& in_rightExpr,
    SEComparisonType in_compOp)
{
    // CodebaseDSII does not support passdown comparison operations where the left and right
    // operands are column references.
    UNUSED(in_leftExpr);
    UNUSED(in_rightExpr);
    UNUSED(in_compOp);

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFilterHandler::PassdownSimpleInPredicate(
    DSIExtColumnRef& in_column,
    LiteralVector& in_literals)
{
    // CodebaseDSII does not support passdown IN predicate operations.
    UNUSED(in_column);
    UNUSED(in_literals);

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFilterHandler::PassdownSimpleNullPredicate(
    DSIExtColumnRef& in_column,
    bool in_isNull)
{
    // CodebaseDSII does not support passdown NULL predicate operations.
    UNUSED(in_column);
    UNUSED(in_isNull);

    return false;
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
void CFilterHandler::ConstructFilter(
    simba_wstring in_columnName,
    simba_int16 in_columnSqlType,
    simba_int16 in_exprSqlType,
    const simba_wstring& in_exprValue,
    SEComparisonType in_compOp)
{
    // Add "AND" operator if the filter is being added to an existing filter.
    if (m_isPassedDown)
    {
        m_filter += " .AND. ";
    }

    // Find the table alias that is used by CodeBase.
    //Data4* dataObj = m_table->GetTableHandle();
    //simba_wstring tableAlias(dataObj->alias());

    // Add table and column name to filter.
    simba_wstring tableAlias(L"ALIAS_TEST");
    m_filter += "( " + tableAlias + "->" + in_columnName + " ";

    // Determine the math symbol for comparison type.
    switch (in_compOp)
    {
    case SE_COMP_EQ:
    {
        m_filter += EQ_SYM;
        break;
    }

    case SE_COMP_NE:
    {
        m_filter += NE_SYM;
        break;
    }

    case SE_COMP_GT:
    {
        m_filter += GT_SYM;
        break;
    }

    case SE_COMP_GE:
    {
        m_filter += GE_SYM;
        break;
    }

    case SE_COMP_LT:
    {
        m_filter += LT_SYM;
        break;
    }

    case SE_COMP_LE:
    {
        m_filter += LE_SYM;
        break;
    }
    }

    m_filter += " ";

    // Adding the literal to the filter string.
    if (SQL_BIT == in_columnSqlType)
    {
        // Dealing with columns of SQL_BIT type, i.e. logical columns.
        if (in_exprValue == "1")
        {
            m_filter += CB_DBASE_TRUE;
        }
        else
        {
            m_filter += CB_DBASE_FALSE;
        }
    }
    else if (SQL_TYPE_DATE == in_columnSqlType)
    {
        // Dealing with columns of SQL_TYPE_DATE, i.e. date columns.
        // Creating a date4 object.
        //Date4 date(
        //    in_exprValue.GetAsUTF8().c_str(),
        //    const_cast<simba_char*> (CB_DATE_FORMAT.c_str()));

        //simba_wstring dateStr(date.str());

        // Use STOD DBASE function to convert string to date.
        //m_filter += "STOD(\"" + dateStr + "\")";
    }
    else if (SqlDataTypeUtilitiesSingleton::GetInstance()->IsCharacterType(in_exprSqlType) ||
        SqlDataTypeUtilitiesSingleton::GetInstance()->IsWideCharacterType(in_exprSqlType))
    {
        // Adding single quotations for character strings.
        m_filter += "'" + in_exprValue + "'";
    }
    else
    {
        // Dealing with numeric column types.
        m_filter += in_exprValue;
    }

    m_filter += " )";

    // Making sure the record is not marked for deletion by CodeBase.
    if (!m_isPassedDown)
    {
        m_filter += " .AND. .NOT. DELETED()";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool CFilterHandler::IsNullIncluded(
    simba_int16 in_columnSqlType,
    simba_int16 in_exprSqlType,
    const simba_wstring& in_exprValue,
    SEComparisonType in_compOp)
{
    // Dealing with filters that might include NULL records. DBASE expression doesn't recognize
    // NULL fields and replaces it with '0' for numeric, logical, and date literal type.
    //
    // Logical Comparison: NULL is treated as '0', i.e. False
    if ((SQL_BIT == in_columnSqlType) &&
        ((SE_COMP_EQ == in_compOp) && (CB_LOGICAL_FALSE == in_exprValue)) ||
        ((SE_COMP_NE == in_compOp) && (CB_LOGICAL_TRUE == in_exprValue)))
    {
        return true;
    }

    // Date Comparison: NULL is included when using "<", "<=", or "<>" operator
    if ((SQL_TYPE_DATE == in_columnSqlType) &&
        ((SE_COMP_LE == in_compOp) || (SE_COMP_LT == in_compOp) || (SE_COMP_NE == in_compOp)))
    {
        return true;
    }

    // Numerical Comparison: NULL is treated as '0' in numerical comparisons
    // so the filters that might include the NULL record have to return false.
    if ((SQL_BIT != in_columnSqlType) &&
        !SqlDataTypeUtilitiesSingleton::GetInstance()->IsCharacterType(in_exprSqlType) &&
        !SqlDataTypeUtilitiesSingleton::GetInstance()->IsWideCharacterType(in_exprSqlType))
    {
        simba_double64 literalValueDb = NumberConverter::ConvertWStringToDouble(in_exprValue);

        if (((SE_COMP_LE == in_compOp) && (0 <= literalValueDb)) ||
            ((SE_COMP_LT == in_compOp) && (0 < literalValueDb)) ||
            ((SE_COMP_GE == in_compOp) && (0 >= literalValueDb)) ||
            ((SE_COMP_GT == in_compOp) && (0 > literalValueDb)) ||
            ((SE_COMP_NE == in_compOp) && (0 != literalValueDb)) ||
            ((SE_COMP_EQ == in_compOp) && (0 == literalValueDb)))
        {
            return true;
        }
    }

    // No NULL records are included in the filter result.
    return false;
}
