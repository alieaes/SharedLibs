#ifndef __HDR_SHARED_STRING__
#define __HDR_SHARED_STRING__

#include <string>

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    namespace String
    {
        std::wstring                 s2ws( const std::string& str );
        std::string                  ws2s( const std::wstring& wstr );

        wchar_t*                     c2wc( const char* str );
        char*                        wc2c( const wchar_t* str );

        bool                         EndsWith( const std::string& sFullString, const std::string& sEnds );
        bool                         EndsWith( const std::wstring& sFullString, const std::wstring& sEnds );
    }
}

#endif