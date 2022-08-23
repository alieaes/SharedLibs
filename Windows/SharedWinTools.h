#ifndef __HDR_SHARED_WIN_TOOLS__
#define __HDR_SHARED_WIN_TOOLS__

#include <string>
#include <windows.h>

#include "Shared_Defs.h"

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    namespace Windows
    {
        bool                           SetRegStrValue( HKEY hkey, const std::wstring& subKey, const std::wstring& valueName, const std::wstring& valueData, bool isWrite32Key );
        bool                           GetRegStrValue( HKEY hkey, const std::wstring& subKey, const std::wstring& sValueName, std::wstring& valueData, bool isRead32View );

        std::wstring                   GetProcessSID( uint32_t dwProcessID, bool* ok = NULLPTR );

        bool                           DeCompressUsing7Zip( const std::wstring& sDest, const std::wstring& sFileName );
    }
}

#endif