//==================================================================================================
///  @file QSTable.cpp
///
///  Implementation of the class QSTable.
///
///  Copyright (C) 2008-2016 Simba Technologies Incorporated.
//==================================================================================================

#include "QSTable.h"
#include "Filter/IsysParameter.h"

#include "DSIResultSetColumn.h"
#include "DSITypeUtilities.h"
#include "ErrorException.h"
#include "IWarningListener.h"
#include "NumberConverter.h"
#include "QSTableMetadataFile.h"
#include "QSUtilities.h"
#include "SEInvalidArgumentException.h"
#include "SqlData.h"
#include "SqlDataTypeUtilities.h"
#include "SqlTypeMetadata.h"
#include "TabbedUnicodeFileReader.h"

using namespace Simba::Quickstart;
using namespace Simba::DSI;
using namespace ISYS::SQL;

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSTable::QSTable(
    QuickstartSettings* in_settings,
    ILogger* in_log,
    const simba_wstring& in_tableName,
    IWarningListener* in_warningListener,
    bool in_isODBCV3, 
    ISYS::SQL::CIsysConn* isysConn) :
        m_log(in_log),
        m_tableName(in_tableName),
        m_settings(in_settings),
        m_warningListener(in_warningListener),
        m_hasStartedFetch(false), 
        m_isysConn(isysConn),
        m_result(in_tableName, isysConn, in_log)
{
    SE_CHK_INVALID_ARG(
        (NULL == in_settings) ||
        (NULL == in_log));

    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "QSTable");

    QSUtilities utilities(m_settings);

    InitializeColumns(in_isODBCV3);

    // Generate the file path to the data file.
    simba_wstring dataFilePath(utilities.GetTableDataFullPathName(in_tableName));

    // Open the file.
    m_fileReader.Attach(new TabbedUnicodeFileReader(dataFilePath, m_isysConn));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_unsigned_native QSTable::GetRowCount()
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "GetRowCount");

    if (m_settings->m_useCustomSQLStates)
    {
        // This is an example of posting a warning with a custom SQL state. If SQLRowCount() is
        // called, the driver will return SQL_SUCCESS_WITH_INFO, a row count of ROW_COUNT_UNKNOWN, a
        // SQL state of QS_TABLE_STATE (QS002) and a warning message that the row count is unknown.
        ErrorException e(QS_TABLE_STATE, QS_ERROR, L"QSRowCountUnknown");
        m_warningListener->PostWarning(e, QS_TABLE_STATE);
    }

    // Return ROW_COUNT_UNKNOWN if HasRowCount() returns false.
    return ROW_COUNT_UNKNOWN;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
IColumns* QSTable::GetSelectColumns()
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "GetSelectColumns");
    return m_columns.Get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSTable::HasRowCount()
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "HasRowCount");
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSTable::IsOffsetSupported()
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "IsOffsetSupported");
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTable::GetCatalogName(simba_wstring& out_catalogName)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "GetCatalogName");
    out_catalogName = QS_CATALOG;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTable::GetSchemaName(simba_wstring& out_schemaName)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "GetSchemaName");
    // QuickstartDSII does not support schemas.
    out_schemaName = "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTable::GetTableName(simba_wstring& out_tableName)
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "GetTableName");
    out_tableName = m_tableName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSTable::RetrieveData(
    simba_uint16 in_column,
    Simba::Support::SqlData* in_data,
    simba_signed_native in_offset,
    simba_signed_native in_maxSize)
{
    DEBUG_ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "RetrieveData");
    assert(in_data);

    ConvertData(in_column, in_data, in_offset, in_maxSize);
    if (in_column + 1 == m_columns.Get()->GetColumnCount())
    {
        //MoveToNextRow();
    }
    return false;
}

// Protected =======================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSTable::~QSTable()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTable::DoCloseCursor()
{
    ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "DoCloseCursor");
    m_hasStartedFetch = false;
    m_result.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTable::MoveToBeforeFirstRow()
{
    DEBUG_ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "MoveToBeforeFirstRow");
    m_hasStartedFetch = false;
    m_result.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSTable::MoveToNextRow()
{
    DEBUG_ENTRANCE_LOG(m_log, "Simba::Quickstart", "QSTable", "MoveToNextRow");

    if (!m_hasStartedFetch)
    {
        m_hasStartedFetch = true;
        //m_result.Read();
    }

    return m_result.NextRow();
}

