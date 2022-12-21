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
        XString                 CreateUUID( bool bUpper = true );
    }
}

#endif