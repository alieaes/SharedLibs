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
        typedef struct _dbInfo
        {
            auto operator<=>( _dbInfo const& ) const = default;

            XString                                 sDBName;
            XString                                 sFilePath;

        } DB_INFO;

        class cSQLiteMgr
        {
        public:
            cSQLiteMgr();

            bool                                   InitDB( DB_INFO dbInfo );
            bool                                   GetDB( XString sDBName );

        protected:
            bool                                   IsExistDBInfo( XString sDBName );

        private:
            std::map< XString, DB_INFO >           _mapNameToInfo;
            std::map< XString, sqlite3* >          _mapNameToSql;
        };
    }
}

typedef Shared::Singletons::Singleton<Shared::Sqlite::cSQLiteMgr> TyStSqliteMgr;

#endif