#include "stdafx.h"

#include "Network/SharedNetwork.h"
#include "String/SharedString.h"

#include <regex>
#include <Windows.h>

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

#define PACKET_SIZE 1024 * 1024

namespace Shared
{
    namespace Network
    {
        std::string ValidateIP( const std::string& sIP )
        {
            bool isSuccess = false;
            std::string strIPAddress;
            std::string sIPAddress = sIP;

            // IP 유효성 검사
            std::string strRegExpIP = "^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[0-1]?[0-9]?[0-9]?)\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[0-1]?[0-9]?[0-9]?)\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[0-1]?[0-9]?[0-9]?)\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[0-1]?[0-9]?[0-9]?)";
            std::regex rxIP( strRegExpIP );

            WSADATA wsaData;
            WSAStartup( MAKEWORD( 2, 2 ), &wsaData );

            if( regex_match( sIPAddress, rxIP ) == false )
            {
                in_addr in;
                LPHOSTENT lphost = gethostbyname( sIPAddress.c_str() );

                if( lphost != NULL )
                {
                    in.s_addr = ( ( LPIN_ADDR )lphost->h_addr )->s_addr;
                    strIPAddress = inet_ntoa( in );
                }
                else
                {
                    return strIPAddress;
                }
            }

            WSACleanup();

            return strIPAddress;
        }

        CNetwork::CNetwork()
        {
        }

        CNetwork::CNetwork( stNetworkInfo networkInfo )
        {
            _networkInfo = networkInfo;
        }

        CNetwork::~CNetwork()
        {
        }

        bool CNetwork::Connect()
        {
            bool isSuccess = false;

            do
            {
            }
            while( false );

            return isSuccess;
        }

        bool CNetwork::Listen()
        {
            bool isSuccess = false;

            do
            {
                if( _networkInfo.eType != NETWORK_SERVER )
                    break;

                WSADATA wsaData;

                if( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 )
                    break;

                SOCKET serverSocket;

                serverSocket = socket( PF_INET, SOCK_STREAM, 0 );

                SOCKADDR_IN serverAddr;
                memset( &serverAddr, 0, sizeof( serverAddr ) );

                serverAddr.sin_family = AF_INET;
                serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );
                serverAddr.sin_port = _networkInfo.sPort;

                if( bind( serverSocket, ( sockaddr* )&serverAddr, sizeof( serverAddr ) ) == SOCKET_ERROR )
                    break;

                if( listen( serverSocket, SOMAXCONN ) == SOCKET_ERROR )
                    break;

                _vecthServer.push_back( std::thread( std::bind( &CNetwork::ServerOpen, this, serverAddr, serverSocket ) ) );
                isSuccess = true;
            }
            while( false );

            return isSuccess;
        }

        void CNetwork::ServerOpen( SOCKADDR_IN serverAddr, SOCKET serverSocket )
        {
            while( true )
            {
                SOCKADDR_IN client_addr;

                int client_addr_size = sizeof( serverAddr );
                int nClientSock = accept( serverSocket, ( sockaddr* )&client_addr, &client_addr_size );

                if( nClientSock == SOCKET_ERROR )
                    break;

                static char cBuffer[ PACKET_SIZE ];
                if( recv( nClientSock, cBuffer, PACKET_SIZE, 0 ) == SOCKET_ERROR )
                {
                    SleepEx( 1, TRUE );
                    continue;
                }

            }
        }
    }
}
