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


typedef enum _tyToastUsingBtn
{
    BTN_NONE,
    BTN_SINGLE,
    BTN_DOUBLE
} TOAST_BUTTON;

class cToastNotificationMgr : public QObject
{
    Q_OBJECT

public:
    cToastNotificationMgr( QWidget* parent = NULLPTR );
    ~cToastNotificationMgr();

    QSize                                 GetPosition();
    void                                  MoveStack();
    Q_INVOKABLE void                      ShowToastMsg( TOAST_ICON icon, TOAST_BUTTON btn, XString sTitle, XString sMsg, int nTimeOutSec = 10 );
    Q_INVOKABLE void                      ShowToastMsg( QString sTitle, QString sMsg );

private:
    QWidget*                              _wdgParent;
    QVector<cToastNotification*>          _vecNotification;

    QSize                                 _ToastSize;
    QSize                                 _SpaceSize;
};

class cToastNotification : public QObject
{
    Q_OBJECT

public:
    cToastNotification();
    cToastNotification( QWidget* parent, TOAST_ICON icon, TOAST_BUTTON btn, XString sTitle, XString sMsg, int nTimeOutSec = 10 );
    //cToastNotification( QWidget* parent, XString sMsg );
    ~cToastNotification();

    void                                  Init();
    void                                  SetToastUI();
    void                                  SetToastBoxSize( QSize size );
    void                                  SetSpaceSize( QSize size );

    void                                  ShowToast();
    void                                  MoveToastUI( int x, int y );

public slots:
    void                                  Close();

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

    XString                               _sTitle;
    XString                               _sMsg;

    QPushButton*                          _btnLeft;
    QPushButton*                          _btnRight;
    QPushButton*                          _btnClose;
};

#endif

#endif