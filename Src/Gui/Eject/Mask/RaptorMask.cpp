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

#include "RaptorMask.h"
#include "ui_RaptorMask.h"

RaptorMask::RaptorMask(QWidget *qParent) : QWidget(qParent),
                                           _Ui(new Ui::RaptorMask),
                                           _Opacity(0)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorMask::~RaptorMask()
{
    qFree(_Ui)
}

void RaptorMask::invokeshow() const
{
    if (_Animation->state() == QAbstractAnimation::Running)
    {
        _Animation->stop();
    }

    _Animation->setStartValue(_Opacity);
    _Animation->setEndValue(200);
    _Animation->setDuration(300);
    _Animation->start();
}

void RaptorMask::invokeHide() const
{
    if (_Animation->state() == QAbstractAnimation::Running)
    {
        _Animation->stop();
    }

    _Animation->setStartValue(_Opacity);
    _Animation->setEndValue(0);
    _Animation->setDuration(300);
    _Animation->start();
}

void RaptorMask::paintEvent(QPaintEvent *qPaintEvent)
{
    auto qPainter = QPainter(this);
    auto qPainterPath = QPainterPath();
    qPainterPath.addRoundedRect(rect(), 2, 2);
    qPainter.setClipPath(qPainterPath);
    qPainter.fillPath(qPainterPath, QColor(24, 24, 28, _Opacity));
}

void RaptorMask::invokeInstanceInit()
{
    _Animation = new QPropertyAnimation(this, "", this);
}

void RaptorMask::invokeUiInit()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void RaptorMask::invokeSlotInit() const
{
    connect(_Animation,
            &QPropertyAnimation::valueChanged,
            this,
            &RaptorMask::onAnimationValueChanged);
}

void RaptorMask::onAnimationValueChanged(const QVariant &qVariant)
{
    _Opacity = qVariant.toUInt();
    update();
}
