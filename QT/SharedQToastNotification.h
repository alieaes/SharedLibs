#ifndef __HDR_SHARED_QTOAST__
#define __HDR_SHARED_QTOAST__

#include "SharedBase.h"

#include <string>

#ifdef USING_QT_LIBS
#include <qobject.h>
#include <qstring.h>
#endif // USING_QTLIB


#ifdef USING_QT_LIBS

class cToastNotification;

typedef enum _tyToastIcon
{
    ICON_NONE,
    ICON_INFO,
    ICON_WARNING,
    ICON_ERROR,
    ICON_SUCCESS
} TOAST_ICON;

Q_DECLARE_METATYPE( TOAST_ICON );

typedef enum _tyToastUsingBtn
{
    BTN_NONE,
    BTN_SINGLE,
    BTN_DOUBLE
} TOAST_BUTTON;

typedef enum _tyToastPosition
{
    RIGHT_BOTTOM,
    RIGHT_TOP
} TOAST_POSITION;

class cToastNotificationMgr : public QObject
{
    Q_OBJECT

public:
    cToastNotificationMgr( QWidget* parent = NULLPTR );
    ~cToastNotificationMgr();

    QSize                                 GetParentSize();
    QSize                                 GetPosition();
    void                                  MoveStack();
    Q_INVOKABLE void                      ShowToastMsg( TOAST_ICON icon, TOAST_BUTTON btn, XString sTitle, XString sMsg, int nTimeOutSec = 10 );
    Q_INVOKABLE void                      ShowToastMsg( QString sTitle, QString sMsg, TOAST_ICON icon /*= ICON_INFO*/ );

    void                                  CloseToast( cToastNotification* toast );

    void                                  SetPosition( TOAST_POSITION position );

    void                                  Clear();
private:

    void                                  deleteToast( cToastNotification* toast );
    void                                  insertToast( cToastNotification* toast );

    std::mutex                            _lck;
    QWidget*                              _wdgParent;
    QVector<cToastNotification*>          _vecNotification;

    TOAST_POSITION                        _position;

    QSize                                 _ToastSize;
    QSize                                 _SpaceSize;
};

const int nTimerInterval = 10;
class cToastNotification : public QDialog
{
    Q_OBJECT

public:
    cToastNotification();
    cToastNotification( cToastNotificationMgr* pToastMgr, QWidget* parent, TOAST_ICON icon, TOAST_BUTTON btn, TOAST_POSITION position, XString sTitle, XString sMsg, int nTimeOutSec = 10 );
    //cToastNotification( QWidget* parent, XString sMsg );
    ~cToastNotification();

    void                                  Init();
    void                                  SetToastUI();
    void                                  SetToastBoxSize( QSize size );
    void                                  SetSpaceSize( QSize size );

    QSize                                 GetPosition();
    void                                  ShowToast();
    void                                  MoveToastUI( int x, int y );

    virtual bool                          eventFilter( QObject* watched, QEvent* event ) override;

public slots:
    void                                  Close();
    void                                  SetTimerValue();

private:
    void                                  prepareToast();

private:
    QWidget*                              _wdgParent;
    QWidget*                              _wdgBase;
    QWidget*                              _wdgToastMsg;
    int                                   _nWidth;
    int                                   _nHeight;
    QSize                                 _spaceSize;

    QLabel*                               _lblIcon;
    int                                   _nTimeOutSec;

    TOAST_ICON                            _eIcon;
    TOAST_BUTTON                          _eBtn;
    TOAST_POSITION                        _position;

    XString                               _sTitle;
    XString                               _sMsg;

    QPushButton*                          _btnLeft;
    QPushButton*                          _btnRight;
    QPushButton*                          _btnClose;

    cToastNotificationMgr*                _pToastMgr;

    QTimer*                               _timer;
    int                                   _nCurrentTimeout = 0;
    QProgressBar*                         _progressbar;

    QPropertyAnimation*                   _progressbarAnimation;
};

// 윈도우에 표기하는 건 Singleton으로 사용하도록 하고 각 UI에서 사용할 때는 따로 인스턴스를 생성하도록 함
typedef Shared::Singletons::Singleton<cToastNotificationMgr> TyStDesktopNotification;

#endif

#endif