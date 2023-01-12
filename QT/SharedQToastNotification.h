#ifndef __HDR_SHARED_QT_UTILS__
#define __HDR_SHARED_QT_UTILS__

#include "SharedBase.h"

#include <string>

#ifdef USING_QT_LIBS
#include <qobject.h>
#include <qstring.h>
#endif // USING_QTLIB


#ifdef USING_QT_LIBS

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

class cToastNotification : public QObject
{
    Q_OBJECT

public:
    cToastNotification();
    cToastNotification( QWidget* parent, TOAST_ICON icon, int nTimeOutSec, TOAST_BUTTON btn, XString sMsg );
    //cToastNotification( QWidget* parent, XString sMsg );
    ~cToastNotification();

    void                                  Init();
    void                                  SetToastUI();
    void                                  SetToastBoxSize( int width, int height );
    void                                  SetSpaceSize( int width, int height );

    void                                  ShowToast();

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

    XString                               _sMsg;

    QPushButton*                          _btnLeft;
    QPushButton*                          _btnRight;
    QPushButton*                          _btnClose;
};

#endif

#endif