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
        bool SetRegStrValue( HKEY hkey, const XString& sSubKey, const XString& sValueName, const XString& valueData, bool is32Key )
        {
            bool isSuccess = false;
            REGSAM accessMask = KEY_WRITE;
            HKEY key = NULL;

            do
            {
                if( is32Key == true )
                    accessMask |= KEY_WOW64_32KEY;
                else
                    accessMask |= KEY_WOW64_64KEY;

                LSTATUS status = RegOpenKeyExW( hkey, sSubKey.c_str(), 0, accessMask, &key );

                if( status != ERROR_SUCCESS )
                {
                    if( status == ERROR_FILE_NOT_FOUND )
                        status = RegCreateKeyExW( hkey, sSubKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, accessMask, NULL, &key, NULL );

                    if( status != ERROR_SUCCESS )
                        break;
                }

                status = RegSetValueExW( key, sValueName.c_str(), 0, REG_SZ, ( PBYTE )( valueData.c_str() ), sizeof( WCHAR ) * ( DWORD )( ( valueData.size() + 1 ) ) );

                if( status == ERROR_SUCCESS )
                    isSuccess = true;
            }
            while( false );

            if( key != NULL )
                RegCloseKey( key );

            return isSuccess;
        }

        bool SetRegDWORDValue( HKEY hkey, const XString& sSubKey, const XString& sValueName, const DWORD valueData, bool is32Key )
        {
            bool isSuccess = false;
            REGSAM accessMask = KEY_WRITE;
            HKEY key = NULL;

            do
            {
                if( is32Key == true )
                    accessMask |= KEY_WOW64_32KEY;
                else
                    accessMask |= KEY_WOW64_64KEY;

                LSTATUS status = RegOpenKeyExW( hkey, sSubKey.c_str(), 0, accessMask, &key );

                if( status != ERROR_SUCCESS )
                {
                    if( status == ERROR_FILE_NOT_FOUND )
                        status = RegCreateKeyExW( hkey, sSubKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, accessMask, NULL, &key, NULL );

                    if( status != ERROR_SUCCESS )
                        break;
                }

                status = RegSetValueExW( key, sValueName.c_str(), 0, REG_DWORD, reinterpret_cast< const BYTE* >( &valueData ), sizeof( DWORD ) );

                if( status == ERROR_SUCCESS )
                    isSuccess = true;
            }
            while( false );

            if( key != NULL )
                RegCloseKey( key );

            return isSuccess;
        }

        prResultString GetRegValue( HKEY hkey, const XString& sSubKey, const XString& sValueName, bool is32Key )
        {
            prResultString prResult( false, "" );
            HKEY key = NULL;
            REGSAM accessMask = KEY_READ;

            do
            {
                if( is32Key == true )
                    accessMask |= KEY_WOW64_32KEY;
                else
                    accessMask |= KEY_WOW64_64KEY;

                LSTATUS status = RegOpenKeyExW( hkey, sSubKey.c_str(), 0, accessMask, &key );

                if( status != ERROR_SUCCESS )
                    break;

                DWORD dwType = 0;
                DWORD dwCbData = 0;

                status = RegQueryValueExW( key, sValueName.c_str(), 0, &dwType, NULL, &dwCbData );

                if( status != ERROR_SUCCESS )
                    break;

                switch( dwType )
                {
                    case REG_SZ:
                    case REG_EXPAND_SZ:
                    case REG_MULTI_SZ:
                    {
                        std::vector< wchar_t > vecRetVal;
                        vecRetVal.resize( dwCbData + 1 );

                        status = RegQueryValueExW( key, sValueName.c_str(), 0, NULL, ( LPBYTE )&vecRetVal[ 0 ], &dwCbData );

                        if( status != ERROR_SUCCESS )
                            break;

                        // TODO : REG_MULTI_SZ의 경우 개행이 들어가게 되면 '\0'으로 들어옴. 수정 필요함
                        if( dwType == REG_MULTI_SZ )
                            prResult.second = std::wstring( &vecRetVal[ 0 ], vecRetVal.size() );
                        else
                            prResult.second = std::wstring( &vecRetVal[ 0 ], vecRetVal.size() );

                        prResult.first = true;
                        break;
                    }
                    case REG_DWORD:
                    case REG_QWORD:
                    {
                        DWORD dwRet = 0;

                        status = RegQueryValueExW( key, sValueName.c_str(), 0, NULL, ( LPBYTE )&dwRet, &dwCbData );

                        if( status != ERROR_SUCCESS )
                            break;

                        prResult.first = true;
                        prResult.second = dwRet;

                        break;
                    }
                    case REG_BINARY:
                    {
                        std::vector<BYTE> vecByte;
                        vecByte.resize( dwCbData + 1 );

                        status = RegQueryValueExW( key, sValueName.c_str(), 0, NULL, ( LPBYTE )&vecByte[ 0 ], &dwCbData );

                        if( status != ERROR_SUCCESS )
                            break;

                        prResult.first = true;
                        prResult.second = &vecByte[ 0 ];

                        break;
                    }
                }
            } while( false );

            if( key != NULL )
                RegCloseKey( key );

            return prResult;
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

        bool StopWindowsService( const XString& sSvcName )
        {
            bool isSuccess = false;
            SC_HANDLE hMgr = NULL;
            SC_HANDLE hSvc = NULL;
            SERVICE_STATUS status{};

            do
            {
                hMgr = OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
                if( hMgr == NULL )
                    break;

                hSvc = OpenServiceW( hMgr, sSvcName, SERVICE_ALL_ACCESS );
                if( hSvc == NULL )
                    break;

                QueryServiceStatus( hSvc, &status );

                if( status.dwCurrentState == SERVICE_STOPPED )
                {
                    isSuccess = true;
                    break;
                }

                if( ControlService( hSvc, SERVICE_CONTROL_STOP, &status ) == TRUE )
                {
                    for( UINT nSec = 0; nSec < 5; ++nSec )
                    {
                        QueryServiceStatus( hSvc, &status );
                        if( status.dwCurrentState == SERVICE_STOPPED )
                        {
                            isSuccess = true;
                            break;
                        }
                        Sleep( 1000 );
                    }
                }
                else
                {
                    isSuccess = false;
                }

            } while( false );

            if( hSvc != NULL )
                CloseServiceHandle( hSvc );

            if( hMgr != NULL )
                CloseServiceHandle( hMgr );

            return isSuccess;
        }

        bool SetPrivilege( LPCTSTR lpszPrivilege, BOOL isEnable )
        {
            TOKEN_PRIVILEGES tokenP;
            LUID luid;
            HANDLE hToken = NULL;
            bool isSuccess = false;

            do
            {
                if( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) == FALSE )
                    break;

                // lookup privilege on local system
                // privilege to lookup 
                // receives LUID of privilege
                if( LookupPrivilegeValueW( NULL, lpszPrivilege, &luid ) == FALSE )
                    break;

                tokenP.PrivilegeCount = 1;
                tokenP.Privileges[ 0 ].Luid = luid;

                if( isEnable == TRUE )
                    tokenP.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;
                else
                    tokenP.Privileges[ 0 ].Attributes = 0;

                if( AdjustTokenPrivileges( hToken, FALSE, &tokenP, sizeof( TOKEN_PRIVILEGES ), ( PTOKEN_PRIVILEGES )NULL, ( PDWORD )NULL ) == FALSE )
                    break;

                if( GetLastError() == ERROR_NOT_ALL_ASSIGNED )
                    break;

                isSuccess = true;

            } while( false );

            if( hToken != NULL )
                CloseHandle( hToken );

            return isSuccess;
        }
    }
}
