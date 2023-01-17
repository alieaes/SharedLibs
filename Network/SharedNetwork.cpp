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

                isSuccess = true;

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

                    auto tuPacket = convertReceiveMsg( vecRcvBuf );
                    _mapMsgIDToMSG[ std::get<1>( tuPacket ) ] = std::get<2>( tuPacket );
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

        std::pair< bool, MSGID > CNetwork::ClientSend( XString sMsg )
        {
            std::pair< bool, MSGID > prRet = std::make_pair( false, 0 );

            do
            {
                if( _pTCPClient->IsConnected() == false )
                    break;

                auto sSend = convertSendMsg( sMsg, _msgID );

                prRet.first = _pTCPClient->Send( sSend );

                if( prRet.first == true )
                    prRet.second = _msgID++;
            }
            while( false );

            return prRet;
        }

        std::pair<bool, XString> CNetwork::ClientReceiveWait( MSGID msgID, TIME_MS timeoutMs /*= 20 * 1000*/ )
        {
            std::pair< bool, XString > prResult = std::make_pair( false, XString() );

            TIME_MS ms = timeoutMs;

            // 타임아웃의 최소치는 10초로 함.
            if( ms < 10 * 1000 )
                ms = 10 * 1000;

            TIME_MS sleepMs = ms / 1000;
            TIME_MS timeout = 1000;
            int nTryCnt = 0;

            do
            {
                while( prResult.first == false &&  nTryCnt++ < timeout )
                {
                    if( _mapMsgIDToMSG.count( msgID ) > 0 )
                    {
                        prResult.first = true;
                        prResult.second = _mapMsgIDToMSG[ msgID ];
                        _mapMsgIDToMSG.erase( msgID );
                        break;
                    }

                    Sleep( sleepMs );
                }
            }
            while( false );

            return prResult;
        }

        void CNetwork::RegisterServerHandler( funcHandler func )
        {
            _serverFunc = func;
        }

        void CNetwork::ServerReceiveThread( DWORD dwID, ASocket::Socket connectionClient )
        {
            CNetworkServerHandler* serverHandler = new CNetworkServerHandler();

            serverHandler->RegisterHandler( _serverFunc );
            serverHandler->SetSocket( connectionClient );
            serverHandler->Start();

            while( true )
            {
                if( _isStop == true )
                    break;

                std::vector<char> vecRcvBuf( PACKET_SIZE );
                int nRcvBytes = _pTCPServer->Receive( connectionClient, vecRcvBuf.data(), PACKET_SIZE );

                if( nRcvBytes > 0 )
                {
                    // Success

                    auto tuPacket = convertReceiveMsg( vecRcvBuf );

                    serverHandler->InsertPacket( tuPacket );
                }
                else if( nRcvBytes == 0 )
                {
                    // Connection Close

                    _pTCPServer->Disconnect( connectionClient );
                    serverHandler->End();
                    break;
                }
                else
                {
                    // Falied..
                }
            }
        }

        std::pair< bool, MSGID > CNetwork::ServerSend( ASocket::Socket connectionClient, XString sMsg )
        {
            std::pair< bool, MSGID > prRet = std::make_pair( false, 0 );

            do
            {
                auto sSend = convertSendMsg( sMsg, _msgID );

                prRet.first = _pTCPServer->Send( connectionClient, sSend );

                if( prRet.first == true )
                    prRet.second = _msgID++;

            } while( false );

            return prRet;
        }

        std::pair<bool, MSGID> CNetwork::ServerResponse( ASocket::Socket connectionClient, MSGID nMsgId, XString sMsg )
        {
            std::pair< bool, MSGID > prRet = std::make_pair( false, 0 );

            do
            {
                auto sSend = convertSendMsg( sMsg, nMsgId );

                prRet.first = _pTCPServer->Send( connectionClient, sSend );

                if( prRet.first == true )
                    prRet.second = nMsgId;

            } while( false );

            return prRet;
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

        bool CNetwork::ServerAllClientSend( XString sMsg )
        {
            bool isSuccess = false;

            do
            {
                if( _pTCPClient->IsConnected() == false )
                    break;

                auto sSend = convertSendMsg( sMsg, 0 );

                for( auto it = _mapIdToClientInfo.begin(); it != _mapIdToClientInfo.end(); ++it )
                {
                    // 전체가 성공 했을 때만 성공으로 봐야 하는가? 일부 성공을 분리해야 하는가?
                    isSuccess = _pTCPServer->Send( it->second.connectionClient, sSend );
                }
            } while( false );

            return isSuccess;
        }

        tuPacketMsg CNetwork::convertReceiveMsg( std::vector<char> vecChar )
        {
            tuPacketMsg packetMsg = std::make_tuple( 0, 0, XString() );

            {
                std::vector<char> vecTmp;
                // 앞 4byte는 사이즈로 판단하기 때문에 삭제하도록 함.
                for( int idx = 0; idx < 4; idx++ )
                {
                    vecTmp.push_back( vecChar.at( 0 ) );
                    vecChar.erase( vecChar.begin() + 0 );
                }

                unsigned int nSize = vecTmp[ 0 ];
                nSize = ( nSize << 8 ) | vecTmp[ 1 ];
                nSize = ( nSize << 8 ) | vecTmp[ 2 ];
                nSize = ( nSize << 8 ) | vecTmp[ 3 ];

                std::get<0>( packetMsg ) = nSize;
            }

            {
                std::vector<char> vecTmp;
                // 그 뒤 4byte는 msgID로 관리하기 때문에 동일하게 삭제하도록 함.
                for( int idx = 0; idx < 4; idx++ )
                {
                    vecTmp.push_back( vecChar.at( 0 ) );
                    vecChar.erase( vecChar.begin() + 0 );
                }

                unsigned int nSize = vecTmp[ 0 ];
                nSize = ( nSize << 8 ) | vecTmp[ 1 ];
                nSize = ( nSize << 8 ) | vecTmp[ 2 ];
                nSize = ( nSize << 8 ) | vecTmp[ 3 ];

                std::get<1>( packetMsg ) = nSize;
            }

            std::get<2>( packetMsg ) = String::s2ws( vecChar.data() );

            return packetMsg;
        }

        std::vector<char> CNetwork::convertSendMsg( XString sMsg, MSGID msgID )
        {
            XString sRet;

            char cSize[ 4 ];
            std::vector<char> vecChar;
            auto vecMsg = sMsg.toCharByte();

            {
                unsigned int nMsgSize = htonl( vecMsg.size() + 8 );
                memcpy( cSize, &nMsgSize, 4 );

                for( int idx = 0; idx < 4; idx++ )
                    vecChar.push_back( cSize[ idx ] );
            }
            
            char cIDSize[ 4 ];

            {
                unsigned int nID = htonl( msgID );
                memcpy( cIDSize, &nID, 4 );

                for( int idx = 0; idx < 4; idx++ )
                    vecChar.push_back( cIDSize[ idx ] );
            }

            vecChar.insert( vecChar.end(), vecMsg.begin(), vecMsg.end() );
            return vecChar;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// NETWORK SERVER HANDLER 
        ////////////////////////////////////////////////////////////////////////////////////////////

        CNetworkServerHandler::CNetworkServerHandler()
        {
        }

        CNetworkServerHandler::~CNetworkServerHandler()
        {
            _isStop = true;
        }

        void CNetworkServerHandler::SetSocket( ASocket::Socket connectionClient )
        {
            _connectionClient = connectionClient;
        }

        void CNetworkServerHandler::RegisterHandler( funcHandler func )
        {
            _func = func;
        }

        void CNetworkServerHandler::InsertPacket( tuPacketMsg packet )
        {
            std::lock_guard<std::recursive_mutex> lck( _lck );
            _queuePacket.push( packet );
        }

        void CNetworkServerHandler::Start()
        {
            _th = std::thread( std::bind( &CNetworkServerHandler::Main, this ) );
        }

        void CNetworkServerHandler::End()
        {
            _isStop = true;
            _th.join();
        }

        void CNetworkServerHandler::Main()
        {
            while( _isStop == false )
            {
                tuPacketMsg tuPacket;
                {
                    std::lock_guard<std::recursive_mutex> lck( _lck );

                    if( _queuePacket.size() == 0 )
                    {
                        Sleep( 5 );
                        continue;
                    }

                    tuPacket = _queuePacket.front();

                    _queuePacket.pop();
                }

                if( _func != NULLPTR )
                    _func( _connectionClient, tuPacket );
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        /// NETWORK MANAGER 
        ////////////////////////////////////////////////////////////////////////////////////////////
        ///
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
