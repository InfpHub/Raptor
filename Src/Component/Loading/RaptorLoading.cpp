/**
 *
 * Copyright (C) 2024 凉州刺史. All rights reserved.
 *
 * This file is part of Raptor.
 *
 * $RAPTOR_BEGIN_LICENSE$
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 * $RAPTOR_END_LICENSE$
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "RaptorLoading.h"

RaptorLoading::RaptorLoading(QWidget* qParent) : QWidget(qParent),
                                                 _DashOffset(0),
                                                 _DashLength(89),
                                                 _Angle(0),
                                                 _PenWidth(5.25),
                                                 _Size(64)
{
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorLoading::~RaptorLoading()
{
    _AnimationTimer->stop();
    _AnimationGroup->stop();
}

void RaptorLoading::invokeStateSet(const State& qState)
{
    Q_EMIT stateChanged(qState);
    if (_AnimationTimer->isActive() && qState == Loading)
    {
        return;
    }

    if (!_AnimationTimer->isActive() && qState == Finished)
    {
        return;
    }

    if (qState == Loading)
    {
        if (!_AnimationTimer->isActive())
        {
            _AnimationTimer->start();
        }

        setVisible(true);
    }
    else if (qState == Finished)
    {
        setVisible(false);
        if (_AnimationTimer->isActive())
        {
            _AnimationTimer->stop();
        }
    }
}

void RaptorLoading::invokeDashOffsetSet(const qreal& qOffset)
{
    _DashOffset = qOffset;
}

qreal RaptorLoading::invokeDashOffsetGet() const
{
    return _DashOffset;
}

void RaptorLoading::invokeDashLengthSet(const qreal& qLength)
{
    _DashLength = qLength;
}

qreal RaptorLoading::invokeDashLengthGet() const
{
    return _DashLength;
}

void RaptorLoading::invokeAngleSet(const int& qAngle)
{
    _Angle = qAngle;
}

int RaptorLoading::invokeAngleGet() const
{
    return _Angle;
}

void RaptorLoading::invokeInstanceInit()
{
    _AnimationGroup = new QParallelAnimationGroup(this);
    _AnimationGroup->setLoopCount(-1);

    _DashOffsetAnimation = new QPropertyAnimation(this);
    _DashOffsetAnimation->setPropertyName("_DashOffset");
    _DashOffsetAnimation->setTargetObject(this);
    _DashOffsetAnimation->setEasingCurve(QEasingCurve::InOutSine);
    _DashOffsetAnimation->setStartValue(0);
    _DashOffsetAnimation->setKeyValueAt(0.15, 0);
    _DashOffsetAnimation->setKeyValueAt(0.6, -7);
    _DashOffsetAnimation->setKeyValueAt(0.7, -7);
    _DashOffsetAnimation->setEndValue(-25);
    _DashOffsetAnimation->setDuration(2024);
    _AnimationGroup->addAnimation(_DashOffsetAnimation);

    _DashLengthAnimation = new QPropertyAnimation(this);
    _DashLengthAnimation->setPropertyName("_DashLength");
    _DashLengthAnimation->setTargetObject(this);
    _DashLengthAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    _DashLengthAnimation->setStartValue(0.1);
    _DashLengthAnimation->setKeyValueAt(0.15, 0.2);
    _DashLengthAnimation->setKeyValueAt(0.6, 20);
    _DashLengthAnimation->setKeyValueAt(0.7, 20);
    _DashLengthAnimation->setEndValue(20);
    _DashLengthAnimation->setDuration(2024);
    _AnimationGroup->addAnimation(_DashLengthAnimation);

    _AngleAnimation = new QPropertyAnimation(this);
    _AngleAnimation->setPropertyName("_Angle");
    _AngleAnimation->setTargetObject(this);
    _AngleAnimation->setStartValue(0);
    _AngleAnimation->setEndValue(719);
    _AngleAnimation->setDuration(2024);
    _AnimationGroup->addAnimation(_AngleAnimation);
    _AnimationGroup->start();

    _AnimationTimer = new QTimer(this);
    _AnimationTimer->setInterval(10);
}

void RaptorLoading::invokeUiInit()
{
    setVisible(false);
    setFixedSize(_Size + 12, _Size + 12);
}

void RaptorLoading::invokeSlotInit()
{
    connect(_AnimationTimer,
            &QTimer::timeout,
            this,
            &RaptorLoading::onAnimationTimerTimeout);
}

void RaptorLoading::onAnimationTimerTimeout()
{
    update();
}

void RaptorLoading::paintEvent(QPaintEvent* qPaintEvent)
{
    Q_UNUSED(qPaintEvent)
    if (parentWidget())
    {
        move(parentWidget()->width() / 2 - width() / 2, parentWidget()->height() / 2 - height() / 2);
    }

    auto qPainter = QPainter(this);
    qPainter.setRenderHint(QPainter::Antialiasing);
    qPainter.translate(width() / 2, height() / 2);
    qPainter.rotate(invokeAngleGet());
    auto qPen = QPen();
    qPen.setCapStyle(Qt::RoundCap);
    qPen.setWidthF(_PenWidth);
    qPen.setColor(QColor(0, 164, 255, 255));
    auto qDashPattern = QVector<qreal>();
    qDashPattern << invokeDashLengthGet() * _Size / 34 << 30 * _Size / 34;
    qPen.setDashOffset(invokeDashOffsetGet() * _Size / 34);
    qPen.setDashPattern(qDashPattern);
    qPainter.setPen(qPen);
    qPainter.drawEllipse(QPoint(0, 0), _Size / 2, _Size / 2);
}
