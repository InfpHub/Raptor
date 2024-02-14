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

#ifndef RAPTORUPLOAD_H
#define RAPTORUPLOAD_H

#include <QFileDialog>
#include <QStandardPaths>

#include "../Toast/RaptorToast.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../Model/Eject/Upload/RaptorUploadViewModel.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorUpload;
}

QT_END_NAMESPACE

class RaptorUpload Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    explicit RaptorUpload(QWidget* qParent = Q_NULLPTR);

    ~RaptorUpload() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeEject(const QVariant& qVariant = QVariant()) Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit() Q_DECL_OVERRIDE;

    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

Q_SIGNALS:
    Q_SIGNAL void itemsUploading(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onItemViewClicked(const QModelIndex& qIndex) const;

    Q_SLOT void onItemViewSelectionChanged(const QItemSelection& qSelected,
                                           const QItemSelection& qDeselected) const;

    Q_SLOT void onCloseClicked();

    Q_SLOT void onAddClicked();

    Q_SLOT void onAddDirClicked();

    Q_SLOT void onDeleteClicked() const;

    Q_SLOT void onClearClicked() const;

    Q_SLOT void onItemLocateClicked() const;

    Q_SLOT void onItemDeleteClicked() const;

    Q_SLOT void onCancelClicked();

    Q_SLOT void onOKClicked();

private:
    Ui::RaptorUpload* _Ui = Q_NULLPTR;
    RaptorTableViewHeader* _ItemViewHeader = Q_NULLPTR;
    RaptorUploadViewModel* _ItemViewModel = Q_NULLPTR;
    RaptorTableViewDelegate* _ItemViewDelegate = Q_NULLPTR;
    QVariant _Variant;
};

#endif // RAPTORUPLOAD_H