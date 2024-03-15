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

#ifndef RAPTORNETWORKPAGE_H
#define RAPTORNETWORKPAGE_H

#include <QButtonGroup>

#include "../../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../../Eject/Toast/RaptorToast.h"
#include "../../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../../Model/Page/Setting/RaptorSettingViewModel.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorNetworkPage;
}

QT_END_NAMESPACE

class RaptorNetworkPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorNetworkPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorNetworkPage() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

Q_SIGNALS:
    Q_SIGNAL void itemProxyConnectTesting(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemProxyConnectTested(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onDebounceTimerTimeout() const;

    Q_SLOT void onProxyNoneClicked(const bool& qChecked) const;

    Q_SLOT void onProxyHTTPClicked(const bool& qChecked) const;

    Q_SLOT void onProxyHTTP1_0Clicked(const bool& qChecked) const;

    Q_SLOT void onProxyHTTPSClicked(const bool& qChecked) const;

    Q_SLOT void onProxyHTTPS2Clicked(const bool& qChecked) const;

    Q_SLOT void onProxySOCKS4Clicked(const bool& qChecked) const;

    Q_SLOT void onProxySOCKS5Clicked(const bool& qChecked) const;

    Q_SLOT void onProxyHostTextChanged(const QString& qValue) const;

    Q_SLOT void onProxyPortTextChanged(const QString& qValue) const;

    Q_SLOT void onProxyUserNameTextChanged(const QString& qValue) const;

    Q_SLOT void onProxyPasswordTextChanged(const QString& qValue) const;

    Q_SLOT void onProxyStateChanged(const int& qState) const;

    Q_SLOT void onProxyTestClicked() const;

    Q_SLOT void onIPResolveAutoClicked(const bool& qChecked) const;

    Q_SLOT void onIPResolveV4Clicked(const bool& qChecked) const;

    Q_SLOT void onIPResolveV6Clicked(const bool& qChecked) const;

private:
    Ui::RaptorNetworkPage* _Ui = Q_NULLPTR;
    QButtonGroup* _ProxyGroup = Q_NULLPTR;
    QButtonGroup* _IPResolveGroup = Q_NULLPTR;
    QTimer* _DebounceTimer = Q_NULLPTR;
};

#endif // RAPTORNETWORKPAGE_H