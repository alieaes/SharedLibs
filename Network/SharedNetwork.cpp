#include "stdafx.h"

#include "Network/SharedNetwork.h"

#include <codecvt>

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

                    _thClientReceiveThread = std::thread( std::bind( &CNetwork::ClientReceiveThread, this ) );

                }
                else if( _networkInfo.eType == NETWORK_SERVER )
                {
                    if( _pTCPServer == NULLPTR )
                        _pTCPServer.reset( new CTCPServer( PRINT_LOG, XString( _networkInfo.sPort ) ) );

                    _thServerListenThread = std::thread( std::bind( &CNetwork::ServerListenThread, this ) );
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

        bool CNetwork::ClientSend( XString sMsg )
        {
            bool isSuccess = false;

            do
            {
                if( _pTCPClient->IsConnected() == false )
                    break;

                unsigned int number2 = htonl( sMsg.size() );
                char numberStr[ 4 ];
                memcpy( numberStr, &number2, 4 );

                // TODO : 이부분 너무 비효율적임. 수정 필요.
                XString sSend = XString( numberStr[ 0 ] ) + XString( numberStr[ 1 ] ) + XString( numberStr[ 2 ] ) + XString( numberStr[ 3 ] ) + sMsg;
                isSuccess = _pTCPClient->Send( sSend.toString() );
            }
            while( false );

            return isSuccess;
        }

        void CNetwork::ServerReceiveThread( DWORD dwID, ASocket::Socket connectionClient )
        {
            while( true )
            {
                if( _isStop == true )
                    break;

                std::vector<char> vecRcvBuf( PACKET_SIZE );
                int nRcvBytes = _pTCPServer->Receive( connectionClient, vecRcvBuf.data(), PACKET_SIZE );

                if( nRcvBytes > 0 )
                {
                    // 앞 4byte는 사이즈로 판단하기 때문에 삭제하도록 함.
                    for( int idx = 0; idx < 4; idx++ )
                        vecRcvBuf.erase( vecRcvBuf.begin() + 0 );

                    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                    XString sRet = converter.from_bytes( vecRcvBuf.data(), vecRcvBuf.data() + vecRcvBuf.size() );
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

        void CNetwork::ServerListenThread()
        {
            while( true )
            {
                if( _isStop == true )
                    break;

                ASocket::Socket ConnectedClient;
                auto bConnect = _pTCPServer->Listen( ConnectedClient );
                if( bConnect == true )
                {
                    while( _mapIdToClientInfo.count( _dwID ) > 0 )
                    {
                        ++_dwID;
                    }

                    tyStClientInfo stClientInfo;
                    stClientInfo.dwId = _dwID;
                    stClientInfo.connectionClient = ConnectedClient;
                    stClientInfo.thServerReceiveThread = std::thread( std::bind( &CNetwork::ServerReceiveThread, this, stClientInfo.dwId, ConnectedClient ) );

                    _mapIdToClientInfo[ _dwID ] = std::move( stClientInfo );
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

        }

        CNetworkMgr::~CNetworkMgr()
        {
        }

        bool CNetworkMgr::NewConnection( tyStNetworkInfo networkInfo )
        {
            bool isSuccess = false;

            do
            {
                if( networkInfo.sName.IsEmpty() == true )
                    break;

                networkInfo.network = new CNetwork( networkInfo );
                _mapNameToNetworkInfo[ networkInfo.sName ] = std::move( networkInfo );

                isSuccess = true;

            } while( false );

            return isSuccess;
        }

        std::pair< bool, Shared::Network::CNetwork* > CNetworkMgr::GetConnection( XString sName )
        {
            std::pair< bool, Shared::Network::CNetwork* > pr = std::make_pair( false, NULLPTR );
            bool isSuccess = false;

            do
            {
                if( _mapNameToNetworkInfo.count( sName ) > 0 )
                {
                    pr.first = true;
                    pr.second = _mapNameToNetworkInfo[ sName ].network;
                }
            } while( false );

            return pr;
        }
    }
}
