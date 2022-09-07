#ifndef __HDR_SHARED_NETWORK__
#define __HDR_SHARED_NETWORK__

#pragma once

#include <string>

#include "SharedBase.h"

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
    std::wstring                    sIP;
    DWORD                           sPort;

    std::wstring                    sName;

    eNetworkType                    eType;

    _stNetworkInfo()
    {
        sPort = 0;
        eType = NETWORK_NONE;
    }
} stNetworkInfo;

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
            CNetwork( stNetworkInfo networkInfo );
            ~CNetwork();

            bool                                    Connect();
            bool                                    Listen();

            void                                    ServerOpen( SOCKADDR_IN serverAddr, SOCKET serverSocket );

        private:
            stNetworkInfo                           _networkInfo;

            std::vector< std::thread >              _vecthServer;
            SOCKADDR_IN                             _serverAddr;
        };
    }
}

typedef Shared::Singletons::Singleton<Shared::Network::CNetwork> TyStNetwork;

#endif