#ifndef __HDR_SHARED_WIN_TOOLS__
#define __HDR_SHARED_WIN_TOOLS__

#include <string>
#include <windows.h>

#include "Shared_Defs.h"
#include "String/SharedXString.h"

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB

class XString;

typedef std::pair< bool, XString > prResultString;

namespace Shared
{
    namespace Windows
    {
        bool                           SetRegStrValue( HKEY hkey, const XString& sSubKey, const XString& sValueName, const XString& valueData, bool is32Key );
        bool                           SetRegDWORDValue( HKEY hkey, const XString& sSubKey, const XString& sValueName, const DWORD valueData, bool is32Key );

        prResultString                 GetRegValue( HKEY hkey, const XString& sSubKey, const XString& sValueName, bool is32Key );

        std::wstring                   GetProcessSID( uint32_t dwProcessID, bool* ok = NULLPTR );

        bool                           DeCompressUsing7Zip( const std::wstring& sDest, const std::wstring& sFileName );

        bool                           CreateWindowsService( const XString& sSvcName, const XString& sDisplay, const XString& sFileFullPath );
        bool                           RunWindowsService( const XString& sSvcName );
        bool                           RemoveWindowsService( const XString& sSvcName );
        bool                           IsRunningService( const XString& sSvcName );
        bool                           IsExistService( const XString& sSvcName );
        bool                           StopWindowsService( const XString& sSvcName );

        bool                           SetPrivilege( LPCTSTR lpszPrivilege, BOOL isEnable );
    }
}

#endif