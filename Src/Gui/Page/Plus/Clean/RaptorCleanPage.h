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

#ifndef RAPTORCLEANPAGE_H
#define RAPTORCLEANPAGE_H

#include <QScrollBar>
#include <QStack>
#include <QWidget>

#include "../../../Eject/CopyUser/RaptorCopyUser.h"
#include "../../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../../Eject/Toast/RaptorToast.h"
#include "../../../../Component/Loading//RaptorLoading.h"
#include "../../../../Component/Menu/RaptorMenu.h"
#include "../../../../Component/TableView/RaptorTableView.h"
#include "../../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../../Model/Page/Plus/Clean/RaptorCleanViewModel.h"
#include "../../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../../Suite/Store/RaptorStoreSuite.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorCleanPage;
}

QT_END_NAMESPACE

class RaptorCleanPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorCleanPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorCleanPage() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeNavigate();

    void invokeItemClean() const;

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

    Q_SIGNAL void itemsCleaning(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemAccessTokenRefreshed(const QVariant& qVariant);

    Q_SLOT void onItemLogouting(const QVariant& qVariant) const;

    Q_SLOT void onItemSpaceChanging();

    Q_SLOT void onItemsFetched(const QVariant& qVariant);

    Q_SLOT void onItemsCleaned(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onItemViewClicked(const QModelIndex &qIndex) const;

    Q_SLOT void onItemViewCustomContextMenuRequested(const QPoint &qPoint) const;

    Q_SLOT void onItemViewVerticalScrollValueChanged(const int& qValue) const;

    Q_SLOT void onItemGoToDirClicked() const;

private:
    Ui::RaptorCleanPage* _Ui = Q_NULLPTR;
    Payload _Payload;
    RaptorMenu *_ItemViewContextMenu = Q_NULLPTR;
    RaptorTableViewHeader* _ItemViewHeader = Q_NULLPTR;
    RaptorCleanViewModel* _ItemViewModel = Q_NULLPTR;
    RaptorLoading* _ItemViewLoading = Q_NULLPTR;
    RaptorTableViewDelegate* _ItemViewDelegate = Q_NULLPTR;
};

#endif // RAPTORCLEANPAGE_H