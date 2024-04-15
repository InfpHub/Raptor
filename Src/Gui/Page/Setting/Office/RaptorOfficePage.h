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

#ifndef RAPTOROFFICEPAGE_H
#define RAPTOROFFICEPAGE_H

#include <QButtonGroup>

#include "../../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../../Eject/Toast/RaptorToast.h"
#include "../../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../../Model/Page/Setting/RaptorSettingViewModel.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorOfficePage;
}

QT_END_NAMESPACE

class RaptorOfficePage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorOfficePage(QWidget *qParent = Q_NULLPTR);

    ~RaptorOfficePage() Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    void invokeUiInit() const;

    void invokeSlotInit() const;

private Q_SLOTS:
    Q_SLOT void onDebounceTimerTimeout() const;

    Q_SLOT void onThirdPartyEngineViewModelItemEdited(const QVariant &qVariant) const;

private:
    Ui::RaptorOfficePage *_Ui = Q_NULLPTR;
    RaptorTableViewDelegate *_ThirdPartyEngineViewDelegate = Q_NULLPTR;
    RaptorTableViewHeader *_ThirdPartyEngineViewHeader = Q_NULLPTR;
    RaptorSettingViewModel *_ThirdPartyEngineViewModel = Q_NULLPTR;
    QTimer *_DebounceTimer = Q_NULLPTR;
};

#endif // RAPTOROFFICEPAGE_H
