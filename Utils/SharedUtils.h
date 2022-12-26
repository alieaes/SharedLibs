#ifndef __HDR_SHARED_UTILS__
#define __HDR_SHARED_UTILS__

#include "SharedBase.h"

#include "String/SharedString.h"
#include "String/SharedXString.h"

#include <string>

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    namespace Utils
    {
        typedef enum _tyVersionCheck
        {
            VERSION_UNKNOWN = 0,
            VERSION_EQUAL   = 1,
            VERISON_LOWER   = 2,
            VERSION_HIGHER  = 3,
            VERSION_UNMATCH = 4
        } VERSION_CHECK;

        XString                 CreateUUID( bool bUpper = true );

        VERSION_CHECK           VersionCheck( XString sCurrentVersion, XString sNextVersion );
    }
}

#endif