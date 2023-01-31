#ifndef __SHARED_LIBS_BASE__
#define __SHARED_LIBS_BASE__

#include "Shared_Config.hpp"
#include "Shared_Defs.h"

#include "Singleton.h"

#include "String/SharedString.h"

#include "String/SharedXString.h"

#ifndef SHARED_SET_CONFIG
static_assert( false, "Modify the 'Shared_Config.hpp.sample' file and save it as a 'Shared_Config.hpp'" );
#endif // ! SHARED_SET_CONFIG

#include <string>
#include <windows.h>

#ifdef USING_QT_LIBS
#include <QString>
#endif

#ifdef USING_SHARED_LOGGER
#include "String/SharedFormat.h"
#include "Logger/SharedLogger.h"

#define CONSOLEP( str, ...) \
        TyStLogger::GetInstance()->LoggingCase( Shared::Logger::LOG_TYPE_CONSOLE, __FILE__, __LINE__, Shared::Format::Format( str, __VA_ARGS__ ) );

#endif // USING_SHARED_LOGGER

#endif