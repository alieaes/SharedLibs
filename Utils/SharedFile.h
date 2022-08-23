#ifndef __HDR_SHARED_PATH__
#define __HDR_SHARED_PATH__

#include "SharedBase.h"

#include "String/SharedString.h"

#include <string>

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    typedef struct _tyFileInfo
    {
        std::wstring                 sFileFullPath;
        std::wstring                 sFileName;
        std::wstring                 sFilePath;
        std::wstring                 sFileExts;
        std::wstring                 sFileBaseName;

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
        std::wstring                 GetCurrentPath( bool bIncludeSeparator = false );
        std::wstring                 NormalizePath( const std::wstring& sFilePath );
        std::wstring                 NormalizePath( const std::string& sFilePath );

        bool                         IsExistFile( const std::wstring& sFilePath );
        bool                         IsExistDir( const std::wstring& sFilePath );

        std::wstring                 GetFileExts( const std::wstring& sFileFullPath );
        std::wstring                 GetFileBaseName( const std::wstring& sFileFullPath );

        std::wstring                 FindFileName( const std::wstring& sFileFullPath );
        std::wstring                 FindFilePath( const std::wstring& sFileFullPath );

        TyEnFileType                 RetrieveFileType( const std::wstring& sFilePath );

        std::vector< std::wstring >  GetFileListFromFolder( const std::wstring& sFileFullPath );

        class SFileInfo
        {
        public:
            SFileInfo()
            {
                _fileInfo = TyStFileInfo();
            }

            SFileInfo( const std::wstring& sFileFullPath )
            {
                _fileInfo = TyStFileInfo();
                _fileInfo.sFileFullPath = NormalizePath( sFileFullPath );
                Init();
            }

            SFileInfo( const std::string& sFileFullPath )
            {
                _fileInfo = TyStFileInfo();
                _fileInfo.sFileFullPath = NormalizePath( String::s2ws( sFileFullPath ) );
                Init();
            }

            std::wstring                 FileName() { return _fileInfo.sFileName; }
            std::wstring                 FilePath() { return _fileInfo.sFilePath; }
            std::wstring                 FileBaseName() { return _fileInfo.sFileBaseName; }
            std::wstring                 FileExts() { return _fileInfo.sFileExts; }
            bool                         IsExist() { return _fileInfo.isExist; }

            bool                         Rename( const std::wstring& sDst )
            {
                bool isSuccess = false;
                std::wstring sNewFileFullPath = NormalizePath( _fileInfo.sFilePath + sDst );
                std::error_code ec;
                std::filesystem::rename( _fileInfo.sFileFullPath, sNewFileFullPath, ec );

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
                _fileInfo.isExist = IsExistFile( _fileInfo.sFileFullPath );

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