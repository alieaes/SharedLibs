#include "stdafx.h"

#include "Utils/SharedTimeChecker.h"

#include <Windows.h>

#include "String/SharedXString.h"

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

#pragma warning(disable:4996)

cTimeChecker::cTimeChecker()
{
}

void cTimeChecker::Start()
{
    _startTime = clock();
}

void cTimeChecker::Start( XString sSeparator )
{
    _startTime = clock();

    _sSeparator = sSeparator;
    _bUseSeparator = true;
}

double cTimeChecker::CheckPoint()
{
    clock_t check = clock();

    double ret = static_cast< double >( check - _startTime ) / CLOCKS_PER_SEC;
    return ret;
}

double cTimeChecker::End()
{
    clock_t check = clock();

    double ret = static_cast< double >( check - _startTime ) / CLOCKS_PER_SEC;
    return ret;
}
