#ifndef __HDR_SHARED_PATH__
#define __HDR_SHARED_PATH__

#include "SharedBase.h"

#include "String/SharedString.h"
#include "String/SharedXString.h"

#include <string>
#include <filesystem>

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    typedef struct _tyFileInfo
    {
        XString                      sFileFullPath;
        XString                      sFileName;
        XString                      sFilePath;
        XString                      sFileExts;
        XString                      sFileBaseName;

        bool                         isExist;

        _tyFileInfo()
        {
            isExist = false;
        }
    } TyStFileInfo;

    typedef enum
    {
        FILE_TYPE_NONE     = 0,
        FILE_TYPE_ZIP      = 1,
        FILE_TYPE_DB       = 2,
        FILE_TYPE_DB_SHM   = 3,
        FILE_TYPE_DB_WAL   = 4
    } TyEnFileType;

    namespace File
    {
        XString                      GetCurrentPath( bool bIncludeSeparator = false );
        XString                      NormalizePath( const XString& sFilePath );

        bool                         IsExistFile( const XString& sFilePath );
        bool                         IsExistDir( const XString& sFilePath );

        XString                      GetFileExts( const XString& sFileFullPath );
        XString                      GetFileBaseName( const XString& sFileFullPath );

        XString                      FindFileName( const XString& sFileFullPath );
        XString                      FindFilePath( const XString& sFileFullPath );

        TyEnFileType                 RetrieveFileType( const XString& sFilePath );

        std::vector< XString >       GetFileListFromFolder( const XString& sFileFullPath );

        bool                         RemoveFile( const XString& sFilePath );

        class SFileInfo
        {
        public:
            SFileInfo()
            {
                _fileInfo = TyStFileInfo();
            }

            SFileInfo( const XString& sFileFullPath )
            {
                _fileInfo = TyStFileInfo();
                _fileInfo.sFileFullPath = NormalizePath( sFileFullPath );
                Init();
            }

            XString                      FileName() { return _fileInfo.sFileName; }
            XString                      FilePath() { return _fileInfo.sFilePath; }
            XString                      FileBaseName() { return _fileInfo.sFileBaseName; }
            XString                      FileExts() { return _fileInfo.sFileExts; }
            bool                         IsExist()
            {
                _fileInfo.isExist = IsExistFile( _fileInfo.sFileFullPath );
                return _fileInfo.isExist;
            }

            bool                         Rename( const std::wstring& sDst )
            {
                bool isSuccess = false;
                std::wstring sNewFileFullPath = NormalizePath( _fileInfo.sFilePath + sDst );
                std::error_code ec;
                std::filesystem::rename( _fileInfo.sFileFullPath.string(), sNewFileFullPath, ec );

                if( ec )
                    isSuccess = false;
                else
                {
                    isSuccess = true;
                    _fileInfo.sFileFullPath = sNewFileFullPath;
                    _fileInfo.sFileBaseName = GetFileBaseName( sDst );
                    _fileInfo.sFileName = sDst;
                    _fileInfo.sFileExts = GetFileExts( _fileInfo.sFileFullPath );

                    _fileInfo.isExist = IsExistFile( _fileInfo.sFileFullPath );
                }

                return isSuccess;
            }
        private:
            void Init()
            {
                _fileInfo.sFileName = FindFileName( _fileInfo.sFileFullPath );
                _fileInfo.sFilePath = FindFilePath( _fileInfo.sFileFullPath );
                _fileInfo.sFileExts = GetFileExts( _fileInfo.sFileFullPath );
                _fileInfo.sFileBaseName = GetFileBaseName( _fileInfo.sFileName );
            }

            TyStFileInfo             _fileInfo;
        };
    }
}

#endif