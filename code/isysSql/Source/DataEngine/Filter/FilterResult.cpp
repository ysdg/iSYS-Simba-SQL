#include "Filter/FilterResult.h"
#include "IResult.h"
#include "IsysParameter.h"
#include "QSTable.h"

using namespace ISYS::SQL;

CFilterResult::CFilterResult(
    Simba::Support::SharedPtr<CAbstractResultSet> in_table,
    const simba_wstring& in_filter,
    SIsysPara& isysPara, 
    CIsysResult* result)
    : m_table(in_table)
    , m_filter(in_filter)
    , m_hasStartedFetch(false)
    , m_isysPara(isysPara)
    , m_result(result)
{

}

void CFilterResult::DeleteRow()
{
}

void CFilterResult::SetDataNeeded(simba_uint16 in_column, bool in_dataNeeded)
{

}

bool CFilterResult::GetDataNeeded(simba_uint16 in_column)
{
    return true;
}

Simba::DSI::ICellmark* CFilterResult::MarkCell(simba_uint16 in_column)
{
    return nullptr;
}

Simba::DSI::IColumns* CFilterResult::GetSelectColumns()
{
    return m_table->GetSelectColumns();
}

void CFilterResult::SetCursorType(Simba::DSI::DSICursorType in_cursorType)
{
    m_table->SetCursorType(in_cursorType);
}

bool CFilterResult::HasRowCount()
{
    return false;
}

simba_unsigned_native CFilterResult::GetRowCount()
{
    return Simba::DSI::ROW_COUNT_UNKNOWN;
}

bool CFilterResult::IsOffsetSupported()
{
    return false;
}

void CFilterResult::GetCatalogName(simba_wstring& out_catalogName)
{
    m_table->GetCatalogName(out_catalogName);
}

void CFilterResult::GetSchemaName(simba_wstring& out_schemaName)
{
    m_table->GetSchemaName(out_schemaName);
}

void CFilterResult::GetTableName(simba_wstring& out_tableName)
{
    m_table->GetTableName(out_tableName);
}

simba_uint16 CFilterResult::GetBookmarkSize()
{
    return m_table->GetBookmarkSize();
}

void CFilterResult::GetBookmark(simba_byte* out_bookmark)
{
    m_table->GetBookmark(out_bookmark);
}

bool CFilterResult::GotoBookmark(const simba_byte* in_bookmark)
{
    return m_table->GotoBookmark(in_bookmark);
}

void CFilterResult::OnFinishRowUpdate()
{
    m_table->OnFinishRowUpdate();
}

bool CFilterResult::RetrieveData(
    simba_uint16 in_column,
    Simba::Support::SqlData* in_data,
    simba_signed_native in_offset,
    simba_signed_native in_maxSize)
{
    return m_table->RetrieveData(in_column, in_data, in_offset, in_maxSize);
}

bool CFilterResult::WriteData(
    simba_uint16 in_column,
    Simba::Support::SqlData* in_sqlData,
    simba_signed_native in_offset,
    bool in_isDefault)
{
    return m_table->WriteData(in_column, in_sqlData, in_offset, in_isDefault);
}

CFilterResult* CFilterResult::GetAsCBFilterResult()
{
    return this;
}

CB_RESULT_TYPE CFilterResult::GetTableType()
{
    return CB_FILTER_RESULT;
}

const simba_wstring& CFilterResult::GetFilter() const
{
    return m_filter;
}

void CFilterResult::DoCloseCursor()
{
    m_hasStartedFetch = false;
}

bool CFilterResult::MoveToNextRow()
{
    // Ensure the row number in the underlying table matches the row number corresponding to the
    // next row in this filtered result.
    // recNo - 1 because SetCurrentRow() takes in a 0-based row number. recNo() returns a 1-based
    // row number.
    //
    // m_numRowsDeleted needs to be substracted because recNo() includes records that have already
    // been marked for deletion.
    // Get the first row
    
    //auto isysPara = CIsysParameter::Instance();
    if (!m_hasStartedFetch)
    {
        m_hasStartedFetch = true;
        m_result->SetPara(&m_isysPara);
        m_result->Read();
        return m_result->IsOver();
    }
    m_table->SetCurrentRow(m_result->GetRowNum());

    return m_result->NextRow();
    
}

void CFilterResult::MoveToBeforeFirstRow()
{
    m_hasStartedFetch = false;
}