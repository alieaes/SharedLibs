/*
 * https://stackoverflow.com/questions/14780517/toggle-switch-in-qt
 *
 * This is nearly complete Material design Switch widget implementation in qtwidgets module.
 * More info: https://material.io/design/components/selection-controls.html#switches
 * Copyright (C) 2018-2020 Iman Ahmadvand
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
*/

#ifndef SWITCH_H
#define SWITCH_H

#include <QtWidgets>

#include <QtCore/qeasingcurve.h>

#define QT_SWITCH_COLOR_CYAN500 QColor("#00bcd4")
#define QT_SWITCH_COLOR_GRAY50 QColor("#fafafa")
#define QT_SWITCH_COLOR_BLACK QColor("#000000")
#define QT_SWITCH_COLOR_GRAY400 QColor("#bdbdbd")

Q_DECL_IMPORT void qt_blurImage( QPainter* p, QImage& blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0 ); // src/widgets/effects/qpixmapfilter.cpp

namespace Shared
{
    namespace Qt
    {
        namespace Style
        {
            using Type = QEasingCurve::Type;

            struct Animation
            {
                Animation() = default;
                Animation( Type _easing, int _duration ) :easing{ _easing }, duration{ _duration } {

                }

                Type easing;
                int duration;
            };

            struct Switch
            {
                Switch() :
                    height{ 36 },
                    font{ QFont( "Roboto medium", 13 ) },
                    indicatorMargin{ QMargins( 8, 8, 8, 8 ) },
                    thumbOnBrush{ QT_SWITCH_COLOR_CYAN500 },
                    thumbOnOpacity{ 1 },
                    trackOnBrush{ QT_SWITCH_COLOR_CYAN500 },
                    trackOnOpacity{ 0.5 },
                    thumbOffBrush{ QT_SWITCH_COLOR_GRAY50 },
                    thumbOffOpacity{ 1 },
                    trackOffBrush{ QT_SWITCH_COLOR_BLACK },
                    trackOffOpacity{ 0.38 },
                    thumbDisabled{ QT_SWITCH_COLOR_GRAY400 },
                    thumbDisabledOpacity{ 1 },
                    trackDisabled{ QT_SWITCH_COLOR_BLACK },
                    trackDisabledOpacity{ 0.12 },
                    textColor{ QT_SWITCH_COLOR_BLACK },
                    disabledTextOpacity{ 0.26 },
                    thumbBrushAnimation{ Animation( Type::Linear, 150 ) },
                    trackBrushAnimation{ Animation( Type::Linear, 150 ) },
                    thumbPosAniamtion{ Animation( Type::InOutQuad, 150 ) }
                {

                }

                int                             height;
                QFont                           font;
                QMargins                        indicatorMargin;
                QColor                          thumbOnBrush;
                double                          thumbOnOpacity;
                QColor                          trackOnBrush;
                double                          trackOnOpacity;
                QColor                          thumbOffBrush;
                double                          thumbOffOpacity;
                QColor                          trackOffBrush;
                double                          trackOffOpacity;
                QColor                          thumbDisabled;
                double                          thumbDisabledOpacity;
                QColor                          trackDisabled;
                double                          trackDisabledOpacity;
                QColor                          textColor;
                double                          disabledTextOpacity;
                Animation                       thumbBrushAnimation;
                Animation                       trackBrushAnimation;
                Animation                       thumbPosAniamtion;
            };

            inline QPixmap drawShadowEllipse( qreal radius, qreal elevation, const QColor& color )
            {
                auto px = QPixmap( radius * 2, radius * 2 );
                px.fill(::Qt::transparent );

                // draw ellipes
                { 
                    QPainter p( &px );
                    p.setBrush( color );
                    p.setPen( ::Qt::NoPen );
                    p.setRenderHint( QPainter::Antialiasing, true );
                    p.drawEllipse( QRectF( 0, 0, px.size().width(), px.size().height() ).center(), radius - elevation, radius - elevation );
                }

                QImage tmp( px.size(), QImage::Format_ARGB32_Premultiplied );
                tmp.setDevicePixelRatio( px.devicePixelRatioF() );
                tmp.fill( 0 );
                QPainter tmpPainter( &tmp );
                tmpPainter.setCompositionMode( QPainter::CompositionMode_Source );
                tmpPainter.drawPixmap( QPointF(), px );
                tmpPainter.end();

                // blur the alpha channel
                QImage blurred( tmp.size(), QImage::Format_ARGB32_Premultiplied );
                blurred.setDevicePixelRatio( px.devicePixelRatioF() );
                blurred.fill( 0 );
                {
                    QPainter blurPainter( &blurred );
                    qt_blurImage( &blurPainter, tmp, elevation * 4., true, false );
                }

                tmp = blurred;

                return QPixmap::fromImage( tmp );
            }
        } // namespace Style    
    }
}

class Animator final : public QVariantAnimation
{
    Q_OBJECT
        Q_PROPERTY( QObject* targetObject READ targetObject WRITE setTargetObject )

public:
    Animator( QObject* target, QObject* parent = nullptr );
    ~Animator() override;

    QObject*                                targetObject() const;
    void                                    setTargetObject( QObject* target );

    inline bool isRunning() const
    {
        return state() == Running;
    }

public slots:
    void                                    setup( int duration, QEasingCurve easing = QEasingCurve::Linear );
    void                                    interpolate( const QVariant& start, const QVariant& end );
    void                                    setCurrentValue( const QVariant& );

protected:
    void                                    updateCurrentValue( const QVariant& value ) override final;
    void                                    updateState( QAbstractAnimation::State newState, QAbstractAnimation::State oldState ) override final;

private:
    QPointer<QObject>                       target;
};

class SelectionControl : public QAbstractButton
{
    Q_OBJECT

public:
    explicit SelectionControl( QWidget* parent = nullptr );
    ~SelectionControl() override;

    Qt::CheckState                          checkState() const;

Q_SIGNALS:
    void                                    stateChanged( int );

protected:
    virtual void                            enterEvent( QEnterEvent* e );
    void                                    checkStateSet() override;
    void                                    nextCheckState() override;
    virtual void                            toggle( Qt::CheckState state ) = 0;
};

class Switch final : public SelectionControl
{
    Q_OBJECT

        static constexpr auto CORNER_RADIUS = 8.0;
    static constexpr auto THUMB_RADIUS = 14.5;
    static constexpr auto SHADOW_ELEVATION = 2.0;

public:
    explicit Switch( QWidget* parent = nullptr );
    Switch( const QString& text, QWidget* parent = nullptr );
    Switch( const QString& text, const QBrush&, QWidget* parent = nullptr );
    ~Switch() override;

    QSize sizeHint() const override final;

protected:
    static inline QColor colorFromOpacity( const QColor& c, qreal opacity )
    {
        return QColor( c.red(), c.green(), c.blue(), qRound( opacity * 255.0 ) );
    }
    static inline bool ltr( QWidget* w )
    {
        if( nullptr != w )
            return w->layoutDirection() == Qt::LeftToRight;

        return false;
    }

    void                                    paintEvent( QPaintEvent* ) override final;
    void                                    resizeEvent( QResizeEvent* ) override final;
    void                                    toggle( Qt::CheckState ) override final;

    void                                    init();
    QRect                                   indicatorRect();
    QRect                                   textRect();

private:
    Shared::Qt::Style::Switch               style;
    QPixmap                                 shadowPixmap;
    QPointer<Animator>                      thumbBrushAnimation;
    QPointer<Animator>                      trackBrushAnimation;
    QPointer<Animator>                      thumbPosAniamtion;
};

#endif // SWITCH_H