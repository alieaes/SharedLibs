#ifndef __HDR_SHARED_PROCESS__
#define __HDR_SHARED_PROCESS__

#include "SharedBase.h"

#include "String/SharedString.h"
#include "String/SharedXString.h"

#include <string>

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    namespace Process
    {
        DWORD                   GetCurrentProcessID();
        DWORD                   GetPIDFromProcessName( const XString& sProcessName );
        std::vector< DWORD >    GetProcessIDList();

        bool                    ExecutionProcess( const XString& sFilePath );
        bool                    ExecutionUsingWinLogon( const XString& sFilePath );
        bool                    ExecutionAsAdministrator( const XString& sFilePath );
        bool                    IsServiceProcess( DWORD dwProcessId );
        bool                    IsElevatedProcess( DWORD dwProcessID );

    }
}

#endif