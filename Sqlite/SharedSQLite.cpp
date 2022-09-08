#include "stdafx.h"

#include "Sqlite/SharedSQLite.h"

#include "Sqlite/sqlite3.h"
#include "Sqlite/sqlite3ext.h"

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
    }
}