// Private =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSTable::ConvertData(
    simba_uint16 in_column,
    SqlData* in_data,
    simba_signed_native in_offset,
    simba_signed_native in_maxSize)
{
    // SqlData is a container class for the internal types that map to the SQL types. The type that
    // is contained is determined by the SQL type that was indicated for each column as defined
    // by the metadata returned by GetSelectColumns() for the result set. See SqlData.h for a list
    // of the contained types and how they map to the SQL types. To indicate NULL data, call
    // in_data->SetNull(true).
    simba_wstring value(ReadWholeColumnAsString(in_column));

    // Copy the data over depending on the data type.
    switch (in_data->GetMetadata()->GetTDWType())
    {
        case TDW_SQL_CHAR:
        case TDW_SQL_VARCHAR:
        case TDW_SQL_LONGVARCHAR:
        {
            simba_string stdString(value.GetAsUTF8());

            // Ensure that the string is a quoted string.
            simba_string::size_type length = stdString.length();
            if ((length < 2) ||
                (stdString[0] != '\'') ||
                (stdString[length - 1] != '\''))
            {
                QSTHROWGEN2(L"QSConvError", value, in_data->GetMetadata()->GetLocalTypeName());
            }

            // Strip quotes from either side.
            stdString = stdString.substr(1, length - 2);

            // This utility function will automatically handle chunking data back if the amount of
            // data in the column exceeds the amount of data requested, which will result in
            // multiple calls to retrieve the data with increasing offsets.
            return DSITypeUtilities::OutputVarCharStringData(
                stdString,
                in_data,
                in_offset,
                in_maxSize);
        }

        case TDW_SQL_WCHAR:
        case TDW_SQL_WVARCHAR:
        case TDW_SQL_WLONGVARCHAR:
        {
            // Ensure that the string is a quoted string.
            //simba_int32 length = value.GetLength();
            //if ((length < 2) ||
            //    (value.CharAt(0) != L'\'') ||
            //    (value.CharAt(length - 1) != L'\''))
            //{
            //    QSTHROWGEN2(L"QSConvError", value, in_data->GetMetadata()->GetLocalTypeName());
            //}

            //// Strip quotes from either side.
            //value = value.Substr(1, length - 2);

            // This utility function will automatically handle chunking data back if the amount of
            // data in the column exceeds the amount of data requested, which will result in
            // multiple calls to retrieve the data with increasing offsets.
            return DSITypeUtilities::OutputWVarCharStringData(
                &value,
                in_data,
                in_offset,
                in_maxSize);
        }

        case TDW_SQL_UBIGINT:
        {
            assert(in_data->GetMetadata()->IsUnsigned());
            *reinterpret_cast<simba_uint64*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToUInt64(value);

            break;
        }

        case TDW_SQL_SBIGINT:
        {
            assert(!in_data->GetMetadata()->IsUnsigned());
            *reinterpret_cast<simba_int64*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToInt64(value);

            break;
        }

        case TDW_SQL_UINTEGER:
        {
            assert(in_data->GetMetadata()->IsUnsigned());
            *reinterpret_cast<simba_uint32*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToUInt32(value);

            break;
        }

        case TDW_SQL_SINTEGER:
        {
            assert(!in_data->GetMetadata()->IsUnsigned());
            *reinterpret_cast<simba_int32*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToInt32(value);

            break;
        }

        case TDW_SQL_USMALLINT:
        {
            assert(in_data->GetMetadata()->IsUnsigned());
            *reinterpret_cast<simba_uint16*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToUInt16(value);

            break;
        }

        case TDW_SQL_SSMALLINT:
        {
            assert(!in_data->GetMetadata()->IsUnsigned());
            *reinterpret_cast<simba_int16*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToInt16(value);

            break;
        }

        case TDW_SQL_UTINYINT:
        {
            assert(in_data->GetMetadata()->IsUnsigned());
            *reinterpret_cast<simba_uint8*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToUInt8(value);

            break;
        }

        case TDW_SQL_STINYINT:
        {
            assert(!in_data->GetMetadata()->IsUnsigned());
            *reinterpret_cast<simba_int8*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToInt8(value);

            break;
        }

        case TDW_SQL_BIT:
        {
            *reinterpret_cast<simba_byte*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToUInt8(value);

            break;
        }

        case TDW_SQL_REAL:
        {
            // Cast the SQLData void* buffer to simba_double32*.
            *reinterpret_cast<simba_double32*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToSingle(value);

            break;
        }

        case TDW_SQL_FLOAT:
        case TDW_SQL_DOUBLE:
        {
            // Cast the SQLData void* buffer to simba_double64*.
            *reinterpret_cast<simba_double64*>(in_data->GetBuffer()) =
                NumberConverter::ConvertWStringToDouble(value);

            break;
        }

        case TDW_SQL_TYPE_TIME:
        {
            reinterpret_cast<TDWTime*>(in_data->GetBuffer())->Set(value, true);
            break;
        }

        case TDW_SQL_TYPE_TIMESTAMP:
        {
            reinterpret_cast<TDWTimestamp*>(in_data->GetBuffer())->Set(value, true);
            break;
        }

        case TDW_SQL_TYPE_DATE:
        {
            reinterpret_cast<TDWDate*>(in_data->GetBuffer())->Set(value, true);
            break;
        }

        case TDW_SQL_BINARY:
        case TDW_SQL_VARBINARY:
        case TDW_SQL_LONGVARBINARY:
        {
            simba_string strValue(value.GetAsPlatformString());
            simba_char* strValuePtr = const_cast<simba_char*>(strValue.c_str());

            // Validate the binary format.
            if ((0 != strncmp("0x", strValuePtr, 2)) ||
                (0 != (strValue.size() % 2)))
            {
                QSTHROWGEN2(L"QSConvError", value, in_data->GetMetadata()->GetLocalTypeName());
            }

            // Flag to set if there is more data left.
            bool hasMoreData = true;

            // The subtraction by 2 is to account for the two bytes "0x" at the beginning of string.
            simba_signed_native outputSizeInBytes = ((strValue.size() - 2) / 2) - in_offset;
            if ((RETRIEVE_ALL_DATA == in_maxSize) ||
                (outputSizeInBytes <= in_maxSize))
            {
                // There is no more data left after conversion.
                hasMoreData = false;
            }
            else
            {
                outputSizeInBytes = in_maxSize;
            }

            // Skip the first two characters of "0x" and the in_offset bytes.
            strValuePtr += 2 + (in_offset * 2);

            // Convert the hex string to binary. Note that this is not taking into account in_offset
            // and in_maxSize, which a production driver should do to ensure binary data can be
            // retrieved in chunks.
            in_data->SetLength(static_cast<simba_uint32>(outputSizeInBytes));
            DSITypeUtilities::HexStringToBinary(
                strValuePtr,
                outputSizeInBytes * 2,
                in_data->GetBuffer());

            return hasMoreData;
        }

        case TDW_SQL_DECIMAL:
        case TDW_SQL_NUMERIC:
        {
            TDWExactNumericType* numeric =
                reinterpret_cast<TDWExactNumericType*>(in_data->GetBuffer());
            numeric->Set(value);

            // Adjust the scale to ensure the data matches what is stated in the metadata.
            numeric->SetScale(in_data->GetMetadata()->GetScale());
            break;
        }

        default:
        {
             QSTHROWGEN1(L"QSUnknownType", in_data->GetMetadata()->GetLocalTypeName());
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void QSTable::InitializeColumns(bool in_isODBCV3)
{
    AutoPtr<DSIResultSetColumns> columns;
    
    QSUtilities utilities(m_settings);

    // Metadata for columns in the Quickstart sample is stored in XML files that are associated with
    // the normal data files.



    // if (utilities.DoesTableMetadataExist(m_tableName))
    {
        // If the XML file exists, read it to determine the column metadata for the data file.
        QSTableMetadataFile metadataFile(m_settings, in_isODBCV3);
        columns = metadataFile.Read(m_tableName);
    }
    //else
    //{
    //    // If the XML file doesn't exist, throw the QSTableDoesNotExist exception.
    //    QSTHROWGEN1(L"QSTableDoesNotExist", m_tableName);
    //}

    m_columns.Attach(columns.Detach());
}

ISYS::SQL::CIsysResult* QSTable::GetResult()
{
    return &m_result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSTable::ReadWholeColumnAsString(simba_uint16 in_column)
{
    if (!m_hasStartedFetch)
    {
        m_hasStartedFetch = true;
        //m_result.Read();
    }
    //::HTAG tag[1] = { 0 };
    //const auto& tagName = CIsysParameter::Instance()->GetFrontTag();
    //auto result = ::GetTagIDByName(m_isysConn->conn, tagName.GetAsPlatformWString().c_str(), tag[0]);

    //HRESULT* ppResult = NULL;
    //TAGVALSTATE* ppTagValues = NULL;
    //result = ::ReadTagsValue(m_isysConn->conn, sizeof(tag) / sizeof(HTAG), tag, &ppResult, &ppTagValues);

    //if (!ISYS_SUCCESS(result))
    //{
    //    QSTHROW2(QS_DATAENGINE_STATE, L"ReadTagValueError", tagName, NumberConverter::ConvertInt32ToWString(result));
    //}
    
    //ISYS::SQL::CIsysParameter::Instance()->IsHis()
    return m_result.GetColStr(in_column);
    //return GetRtdColStr(in_column, ppTagValues[0]);
}
