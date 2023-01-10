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

        VERSION_CHECK VersionCheck( XString sCurrentVersion, XString sNextVersion )
        {
            VERSION_CHECK eVersion = VERSION_UNKNOWN;

            auto vecCurrent = sCurrentVersion.split( "." );
            auto vecNext = sNextVersion.split( "." );

            do
            {
                if( vecCurrent.size() != vecNext.size() )
                {
                    eVersion = VERSION_UNMATCH;
                    break;
                }

                for( int idx = 0; idx < vecCurrent.size(); idx++ )
                {
                    if( vecCurrent[ idx ] == vecNext[ idx ] )
                    {
                        eVersion = VERSION_EQUAL;
                    }
                    else if( vecCurrent[ idx ] > vecNext[ idx ] )
                    {
                        eVersion = VERSION_HIGHER;
                        break;
                    }
                    else if( vecCurrent[ idx ] < vecNext[ idx ] )
                    {
                        eVersion = VERISON_LOWER;
                        break;
                    }
                    else
                    {
                        eVersion = VERSION_UNKNOWN;
                        break;
                    }
                }
            }
            while( false );

            return eVersion;
        }
    }
}
