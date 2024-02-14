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

#ifndef RAPTORUPLOADINGPAGE_H
#define RAPTORUPLOADINGPAGE_H

#include <QWidget>

#include "../../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../../Eject/Toast/RaptorToast.h"
#include "../../../../Delegate/Page/Transfer/Uploading/RaptorUploadingViewDelegate.h"
#include "../../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../../Model/Page/Transfer/Uploading/RaptorUploadingViewModel.h"
#include "../../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorUploadingPage;
}

QT_END_NAMESPACE

class RaptorUploadingPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorUploadingPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorUploadingPage() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    QModelIndexList invokeSelectItemsGet() const;

    void invokeItemsPause() const;

    void invokeItemsResume() const;

    void invokeItemsCancel() const;

private:
    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

Q_SIGNALS:
    Q_SIGNAL void itemsPausing(const QVariant& qVariant) const;

    Q_SIGNAL void itemsResuming(const QVariant& qVariant) const;

    Q_SIGNAL void itemCancelling(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemsLoaded(const QVariant& qVariant) const;

    Q_SLOT void onItemLogouting(const QVariant& qVariant) const;

    Q_SLOT void onItemSwitching(const QVariant& qVariant) const;

    Q_SLOT void onItemsQueuing(const QVariant& qVariant) const;

    Q_SLOT void onItemsStatusChanged(const QVariant& qVariant) const;

    Q_SLOT void onItemPaused(const QVariant& qVariant) const;

    Q_SLOT void onItemCompleted(const QVariant& qVariant) const;

    Q_SLOT void onItemErrored(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onItemViewClicked(const QModelIndex& qIndex) const;

    Q_SLOT void onItemViewSelectionChanged(const QItemSelection& qSelected,
                                           const QItemSelection& qDeselected) const;

private:
    Ui::RaptorUploadingPage* _Ui = Q_NULLPTR;
    RaptorTableViewHeader* _ItemViewHeader = Q_NULLPTR;
    RaptorUploadingViewModel* _ItemViewModel = Q_NULLPTR;
    RaptorUploadingViewDelegate* _ItemViewDelegate = Q_NULLPTR;
};

#endif // RAPTORUPLOADINGPAGE_H
