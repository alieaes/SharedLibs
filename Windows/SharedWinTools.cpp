#include "stdafx.h"

#include "Windows/SharedWinTools.h"
#include "Utils/SharedFile.h"

#include <Windows.h>
#include <sddl.h>

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

namespace Shared
{
    namespace Windows
    {
        bool SetRegStrValue( HKEY hkey, const std::wstring& subKey, const std::wstring& valueName, const std::wstring& valueData, bool isWrite32Key )
        {
            DWORD dwFlags = KEY_WRITE;
            HKEY key = NULL;
            bool isSuccess = false;

            if( isWrite32Key == true )
                dwFlags |= KEY_WOW64_32KEY;
            else
                dwFlags |= KEY_WOW64_64KEY;

            LONG lErr = RegOpenKeyExW( hkey, subKey.c_str(), 0, dwFlags, &key );
            isSuccess = lErr == ERROR_SUCCESS;
            if( isSuccess == false )
            {
                if( lErr == ERROR_FILE_NOT_FOUND )
                {
                    lErr = RegCreateKeyExW( hkey, subKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, dwFlags, NULL, &key, NULL );
                }
            }

            if( lErr != ERROR_SUCCESS )
                return isSuccess;

            lErr = RegSetValueExW( key, valueName.c_str(), 0, REG_SZ, ( PBYTE )( valueData.c_str() ), sizeof( WCHAR ) * ( DWORD )( ( valueData.size() + 1 ) ) );
            isSuccess = lErr == ERROR_SUCCESS;

            if( key != NULL )
                RegCloseKey( key );

            return isSuccess;
        }

