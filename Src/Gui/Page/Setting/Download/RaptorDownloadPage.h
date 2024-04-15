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

#ifndef RAPTORDOWNLOADPAGE_H
#define RAPTORDOWNLOADPAGE_H

#include <QButtonGroup>
#include <QTimer>

#include "../../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../../Eject/Toast/RaptorToast.h"
#include "../../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../../Model/Page/Setting/RaptorSettingViewModel.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorDownloadPage;
}

QT_END_NAMESPACE

class RaptorDownloadPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorDownloadPage(QWidget *qParent = Q_NULLPTR);

    ~RaptorDownloadPage() Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

Q_SIGNALS:
    Q_SIGNAL void itemAriaConnectTesting(const QVariant &qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemAriaConnectTested(const QVariant &qVariant) const;

private Q_SLOTS:
    Q_SLOT void onDebounceTimerTimeout() const;

    Q_SLOT void onEmbedStateChanged(const int &qState) const;

    Q_SLOT void onAriaStateChanged(const int &qState) const;

    Q_SLOT void onAriaHostTextChanged(const QString &qValue) const;

    Q_SLOT void onAriaPortTextChanged(const QString &qValue) const;

    Q_SLOT void onAriaTokenTextChanged(const QString &qValue) const;

    Q_SLOT void onAriaRemotePathTextChanged(const QString &qValue) const;

    Q_SLOT void onAriaSSLStateChanged(const int &qState) const;

    Q_SLOT void onAriaTestClicked() const;

    Q_SLOT void onThirdPartyEngineViewModelItemEdited(const QVariant &qVariant) const;

    Q_SLOT void onThirdPartyEngineViewDoubleClicked(const QModelIndex &qIndex) const;

    Q_SLOT void onPathSelectClicked() const;

    Q_SLOT void onDefaultPathStateChanged(const int &qState) const;

    Q_SLOT void onFullPathStateChanged(const int &qState) const;

    Q_SLOT void onSkipViolationStateChanged(const int &qState) const;

    Q_SLOT void onAutoRenameStateChanged(const int &qState) const;

    Q_SLOT void onConcurrentSliderValueChanged(const int &qValue) const;

private:
    Ui::RaptorDownloadPage *_Ui = Q_NULLPTR;
    QButtonGroup* _PrimaryEngeineGroup = Q_NULLPTR;
    RaptorTableViewDelegate *_ThirdPartyEngineViewDelegate = Q_NULLPTR;
    RaptorTableViewHeader *_ThirdPartyEngineViewHeader = Q_NULLPTR;
    RaptorSettingViewModel *_ThirdPartyEngineViewModel = Q_NULLPTR;
    QTimer *_DebounceTimer = Q_NULLPTR;
};

#endif // RAPTORDOWNLOADPAGE_H
