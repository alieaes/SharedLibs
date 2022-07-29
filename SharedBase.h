#ifndef __SHARED_LIBS_BASE__
#define __SHARED_LIBS_BASE__

#include "Shared_Config.hpp"

#include "Shared_Defs.h"

#include "Singleton.h"

#ifndef SHARED_SET_CONFIG
static_assert( false, "Modify the 'Shared_Config.hpp.sample' file and save it as a 'Shared_Config.hpp'" );
#endif // ! SHARED_SET_CONFIG

#ifdef USING_QT_LIBS
#include <QString>
#endif

#endif