#ifndef __HDR_SHARED_STRING__
#define __HDR_SHARED_STRING__

#include <string>

namespace Shared
{
    namespace String
    {
        std::wstring                 s2ws( const std::string& str );
        std::string                  ws2s( const std::wstring& wstr );

        wchar_t*                     c2wc( const char* str );
        char*                        wc2c( const wchar_t* str );

        std::wstring                 u82ws( const std::string& utf8 );
        std::string                  ws2u8( const std::wstring& utf16 );

        bool                         EndsWith( const std::string& sFullString, const std::string& sEnds );
        bool                         EndsWith( const std::wstring& sFullString, const std::wstring& sEnds );
    }
}

#endif