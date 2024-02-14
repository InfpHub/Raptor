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

#ifndef RAPTORAUTHENTICATIONSUITE_H
#define RAPTORAUTHENTICATIONSUITE_H

#include <QJsonObject>
#include <QObject>
#include <QTimer>

#include "../Http/RaptorHttpSuite.h"
#include "../Persistence/RaptorPersistenceSuite.h"
#include "../Store/RaptorStoreSuite.h"
#include "../../Common/RaptorDeclare.h"
#include "../../Util/RaptorUtil.h"

class RaptorAuthenticationSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public :
    explicit RaptorAuthenticationSuite();

    static RaptorAuthenticationSuite* invokeSingletonGet();

    Q_INVOKABLE void invokeStop() const;

private:
    void invokeInstanceInit();

    void invokeSlotInit() const;

    static void invokeItemDelete(const QString& qId);

    static QPair<QString, RaptorAuthenticationItem> invokeItemByActiveFind();

    static void invokeItemSave(const RaptorAuthenticationItem& item);

    static void invokeItemUpdate(const RaptorAuthenticationItem& item);

    QString invokeItemAccessTokenRefresh(RaptorAuthenticationItem& item) const;

    static RaptorOutput invokeSessionInit(const RaptorAuthenticationItem& item);

    static QString invokeItemDetailFetch(RaptorAuthenticationItem& item,
                                         const QString& qUrl);

    static QString invokeItemCapacityFetch(RaptorAuthenticationItem& item);


Q_SIGNALS:
    Q_SIGNAL void itemsLoaded(const QVariant& qVariant) const;

    Q_SIGNAL void itemLogouted(const QVariant& qVariant) const;

    Q_SIGNAL void itemSwitched(const QVariant& qVariant) const;

    Q_SIGNAL void itemCapacityRefreshed(const QVariant& qVariant) const;

    Q_SIGNAL void itemQrCodeGenerated(const QVariant& qVariant) const;

    Q_SIGNAL void itemQrCodeStatusFetched(const QVariant& qVariant) const;

    Q_SIGNAL void itemAccessTokenRefreshed(const QVariant& qVariant) const;

    Q_SIGNAL void itemsAccessTokenRefreshed(const QVariant& qVariant) const;

    Q_SIGNAL void itemSignInConfirmed(const QVariant& qVariant) const;

    Q_SIGNAL void itemSignedIn(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemsLoading() const;

    Q_SLOT void onItemLogouting(const QVariant& qVariant) const;

    Q_SLOT void onItemSwitching(const QVariant& qVariant) const;

    Q_SLOT void onItemCapacityRefreshing() const;

    Q_SLOT void onItemQrCodeGenerating() const;

    Q_SLOT void onItemQrCodeStatusFetching(const QVariant& qVariant) const;

    Q_SLOT void onItemAccessTokenRefreshing() const;

    Q_SLOT void onItemsAccessTokenRefreshing(const QVariant& qVariant) const;

    Q_SLOT void onItemSignInConfirming() const;

    Q_SLOT void onItemSigningIn() const;

private Q_SLOTS:
    Q_SLOT void onItemSessionTimerTimeout() const;

    Q_SLOT void onItemAccessTokenTimerTimeout() const;

private:
    QTimer* _ItemSessionTimer = Q_NULLPTR;
    QTimer* _ItemAccessTokenTimer = Q_NULLPTR;
};

#endif // RAPTORAUTHENTICATIONSUITE_H