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

#ifndef RAPTORLOADING_H
#define RAPTORLOADING_H

#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

class RaptorLoading Q_DECL_FINAL : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal _DashOffset WRITE invokeDashOffsetSet READ invokeDashOffsetGet)
    Q_PROPERTY(qreal _DashLength WRITE invokeDashLengthSet READ invokeDashLengthGet)
    Q_PROPERTY(int _Angle WRITE invokeAngleSet READ invokeAngleGet)

public:
    enum State
    {
        Loading,
        Finished
    };

    explicit RaptorLoading(QWidget* qParent = Q_NULLPTR);

    ~RaptorLoading() Q_DECL_OVERRIDE;

    void invokeStateSet(const State& qState);

    void invokeDashOffsetSet(const qreal& qOffset);

    qreal invokeDashOffsetGet() const;

    void invokeDashLengthSet(const qreal& qLength);

    qreal invokeDashLengthGet() const;

    void invokeAngleSet(const int& qAngle);

    int invokeAngleGet() const;

protected:
    void paintEvent(QPaintEvent* qPaintEvent) Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit();

Q_SIGNALS:
    Q_SIGNAL void stateChanged(const State& qState);

private Q_SLOTS:
    Q_SLOT void onAnimationTimerTimeout();

private:
    qreal _DashOffset;
    qreal _DashLength;
    int _Angle;
    qreal _PenWidth;
    int _Size;
    QPropertyAnimation* _DashLengthAnimation = Q_NULLPTR;
    QPropertyAnimation* _DashOffsetAnimation = Q_NULLPTR;
    QPropertyAnimation* _AngleAnimation = Q_NULLPTR;
    QParallelAnimationGroup* _AnimationGroup = Q_NULLPTR;
    QTimer* _AnimationTimer = Q_NULLPTR;
};


#endif // RAPTORLOADING_H
