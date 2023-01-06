#ifndef __HDR_SHARED_FORMAT__
#define __HDR_SHARED_FORMAT__

#include <string>

#include "String/SharedXString.h"

#ifdef USING_QTLIB
#include <qstring.h>
#endif // USING_QTLIB


namespace Shared
{
    namespace Format
    {
        typedef enum _eDataType
        {
            DATA_TYPE_NONE,
            DATA_TYPE_INT,
            DATA_TYPE_UINT,
            DATA_TYPE_LONG,
            DATA_TYPE_ULONG,
            DATA_TYPE_DOUBLE,
            DATA_TYPE_STRING,
            DATA_TYPE_BOOL
        } DATA_TYPE;

        class FormatArg
        {
        public:
            FormatArg( int n )
            {
                _eType = DATA_TYPE_INT;
                _n = n;
            };
            FormatArg( double d )
            {
                _eType = DATA_TYPE_DOUBLE;
                _d = d;
            }
            FormatArg( DWORD dw )
            {
                _eType = DATA_TYPE_ULONG;
                _dw = dw;
            }
            FormatArg( char* c )
            {
                _eType = DATA_TYPE_STRING;
                _xs = c;
            };
            FormatArg( const char* cc )
            {
                _eType = DATA_TYPE_STRING;
                _xs = cc;
            };
            FormatArg( std::wstring ws )
            {
                _eType = DATA_TYPE_STRING;
                _xs = ws;
            }
            FormatArg( bool b )
            {
                _eType = DATA_TYPE_BOOL;
                _b = b;
            }

            bool IsInit() { return _init == INT_MAX ? true : false; }

            XString Value()
            {
                switch( _eType )
                {
                case DATA_TYPE_NONE:
                    {
                        
                    } break;

                
                case DATA_TYPE_INT: { return ( int )_n; } break;
                case DATA_TYPE_UINT: { return ( int )_d; } break;
                case DATA_TYPE_LONG: { return ( int )_d; } break;
                case DATA_TYPE_ULONG: { return ( int )_d; } break;
                case DATA_TYPE_DOUBLE: { return ( int )_d; } break;
                case DATA_TYPE_STRING: { return _xs; } break;
                case DATA_TYPE_BOOL: { return ( int )_b; } break;
                default: ;
                }
            }
        private:
            int _init = INT_MAX;
            DATA_TYPE _eType = DATA_TYPE_NONE;
            int _n = 0;
            double _d = 0;
            unsigned long _dw = 0;
            char* _c = NULL;
            XString _xs;
            bool _b = false;
        };

        template< typename ... Args >
        XString Format( XString fmt, Args ... args )
        {
            XString sRet = fmt;
            FormatArg arr[] = { args... };

            int nArrIdx = 0;
            int nSize = 0;

            for( int idx = 0; ; ++idx )
            {
                if( arr[ idx ].IsInit() == false )
                    break;

                nSize++;
            }
            
            while( sRet.Endl() == false )
            {
                if( *sRet != '{' )
                {
                    *sRet++;
                    continue;
                }

                if( *sRet == '{' )
                {
                    int nStart = sRet.find( "{" );
                    int nEnd = sRet.find( "}" );

                    XString sSub = sRet.substr( nStart, nEnd - nStart + 1 );

                    if( sSub.compare( "{}" ) == 0 )
                    {
                        if( nArrIdx >= nSize )
                            assert( false );

                        sRet = sRet.replace( "{}", arr[ nArrIdx++ ].Value() );
                    }
                    else if( sSub.compare( "{:b}" ) == 0 )
                    {
                        if( nArrIdx >= nSize )
                            assert( false );

                        sRet = sRet.replace( "{:b}", arr[ nArrIdx++ ].Value() == 1 ? "true" : "false" );
                    }
                }
            }

            return sRet;
        }
    }
}

#endif