#ifndef __HDR_SHARED_TIME_CHECKER__
#define __HDR_SHARED_TIME_CHECKER__

#include <sstream>
#include <vector>

#include "String/SharedXString.h"

#define CHECK_TIME_START \
    cTimeChecker cTimeCheckerVariable; \
    CONSOLEP( "[TimeChecker][START] {} ", __FUNCTION__ ); \
    cTimeCheckerVariable.Start();

#define CHECK_TIME_START_WITH( print ) \
    cTimeChecker cTimeCheckerVariable; \
    CONSOLEP( "[TimeChecker][START]({}) {} ", print, __FUNCTION__ ); \
    cTimeCheckerVariable.Start( print );

#define CHECK_TIME_POINT cTimeCheckerVariable.CheckPoint();
#define CHECK_TIME_POINT_LOG \
    CONSOLEP( "[TimeChecker][POINT] {} | {}s", __FUNCTION__, cTimeCheckerVariable.CheckPoint() );

#define CHECK_TIME_END cTimeCheckerVariable.End();
#define CHECK_TIME_END_LOG                                                                            \
    if( cTimeCheckerVariable.IsUseSeparator() == true )                                               \
    {                                                                                                 \
        CONSOLEP( "[TimeChecker][ END ]({}) {} | {}s"                                                 \
                  , cTimeCheckerVariable.GetSeparator() ,__FUNCTION__, cTimeCheckerVariable.End() );  \
    }                                                                                                 \
    else                                                                                              \
    {                                                                                                 \
        CONSOLEP( "[TimeChecker][ END ] {} | {}s", __FUNCTION__, cTimeCheckerVariable.End() );        \
    }                                                                                                 \


class cTimeChecker
{
public:
    cTimeChecker();
    ~cTimeChecker() {};

    void                     Start();
    void                     Start( XString sSeparator );
    double                   CheckPoint();
    double                   End();

    XString                  GetSeparator() { return _sSeparator; }
    bool                     IsUseSeparator() { return _bUseSeparator; }

private:
    clock_t                   _startTime;
    XString                   _sSeparator;

    bool                      _bUseSeparator = false;
};

#endif
