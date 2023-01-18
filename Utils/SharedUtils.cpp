#include "stdafx.h"

#include "SharedBase.h"

#include "Utils/SharedFile.h"

#include <fstream>
#include <filesystem>

#include "String/SharedString.h"
#include "String/SharedXString.h"
#include "Utils/SharedUtils.h"
#include "String/SharedFormat.h"

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

        XString ConvertByte( ULONG64 uByte )
        {
            const char* cUnit[] = { "B", "KB", "MB", "GB", "TB", "PB" };
            int idx = 0;
            double dRet = 0;

            if( uByte >= 1024 )
            {
                dRet = uByte;
                for( idx = 0; dRet >= 1024; idx++ )
                    dRet /= 1024.0f;
            }

            double dInt = 0;
            auto fraction = modf( dRet, &dInt );
            XString sRet;

            if( fabs( fraction - 0 ) <= FLT_EPSILON )
            {
                sRet = Format::Format( "{}{}", ( int )dInt, cUnit[ idx ] );
            }
            else
            {
                XString sTmp = dRet;
                sTmp = sTmp.substr( 0, sTmp.find( "." ) + 3 );
                sRet = Format::Format( "{}{}", sTmp, cUnit[ idx ] );
            }

            return sRet;
        }
    }
}
