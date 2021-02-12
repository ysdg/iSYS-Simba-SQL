#pragma once

#include "IsysCommon.h"
#include "Quickstart.h"

#include "IColumn.h"
#include "IHasher.h"
#include "DSIColumnMetadata.h"

ISYS_SQL_NAMESPACE_BEGIN

class CColumnHolder : public Simba::DSI::IColumn
{
    // Public ======================================================================================
public:
    /// @brief Constructor.
    /// 
    /// @param in_column            The IColumn reference. (NOT OWN)
    CColumnHolder(Simba::DSI::IColumn* in_column) :
        m_column(in_column)
    {
        assert(in_column);
    }

    /// @brief Destructor.
    virtual ~CColumnHolder() {}

    /// @brief Returns the size for the column for character or binary data. 
    ///
    /// For character data, this size should be in terms of the number of characters, not the 
    /// number of bytes. 
    /// For example, VARCHAR(10) and NVARCHAR(10) would both correspond to size of 10, even 
    /// if NVARCHAR used 2 bytes per character.
    ///
    /// This method is not used for non-character/binary data types.
    ///
    /// @return Size for the column for character or binary data.
    simba_uint32 GetColumnSize() const
    {
        return m_column->GetColumnSize();
    }

    /// @brief Indicates how the column can be used in a WHERE clause.
    ///
    /// @return An enum that shows the behaviors of the column in a WHERE clause.
    Simba::DSI::DSIColumnSearchable GetSearchable() const
    {
        return m_column->GetSearchable();
    }

    /// @brief Gets the column label or title. 
    ///
    /// May return an empty string.
    ///
    /// @param out_label        Output column label.
    void GetLabel(simba_wstring& out_label) const
    {
        m_column->GetLabel(out_label);
    }

    /// @brief Gets the column alias. 
    ///
    /// If there is no alias, returns the column name. May return an empty string.
    ///
    /// @param out_label        Output column name.
    void GetName(simba_wstring& out_name) const
    {
        m_column->GetName(out_name);
    }

    /// @brief Gets the name of the table.
    /// 
    /// @param out_label        Output table name.
    void GetTableName(simba_wstring& out_tableName) const
    {
        m_column->GetTableName(out_tableName);
    }

    /// @brief Gets the Catalog name.
    /// 
    /// @param out_label        Output catalog name.
    void GetCatalogName(simba_wstring& out_catalogName) const
    {
        m_column->GetCatalogName(out_catalogName);
    }

    /// @brief Gets the Schema name.
    /// 
    /// @param out_label        Output schema name.
    void GetSchemaName(simba_wstring& out_schemaName) const
    {
        m_column->GetSchemaName(out_schemaName);
    }

    /// @brief Gets a SqlTypeMetadata* describing the column.
    ///
    /// @return SQLTypeMetadata pointer describing the column. (NOT OWN)
    Simba::Support::SqlTypeMetadata* GetMetadata() const
    {
        return m_column->GetMetadata();
    }

    /// @brief Gets a reference to the DSIColumnMetadata describing the column.
    /// 
    /// @return Reference to the DSIColumnMetadata describing the column.
    const Simba::DSI::DSIColumnMetadata& GetColumnMetadata() const
    {
        return m_column->GetColumnMetadata();
    }

    /// @brief Describes whether the column is updatable in the result set.
    ///
    /// @return An enum that specifies the updatability of the column in the result set.
    Simba::DSI::DSIColumnUpdatable GetUpdatable() const
    {
        return m_column->GetUpdatable();
    }

    /// @brief Logs the entire column into the input log.
    /// 
    /// @param in_log           The logger to use to log this column. (NOT OWN)
    void LogColumn(Simba::Support::ILogger* in_log) const
    {
        m_column->LogColumn(in_log);
    }

    /// @brief Indicates whether the column is auto-incrementing or not.
    ///
    /// @return True if the column is auto-incrementing; false otherwise.
    bool IsAutoUnique() const
    {
        return m_column->IsAutoUnique();
    }

    /// @brief Indicates whether the column is case sensitive.
    ///
    /// @return True if the column is case-sensitive; false otherwise.
    bool IsCaseSensitive() const
    {
        return m_column->IsCaseSensitive();
    }

    /// @brief Indicates whether the column may hold null data.
    ///
    /// @return An enum that specifies if the column can contain NULLs.
    Simba::DSI::DSINullable IsNullable() const
    {
        return m_column->IsNullable();
    }

    /// @brief Indicates whether the column contains a column alias or name.
    ///
    /// @return True if the column contains a column alias or name; false otherwise.
    bool IsUnnamed() const
    {
        return m_column->IsUnnamed();
    }

    /// @brief Indicates whether the column is sortable.
    ///
    /// @return True if the column is sortable.
    bool IsSortable() const
    {
        return m_column->IsSortable();
    }

    /// @brief Gets the collating coercibility of the column.
    ///
    /// @return An enum that specifies the coercibility of the column.
    Simba::DSI::DSICollatingCoercibility GetCollatingCoercibility() const
    {
        return m_column->GetCollatingCoercibility();
    }

    /// @brief Gets the collation of the column.
    ///
    /// @return The collation of the column.
    virtual SharedPtr<Simba::DSI::ICollation> GetCollation() const
    {
        return m_column->GetCollation();
    }

    /// @brief Create a hasher of the column that will be used by a hash-based join algorithm.
    /// 
    /// @param in_seed      Random seed used for hashing.
    ///
    /// @return A hasher pointer of the column. Null if no hasher is created for this column. 
    virtual AutoPtr<Simba::Support::IHasher> CreateHasher(simba_uint32 in_seed) const
    {
        return m_column->CreateHasher(in_seed);
    }

    // Private =====================================================================================
private:
    /// The column reference. (NOT OWN)
    Simba::DSI::IColumn* m_column;
};

ISYS_SQL_NAMESPACE_END
