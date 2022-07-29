#ifndef __SHARED_LIBS_SINGLETON__
#define __SHARED_LIBS_SINGLETON__

#include <mutex>

#include "Shared_Defs.h"

/*
* Class T를 SingleTon으로 사용할 경우, 헤더파일의 전역으로 typedef를 설정
* typedef Shared::Singletons::Singleton<T> typedefName;
* 이후 GetInstance로 Instance를 가져옴
*/

namespace Shared
{
    namespace Singletons
    {
        template < typename T >
        class Singleton
        {
        public:
            Singleton() {}

            static T* GetInstance()
            {
                if( _instance == NULLPTR )
                    std::call_once( _flag, []() { _instance = new T; } );

                return _instance;
            }

            static void DestroyInstance()
            {
                if( _instance != NULLPTR )
                    delete _instance;
            }

        private:
            static T* _instance;
            static std::once_flag _flag;
        };

        template < typename T >
        T* Singleton<T>::_instance = NULLPTR;

        template < typename T >
        std::once_flag Singleton<T>::_flag;
    }
}

#endif
