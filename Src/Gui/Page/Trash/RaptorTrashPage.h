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

#ifndef RAPTORTRASHPAGE_H
#define RAPTORTRASHPAGE_H

#include <QScrollBar>
#include <QWidget>

#include "../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../Eject/Toast/RaptorToast.h"
#include "../../../Component/Loading/RaptorLoading.h"
#include "../../../Component/Menu/RaptorMenu.h"
#include "../../../Component/TableView/RaptorTableView.h"
#include "../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../Model/Page/Trash/RaptorTrashViewModel.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorTrashPage;
}

QT_END_NAMESPACE

class RaptorTrashPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorTrashPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorTrashPage() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeNavigate();

private:
    struct Payload
    {
        QString _Marker;
    };

    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

Q_SIGNALS:
    Q_SIGNAL void itemsFetching(const QVariant& qVariant) const;

    Q_SIGNAL void itemsDeleting(const QVariant& qVariant) const;

    Q_SIGNAL void itemsRecovering(const QVariant& qVariant) const;

    Q_SIGNAL void itemsClearing() const;

public Q_SLOTS:
    Q_SLOT void onItemCopyWriterHaveFound(const QVariant& qVariant) const;

    Q_SLOT void onItemAccessTokenRefreshed(const QVariant& qVariant);

    Q_SLOT void onItemLogouting(const QVariant& qVariant) const;

    Q_SLOT void onItemSpaceChanging();

    Q_SLOT void onItemSwitching(const QVariant& qVariant) const;

    Q_SLOT void onItemsFetched(const QVariant& qVariant);

    Q_SLOT void onItemsRecovered(const QVariant& qVariant) const;

    Q_SLOT void onItemsDeleted(const QVariant& qVariant) const;

    Q_SLOT void onItemsCleared(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onItemViewLoadingStateChanged(const RaptorLoading::State& state) const;

    Q_SLOT void onRecoverClicked() const;

    Q_SLOT void onDeleteClicked() const;

    Q_SLOT void onClearClicked() const;

    Q_SLOT void onRefreshClicked();

    Q_SLOT void onItemViewIndicatorClicked(const RaptorTableView::Code& qCode) const;

    Q_SLOT void onItemViewClicked(const QModelIndex& qIndex) const;

    Q_SLOT void onItemViewCustomContextMenuRequested(const QPoint &qPoint) const;

    Q_SLOT void onItemViewSelectionChanged(const QItemSelection& qSelected,
                                           const QItemSelection& qDeselected) const;

    Q_SLOT void onItemViewVerticalScrollValueChanged(const int& qValue) const;

private:
    Ui::RaptorTrashPage* _Ui = Q_NULLPTR;
    RaptorMenu *_ItemViewContextMenu = Q_NULLPTR;
    RaptorTableViewDelegate* _ItemViewDelegate = Q_NULLPTR;
    RaptorTableViewHeader* _ItemViewHeader = Q_NULLPTR;
    RaptorTrashViewModel* _ItemViewModel = Q_NULLPTR;
    RaptorLoading* _ItemViewLoading = Q_NULLPTR;
    Payload _Payload;
};

#endif // RAPTORTRASHPAGE_H