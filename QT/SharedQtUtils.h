#ifndef __HDR_SHARED_QT_UTILS__
#define __HDR_SHARED_QT_UTILS__

#include "SharedBase.h"

#include <string>

#ifdef USING_QT_LIBS
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    namespace Qt
    {
#ifdef USING_QT_LIBS
        bool                         DeCompressQTResources( const QString& sResPath, const QString& sFileName );
#endif
    }
}

#endif