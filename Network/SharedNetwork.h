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

    std::thread                     th;

} tyStNetworkInfo;

namespace Shared
{
    namespace Network
    {
        std::string                                 ValidateIP( const std::string& sIP );
        bool                                        ValidatePort( DWORD dwPort );

        class CNetwork
        {
        public:
            CNetwork();
            CNetwork( tyStNetworkInfo networkInfo );
            ~CNetwork();

            bool                                    Connect();
            void                                    ClientReceiveThread();
            void                                    ClientSendThread();

        private:
            bool                                    _isStop = false;

            tyStNetworkInfo                         _networkInfo;

            std::unique_ptr<CTCPClient>             _pTCPClient;
            std::unique_ptr<CTCPServer>             _pTCPServer;
        };

        class CNetworkMgr
        {
        public:
            CNetworkMgr();
            CNetworkMgr( tyStNetworkInfo networkInfo );
            ~CNetworkMgr();

            bool                                    Connect();
            bool                                    Listen();

            void                                    ServerOpen( SOCKADDR_IN serverAddr, SOCKET serverSocket );

        private:

            std::map< XString, tyStNetworkInfo >    _mapNameToNetworkInfo;

            std::vector< std::thread >              _vecthServer;
            SOCKADDR_IN                             _serverAddr;
        };
    }
}

typedef Shared::Singletons::Singleton<Shared::Network::CNetworkMgr> TyStNetworkMgr;

#endif