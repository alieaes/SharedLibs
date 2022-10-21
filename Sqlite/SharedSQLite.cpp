#include "stdafx.h"

#include "Utils/SharedFile.h"

#include "Sqlite/SharedSQLite.h"

#include "Sqlite/sqlite3.h"

#include <Windows.h>

#ifdef USING_QT_LIBS
#include <qdatetime.h>
#endif

namespace Shared
{
    namespace Sqlite
    {
        cSQLiteMgr::cSQLiteMgr()
        {
        }

        bool cSQLiteMgr::InitDB( DB_INFO dbInfo )
        {
            bool isSuccess = false;

            do
            {
                if( dbInfo.sFilePath.IsEmpty() == true || dbInfo.sDBName.IsEmpty() == true )
                    break;

                if( IsExistDBInfo( dbInfo.sDBName ) == true )
                {
                    isSuccess = true;
                    break;
                }

                bool isExist = false;

                if( File::IsExistFile( dbInfo.sFilePath ) == true )
                    isExist = true;

                sqlite3* db;
                sqlite3_stmt* res;

                int rc = sqlite3_open16( dbInfo.sFilePath.c_str(), &db );
            }
            while( false );

            return isSuccess;
        }

        bool cSQLiteMgr::GetDB( XString sDBName )
        {
            bool isSuccess = false;

            return isSuccess;
        }

        bool cSQLiteMgr::IsExistDBInfo( XString sDBName )
        {
            return true;
        }
    }
}


