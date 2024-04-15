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

#ifndef RAPTORMASK_H
#define RAPTORMASK_H

#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QWidget>

#include "../../../Common/RaptorDeclare.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorMask;
}

QT_END_NAMESPACE

class RaptorMask Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorMask(QWidget *qParent = Q_NULLPTR);

    ~RaptorMask() Q_DECL_OVERRIDE;

    void invokeshow() const;

    void invokeHide() const;

protected:
    void paintEvent(QPaintEvent *qPaintEvent) Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

private Q_SLOTS:
    Q_SLOT void onAnimationValueChanged(const QVariant &qVariant);

private:
    Ui::RaptorMask *_Ui = Q_NULLPTR;
    QPropertyAnimation *_Animation = Q_NULLPTR;
    quint16 _Opacity;
};


#endif // RAPTORMASK_H
