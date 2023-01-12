#include "stdafx.h"

#include "SharedQToastNotification.h"

#include "SharedBase.h"

#include <Windows.h>

#include "String/SharedFormat.h"


#ifdef USING_QTLIB
#include <qdatetime.h>
#endif

const QString BTN_CSS = "QPushButton { border: 2px solid rgb(52, 59, 72); border-radius: 5px; background-color: rgb(52, 59, 72); color: white; } QPushButton:hover { background-color: rgb(57, 65, 80); border: 2px solid rgb(61, 70, 86); } QPushButton:pressed { background-color: rgb(35, 40, 49); border: 2px solid rgb(43, 50, 61); }";

cToastNotification::cToastNotification()
{
    Init();
    SetToastUI();
}

cToastNotification::cToastNotification( QWidget* parent, TOAST_ICON icon, int nTimeOutSec, TOAST_BUTTON btn, XString sMsg )
{
    Init();

    _wdgParent = parent;
    _eIcon = icon;
    _eBtn = btn;
    _sMsg = sMsg;

    SetToastUI();
}

cToastNotification::~cToastNotification()
{
}

void cToastNotification::Init()
{
    _wdgToastMsg = NULLPTR;
    _wdgParent = NULLPTR;
    _eIcon = ICON_INFO;
    _eBtn = BTN_DOUBLE;
    _nTimeOutSec = 10;
}

void cToastNotification::SetToastUI()
{
    SetToastBoxSize( 350, 100 );
    SetSpaceSize( 10, 20 );

    _wdgBase = new QWidget();
    _wdgBase->setAttribute( Qt::WA_TranslucentBackground, true );

    _wdgBase->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _wdgBase->setBaseSize( QSize( _nWidth + _spaceSize.width(), _nHeight + _spaceSize.height() ) );
    _wdgBase->setMaximumSize( QSize( _nWidth + _spaceSize.width(), _nHeight + _spaceSize.height() ) );
    _wdgBase->setMinimumSize( QSize( _nWidth + _spaceSize.width(), _nHeight + _spaceSize.height() ) );
    _wdgBase->setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SubWindow );

    _wdgToastMsg = new QWidget( _wdgBase );

    _wdgToastMsg->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    _wdgToastMsg->setBaseSize( QSize( _nWidth, _nHeight ) );
    _wdgToastMsg->setMaximumSize( QSize( _nWidth, _nHeight ) );
    _wdgToastMsg->setMinimumSize( QSize( _nWidth, _nHeight ) );
    _wdgToastMsg->setWindowFlags( Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SubWindow );

    XString sIcon;
    XString sBgColor;
    switch( _eIcon )
    {
        case ICON_NONE: { sBgColor = "#272727"; } break;
        case ICON_INFO: { sIcon = ":/ICO/icon/icoInfo64.png"; sBgColor = "#3cacc4"; }break;
        case ICON_WARNING: { sIcon = ":/ICO/icon/icoWarning64.png"; sBgColor = "#3cacc4"; } break;
        case ICON_ERROR: { sIcon = ":/ICO/icon/icoError64.png"; sBgColor = "#3cacc4"; } break;
        case ICON_SUCCESS: { sIcon = ":/ICO/icon/icoAccept64.png"; sBgColor = "#3cacc4"; } break;
        default: break;
    }

    // format 에러 처리 필요 XString sStyle = Shared::Format::Format( "QWidget { background-color:{}; border-radius: 3px; }", sBgColor );

    XString sStyle = QString( "QWidget { background-color:%1; border-radius: 3px; }" ).arg( sBgColor );
    _wdgToastMsg->setStyleSheet( sStyle );

    QLayout* left = new QHBoxLayout;
    left->setContentsMargins( 0, 0, 0, 0 );

    if( _eIcon != ICON_NONE )
    {
        QFrame* frmTmp = new QFrame;
        QLayout* tmp = new QHBoxLayout;
        tmp->setContentsMargins( 10, 0, 10, 0 );

        frmTmp->setMaximumHeight( 50 );
        frmTmp->setMaximumWidth( 50 );

        _lblIcon = new QLabel;
        _lblIcon->setMaximumWidth( 30 );
        _lblIcon->setMaximumHeight( 30 );

        tmp->addWidget( _lblIcon );
        frmTmp->setLayout( tmp );

        QPixmap pixmap( sIcon );
        _lblIcon->setPixmap( pixmap );
        _lblIcon->setScaledContents( true );
        _lblIcon->resize( 50, 50 );

        left->addWidget( frmTmp );
    }

    QFrame* frm = new QFrame;
    QLayout* center = new QVBoxLayout;
    center->setContentsMargins( 0, 0, 0, 0 );
    center->setSpacing( 2 );
    frm->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    QLabel* lblTitle = new QLabel;
    lblTitle->setText( "<b>TITLE</b>" );
    center->addWidget( lblTitle );
    QLabel* lblContens = new QLabel;
    lblContens->setText( "TESTSETESTSET" );
    center->addWidget( lblContens );

    if( _eBtn != BTN_NONE )
    {
        QFrame* frm2 = new QFrame;
        QLayout* btnLayout = new QHBoxLayout;
        btnLayout->setContentsMargins( 0, 0, 0, 0 );
        frm2->setContentsMargins( 0, 10, 0, 0 );
        frm2->setLayout( btnLayout );

        _btnLeft = new QPushButton;
        _btnLeft->setText( "LEFT" );
        btnLayout->addWidget( _btnLeft );
        _btnLeft->setStyleSheet( BTN_CSS );
        if( _eBtn == BTN_DOUBLE )
        {
            _btnRight = new QPushButton;
            _btnRight->setText( "RIGHT" );
            _btnRight->setStyleSheet( BTN_CSS );
            btnLayout->addWidget( _btnRight );
        }

        center->addWidget( frm2 );
    }
    else
    {
        
    }

    frm->setLayout( center );
    left->addWidget( frm );

    _btnClose = new QPushButton;
    _btnClose->setIcon( QIcon( ":/ICO/icon/cil-x.png" ) );
    _btnClose->setMaximumWidth( 30 );
    _btnClose->setMaximumHeight( 30 );

    QFrame* frmRight = new QFrame;
    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins( 0, 5, 5, 0 );

    lay->addWidget( _btnClose, 0, Qt::AlignTop );
    frmRight->setLayout( lay );

    left->addWidget( frmRight );
    _wdgToastMsg->setLayout( left );

    prepareToast();
}

