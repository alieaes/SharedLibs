#ifndef __HDR_SHARED_LOGGER__
#define __HDR_SHARED_LOGGER__

#include "SharedBase.h"

class XString;

namespace Shared
{
    namespace Logger
    {
        typedef enum _eLogcase
        {
            LOG_TYPE_NONE,
            LOG_TYPE_CONSOLE,
            LOG_TYPE_FILE,
            LOG_TYPE_ALL
        } LOG_CASE;

        typedef struct _stLogInfo
        {
            std::wstring               sLoggerName;
            std::wstring               sFileName;
            std::wstring               sFilePath;
        } LOGGER_INFO;

        class cLogger
        {
        public:
            cLogger();
            ~cLogger();

            bool                  Init( LOGGER_INFO info );
            void                  LoggingCase( LOG_CASE eCase, XString sFileName, int nCodeLine, XString sStr );
            template< typename ... Args >
            void                  LoggingCase( LOG_CASE eCase, XString sFileName, int nCodeLine, XString sStr, Args ... args );
            void                  LoggingToConsole( XString sLog );
            void                  LoggingToFile( XString sLog );
        private:
            bool                  _isInit = false;

            LOGGER_INFO           _info;
        };

        template <typename ... Args>
        void cLogger::LoggingCase( LOG_CASE eCase, XString sFileName, int nCodeLine, XString sStr, Args... args )
        {
        }
    }
}

typedef Shared::Singletons::Singleton<Shared::Logger::cLogger> TyStLogger;

#endif