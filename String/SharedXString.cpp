#include "stdafx.h"

#include "String/SharedXString.h"

#include <algorithm>
#include <codecvt>
#include <Windows.h>

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

#pragma warning(disable:4996)

std::string XString::toString() const
{
    return Shared::String::ws2s( _str );
}

std::wstring XString::toWString() const
{
    return _str;
}

#ifdef USING_QT_LIBS
QString XString::toQString() const
{
    return QString::fromStdWString( _str );
}
#endif

bool XString::toBool() const
{
    if( auto tmp = toLower(); tmp == L"y" || tmp == L"true" || tmp == L"1" || tmp == L"yes" )
        return true;

    return false;
}

int XString::toInt() const
{
    return std::stoi( _str );
}

long XString::toLong() const
{
    return std::stol( _str );
}

XString XString::toLower() const
{
    std::wstring tmp = _str;
    std::ranges::transform( tmp, tmp.begin(), towlower );

    return tmp;
}

XString XString::toUpper() const
{
    std::wstring tmp = _str;
    std::ranges::transform( tmp, tmp.begin(), towupper );

    return tmp;
}

std::vector<char> XString::toCharByte() const
{
    std::wstring tmp = _str;

    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string string = converter.to_bytes( tmp );
    std::vector< char > ret( string.begin(), string.end() );

    return ret;
}

bool XString::IsEmpty() const
{
    return _str.empty();
}

bool XString::Endl() const
{
    return _nPos > size() ? true : false;
}

int XString::size() const
{
    return _str.size();
}

int XString::count( const XString& find ) const
{
    int nCount = 0;

    if( find.IsEmpty() == true )
        return nCount;

    for( int idx = 0; idx < _str.size(); idx++ )
    {
        if( _str.find( find.toWString() ) == -1 )
            break;
        else if( _str.find( find.toWString(), idx ) <= idx )
            nCount += 1;
    }

    return nCount;
}

XString XString::substr( int nDst ) const
{
    return _str.substr( nDst );
}

XString XString::substr( int nSrc, int nSize ) const
{
    return _str.substr( nSrc, nSize );
}

int XString::find_last_of( XString xs ) const
{
    return _str.find_last_of( xs.toWString() );
}

int XString::find( XString xs ) const
{
    return _str.find( xs.toWString() );
}

const wchar_t* XString::c_str() const
{
    return _str.c_str();
}

std::vector<XString> XString::split( const XString& sSplit ) const
{
    std::wstring tmp = _str;
    std::vector<XString> vecList;
    std::string::size_type st = tmp.find( sSplit.toWString() );
    std::string::size_type nCurrent = 0;

    while( st != std::string::npos )
    {
        vecList.push_back( tmp.substr( nCurrent, st - nCurrent ) );

        nCurrent = st + 1;
        st = tmp.find( sSplit.toWString(), st + 1 );
    }

    return vecList;
}

XString XString::replaceAll( const XString& xa, const XString& xb ) const
{
    std::wstring tmp = _str;

    for( int idx = tmp.find( xa.toWString() ); idx >= 0; idx = tmp.find( xa.toWString() ) )
        tmp.replace( idx, xa.size(), xb.toWString() );

    return tmp;
}

XString XString::replace( const XString& xa, const XString& xb ) const
{
    std::wstring tmp = _str;
    tmp.replace( tmp.find( xa.toWString() ), xa.size(), xb.toWString() );

    return tmp;
}

int XString::compare( const XString& xs, bool isCaseInsensitive /*= false*/ ) const
{
    if( isCaseInsensitive == true )
    {
        const std::wstring xa = toLower();
        const std::wstring xb = xs.toLower();

        return xa.compare( xb );
    }
    else
    {
        return _str.compare( xs.toWString() );
    }
}

bool XString::contains( const XString& xs, bool isCaseInsensitive /*= false*/ ) const
{
    if( isCaseInsensitive == true )
    {
        const std::wstring tmp = toLower();
        return tmp.contains( xs.toLower() );
    }
    return _str.contains( xs );
}

bool XString::startswith( const XString& xs, bool isCaseInsensitive /*= false*/ ) const
{
    if( isCaseInsensitive == true )
    {
        const std::wstring tmp = toLower();
        return tmp.starts_with( xs.toLower() );
    }
    return _str.starts_with( xs );
}

bool XString::endswith( const XString& xs, bool isCaseInsensitive /*= false*/ ) const
{
    if( isCaseInsensitive == true )
    {
        const std::wstring tmp = toLower();
        return tmp.ends_with( xs.toLower() );
    }
    return _str.ends_with( xs );
}
