#include "stdafx.h"

#include "Utils/SharedWorker.h"

#include <Windows.h>

#ifdef USING_QT_LIBS
#include <qdatetime.h>
#endif

#pragma warning(disable:4996)

namespace Shared
{
    cSWorker::cSWorker()
    {
    }

    bool cSWorker::Register( WORK_ITEM item )
    {
        bool isSuccess = false;

        do
        {
            if( item.sWorkerName.IsEmpty() == true )
                break;

            if( item.isCycle == true )
            {
                if( item.nCycleSec <= 0 )
                    break;
            }

            setWorkItem( item );

            isSuccess = true;

        } while( false );

        return isSuccess;
    }

    bool cSWorker::DeRegister( XString sWorkerName )
    {
        bool isSuccess = false;

        do
        {
            if( _mapWorkItem.count( sWorkerName ) <= 0 )
                break;

            auto& mgr = getWorkItem( sWorkerName );

            mgr.isStop = true;

            if( mgr.item.isCycle == true )
                mgr.thWorker.join();

            _mapWorkItem.erase( sWorkerName );

            isSuccess = true;

        } while( false );

        return isSuccess;
    }

    bool cSWorker::RegisterAndRunning( WORK_ITEM item )
    {
        bool isSuccess = false;

        do
        {
            if( Register( item ) == false )
                break;

            if( Run( item.sWorkerName ) == false )
                break;

            isSuccess = true;
        }
        while( false );

        return isSuccess;
    }

    bool cSWorker::Run( XString sWorkerName )
    {
        bool isSuccess = false;

        do
        {
            if( _mapWorkItem.count( sWorkerName ) <= 0 )
                break;

            auto& mgr = getWorkItem( sWorkerName );

            if( mgr.item.isCycle == false )
            {
                auto delayed_call = std::async( std::launch::async, [&] {
                    std::this_thread::sleep_for( std::chrono::milliseconds( mgr.item.nFirstRunSec ) ); 
                    mgr.item.invoke->Invoke();
                    DeRegister( sWorkerName );
                } );
            }
            else
            {
                mgr.thWorker = std::thread( std::bind( &cSWorker::worker, this, sWorkerName ) );
            }

            isSuccess = true;
        }
        while( false );

        return isSuccess;
    }

    bool cSWorker::IsExistWorker( XString sWorkerName )
    {
        bool isSuccess = false;

        do
        {
            if( _mapWorkItem.count( sWorkerName ) > 0 )
                isSuccess = true;
        }
        while( false );

        return isSuccess;
    }

    S_WORK_MGR& cSWorker::getWorkItem( XString sWorkerName )
    {
        std::lock_guard<std::mutex> lck( _lck );

        // NULL 체크를 하지 않음 주의
        return _mapWorkItem[ sWorkerName ];
    }
    
    void cSWorker::setWorkItem( WORK_ITEM item )
    {
        std::lock_guard<std::mutex> lck( _lck );

        S_WORK_MGR mgr;
        mgr.item = item;
        _mapWorkItem[ item.sWorkerName ] = std::move( mgr );
    }

    void cSWorker::worker( XString sWorkerName )
    {
        S_WORK_MGR& mgr = getWorkItem( sWorkerName );

        while( true )
        {
            if( mgr.isStop == true )
                break;

            if( mgr.isFirstRun == false )
                Sleep( mgr.item.nFirstRunSec );
            else
                Sleep( mgr.item.nCycleSec );

            mgr.isRunning = true;
            mgr.item.invoke->Invoke();
            mgr.isRunning = false;

            mgr.nRunCnt++;

            if( mgr.isFirstRun == false )
                mgr.isFirstRun = true;

            if( mgr.item.isCycle == false )
                break;

            if( mgr.nRunCnt >= mgr.item.dwRepeatCnt )
                break;
        }
    }
}
