#ifndef __HDR_SHARED_INVOKE_UTILS__
#define __HDR_SHARED_INVOKE_UTILS__

#include "SharedBase.h"

class BaseInvoker
{
public:
    virtual void Invoke() = 0;
};

// Qt에서 실행되어야 하는 함수의 경우 예외 발생함. Wrapper함수로 invokeMethod 호출이 필요함.
template<typename Ret, typename... Args>
class cSInvoke : public BaseInvoker
{
public:
    cSInvoke( std::function<Ret( Args... )> f, Args... args )
    : func( std::move( f ) ), params( args... )
    {
        
    }

    Ret Invoke()
    {
        return std::apply( func, params );
    }

    void InvokeInThread()
    {
        auto f = std::async( std::launch::async, [this]() {return std::apply( func, params ); } );
    }

private:
    std::function<Ret( Args... )> func;
    std::tuple<Args...>           params;
};

#endif