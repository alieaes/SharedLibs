#include "stdafx.h"

#include "SharedBase.h"

#include "Utils/SharedFile.h"

#include <fstream>
#include <filesystem>

#include "String/SharedString.h"

#include <Windows.h>

#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

namespace Shared
{
    namespace File
    {
        XString GetCurrentPath( bool bIncludeSeparator /*= false*/ )
        {
            std::wstring ret;
            wchar_t result[ MAX_PATH ];
            std::wstring sCurrentPath = std::wstring( result, GetModuleFileName( NULL, result, MAX_PATH ) );
            sCurrentPath = NormalizePath( sCurrentPath ).toWString();

            int find = sCurrentPath.rfind( L'/' );
            ret = sCurrentPath.substr( 0, find );

            if( bIncludeSeparator == true )
                ret = ret + L'/';

            return ret;
        }

        XString NormalizePath( const XString& sFilePath )
        {
            std::wstring s = sFilePath;
            std::replace( s.begin(), s.end(), L'\\', L'/' );
            return s;
        }

        bool IsExistFile( const XString& sFilePath )
        {
            bool isExist = false;
            struct _stat64i32 info;

            if( _wstat( sFilePath.c_str(), &info ) != 0 )
                isExist = false;
            else if( info.st_mode & S_IFDIR )  // 폴더의 경우는 실패로 처리
                isExist = false;
            else
                isExist = true;

            return isExist;
        }

        bool IsExistDir( const XString& sFilePath )
        {
            bool isExist = false;
            struct _stat64i32 info;

            if( _wstat( sFilePath.c_str(), &info ) != 0 )
                isExist = false;
            else if( info.st_mode & S_IFDIR )
                isExist = true;
            else
                isExist = false;

            return isExist;
        }

        XString GetFileExts( const XString& sFileFullPath )
        {
            if( sFileFullPath.IsEmpty() == true )
                return XString();

            std::wstring sExts = sFileFullPath;
            int find = sExts.rfind( L'.' );

            // 확장자가 없는 경우 전체경로를 그대로 돌려줌
            if( find == -1 )
                return sFileFullPath;

            return sExts.substr( find, sExts.length() - find );
        }

        XString GetFileBaseName( const XString& sFileFullPath )
        {
            // TODO : 현재 확장자가 없는 파일은 고려되고 있지 않음 추가 필요
            if( sFileFullPath.IsEmpty() == true )
                return XString();

            std::wstring sExts = sFileFullPath;
            int find = sExts.rfind( L'.' );
            return sExts.substr( 0, find );
        }

        XString FindFileName( const XString& sFileFullPath )
        {
            XString ws = sFileFullPath.substr( sFileFullPath.find_last_of( L"/" ) + 1 );
            return ws;
        }

        XString FindFilePath( const XString& sFileFullPath )
        {
            XString ws = sFileFullPath.substr( 0, sFileFullPath.find_last_of( L"/" ) + 1 );
            return ws;
        }

        TyEnFileType RetrieveFileType( const XString& sFilePath )
        {
            TyEnFileType eFileType = FILE_TYPE_NONE;

            do
            {
                if( IsExistFile( sFilePath ) == false )
                    break;

                std::ifstream file( sFilePath.c_str() );

                if( file.is_open() == false )
                    break;

                unsigned char sReadData[ 4 ];
                ZeroMemory( sReadData, 4 );

                file.read( ( char* )sReadData, 4 );

                if( sReadData[ 0 ] == 0x50 && sReadData[ 1 ] == 0x4b && sReadData[ 2 ] == 0x03 && sReadData[ 3 ] == 0x04 )
                    eFileType = FILE_TYPE_ZIP;
                else if( sReadData[ 0 ] == 0x53 && sReadData[ 1 ] == 0x51 && sReadData[ 2 ] == 0x4C && sReadData[ 3 ] == 0x69 )
                    eFileType = FILE_TYPE_DB;
                else if( sReadData[ 0 ] == 0x18 && sReadData[ 1 ] == 0xE2 && sReadData[ 2 ] == 0x2D && sReadData[ 3 ] == 0x00 )
                    eFileType = FILE_TYPE_DB_SHM;
                else if( sReadData[ 0 ] == 0x37 && sReadData[ 1 ] == 0x7F && sReadData[ 2 ] == 0x06 && sReadData[ 3 ] == 0x82 )
                    eFileType = FILE_TYPE_DB_WAL;
            }
            while( false );

            return eFileType;
        }

        std::vector<XString> GetFileListFromFolder( const XString& sFileFullPath )
        {
            std::vector< XString > vecFileList;

            for( const auto& entry : std::filesystem::directory_iterator( sFileFullPath.c_str() ) )
                vecFileList.push_back( entry.path().wstring() );

            return vecFileList;
        }
    }
}
