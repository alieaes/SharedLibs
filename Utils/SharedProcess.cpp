#include "stdafx.h"

#include "SharedBase.h"

#include "Utils/SharedProcess.h"

#include "String/SharedString.h"
#include "String/SharedXString.h"
#include "Utils/SharedUtils.h"

#include <Windows.h>
#include "TlHelp32.h"
#include "UserEnv.h"

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

namespace Shared
{
    namespace Process
    {
        DWORD GetCurrentProcessID()
        {
            return GetCurrentProcessId();
        }

        DWORD GetPIDFromProcessName( const XString& sProcessName )
        {
            DWORD dwPID;
            HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

            do
            {
                if( hSnapShot == INVALID_HANDLE_VALUE )
                    break;

                PROCESSENTRY32W proc32w;
                proc32w.dwSize = sizeof( PROCESSENTRY32W );

                if( Process32FirstW( hSnapShot, &proc32w ) == FALSE )
                    break;

                do
                {
                    if( sProcessName.compare( proc32w.szExeFile, true ) == 0 )
                    {
                        dwPID = proc32w.th32ProcessID;
                        break;
                    }
                } while( Process32NextW( hSnapShot, &proc32w ) != FALSE );

            } while( false );

            if( hSnapShot != INVALID_HANDLE_VALUE )
                CloseHandle( hSnapShot );

            return dwPID;
        }

        std::vector<DWORD> GetProcessIDList()
        {
            std::vector<DWORD> vecPID;
            HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

            do
            {
                if( hSnapShot == INVALID_HANDLE_VALUE )
                    break;

                PROCESSENTRY32W proc32w;
                proc32w.dwSize = sizeof( PROCESSENTRY32W );

                if( Process32FirstW( hSnapShot, &proc32w ) == FALSE )
                    break;

                do
                {
                    vecPID.push_back( proc32w.th32ProcessID );
                }
                while( Process32NextW( hSnapShot, &proc32w ) != FALSE );

            } while( false );

            if( hSnapShot != INVALID_HANDLE_VALUE )
                CloseHandle( hSnapShot );

            return vecPID;
        }

        bool ExecutionProcess( const XString& sFilePath )
        {
            bool isSuccess = false;
            if( IsServiceProcess( GetCurrentProcessID() ) == true )
                isSuccess = ExecutionUsingWinLogon( sFilePath );
            else
                isSuccess = ExecutionAsAdministrator( sFilePath );

            return isSuccess;
        }

        bool ExecutionUsingWinLogon( const XString& sFilePath )
        {
            BOOL isSuccess = FALSE;

            do
            {
                DWORD dwWinLogon = GetPIDFromProcessName( "winlogon.exe" );

                if( dwWinLogon <= 0 )
                    break;

                LUID luid;

                STARTUPINFO si;
                ZeroMemory( &si, sizeof( STARTUPINFO ) );
                si.cb = sizeof( STARTUPINFO );
                si.lpDesktop = const_cast< wchar_t* >( L"winsta0\\default" );

                PROCESS_INFORMATION pi;
                ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );
                DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;

                HANDLE hProcess = OpenProcess( MAXIMUM_ALLOWED, FALSE, dwWinLogon );
                HANDLE hToken = NULL;

                if( ::OpenProcessToken( hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
                                        | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID
                                        | TOKEN_READ | TOKEN_WRITE, &hToken ) == FALSE )
                {
                    break;
                }

                if( LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &luid ) == FALSE )
                    break;

                LPVOID lpEnv = NULL;

                if( CreateEnvironmentBlock( &lpEnv, hToken, TRUE ) == TRUE )
                    dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
                else
                    lpEnv = NULL;

                // Launch the process in the client's logon session.  
                isSuccess = CreateProcessAsUserW( hToken,            // client's access token  
                                                sFilePath,              // file to execute  
                                                  NULL,     // command line  
                                                  NULL,              // pointer to process SECURITY_ATTRIBUTES  
                                                  NULL,              // pointer to thread SECURITY_ATTRIBUTES  
                                                  FALSE,             // handles are not inheritable  
                                                  dwCreationFlags,  // creation flags  
                                                  lpEnv,              // pointer to new environment block   
                                                  NULL,              // name of current directory   
                                                  &si,               // pointer to STARTUPINFO structure  
                                                  &pi                // receives information about new process  
                );


                if( pi.hProcess != NULL )
                {
                    if( hProcess != NULLPTR )
                        hProcess = pi.hProcess;
                    else
                        CloseHandle( pi.hProcess );
                }

                if( pi.hThread != NULL )
                    CloseHandle( pi.hThread );

                if( lpEnv != NULL )
                    DestroyEnvironmentBlock( lpEnv );

                if( hToken != INVALID_HANDLE_VALUE )
                    CloseHandle( hToken );

            } while( false );

