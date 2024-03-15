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

#ifndef RAPTORUSER_H
#define RAPTORUSER_H

#include <QDialog>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include "../MessageBox/RaptorMessageBox.h"
#include "../Toast/RaptorToast.h"
#include "../../../Delegate/Eject/User/RaptorUserViewDelegate.h"
#include "../../../Header/Eject/User/RaptorUserViewHeader.h"
#include "../../../Model/Eject/User/RaptorUserViewModel.h"
#include "../../../Suite/Store/RaptorStoreSuite.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorUser;
}

QT_END_NAMESPACE

class RaptorUser Q_DECL_FINAL : public QDialog
{
    Q_OBJECT

public:
    enum Operate
    {
        Nothing,
        Logout,
        Login,
        Switch,
        Device
    };

    Q_ENUM(Operate)

    explicit RaptorUser(QWidget* qParent = Q_NULLPTR);

    ~RaptorUser() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

protected:
    int exec() Q_DECL_OVERRIDE;

public:
    QPair<Operate, RaptorAuthenticationItem> invokeEject(const QPoint& qPoint);

private:
    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

Q_SIGNALS:
    Q_SIGNAL void itemsLoading() const;

    Q_SIGNAL void itemSignInConfirming() const;

    Q_SIGNAL void itemSigningIn() const;

    Q_SIGNAL void itemCapacityRefreshing() const;

public Q_SLOTS:
    Q_SLOT void onItemsLoaded(const QVariant& qVariant) const;

    Q_SLOT void onItemLogoutd(const QVariant& qVariant) const;

    Q_SLOT void onItemAccessTokenRefreshed(const QVariant& qVariant) const;

    Q_SLOT void onItemSwitching(const QVariant& qVariant) const;

    Q_SLOT void onItemSignedIn(const QVariant& qVariant) const;

    Q_SLOT void onItemCapacityRefreshed(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onRefreshClicked() const;

    Q_SLOT void onSignInClicked() const;

    Q_SLOT void onDeviceClicked();

    Q_SLOT void onLogoutClicked();

    Q_SLOT void onLoginClicked();

    Q_SLOT void onStartAnimationFinished();

    Q_SLOT void onItemViewDoubleClicked(const QModelIndex& qIndex);

private:
    Ui::RaptorUser* _Ui = Q_NULLPTR;
    QParallelAnimationGroup* _StartAnimationGroup = Q_NULLPTR;
    QPropertyAnimation* _StartAnimation = Q_NULLPTR;
    QPropertyAnimation* _StartMoveAnimation = Q_NULLPTR;
    RaptorUserViewDelegate* _ItemViewDelegate = Q_NULLPTR;
    RaptorUserViewHeader* _ItemViewHeader = Q_NULLPTR;
    RaptorUserViewModel* _ItemViewModel = Q_NULLPTR;
    QPoint _Point;
    Operate _Operate;
    RaptorAuthenticationItem _User;
};

#endif // RAPTORUSER_H
