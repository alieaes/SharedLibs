#ifndef __HDR_SHARED_FORMAT__
#define __HDR_SHARED_FORMAT__

#include <cassert>

#include "SharedBase.h"

#include "String/SharedXString.h"

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
            DATA_TYPE_QDATETIME,
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
            FormatArg( long l )
            {
                _eType = DATA_TYPE_LONG;
                _dw = l;
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
            FormatArg( const wchar_t wc )
            {
                _eType = DATA_TYPE_STRING;
                _xs = wc;
            };
            FormatArg( const wchar_t* wc )
            {
                _eType = DATA_TYPE_STRING;
                _xs = wc;
            }
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
            FormatArg( XString xs )
            {
                _eType = DATA_TYPE_STRING;
                _xs = xs;
            }
#ifdef USING_QT_LIBS
            FormatArg( QDateTime dt )
            {
                _eType = DATA_TYPE_STRING;
                _dt = dt;
            }
#endif
            bool IsInit() { return _init == INT_MAX ? true : false; }

            XString Value()
            {
                switch( _eType )
                {
                case DATA_TYPE_NONE: { return "FMT ERROR"; } break;
                case DATA_TYPE_INT: { return ( int )_n; } break;
                case DATA_TYPE_UINT: { return ( int )_d; } break;
                case DATA_TYPE_LONG: { return _dw; } break;
                case DATA_TYPE_ULONG: { return ( int )_d; } break;
                case DATA_TYPE_DOUBLE: { return ( double )_d; } break;
                case DATA_TYPE_STRING: { return _xs; } break;
                case DATA_TYPE_BOOL: { return ( int )_b; } break;
#ifdef USING_QT_LIBS
                case DATA_TYPE_QDATETIME: { return _dt.toString( DEFAULT_TIME_STRING ); } break;
#endif
                default: { return "FMT ERROR"; } break;
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
#ifdef USING_QT_LIBS
            QDateTime _dt;
#endif
        };

        inline XString TypeCvt( XString sType, int nNum, bool bNumbers )
        {
            XString sRet = "{";
            if( bNumbers == true )
                sRet += XString( nNum ) + sType;
            else
                sRet += sType;

            sRet += "}";

            return sRet;
        }

        template< typename ... Args >
        XString Format( XString fmt, Args ... args )
        {
            XString sRet = fmt;
            FormatArg arr[] = { args..., NULL };

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
                    size_t nStart = sRet.find( "{" );
                    size_t nEnd = sRet.find( "}" );

                    size_t nType = nEnd - nStart;
                    XString sSub = sRet.substr( nStart, nEnd - nStart + 1 );

                    if( sSub.startswith( "{{" ) == true )
                    {
                        XString sTmp = sRet;
                        sTmp = sRet.replace( "{{", "[[" );

                        nStart = sTmp.find( "{" );

                        if( nStart < 0 )
                        {
                            *sRet++;
                            continue;
                        }

                        nEnd = sRet.find( "}" );

                        nType = nEnd - nStart;
                        sSub = sRet.substr( nStart, nEnd - nStart + 1 );
                    }

                    if( nType == 1 )
                    {
                        if( nArrIdx >= nSize )
                            assert( false );

                        sRet = sRet.replace( "{}", arr[ nArrIdx++ ].Value() );
                    }
                    else if( nType > 1 )
                    {
                        size_t nTypeStart = nStart + 1;
                        size_t nTypeEnd = nEnd;
                        XString sType = sRet.substr( nTypeStart, nTypeEnd - nTypeStart );

                        if( sType.contains( ":" ) == true )
                        {
                            int nNum = nArrIdx;
                            bool bNumbers = false;

                            if( sType.startswith( ":" ) == true )
                            {
                                nArrIdx++;
                            }
                            else
                            {
                                size_t nSep = sType.find( ":" );
                                XString sNum = sType.substr( 0, nSep );

                                if( sNum.IsDigit() == false )
                                {
                                    *sRet++;
                                    continue;
                                }

                                nNum = sNum.toInt();
                                bNumbers = true;
                            }

                            if( sType.contains( ":b" ) == true )
                            {
                                if( nArrIdx >= nSize )
                                    assert( false );

                                sRet = sRet.replace( TypeCvt( ":b", nNum, bNumbers ), arr[ nNum ].Value() == 1 ? "true" : "false" );
                            }
                        }
                        else
                        {
                            if( sType.IsDigit() == false )
                            {
                                *sRet++;
                                continue;
                            }

                            int nNum = sType.toInt();
                            if( nNum > nSize )
                                assert( false );

                            XString sTmp = "{";
                            sRet = sRet.replace( sTmp + sType + "}", arr[ nNum ].Value() );
                        }
                    }
                }
            }

            sRet = sRet.replaceAll( "{{", "{" );
            sRet = sRet.replaceAll( "}}", "}" );
            return sRet;
        }
    }
}

#endif