            return isSuccess;
        }

        bool ExecutionAsAdministrator( const XString& sFilePath )
        {
            bool isSuccess = false;
            HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
            DWORD dwPID = 0;
            do
            {
                if( hSnapShot == INVALID_HANDLE_VALUE )
                    break;

                PROCESSENTRY32W proc32w;
                proc32w.dwSize = sizeof( PROCESSENTRY32W );

                if( Process32FirstW( hSnapShot, &proc32w ) == FALSE )
                    break;

                do
                {
                    if( IsElevatedProcess( proc32w.th32ProcessID ) == true )
                        dwPID = proc32w.th32ProcessID;

                } while( Process32NextW( hSnapShot, &proc32w ) != FALSE );

                if( dwPID == 0 )
                    dwPID = GetPIDFromProcessName( "explorer.exe" );

                if( dwPID <= 0 )
                    break;

                LUID luid;

                STARTUPINFO si;
                ZeroMemory( &si, sizeof( STARTUPINFO ) );
                si.cb = sizeof( STARTUPINFO );
                si.lpDesktop = const_cast< wchar_t* >( L"winsta0\\default" );

                PROCESS_INFORMATION pi;
                ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );
                DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;

                HANDLE hProcess = OpenProcess( MAXIMUM_ALLOWED, FALSE, dwPID );
                HANDLE hToken = NULL;

                if( ::OpenProcessToken( hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
                                        | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_SESSIONID
                                        | TOKEN_READ | TOKEN_WRITE, &hToken ) == FALSE )
                {
                    break;
                }

                if( LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &luid ) == FALSE )
                    break;

                LPVOID lpEnv = NULL;

                if( CreateEnvironmentBlock( &lpEnv, hToken, TRUE ) == TRUE )
                    dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
                else
                    lpEnv = NULL;

                // Launch the process in the client's logon session.  
                isSuccess = CreateProcessAsUserW( hToken,            // client's access token  
                                                  NULL,              // file to execute  
                                                  (LPWSTR) sFilePath.c_str(),     // command line  
                                                  NULL,              // pointer to process SECURITY_ATTRIBUTES  
                                                  NULL,              // pointer to thread SECURITY_ATTRIBUTES  
                                                  FALSE,             // handles are not inheritable  
                                                  dwCreationFlags,  // creation flags  
                                                  lpEnv,              // pointer to new environment block   
                                                  NULL,              // name of current directory   
                                                  &si,               // pointer to STARTUPINFO structure  
                                                  &pi                // receives information about new process  
                );


                if( pi.hProcess != NULL )
                {
                    if( hProcess != NULLPTR )
                        hProcess = pi.hProcess;
                    else
                        CloseHandle( pi.hProcess );
                }

                if( pi.hThread != NULL )
                    CloseHandle( pi.hThread );

                if( lpEnv != NULL )
                    DestroyEnvironmentBlock( lpEnv );

                if( hToken != INVALID_HANDLE_VALUE )
                    CloseHandle( hToken );

            } while( false );

            if( hSnapShot != INVALID_HANDLE_VALUE )
                CloseHandle( hSnapShot );

            return isSuccess;
        }

        bool IsServiceProcess( DWORD dwProcessId )
        {
            bool isSuccess = false;
            SC_HANDLE hScm = OpenSCManagerW( 0, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ENUMERATE_SERVICE );

            do
            {
                if( hScm == NULL )
                    break;

                DWORD ServicesBufferRequired = 0;
                DWORD ResumeHandle = 0;

                DWORD ServicesBufferSize = 0;
                DWORD ServicesCount = 0;
                ENUM_SERVICE_STATUS_PROCESS* ServicesBuffer = 0;

                EnumServicesStatusEx( hScm, SC_ENUM_PROCESS_INFO, SERVICE_WIN32,
                                      SERVICE_ACTIVE, 0, 0, &ServicesBufferRequired, &ServicesCount, &ResumeHandle, 0 );

                if( GetLastError() != ERROR_MORE_DATA )
                    break;

                // Todo: Error handling (GetLastError() results are currently bogus?)
                ServicesBuffer = ( ENUM_SERVICE_STATUS_PROCESS* ) new char[ ServicesBufferRequired ];
                ServicesBufferSize = ServicesBufferRequired;
                EnumServicesStatusEx( hScm, SC_ENUM_PROCESS_INFO, SERVICE_WIN32,
                                      SERVICE_ACTIVE, ( LPBYTE )ServicesBuffer, ServicesBufferSize,
                                      &ServicesBufferRequired, &ServicesCount, &ResumeHandle, 0 );

                if( GetLastError() != ERROR_SUCCESS )
                    break;

                ENUM_SERVICE_STATUS_PROCESS* ServicesBufferPtr = ServicesBuffer;

                while( ServicesCount-- )
                {
                    if( ServicesBufferPtr->ServiceStatusProcess.dwProcessId == dwProcessId )
                    {
                        isSuccess = true;
                        break;
                    }

                    ServicesBufferPtr++;
                }

                delete[] ServicesBuffer;

            } while( false );

            CloseServiceHandle( hScm );

            return isSuccess;

        }

        bool IsElevatedProcess( DWORD dwProcessID )
        {
            BOOL bElevated = FALSE;
            HANDLE hToken = NULL;
            HANDLE processHandle = NULL;

            do
            {
                if( dwProcessID == 0 )
                    break;

                processHandle = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, dwProcessID );
                if( processHandle == NULL )
                    break;

                if( OpenProcessToken( processHandle, TOKEN_QUERY, &hToken ) == FALSE )
                    break;

                TOKEN_ELEVATION Elevation;
                DWORD dwSize = sizeof( TOKEN_ELEVATION );
                if( GetTokenInformation( hToken, TokenElevation, &Elevation, sizeof( Elevation ), &dwSize ) == FALSE )
                    break;

                bElevated = ( BOOL )Elevation.TokenIsElevated;

            } while( false );

            if( hToken != NULL )
                CloseHandle( hToken );

            if( processHandle != NULL )
                CloseHandle( processHandle );

            return bElevated != FALSE ? true : false;
        }
    }
}
