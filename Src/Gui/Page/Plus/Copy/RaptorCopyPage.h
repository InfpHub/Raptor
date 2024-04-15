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

#ifndef RAPTORCOPYPAGE_H
#define RAPTORCOPYPAGE_H

#include <QScrollBar>
#include <QStack>
#include <QWidget>

#include "../../../Eject/CopyUser/RaptorCopyUser.h"
#include "../../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../../Eject/Toast/RaptorToast.h"
#include "../../../../Component/Loading//RaptorLoading.h"
#include "../../../../Component/TableView/RaptorTableView.h"
#include "../../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../../Model/Page/Space/RaptorSpaceViewModel.h"
#include "../../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../../Suite/Store/RaptorStoreSuite.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorCopyPage;
}

QT_END_NAMESPACE

class RaptorCopyPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorCopyPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorCopyPage() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    [[nodiscard]]
    RaptorCopyUser* invokeCopyUserUiGet() const;

    void invokeRefresh();

    void invokeItemSearch();

    void invokeItemsCopy();

    void invokeItemCancel() const;

private:
    struct Payload
    {
        QPair<QString, QString> _Parent = qMakePair(QString("root"), QString("root"));
        QPair<QStack<RaptorFileItem>, QStack<RaptorFileItem>> _Stack;
        QPair<QString, QString> _Marker;
        QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> _User;
        QString _Keyword;
    };

    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

    [[nodiscard]]
    bool invokeSourceViewIsActiveConfirm() const;

    [[nodiscard]]
    bool invokeTargetViewIsActiveConfirm() const;

Q_SIGNALS:
    Q_SIGNAL void itemsLoading() const;

    Q_SIGNAL void itemsAccessTokenRefreshing(const QVariant& qVariant) const;

    Q_SIGNAL void itemsByParentIdFetching(const QVariant& qVariant) const;

    Q_SIGNAL void itemsByIdFetching(const QVariant& qVariant) const;

    Q_SIGNAL void itemsByConditionFetching(const QVariant& qVariant) const;

    Q_SIGNAL void itemsCopying(const QVariant& qVariant) const;

    Q_SIGNAL void itemsCancelling() const;

public Q_SLOTS:
    Q_SLOT void onItemAccessTokenRefreshed(const QVariant& qVariant);

    Q_SLOT void onItemsAccessTokenRefreshed(const QVariant& qVariant);

    Q_SLOT void onItemLogouting(const QVariant& qVariant) const;

    Q_SLOT void onItemSpaceChanging();

    Q_SLOT void onItemsFetched(const QVariant& qVariant);

    Q_SLOT void onItemCopied(const QVariant& qVariant) const;

    Q_SLOT void onItemsCancelled(const QVariant& qVariant) const;

    Q_SLOT void onItemErrored(const QVariant& qVariant) const;

    Q_SLOT void onSearchEditTextChanged(const QString& qText);

private Q_SLOTS:
    Q_SLOT void onItemSourceViewClicked() const;

    Q_SLOT void onItemSourceViewDoubleClicked(const QModelIndex& qIndex);

    Q_SLOT void onItemTargetViewClicked() const;

    Q_SLOT void onItemTargetViewDoubleClicked(const QModelIndex& qIndex);

    Q_SLOT void onItemSourceViewVerticalScrollValueChanged(const int& qValue) const;

    Q_SLOT void onItemTargetViewVerticalScrollValueChanged(const int& qValue) const;

    Q_SLOT void onItemUserClicked();

    Q_SLOT void onItemClearLogClicked() const;

    Q_SLOT void onItemShowLogClicked(const bool& qChecked) const;

    Q_SLOT void onItemBackClicked();

    Q_SLOT void onItemRootClicked();

private:
    Ui::RaptorCopyPage* _Ui = Q_NULLPTR;
    Payload _Payload;
    RaptorCopyUser* _CopyUser = Q_NULLPTR;
    RaptorTableViewHeader* _ItemSourceViewHeader = Q_NULLPTR;
    RaptorTableViewHeader* _ItemTargetViewHeader = Q_NULLPTR;
    RaptorTableViewHeader* _ItemQueueViewHeader = Q_NULLPTR;
    RaptorSpaceViewModel* _ItemQueueViewModel = Q_NULLPTR;
    RaptorLoading* _ItemSourceViewLoading = Q_NULLPTR;
    RaptorLoading* _ItemTargetViewLoading = Q_NULLPTR;
    RaptorSpaceViewModel* _ItemSourceViewModel = Q_NULLPTR;
    RaptorSpaceViewModel* _ItemTargetViewModel = Q_NULLPTR;
    RaptorTableViewDelegate* _ItemSourceViewDelegate = Q_NULLPTR;
    RaptorTableViewDelegate* _ItemTargetViewDelegate = Q_NULLPTR;
    RaptorTableViewDelegate* _ItemQueueViewDelegate = Q_NULLPTR;
};

#endif // RAPTORCOPYPAGE_H