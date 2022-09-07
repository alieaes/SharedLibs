#ifndef __HDR_SHARED_FORMAT__
#define __HDR_SHARED_FORMAT__

#include <string>

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    namespace Format
    {
        class SFormat
        {
        public:
            SFormat() {};
            ~SFormat() {};

            void Format( const std::wstring& sStr, ... );
        };
    }
}

#endif