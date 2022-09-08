#ifndef __HDR_SHARED_SQLITE__
#define __HDR_SHARED_SQLITE__

#include "SharedBase.h"

#include <string>

#ifdef USING_QT_LIBS
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    namespace Sqlite
    {
        class cSQLiteMgr
        {
        public:
            cSQLiteMgr();
        };
    }
}

typedef Shared::Singletons::Singleton<Shared::Sqlite::cSQLiteMgr> TyStSqliteMgr;

#endif