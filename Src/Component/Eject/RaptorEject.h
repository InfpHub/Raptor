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

#ifndef RAPTOREJECT_H
#define RAPTOREJECT_H

#include <QCloseEvent>
#include <QDialog>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QRandomGenerator>

class RaptorEject : public QDialog
{
    Q_OBJECT

public:
    enum struct Direction
    {
        T2B = 0,
        L2R = 1,
        B2T = 2,
        R2L = 3,
        Random = 4
    };

    explicit RaptorEject(QWidget* qParent = Q_NULLPTR);

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    int exec() Q_DECL_OVERRIDE;

protected:
    virtual void invokeEject(const QVariant& qVariant = QVariant());

    virtual void invokeInstanceInit();

    virtual void invokeUiInit();

    virtual void invokeSlotInit();

    void invokeCloseCallbackSet(const std::function<void()>& qCallback);

private Q_SLOTS:
    Q_SLOT void onStartAnimationGroupFinished();

private:
    QPropertyAnimation* _StartAnimatioo = Q_NULLPTR;
    QPropertyAnimation* _StartAnimation = Q_NULLPTR;
    QParallelAnimationGroup* _StartAnimationGroup = Q_NULLPTR;
    std::function<void()> _CloseCallback = Q_NULLPTR;
    Direction _Direction;
};

#endif // RAPTOREJECT_H