#pragma once
#include "AbStractResultSet.h"
#include "Quickstart.h"

ISYS_SQL_NAMESPACE_BEGIN

class CFilterResult : public CAbstractResultSet
{
public:
    CFilterResult(
        Simba::Support::SharedPtr<CAbstractResultSet> in_table,
        const simba_wstring& in_filter);

    virtual void DeleteRow();

    virtual void SetDataNeeded(simba_uint16 in_column, bool in_dataNeeded);

    virtual bool GetDataNeeded(simba_uint16 in_column);

    virtual Simba::DSI::ICellmark* MarkCell(simba_uint16 in_column);

    virtual Simba::DSI::IColumns* GetSelectColumns();

    virtual void SetCursorType(Simba::DSI::DSICursorType in_cursorType);

    virtual bool HasRowCount();

    virtual simba_unsigned_native GetRowCount();

    virtual bool IsOffsetSupported();

    virtual void GetCatalogName(simba_wstring& out_catalogName);

    virtual void GetSchemaName(simba_wstring& out_schemaName);

    virtual void GetTableName(simba_wstring& out_tableName);

    virtual simba_uint16 GetBookmarkSize();

    virtual void GetBookmark(simba_byte* out_bookmark);

    virtual bool GotoBookmark(const simba_byte* in_bookmark);

    virtual void OnFinishRowUpdate();

    virtual bool RetrieveData(
        simba_uint16 in_column,
        Simba::Support::SqlData* in_data,
        simba_signed_native in_offset,
        simba_signed_native in_maxSize);

    virtual bool WriteData(
        simba_uint16 in_column,
        Simba::Support::SqlData* in_sqlData,
        simba_signed_native in_offset,
        bool in_isDefault);

    virtual CFilterResult* GetAsCBFilterResult();

    virtual CB_RESULT_TYPE GetTableType();

    const simba_wstring& GetFilter() const;

    virtual void DoCloseCursor();

    virtual bool MoveToNextRow();

    virtual void MoveToBeforeFirstRow();

private:
    simba_wstring m_filter;
    Simba::Support::SharedPtr<CAbstractResultSet> m_table;

    /// Flag indicating if Move() has been called yet.
    bool m_hasStartedFetch;
};

ISYS_SQL_NAMESPACE_END