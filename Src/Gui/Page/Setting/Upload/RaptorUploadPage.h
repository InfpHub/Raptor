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

#ifndef RAPTORUPLOADPAGE_H
#define RAPTORUPLOADPAGE_H

#include <QWidget>

#include "../../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../../Eject/Toast/RaptorToast.h"
#include "../../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../../Model/Page/Setting/RaptorSettingViewModel.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorUploadPage;
}

QT_END_NAMESPACE

class RaptorUploadPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorUploadPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorUploadPage() Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    void invokeUiInit() const;

    void invokeSlotInit() const;

private Q_SLOTS:
    Q_SLOT void onFilterAddClicked() const;

    Q_SLOT void onFilterRemoveClicked() const;

    Q_SLOT void onFilterClearClicked() const;

    Q_SLOT void onSkipNoRapidStateChanged(const int& qState) const;

    Q_SLOT void onConcurrentSliderValueChanged(const int& qValue) const;

private:
    Ui::RaptorUploadPage* _Ui = Q_NULLPTR;
    RaptorTableViewDelegate* _FilterDelegate = Q_NULLPTR;
    RaptorTableViewHeader* _FilterHeader = Q_NULLPTR;
    RaptorSettingViewModel* _FilterModel = Q_NULLPTR;
    QTimer* _DebounceTimer = Q_NULLPTR;
};

#endif // RAPTORUPLOADPAGE_H