void cToastNotification::SetToastBoxSize( int width, int height )
{
    _nWidth = width;
    _nHeight = height;
}

void cToastNotification::SetSpaceSize( int width, int height )
{
    _spaceSize.setWidth( width );
    _spaceSize.setHeight( height );
}

void cToastNotification::prepareToast()
{
    // parent가 없는 경우에는 데스크탑에 띄우도록 함
    if( _wdgParent == NULLPTR )
    {
        auto primayScreen = QGuiApplication::primaryScreen()->availableGeometry();

        _wdgBase->move( primayScreen.width() - _wdgBase->width(), primayScreen.height() - _wdgBase->height() );
        _wdgBase->show();
    }
    else
    {
        auto parentScreen = _wdgParent->geometry();

        _wdgBase->move( parentScreen.width() - _wdgBase->width(), parentScreen.height() - _wdgBase->height() );
        _wdgBase->show();
    }

    _wdgToastMsg->move( _wdgBase->width(), _wdgBase->height() - _wdgToastMsg->height() - _spaceSize.height() );
    _wdgToastMsg->show();

}

void cToastNotification::ShowToast()
{
    QPropertyAnimation* _animation = new QPropertyAnimation( _wdgToastMsg, "geometry", this );

    _animation->setDuration( 500 );

    _animation->setStartValue( QRect( _wdgBase->width(),
                                      _wdgBase->height() - _wdgToastMsg->height() - _spaceSize.height(),
                                      _wdgToastMsg->width(), _wdgToastMsg->height() ) );

    _animation->setEndValue( QRect( _wdgBase->width() - _nWidth - _spaceSize.width(),
                                    _wdgBase->height() - _nHeight - _spaceSize.height(), _wdgToastMsg->width(), _wdgToastMsg->height() ) );

    connect( _animation, SIGNAL( finished() ), _animation, SLOT( deleteLater() ) );

    _animation->start( QAbstractAnimation::DeleteWhenStopped );
}
