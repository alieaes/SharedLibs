#include "stdafx.h"

#include "Logger/SharedLogger.h"
#include "Utils/SharedFile.h"

#include <Windows.h>

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

namespace Shared
{
    namespace Logger
    {
        cLogger::cLogger()
        {
        }

        cLogger::~cLogger()
        {
        }

        bool cLogger::Init( LOGGER_INFO info )
        {
            bool isSuccess = false;

            do
            {
                if( File::IsExistFile( info.sFilePath ) == false )
                    break;

                _info = info;
                isSuccess = true;
            }
            while( false );
            
            return isSuccess;
        }

        void cLogger::LoggingCase( LOG_CASE eCase, XString sFileName, int nCodeLine, XString sStr )
        {
            time_t now = time( 0 );
            struct tm  tstruct;
            char       timeForm[ 80 ];
            localtime_s( &tstruct, &now );
            strftime( timeForm, sizeof( timeForm ), "%Y-%m-%d %X", &tstruct ); // YYYY-MM-DD.HH:mm:ss

            XString sLog = Format::Format( "[{}] <{}:{}> {} \n", timeForm, strrchr( sFileName.toString().c_str(), '\\' ) + 1, nCodeLine, sStr );

            switch( eCase )
            {
                case LOG_TYPE_NONE: break;
                case LOG_TYPE_CONSOLE: 
                {
                    DWORD dwNumberOfCharsWritten = 0;
                    OutputDebugStringW( sLog );
                    WriteConsoleW( GetStdHandle( STD_OUTPUT_HANDLE ), sLog, ( DWORD )sLog.size(), &dwNumberOfCharsWritten, NULL );
                } break;
                case LOG_TYPE_FILE: break;
                case LOG_TYPE_ALL:
                {
                    DWORD dwNumberOfCharsWritten = 0;
                    OutputDebugStringW( sLog );
                    WriteConsoleW( GetStdHandle( STD_OUTPUT_HANDLE ), sLog, ( DWORD )sLog.size(), &dwNumberOfCharsWritten, NULL );
                } break;
                default: break;
            }
        }
    }
}
