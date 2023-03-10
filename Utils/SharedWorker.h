#ifndef __HDR_SHARED_WORKER__
#define __HDR_SHARED_WORKER__

#include <sstream>
#include <vector>

#include "Utils/SharedInvokeUtils.h"
#include "String/SharedXString.h"

#include "Shared_Defs.h"

namespace Shared
{
    typedef struct _tyWorkItem
    {
        // UNIQUE NAME
        XString                            sWorkerName;

        // 최초 실행 시 실행될 주기, 0인 경우 즉시 실행
        N_TIME_MSEC                        nFirstRunSec       = 0;

        // 최초 실행 이후 실행될 주기, isCycle을 사용하고 있을 때에만 확인함. ( 0인 경우 실행되지 않음 )
        N_TIME_MSEC                        nCycleSec          = 0;

        // 반복하여 실행할지의 여부, false인 경우 반복하지 않고 1회만 실행함. 기본 값 false
        bool                               isCycle = false;

        // 반복될 횟수, 0이하인 경우 무한 반복함. ( 1인 경우 isCycle이 false인 경우와 같음 )
        DWORD                              dwRepeatCnt         = 0;

        // 현재 실행되는 작업이 종료된 후 실행할지 여부, 기본 값 true
        bool                               isRunAfterShutdown = true;

        // 실행하려는 함수, cInvoke의 인스턴스를 생성해야함.
        std::shared_ptr<BaseInvoker>       invoke;

    } WORK_ITEM;

    typedef struct _tyWorkMgr
    {
        WORK_ITEM                          item;

        bool                               isStop             = false;
        bool                               isFirstRun         = false;
        bool                               isRunning          = false;
        DWORD                              nRunCnt            = 0;

        std::thread                        thWorker;

    } S_WORK_MGR;

    class cSWorker
    {
    public:
        cSWorker();
        ~cSWorker() {};

        bool                               Register( WORK_ITEM item );
        bool                               DeRegister( XString sWorkerName );
        bool                               RegisterAndRunning( WORK_ITEM item );
        bool                               Run( XString sWorkerName );
        bool                               IsExistWorker( XString sWorkerName );
    private:
        void                               worker( XString sWorkerName );

        S_WORK_MGR&                        getWorkItem( XString sWorkerName );
        void                               setWorkItem( WORK_ITEM item );

        std::map< XString, S_WORK_MGR >     _mapWorkItem;
        std::mutex                          _lck;
    };
}

typedef Shared::Singletons::Singleton<Shared::cSWorker> TyStWorker;


#endif
