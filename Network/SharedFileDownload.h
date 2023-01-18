#ifndef __HDR_SHARED_FILE_DOWNLOADER__
#define __HDR_SHARED_FILE_DOWNLOADER__

#include "SharedBase.h"

#include <string>

#include "Singleton.h"
#include <wininet.h>

#pragma comment(lib, "Wininet.lib")

#ifdef USING_QT_LIBS
#include <qstring.h>
#endif // USING_QTLIB

class XString;

namespace Shared
{
    namespace Network
    {
        class cFileDownloader
        {
        public:
            cFileDownloader( XString sName );
            ~cFileDownloader();

            bool                  Init();
            ULONG64               GetFileSize( const XString& sUrl );
            bool                  DownloadFile( const XString& sUrl, const XString& sSaveFileFullPath );
            bool                  DownloadFile( const XString& sUrl, const XString& sSaveFilePath, const XString& sSaveFileName );
        private:
            XString               _sName;
            bool                  _isInit = false;
            HINTERNET             _hInternet = NULLPTR;

            bool                  _isProcesss = false;
            ULONG64               _ulDownLoadSize = 0;
        };
    }
}

#endif