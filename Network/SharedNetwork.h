#ifndef __HDR_SHARED_NETWORK__
#define __HDR_SHARED_NETWORK__

#pragma once

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
            bool                                    ClientSend( XString sMsg );

            void                                    ServerReceiveThread( DWORD dwID, ASocket::Socket connectionClient );
            void                                    ServerListenThread();

        private:
            bool                                    _isStop = false;

            tyStNetworkInfo                         _networkInfo;

            std::unique_ptr<CTCPClient>             _pTCPClient;
            std::thread                             _thClientReceiveThread;

            std::unique_ptr<CTCPServer>             _pTCPServer;
            std::thread                             _thServerListenThread;
            std::map< DWORD, tyStClientInfo >       _mapIdToClientInfo;

            DWORD                                   _dwID = 0;
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