#ifndef __HDR_SHARED_SQLITE__
#define __HDR_SHARED_SQLITE__

#include "SharedBase.h"

#include "String/SharedXString.h"

#include <string>

#include "sqlite3.h"

#ifdef USING_QT_LIBS
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    namespace Sqlite
    {
        class cSQLiteDB
        {
        public:
            cSQLiteDB( sqlite3* db );
            bool                                   SetDB( sqlite3* db );
            sqlite3*                               Data();
            bool                                   PrepareSQL( sqlite3_stmt* stmt, const XString& sQuery );
            bool                                   BindValue( sqlite3_stmt* stmt, const XString& sBind, const XString& sValue );
            bool                                   ExecuteSQL( sqlite3_stmt* stmt, const XString& sQuery );
            bool                                   Next();

        private:
            sqlite3*                               _db;
        };

        typedef std::shared_ptr<cSQLiteDB> spSQLiteDB;

        typedef struct _dbInfo
        {
            auto operator<=>( _dbInfo const& ) const = default;

            XString                                 sDBName;
            XString                                 sFilePath;

            spSQLiteDB                              spSQLite;

        } DB_INFO;

        class cSQLiteMgr
        {
        public:
            cSQLiteMgr();

            bool                                   InitDB( DB_INFO dbInfo );
            spSQLiteDB                             GetDB( XString sDBName );
            bool                                   Close( XString sDBName );


        protected:
            bool                                   IsExistDBInfo( XString sDBName );

        private:
            std::map< XString, DB_INFO >           _mapNameToInfo;
        };

    }
}

typedef Shared::Singletons::Singleton<Shared::Sqlite::cSQLiteMgr> TyStSqliteMgr;

#endif