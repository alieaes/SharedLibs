#ifndef __HDR_SHARED_XSTRING__
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
        _str = Shared::String::s2ws( c );
    }

    XString( const wchar_t* wc )
    {
        _str = wc;
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

    /////////////// XString <- X ///////////////
    XString operator = ( std::wstring& ws ) // XString xs = std::wstring( L"TEST" );
    {
        return XString( ws );
    }

    XString operator = ( std::string& s ) // XString xs = std::string( "TEST" );
    {
        return XString( s );
    }

    XString operator = ( const char* c ) // XString xs = "TEST";
    {
        return XString( c );
    }

    XString operator = ( const wchar_t* wc ) // XString xs = L"TEST";
    {
        return XString( wc );
    }

#ifdef USING_QT_LIBS
    XString operator = ( QString& qs ) // XString xs = QString( "TEST" );
    {
        return XString( qs );
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

    std::string                      toString();
    std::wstring                     toWString();

    // 값이 존재한다고 TRUE가 되지 않음, 1/Yes/True/y 의 값이여야 TRUE로 반환됨
    bool                             toBool();
    int                              toInt();
    long                             toLong();

    bool                             IsEmpty() const;

    int                              size() const;

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