        bool GetRegStrValue( HKEY hkey, const std::wstring& subKey, const std::wstring& sValueName, std::wstring& valueData, bool isRead32View )
        {
            HKEY key = NULL;
            bool isSuccess = false;

            do
            {
                valueData.clear();
                DWORD dwFlags = KEY_READ;

                if( isRead32View == true )
                    dwFlags |= KEY_WOW64_32KEY;
                else
                    dwFlags |= KEY_WOW64_64KEY;

                LONG lErr = RegOpenKeyExW( hkey, subKey.c_str(), 0, dwFlags, &key );
                isSuccess = lErr == ERROR_SUCCESS;

                if( isSuccess == false )
                    break;

                DWORD dwDataLength = 0;
                DWORD dwDataType = 0;

                lErr = RegQueryValueExW( key, sValueName.c_str(), 0, &dwDataType, NULL, &dwDataLength );
                isSuccess = lErr == ERROR_SUCCESS;

                if( isSuccess == false )
                    break;

                switch( dwDataType )
                {
                    case REG_SZ:
                    case REG_EXPAND_SZ:
                    case REG_MULTI_SZ:
                    {
                        std::vector< wchar_t > vecRetVal;
                        vecRetVal.resize( dwDataLength + 1 );

                        lErr = RegQueryValueExW( key, sValueName.c_str(), 0, NULL, ( LPBYTE )&vecRetVal[ 0 ], &dwDataLength );
                        isSuccess = lErr == ERROR_SUCCESS;

                        if( isSuccess == false )
                            break;

                        if( dwDataType != REG_MULTI_SZ )
                            valueData = std::wstring( &vecRetVal[ 0 ] );
                        else if( dwDataType == REG_MULTI_SZ )
                            valueData.assign( &vecRetVal[ 0 ], vecRetVal.size() );

                        isSuccess = true;
                        break;
                    }
                    case REG_DWORD:
                    case REG_QWORD:
                    {
                        unsigned __int64 retVal = 0;

                        lErr = RegQueryValueExW( key, sValueName.c_str(), 0, NULL, ( LPBYTE )&retVal, &dwDataLength );
                        isSuccess = lErr == ERROR_SUCCESS;

                        if( isSuccess == false )
                            break;

                        wchar_t wszBuffer[ 1024 ] = { 0, };
                        errno_t err = _ui64tow_s( retVal, wszBuffer, 1024, 10 );
                        if( err == 0 )
                        {
                            isSuccess = true;
                            valueData = wszBuffer;
                        }

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

            } while( false );

            if( key != NULL )
                RegCloseKey( key );

            return isSuccess;
        }

        std::wstring GetProcessSID( uint32_t dwProcessID, bool* ok /*= NULLPTR */ )
        {
            std::wstring sProcessSID;

            HANDLE hProcess = NULL;
            HANDLE hProcessToken = NULL;
            PTOKEN_USER pUserToken = NULL;

            do
            {
                if( ok != NULLPTR )
                    *ok = false;

                hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, dwProcessID );
                if( hProcess == NULL )
                    break;

                if( OpenProcessToken( hProcess, TOKEN_READ, &hProcessToken ) == FALSE || hProcessToken == NULL )
                    break;

                DWORD dwProcessTokenInfoAllocSize = 0;
                GetTokenInformation( hProcessToken, TokenUser, NULL, 0, &dwProcessTokenInfoAllocSize );
                if( ::GetLastError() != ERROR_INSUFFICIENT_BUFFER )
                    break;

                pUserToken = reinterpret_cast< PTOKEN_USER >( new BYTE[ dwProcessTokenInfoAllocSize ] );
                if( pUserToken == NULL )
                    break;

                if( GetTokenInformation( hProcessToken, TokenUser, pUserToken, dwProcessTokenInfoAllocSize, &dwProcessTokenInfoAllocSize ) == FALSE )
                    break;

                LPTSTR sidString = NULLPTR;
                if( ConvertSidToStringSidW( pUserToken->User.Sid, &sidString ) == FALSE )
                    break;

                sProcessSID = sidString;
                LocalFree( sidString );

                if( ok != NULLPTR )
                    *ok = true;

            } while( false );

            if( pUserToken != NULL )
                delete[] pUserToken;
            pUserToken = NULL;

            if( hProcess != NULL )
                CloseHandle( hProcess );
            hProcess = NULL;

            if( hProcessToken != NULL )
                CloseHandle( hProcessToken );
            hProcessToken = NULL;

            return sProcessSID;
        }

        bool DeCompressUsing7Zip( const std::wstring& sDest, const std::wstring& sFileName )
        {
            bool isSuccess = false;

            if( File::IsExistDir( sDest ) == false )
                CreateDirectory( sDest.c_str(), NULL );

            std::wstring sArgs = std::format( L"{}/7za.exe x -y -o\"{}\" \"{}\"", File::GetCurrentPath().toWString(), sDest, sFileName );

            std::replace( sArgs.begin(), sArgs.end(), L'/', L'\\' );

            WCHAR* wsz7zFile = new WCHAR[ sArgs.size() + 1 ];
            wcscpy_s( wsz7zFile, sArgs.size() + 1, sArgs.c_str() );

            do
            {
                STARTUPINFO si;
                PROCESS_INFORMATION pi;

                ZeroMemory( &si, sizeof( si ) );
                si.cb = sizeof( si );
                ZeroMemory( &pi, sizeof( pi ) );
                si.dwFlags = STARTF_USESHOWWINDOW;
                si.wShowWindow = SW_HIDE;

                if( CreateProcessW( NULL,   // No module name (use command line). 
                                    wsz7zFile, // Command line. 
                                    NULL,             // Process handle not inheritable. 
                                    NULL,             // Thread handle not inheritable. 
                                    FALSE,            // Set handle inheritance to FALSE. 
                                    0,                // No creation flags. 
                                    NULL,             // Use parent's environment block. 
                                    NULL,             // Use parent's starting directory. 
                                    &si,              // Pointer to STARTUPINFO structure.
                                    &pi              // Pointer to PROCESS_INFORMATION structure.
                ) == FALSE )
                {
                    break;
                }

                isSuccess = true;

                WaitForSingleObject( pi.hProcess, INFINITE );

                CloseHandle( pi.hProcess );
                CloseHandle( pi.hThread );
            }
            while( false );

            if( wsz7zFile )
                delete[] wsz7zFile;

            return isSuccess;
        }

        bool CreateWindowsService( const XString& sSvcName, const XString& sDisplay, const XString& sFileFullPath )
        {
            bool isSuccess = false;

            SC_HANDLE hMgr = NULL;
            SC_HANDLE hSvc = NULL;

            do
            {
                hMgr = OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
                if( hMgr == NULL )
                    break;

                hSvc = CreateServiceW( hMgr, sSvcName, sDisplay,
                                       SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
                                       sFileFullPath,
                                       NULL, NULL, NULL, NULL, NULL );

                if( hSvc == NULL )
                    break;

                isSuccess = true;

            } while( false );

            if( hSvc != NULL )
                CloseServiceHandle( hSvc );

            if( hMgr != NULL )
                CloseServiceHandle( hMgr );

            return isSuccess;
        }

        bool RunWindowsService( const XString& sSvcName )
        {
            bool isSuccess = false;

            SC_HANDLE hMgr = NULL;
            SC_HANDLE hSvc = NULL;

            do
            {
                hMgr = OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
                if( hMgr == NULL )
                    break;

                hSvc = OpenServiceW( hMgr, sSvcName, SERVICE_ALL_ACCESS );
                if( hSvc == NULL )
                    break;

                isSuccess = StartServiceW( hSvc, 0, NULL ) == TRUE ? true : false;

            } while( false );

            if( hSvc != NULL )
                CloseServiceHandle( hSvc );

            if( hMgr != NULL )
                CloseServiceHandle( hMgr );

            return isSuccess;
        }

        bool RemoveWindowsService( const XString& sSvcName )
        {
            bool isSuccess = false;

            SC_HANDLE hMgr = NULL;
            SC_HANDLE hSvc = NULL;

            do
            {
                hMgr = OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
                if( hMgr == NULL )
                    break;

                hSvc = OpenServiceW( hMgr, sSvcName, SERVICE_ALL_ACCESS );
                if( hSvc == NULL )
                    break;

                isSuccess = DeleteService( hSvc ) == TRUE ? true : false;

            } while( false );

            if( hSvc != NULL )
                CloseServiceHandle( hSvc );

            if( hMgr != NULL )
                CloseServiceHandle( hMgr );

            return isSuccess;
        }

        bool IsRunningService( const XString& sSvcName )
        {
            bool isSuccess = false;

            SC_HANDLE hMgr = NULL;
            SC_HANDLE hSvc = NULL;
            SERVICE_STATUS status;

            do
            {
                hMgr = OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
                if( hMgr == NULL )
                    break;

                hSvc = OpenServiceW( hMgr, sSvcName, SERVICE_ALL_ACCESS );
                if( hSvc == NULL )
                    break;

                QueryServiceStatus( hSvc, &status );

                if( status.dwCurrentState == SERVICE_RUNNING )
                    isSuccess = true;

            } while( false );

            if( hSvc != NULL )
                CloseServiceHandle( hSvc );

            if( hMgr != NULL )
                CloseServiceHandle( hMgr );

            return isSuccess;
        }

        bool IsExistService( const XString& sSvcName )
        {
            bool isSuccess = false;

            SC_HANDLE hMgr = NULL;
            SC_HANDLE hSvc = NULL;
            SERVICE_STATUS status;

            do
            {
                hMgr = OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
                if( hMgr == NULL )
                    break;

                hSvc = OpenServiceW( hMgr, sSvcName, SERVICE_ALL_ACCESS );
                if( hSvc == NULL )
                    break;

                hSvc = OpenServiceW( hMgr, sSvcName, SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS );
                if( hSvc == NULL )
                {
                    if( ::GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST )
                    break;
                }

                isSuccess = true;

            } while( false );

            if( hSvc != NULL )
                CloseServiceHandle( hSvc );

            if( hMgr != NULL )
                CloseServiceHandle( hMgr );

            return isSuccess;
        }
    }
}
