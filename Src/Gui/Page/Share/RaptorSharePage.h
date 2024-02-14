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

#ifndef RAPTORSHAREPAGE_H
#define RAPTORSHAREPAGE_H

#include <QButtonGroup>
#include <QClipboard>
#include <QScrollBar>
#include <QWidget>

#include "../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../Eject/Toast/RaptorToast.h"
#include "../../../Component/Loading/RaptorLoading.h"
#include "../../../Component/TableView/RaptorTableView.h"
#include "../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../Model/Page/Share//RaptorShareViewModel.h"
#include "../../../Suite/Store/RaptorStoreSuite.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorSharePage;
}

QT_END_NAMESPACE

class RaptorSharePage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorSharePage(QWidget* qParent = Q_NULLPTR);

    ~RaptorSharePage() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeNavigate();

private:
    struct Payload
    {
        QString rMarker;
    };

    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

    QPair<QString, QString> invokeTypeWithCategoryFilter() const;

Q_SIGNALS:
    Q_SIGNAL void itemsFetching(const QVariant& qVariant) const;

    Q_SIGNAL void itemsRapidFetching() const;

    Q_SIGNAL void itemsExporting(const QVariant& qVariant) const;

    Q_SIGNAL void itemsCancelling(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemCopyWriterHaveFound(const QVariant& qVariant) const;

    Q_SLOT void onItemAccessTokenRefreshed(const QVariant& qVariant);

    Q_SLOT void onItemLogouting(const QVariant& qVariant) const;

    Q_SLOT void onItemSpaceChanging();

    Q_SLOT void onItemSwitching(const QVariant& qVariant) const;

    Q_SLOT void onItemsFetched(const QVariant& qVariant);

    Q_SLOT void onItemsExported(const QVariant& qVariant);

    Q_SLOT void onItemsCancelled(const QVariant& qVariant);

private Q_SLOTS:
    Q_SLOT void onLoadingStateChanged(const RaptorLoading::State& state) const;

    Q_SLOT void onExportClicked() const;

    Q_SLOT void onCancelClicked() const;

    Q_SLOT void onTabPrevClicked() const;

    Q_SLOT void onTabAllToggled(const bool& qChecked);

    Q_SLOT void onTabSharingToggled(const bool& qChecked);

    Q_SLOT void onTabCanceledToggled(const bool& qChecked);

    Q_SLOT void onTabExpiredToggled(const bool& qChecked);

    Q_SLOT void onTabNeverExpireToggled(const bool& qChecked);

    Q_SLOT void onTabRapidToggled(const bool& qChecked);

    Q_SLOT void onTabNextClicked() const;

    Q_SLOT void onSearchClicked();

    Q_SLOT void onRefreshClicked();

    Q_SLOT void onItemViewIndicatorClicked(const RaptorTableView::Code& qCode) const;

    Q_SLOT void onItemViewClicked(const QModelIndex& qIndex) const;

    Q_SLOT void onItemViewSelectionChanged(const QItemSelection& qSelected,
                                           const QItemSelection& qDeselected) const;

    Q_SLOT void onItemViewVerticalScrollValueChanged(const int& qValue) const;

    Q_SLOT void onItemPasswordCopyClicked() const;

    Q_SLOT void onItemCopyClicked() const;

private:
    Ui::RaptorSharePage* _Ui = Q_NULLPTR;
    QButtonGroup* _TabGroup = Q_NULLPTR;
    RaptorTableViewHeader* _ItemViewHeader = Q_NULLPTR;
    RaptorShareViewModel* _ItemViewModel = Q_NULLPTR;
    RaptorTableViewDelegate* _ItemViewDelegate = Q_NULLPTR;
    RaptorLoading* _Loading = Q_NULLPTR;
    Payload _Payload;
};

#endif // RAPTORSHAREPAGE_H