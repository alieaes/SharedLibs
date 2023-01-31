#include "stdafx.h"

#include "Network/SharedFileDownload.h"

#include "SharedBase.h"

#include "Utils/SharedFile.h"

#include <Windows.h>
#include <iostream>

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

namespace Shared
{
    namespace Network
    {
        cFileDownloader::cFileDownloader( XString sName )
        {
            Init();
        }

        cFileDownloader::~cFileDownloader()
        {
            if( _hInternet != NULLPTR )
            {
                InternetCloseHandle( _hInternet );
                _hInternet = NULLPTR;
            }
        }

        bool cFileDownloader::Init()
        {
            bool isSuccess = false;

            do
            {
                _hInternet = InternetOpen( _sName, INTERNET_OPEN_TYPE_DIRECT, NULLPTR, NULLPTR, 0 );

                if( _hInternet == NULLPTR )
                    CONSOLEP( "InternetOpen Failed.." );

                isSuccess = true;
                _isInit = true;
            }
            while( false );
            
            return isSuccess;
        }

        ULONG64 cFileDownloader::GetFileSize( const XString& sUrl )
        {
            TCHAR buffer[ 32 ];
            DWORD dwRead = 32;
            DWORD dwFileSize = 0;
            HINTERNET hURL = NULLPTR;

            do
            {
                HINTERNET hURL = InternetOpenUrl( _hInternet, sUrl, NULL, 0, 0, 0 );

                bool isSuccess = HttpQueryInfo( hURL, HTTP_QUERY_CONTENT_LENGTH, buffer, &dwRead, NULL );

                if( isSuccess == false )
                    break;

                dwFileSize = atoi( ( const char* )buffer );
            }
            while( false );

            if( hURL != NULLPTR )
                InternetCloseHandle( hURL );

            return dwFileSize;
        }

        bool cFileDownloader::DownloadFile( const XString& sUrl, const XString& sSaveFileFullPath )
        {
            _ulDownLoadSize = 0;
            bool isSuccess = false;
            HINTERNET hOpenUrl = NULLPTR;
            do
            {
                if( _hInternet == NULLPTR )
                    break;

                if( sUrl.IsEmpty() == true || sSaveFileFullPath.IsEmpty() == true )
                    break;

                XString sFilePath = File::FindFilePath( sSaveFileFullPath );

                if( File::IsExistDir( sFilePath ) == false )
                {
                    if( File::MakeDir( sFilePath ) == false )
                        break;
                }

                hOpenUrl = InternetOpenUrl( _hInternet, sUrl, nullptr, 0, INTERNET_FLAG_RELOAD, 0 );

                if( hOpenUrl == NULLPTR )
                    break;

                DWORD dwStatus = 0;
                DWORD dwStatusSize = sizeof( dwStatus );

                if( HttpQueryInfo( hOpenUrl, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwStatusSize, NULLPTR ) == FALSE )
                {
                    //CONSOLEP( "HttpQueryInfo failed.. {}", ::GetLastError() );
                    break;
                }

                if( sizeof( dwStatus ) != dwStatusSize )
                {
                    //CONSOLEP( "statusSize unMatch" );
                    break;
                }

                if( dwStatus != HTTP_STATUS_OK )
                {
                    //CONSOLEP( "HttpQueryInfo Error {}", dwStatus );
                    break;
                }

                const int nMaxBufferSize = 2048;
                char buffer[ nMaxBufferSize ] = {};

                HANDLE hFile = CreateFile( sSaveFileFullPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
                if( hFile == INVALID_HANDLE_VALUE )
                    break;

                DWORD dwReadBytes = 0;
                DWORD dwWrite = 0;
                bool bDone = true;

                do
                {
                    _isProcesss = true;

                    DWORD dwAvailable = 0;
                    if( InternetQueryDataAvailable( hOpenUrl, &dwAvailable, 0, 0 ) == FALSE )
                    {
                        bDone = false;
                        break;
                    }

                    if( dwAvailable > nMaxBufferSize )
                        dwAvailable = nMaxBufferSize;

                    if( InternetReadFile( hOpenUrl, buffer, dwAvailable, &dwReadBytes ) == FALSE )
                    {
                        bDone = false;
                        break;
                    }

                    if( dwReadBytes > 0 )
                    {
                        if( WriteFile( hFile, buffer, dwReadBytes, &dwWrite, NULL ) == FALSE )
                        {
                            bDone = false;
                            break;
                        }
                        _ulDownLoadSize += dwReadBytes;
                        memset( buffer, 0x00, dwReadBytes );
                    }

                } while( dwReadBytes != 0 );

                _isProcesss = false;

                if( hFile != NULLPTR )
                    CloseHandle( hFile );

                if( bDone == false )
                    File::RemoveFile( sSaveFileFullPath );

                isSuccess = bDone;

            } while( false );

            if( hOpenUrl != NULLPTR )
                InternetCloseHandle( hOpenUrl );

            return isSuccess;
        }

        bool cFileDownloader::DownloadFile( const XString& sUrl, const XString& sSaveFilePath, const XString& sSaveFileName )
        {
            return DownloadFile( sUrl, Format::Format( "{}\\{}", sSaveFilePath, sSaveFileName ) );
        }

        ULONG64 cFileDownloader::GetDownLoadSize()
        {
            if( _isProcesss == true )
                return _ulDownLoadSize;
            else
                return 0;
        }
    }
}
