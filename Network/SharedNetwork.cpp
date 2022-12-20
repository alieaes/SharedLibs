#include "stdafx.h"

#include "Network/SharedNetwork.h"
#include "String/SharedString.h"

#include "Network/include/Socket/TCPClient.h"
#include "Network/include/Socket/TCPServer.h"

#include <regex>
#include <Windows.h>

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

#define PACKET_SIZE 1024 * 1024 * 10
#define PRINT_LOG [](const std::string& strLogMsg) { std::cout << strLogMsg << std::endl;  }

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
            : _pTCPClient( NULLPTR ), _pTCPServer( NULLPTR )
        {
        }

        CNetwork::CNetwork( tyStNetworkInfo networkInfo )
        {
            _networkInfo = std::move( networkInfo );
        }

        CNetwork::~CNetwork()
        {
            _isStop = true;
        }

        bool CNetwork::Connect()
        {
            bool isSuccess = false;

            do
            {
                if( _networkInfo.eType == NETWORK_CLIENT )
                {
                    if( _pTCPClient == NULLPTR )
                        _pTCPClient.reset( new CTCPClient( PRINT_LOG ) );

                    if( _pTCPClient->IsConnected() == true )
                        break;

                    if( _pTCPClient->Connect( _networkInfo.sIP, XString( _networkInfo.sPort ) ) == false )
                    {
                        //실패 시 로깅 필요
                        assert( false );
                        break;
                    }
                }
                else if( _networkInfo.eType == NETWORK_SERVER )
                {
                    
                }
                else
                {
                    
                }

            } while( false );

            return isSuccess;
        }

        void CNetwork::ClientReceiveThread()
        {
            while( true )
            {
                if( _isStop == true )
                    break;

                if( _pTCPClient->IsConnected() == false )
                    break;

                std::vector<char> vecRcvBuf( PACKET_SIZE );
                int nRcvBytes = _pTCPClient->Receive( vecRcvBuf.data(), PACKET_SIZE );

                if( nRcvBytes > 0 )
                {
                    // Success
                }
                else if( nRcvBytes == 0 )
                {
                    // Connection Close
                }
                else
                {
                    // Falied..
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// NETWORK MANAGER 
        ////////////////////////////////////////////////////////////////////////////////////////////

        CNetworkMgr::CNetworkMgr()
        {
        }

        CNetworkMgr::CNetworkMgr( tyStNetworkInfo networkInfo )
        {
            if( networkInfo.sName.empty() == true )
                return;

            _mapNameToNetworkInfo[ networkInfo.sName ] = std::move( networkInfo );
        }

        CNetworkMgr::~CNetworkMgr()
        {
        }

        bool CNetworkMgr::Connect()
        {
            bool isSuccess = false;

            do
            {
            }
            while( false );

            return isSuccess;
        }
    }
}
