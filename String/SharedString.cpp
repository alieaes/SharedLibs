#include "stdafx.h"

#include "String/SharedString.h"

#include <codecvt>
#include <Windows.h>

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

#pragma warning(disable:4996)

namespace Shared
{
    namespace String
    {
        std::wstring a2ws(const std::string& str)
        {
            if( str.empty() == true )
                return std::wstring();

            int size_needed = MultiByteToWideChar( CP_ACP, 0, &str[ 0 ], ( int )str.size() + 1, NULL, 0 );
            std::wstring wstrTo( size_needed, 0 );

            MultiByteToWideChar( CP_ACP, 0, &str[ 0 ], ( int )str.size(), &wstrTo[ 0 ], size_needed );

            return wstrTo;
        }

        std::wstring s2ws( const std::string& str )
        {
            if( str.empty() == true ) 
                return std::wstring();

            int size_needed = MultiByteToWideChar( CP_UTF8, 0, &str[ 0 ], ( int )str.size() + 1, NULL, 0 );
            std::wstring wstrTo( size_needed, 0 );

            MultiByteToWideChar( CP_UTF8, 0, &str[ 0 ], ( int )str.size() + 1, &wstrTo[ 0 ], size_needed );
                
            return wstrTo;
        }

        wchar_t* c2wc( const char* c )
        {
            const size_t cSize = strlen( c ) + 1;
            wchar_t* wc = new wchar_t[ cSize ];
            mbstowcs( wc, c, cSize );

            return wc;
        }

        std::string ws2s( const std::wstring& wstr )
        {
            if( wstr.empty() == true )
                return std::string();

            int size_needed = WideCharToMultiByte( CP_UTF8, 0, &wstr[ 0 ], ( int )wstr.size(), NULL, 0, NULL, NULL );
            std::string strTo( size_needed, 0 );

            WideCharToMultiByte( CP_UTF8, 0, &wstr[ 0 ], ( int )wstr.size(), &strTo[ 0 ], size_needed, NULL, NULL );

            return strTo;
        }

        char* wc2c( const wchar_t* wc )
        {
            int size_needed = WideCharToMultiByte( CP_ACP, 0, wc, -1, NULL, 0, NULL, NULL );
            char* c = new char[ size_needed ];

            WideCharToMultiByte( CP_UTF8, 0, &wc[ 0 ], -1, c, size_needed, NULL, NULL );

            return c;
        }

        std::wstring u82ws( const std::string& utf8 )
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
            return wcu8.from_bytes( utf8 );
        }

        std::string ws2u8( const std::wstring& utf16 )
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
            return wcu8.to_bytes( utf16 );
        }

        bool EndsWith( const std::string& sFullString, const std::string& sEnds )
        {
            if( sFullString.length() >= sEnds.length() )
            {
                // fullString의 길이에서 ending의 길이만큼 뺀 위치부터 같은지 검사
                return ( 0 == sFullString.compare( sFullString.length() - sEnds.length(), sEnds.length(), sEnds ) ); 
            }
            else
            {
                // fullString의 길이보다 ending의 길이가 긴 경우 -> 항상 거짓
                return false;
            }
        }

        bool EndsWith( const std::wstring& sFullString, const std::wstring& sEnds )
        {
            if( sFullString.length() >= sEnds.length() )
            {
                // fullString의 길이에서 ending의 길이만큼 뺀 위치부터 같은지 검사
                return ( 0 == sFullString.compare( sFullString.length() - sEnds.length(), sEnds.length(), sEnds ) ); 
            }
            else
            {
                // fullString의 길이보다 ending의 길이가 긴 경우 -> 항상 거짓
                return false;
            }
        }
    }
}
