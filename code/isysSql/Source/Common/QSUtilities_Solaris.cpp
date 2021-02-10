//==================================================================================================
///  @file QSUtilities_Linux.cpp
///
///  Implementation of the class QSUtilities for Windows.
///
///  Copyright (C) 2008-2015 Simba Technologies Incorporated.
//==================================================================================================

#include "QSUtilities.h"

#include "TabbedUnicodeFileReader.h"

#include <sys/stat.h>
#include <dirent.h>

using namespace Simba::Quickstart;
using namespace std;

// Helpers =========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Determine if the given file exists or not.
/// 
/// @param in_file              The file name to check for existence.
/// 
/// @return True if the file exists; false otherwise.
//////////////////////////////////////////////////////////////////////////////////////////////////// 
inline bool DoesExist(const simba_wstring& in_file)
{
    // Try to find the file.
    struct stat stat_st;
    return (0 == stat(in_file.GetAsPlatformString().c_str(), &stat_st));
}

// Public ==========================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////
QSUtilities::QSUtilities(QuickstartSettings* in_settings) : m_settings(in_settings)
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
QSUtilities::~QSUtilities()
{
    ; // Do nothing.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSUtilities::DoesTableDataExist(const simba_wstring& in_tableName) const
{
    return DoesExist(GetTableDataFullPathName(in_tableName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool QSUtilities::DoesTableMetadataExist(const simba_wstring& in_tableName) const
{
    return DoesExist(GetTableMetadataFullPathName(in_tableName));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSUtilities::GetTableDataFullPathName(const simba_wstring& in_fileName) const
{
#if defined (PLATFORM_IOS)
    return GetTableDataFullPathNameIOS(in_fileName) + QS_PATH_SEPARATOR + in_fileName + QS_DATAFILE_SUFFIX;
#endif
    return m_settings->m_dbfPath + QS_PATH_SEPARATOR + in_fileName + QS_DATAFILE_SUFFIX;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
simba_wstring QSUtilities::GetTableMetadataFullPathName(const simba_wstring& in_fileName) const
{
#if defined (PLATFORM_IOS)
    return GetTableDataFullPathNameIOS(in_fileName) + QS_PATH_SEPARATOR + in_fileName + QS_METADATAFILE_SUFFIX;
#endif
    return m_settings->m_dbfPath + QS_PATH_SEPARATOR + in_fileName + QS_METADATAFILE_SUFFIX;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void QSUtilities::GetTables(std::vector<simba_wstring>& out_tables) const
{
    // Open the directory to find the tables.
    DIR* directory = opendir(m_settings->m_dbfPath.GetAsPlatformString().c_str());

    if (NULL != directory)
    {
        try
        {
            // On Solaris, the struct dirent is defined with a d_name member that is only 1 byte so
            // it needs a pointer to enough storage to hold a large enough d_name. 
            void* buf[2048 / sizeof(void*)];
            struct dirent* entry = reinterpret_cast<struct dirent*>(buf);
            struct dirent* entryPtr = NULL;

            readdir_r(directory, entry, &entryPtr);
            while (NULL != entryPtr)
            {
                simba_string fileName(entry->d_name);
                simba_size_t nameLen = fileName.length();
                if ((QS_DATAFILE_SUFFIX.length() < nameLen) &&
                    (fileName.substr(nameLen - QS_DATAFILE_SUFFIX.length()) == QS_DATAFILE_SUFFIX))
                {
                    // Get the table name without the .dbf extension.
                    simba_int32 length =
                        static_cast<simba_int32>(fileName.size() - QS_DATAFILE_SUFFIX.length());
                    simba_wstring tableName = fileName.substr(0, length);

                    out_tables.push_back(tableName);
                }

                // Find the next table.
                readdir_r(directory, entry, &entryPtr);
            }

            // Close the directory.
            closedir(directory);
        }
        catch (...)
        {
            closedir(directory);
            throw;
        }
    }
}
