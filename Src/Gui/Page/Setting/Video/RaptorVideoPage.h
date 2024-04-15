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

#ifndef RAPTORVIDEOPAGE_H
#define RAPTORVIDEOPAGE_H

#include <QButtonGroup>

#include "../../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../../Eject/Toast/RaptorToast.h"
#include "../../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../../Model/Page/Setting/RaptorSettingViewModel.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorVideoPage;
}

QT_END_NAMESPACE

class RaptorVideoPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorVideoPage(QWidget *qParent = Q_NULLPTR);

    ~RaptorVideoPage() Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    void invokeUiInit() const;

    void invokeSlotInit() const;

private Q_SLOTS:
    Q_SLOT void onDebounceTimerTimeout() const;

    Q_SLOT void onThirdPartyEngineViewModelItemEdited(const QVariant &qVariant) const;

    Q_SLOT void onThirdPartyEngineViewDoubleClicked(const QModelIndex &qIndex) const;

    Q_SLOT void onQualitySDClicked(const bool &qChecked) const;

    Q_SLOT void onQualityHDClicked(const bool &qChecked) const;

    Q_SLOT void onQualityFHDClicked(const bool &qChecked) const;

    Q_SLOT void onQualityUHDClicked(const bool &qChecked) const;

    Q_SLOT void onQualityOriginClicked(const bool &qChecked) const;

    Q_SLOT void onStreamQuickClicked(const bool &qChecked) const;

    Q_SLOT void onStreamLiveClicked(const bool &qChecked) const;

    Q_SLOT void onUseOriginStateChanged(const int &qState) const;

    Q_SLOT void onUseSubStateChanged(const int &qState) const;

private:
    Ui::RaptorVideoPage *_Ui = Q_NULLPTR;
    QButtonGroup *_QualityGroup = Q_NULLPTR;
    QButtonGroup *_StreamGroup = Q_NULLPTR;
    RaptorTableViewDelegate *_ThirdPartyEngineViewDelegate = Q_NULLPTR;
    RaptorTableViewHeader *_ThirdPartyEngineViewHeader = Q_NULLPTR;
    RaptorSettingViewModel *_ThirdPartyEngineViewModel = Q_NULLPTR;
    QTimer *_DebounceTimer = Q_NULLPTR;
};

#endif // RAPTORVIDEOPAGE_H
