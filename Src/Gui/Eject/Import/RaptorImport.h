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

#ifndef RAPTORIMPORT_H
#define RAPTORIMPORT_H

#include <QButtonGroup>
#include <QStack>

#include "../Toast/RaptorToast.h"
#include "../../../Common/RaptorDeclare.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Component/Loading/RaptorLoading.h"
#include "../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../Model/Page/Space/RaptorSpaceViewModel.h"
#include "../../../Suite/Setting/RaptorSettingSuite.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorImport;
}

QT_END_NAMESPACE

class RaptorImport Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    explicit RaptorImport(QWidget *qParent = Q_NULLPTR);

    ~RaptorImport() override;

    bool eventFilter(QObject *qObject, QEvent *qEvent) Q_DECL_OVERRIDE;

    void invokeEject(const QVariant &qVariant = QVariant()) Q_DECL_OVERRIDE;

private:
    struct Payload
    {
        QString _Parent = "root";
        QStack<RaptorFileItem> _Stack;
        QString _Marker;
        QString _Token;
    };

    void invokeInstanceInit() Q_DECL_OVERRIDE;

    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

    void invokeIconDrawing() const;

Q_SIGNALS:
    Q_SIGNAL void itemsByParentIdFetching(const QVariant &qVariant) const;

    Q_SIGNAL void itemImporting(const QVariant &qVariant) const;

    Q_SIGNAL void itemParsing(const QVariant &qVariant) const;

    Q_SIGNAL void itemRapidImporting(const QVariant &qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemParsed(const QVariant &qVariant);

private Q_SLOTS:
    Q_SLOT void onLoadingStateChanged(const RaptorLoading::State &state) const;

    Q_SLOT void onCloseClicked();

    Q_SLOT void onOneStateChanged(const int &qState) const;

    Q_SLOT void onParseClicked();

    Q_SLOT void onCancelClicked();

    Q_SLOT void onItemViewClicked(const QModelIndex &qIndex) const;

    Q_SLOT void onItemViewDoubleClicked(const QModelIndex &qIndex);

    Q_SLOT void onItemViewSelectionChanged(const QItemSelection &qSelected,
                                           const QItemSelection &qDeselected) const;

    Q_SLOT void onItemRootClicked();

    Q_SLOT void onItemBackClicked();

    Q_SLOT void onImportClicked();

private:
    Ui::RaptorImport *_Ui = Q_NULLPTR;
    QButtonGroup *_TypeGroup = Q_NULLPTR;
    RaptorTableViewDelegate *_ItemViewDelegate = Q_NULLPTR;
    RaptorTableViewHeader *_ItemViewHeader = Q_NULLPTR;
    RaptorSpaceViewModel *_ItemViewModel = Q_NULLPTR;
    RaptorLoading *_Loading = Q_NULLPTR;
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());
    Payload _Payload;
    QVariant _Variant;
};

#endif //RAPTORIMPORT_H
