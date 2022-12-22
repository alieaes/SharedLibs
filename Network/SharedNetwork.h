#ifndef __HDR_SHARED_NETWORK__
#define __HDR_SHARED_NETWORK__

#pragma once

#include <queue>
#include <string>

#include "SharedBase.h"

#include "Network/include/Socket/TCPClient.h"
#include "Network/include/Socket/TCPServer.h"

#include "String/SharedXString.h"

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB

namespace Shared
{
    namespace Network
    {
        class CNetwork;
    }
}

typedef unsigned int MSGID;
typedef unsigned int TIME_MS;

typedef std::tuple< int, MSGID, XString > tuPacketMsg;
typedef std::function<void( ASocket::Socket, tuPacketMsg )> funcHandler;

enum eNetworkType
{
    NETWORK_NONE = 0,
    NETWORK_CLIENT = 1,
    NETWORK_SERVER = 2
};

typedef struct _stNetworkInfo
{
    XString                         sIP;
    DWORD                           sPort;

    XString                         sName;

    eNetworkType                    eType;

    Shared::Network::CNetwork*      network;

} tyStNetworkInfo;

namespace Shared
{
    namespace Network
    {
        std::string                                 ValidateIP( const std::string& sIP );
        bool                                        ValidatePort( DWORD dwPort );

        typedef struct _stClientInfo
        {
            DWORD                   dwId;
            ASocket::Socket         connectionClient;
            std::thread             thServerReceiveThread;
        } tyStClientInfo;

        class CNetwork
        {
        public:
            CNetwork();
            CNetwork( tyStNetworkInfo networkInfo );
            ~CNetwork();

            bool                                    Connect();

            void                                    ClientReceiveThread();
            std::pair< bool, MSGID >                ClientSend( XString sMsg );
            std::pair< bool, XString >              ClientReceiveWait( MSGID msgID, TIME_MS timeoutMs = 20 * 1000 );

            void                                    RegisterServerHandler( funcHandler func );
            void                                    ServerReceiveThread( DWORD dwID, ASocket::Socket connectionClient );
            std::pair< bool, MSGID >                ServerSend( ASocket::Socket connectionClient, XString sMsg );
            void                                    ServerListenThread();
            bool                                    ServerAllClientSend( XString sMsg );

        private:
            tuPacketMsg                             convertReceiveMsg( std::vector<char> vecChar );
            std::vector<char>                       convertSendMsg( XString sMsg, MSGID msgID );

            funcHandler                             _serverFunc;

            bool                                    _isStop = false;

            tyStNetworkInfo                         _networkInfo;

            std::unique_ptr<CTCPClient>             _pTCPClient;
            std::thread                             _thClientReceiveThread;
            std::map< MSGID, XString >              _mapMsgIDToMSG;

            std::unique_ptr<CTCPServer>             _pTCPServer;
            std::thread                             _thServerListenThread;
            std::map< DWORD, tyStClientInfo >       _mapIdToClientInfo;

            DWORD                                   _dwID = 0;
            MSGID                                   _msgID = 0;
        };

        class CNetworkServerHandler
        {
        public:
            CNetworkServerHandler();
            ~CNetworkServerHandler();

            void                                    SetSocket( ASocket::Socket connectionClient );
            void                                    RegisterHandler( funcHandler func );
            void                                    InsertPacket( tuPacketMsg packet );

            void                                    Start();
            void                                    End();

            void                                    Main();
        private:

            bool                                    _isStop = false;

            std::recursive_mutex                    _lck;
            std::thread                             _th;
            std::queue< tuPacketMsg >               _queuePacket;
            ASocket::Socket                         _connectionClient;
            funcHandler                             _func;
        };

        class CNetworkMgr
        {
        public:
            CNetworkMgr();
            CNetworkMgr( tyStNetworkInfo networkInfo );
            ~CNetworkMgr();

            bool                                          NewConnection( tyStNetworkInfo networkInfo );
            std::pair< bool, Shared::Network::CNetwork* > GetConnection( XString sName );

        private:

            std::map< XString, tyStNetworkInfo >    _mapNameToNetworkInfo;

            std::vector< std::thread >              _vecthServer;
            SOCKADDR_IN                             _serverAddr;
        };
    }
}

typedef Shared::Singletons::Singleton<Shared::Network::CNetworkMgr> TyStNetworkMgr;

#endif