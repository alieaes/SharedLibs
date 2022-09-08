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

bool XString::toBool() const
{
    std::wstring tmp = _str;
    std::transform( tmp.begin(), tmp.end(), tmp.begin(), towlower );

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

bool XString::IsEmpty() const
{
    return _str.empty();
}

int XString::size() const
{
    return _str.size();
}

XString XString::substr(int nDst) const
{
    return _str.substr( nDst );
}

XString XString::substr(int nSrc, int nDst) const
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
