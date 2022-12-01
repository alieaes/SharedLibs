#include "stdafx.h"

#include "String/SharedXString.h"

#include <Windows.h>

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

std::string XString::toString() const
{
    return Shared::String::ws2s( _str );
}

std::wstring XString::toWString()
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
    auto tmp = toLower();

    if( tmp == L"y" || tmp == L"true" || tmp == L"1" || tmp == L"yes" )
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
    std::transform( tmp.begin(), tmp.end(), tmp.begin(), towlower );

    return tmp;
}

XString XString::toUpper() const
{
    std::wstring tmp = _str;
    std::transform( tmp.begin(), tmp.end(), tmp.begin(), towupper );

    return tmp;
}

bool XString::IsEmpty() const
{
    return _str.empty();
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
        if( _str.find( find ) == -1 )
            break;
        else if( _str.find( find, idx ) <= idx )
            nCount += 1;
    }

    return nCount;
}

XString XString::substr( int nDst ) const
{
    return _str.substr( nDst );
}

XString XString::substr( int nSrc, int nDst ) const
{
    return _str.substr( nSrc, nDst );
}

int XString::find_last_of( XString xs ) const
{
    return _str.find_last_of( xs );
}

const wchar_t* XString::c_str() const
{
    return _str.c_str();
}
