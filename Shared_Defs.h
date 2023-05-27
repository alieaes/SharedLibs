#ifndef __SHARED_LIBS_DEFS__
#define __SHARED_LIBS_DEFS__

#define NULLPTR nullptr

#define IF_FALSE_BREAK(x) if( (x) == false ) break;
#define IF_TRUE_BREAK(x) if( (x) == true ) break;

#define IF_FALSE_CONTINUE(x) if( (x) == false ) continue;
#define IF_TRUE_CONTINUE(x) if( (x) == true ) continue;

#define DEFAULT_TIME_STRING "yyyy-MM-dd HH:mm:ss"

#define PORT_RANGE_MAX 65535

typedef unsigned int N_TIME_SEC;
typedef unsigned int N_TIME_MSEC;

#endif

