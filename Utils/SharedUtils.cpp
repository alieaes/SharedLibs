#include "stdafx.h"

#include "SharedBase.h"

#include "Utils/SharedFile.h"

#include <fstream>
#include <filesystem>

#include "String/SharedString.h"
#include "String/SharedXString.h"
#include "Utils/SharedUtils.h"

#include <Windows.h>
#include <Rpc.h>
#pragma comment(lib, "Rpcrt4.lib")

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

namespace Shared
{
    namespace Utils
    {
        XString CreateUUID( bool bUpper /*= true*/ )
        {
            XString sRet;

            UUID uuid;
            UuidCreate( &uuid );
            char* strUUID;
            UuidToStringA( &uuid, ( RPC_CSTR* )&strUUID );

            sRet = strUUID;

            if( bUpper == true )
                sRet = sRet.toUpper();

            return sRet;
        }
    }
}
