#include "stdafx.h"

#include "Qt/SharedQtUtils.h"

#include "Utils/SharedFile.h"

#include <Windows.h>

#ifdef USING_QT_LIBS
#include <qdatetime.h>
#endif

namespace Shared
{
    namespace Qt
    {
#ifdef USING_QT_LIBS
        bool DeCompressQTResources( const QString& sResPath, const QString& sFileName )
        {
            bool isSuccess = false;

            do
            {
                QFile resFile( sResPath );
                if( resFile.open( QIODevice::ReadOnly ) == false )
                    break;

                QFile compressor( QString( "%1/%2" ).arg( File::GetCurrentPath() ).arg( sFileName ) );
                if( compressor.open( QIODevice::WriteOnly ) == false )
                    break;

                compressor.write( resFile.readAll() );
                compressor.close();

                isSuccess = true;
            }
            while( false );

            return isSuccess;
        }
#endif
    }
}
