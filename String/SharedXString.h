﻿#ifndef __HDR_SHARED_XSTRING__
#define __HDR_SHARED_XSTRING__

#include <string>

#include "SharedBase.h"
#include "String/SharedString.h"

#ifdef USING_QT_LIBS
#include <qstring.h>
#endif // USING_QT_LIBS


class XString
{
public:
    XString()
    {
        _str = std::wstring();
    }

    XString( const char* c )
    {
        if( c == NULL )
            return;

        _str = Shared::String::s2ws( c );
        if( _str.empty() == false )
            _str.pop_back();
    }

    XString( const wchar_t* wc )
    {
        if( wc == NULL )
            return;

        _str = wc;
    }

    XString( const unsigned char* c )
    {
        if( c == NULL )
            return;

        std::string s = static_cast< std::string >( reinterpret_cast< const char* >( c ) ); // new style
        _str = std::wstring( s.begin(), s.end() );
    }

    XString( DWORD dw )
    {
        _str = std::to_wstring( dw );
    }

    XString( int n )
    {
        _str = std::to_wstring( n );
    }

    XString( const std::wstring& ws )
    {
        _str = ws;
    }

    XString( const std::string& s )
    {
        _str = std::wstring( s.begin(), s.end() );
    }

#ifdef USING_QT_LIBS
    XString( const QString& qs )
    {
        _str = qs.toStdWString();
    }
#endif // USING_QT_LIBS

    ~XString()
    {
    }

    auto operator<=>( XString const& ) const = default;

    /////////////// XString <- X ///////////////
    XString operator = ( std::wstring& ws )          // XString xs = std::wstring( L"TEST" );
    {
        _str = ws;
        return _str;
    }

    XString operator = ( std::string& s )            // XString xs = std::string( "TEST" );
    {
        _str = std::wstring( s.begin(), s.end() );
        return _str;
    }

    XString operator = ( const char* c )             // XString xs = "TEST";
    {
        _str = Shared::String::s2ws( c );
        if( _str.empty() == false )
            _str.pop_back();

        return _str;
    }

    XString operator = ( const unsigned char* c )             // XString xs = "TEST";
    {
        std::string s = static_cast< std::string >( reinterpret_cast< const char* >( c ) ); // new style

        _str = Shared::String::s2ws( s );
        if( _str.empty() == false )
            _str.pop_back();

        return _str;
    }

    XString operator = ( const wchar_t* wc )         // XString xs = L"TEST";
    {
        _str = wc;
        return _str;
    }

    XString operator = ( const XString& wc )         // XString xs = L"TEST";
    {
        _str = wc;
        return _str;
    }

#ifdef USING_QT_LIBS
    XString operator = ( QString& qs )               // XString xs = QString( "TEST" );
    {
        _str = qs.toStdWString();
        return _str;
    }
#endif  // USING_QT_LIBS
    ////////////////////////////////////////////

    /////////////// XString -> X ///////////////
    operator std::string() const { return Shared::String::ws2s( _str ); }
    operator std::wstring() const { return _str; }

#ifdef USING_QT_LIBS
    operator QString() const { return QString::fromStdWString( _str ); }
#endif  // USING_QT_LIBS
    ////////////////////////////////////////////

    XString operator+= ( XString& xs )
    {
        _str += xs.string();
        return _str;
    }

    XString operator +=( char c )
    {
        _str += std::to_wstring( c );
        return _str;
    }

    XString operator +=( const char* c )
    {
        _str += Shared::String::s2ws( c );
        return _str;
    }

    XString operator +( std::wstring& xs )
    {
        _str += xs;
        return _str;
    }

    XString operator +( const std::wstring& xs )
    {
        _str += xs;
        return _str;
    }

    bool operator ==( std::wstring& xs )
    {
        return xs.compare( _str ) == 0 ? true : false;
    }

    bool operator ==( const std::wstring& xs )
    {
        return xs.compare( _str ) == 0 ? true : false;
    }

    std::string                      toString() const;
    std::wstring                     toWString();
#ifdef USING_QT_LIBS
    QString                          toQString() const;
#endif  // USING_QT_LIBS
    // 값이 존재한다고 TRUE가 되지 않음, 1/Yes/True/y 의 값이여야 TRUE로 반환됨
    bool                             toBool() const;
    int                              toInt() const;
    long                             toLong() const;

    XString                          toLower() const;
    XString                          toUpper() const;

    std::vector< char >              toCharByte() const;

    bool                             IsEmpty() const;

    int                              size() const;
    int                              count( const XString& find ) const;

    XString                          substr( int nDst ) const;
    XString                          substr( int nSrc, int nDst ) const;

    int                              find_last_of( XString xs ) const;

    const wchar_t*                   c_str() const;

    std::wstring                     string() { return _str; }
protected:

private:
    std::wstring                     _str;
};

#endif
