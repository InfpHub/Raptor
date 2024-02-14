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

#ifndef RAPTORSETTINGPAGE_H
#define RAPTORSETTINGPAGE_H

#include <QButtonGroup>
#include <QWidget>

#include "Network/RaptorNetworkPage.h"
#include "../../Eject/MessageBox/RaptorMessageBox.h"
#include "../../Eject/Toast/RaptorToast.h"
#include "../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../Model/Page/Setting/RaptorSettingViewModel.h"
#include "../../../Suite/Setting/RaptorSettingSuite.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorSettingPage;
}

QT_END_NAMESPACE

class RaptorSettingPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorSettingPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorSettingPage() Q_DECL_OVERRIDE;

    RaptorNetworkPage* invokeNetworkPageGet() const;

private:
    void invokeInstanceInit();

    void invokeUiInit() const;

    void invokeSlotInit() const;

public Q_SLOTS:
    Q_SLOT void onItemCopyWriterHaveFound(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onTabPrevClicked() const;

    Q_SLOT void onTabUiToggled(const bool& qChecked) const;

    Q_SLOT void onTabDownloadToggled(const bool& qChecked) const;

    Q_SLOT void onTabUploadToggled(const bool& qChecked) const;

    Q_SLOT void onTabPlayToggled(const bool& qChecked) const;

    Q_SLOT void onTabNetworkToggled(const bool& qChecked) const;

    Q_SLOT void onTabOtherToggled(const bool& qChecked) const;

    Q_SLOT void onTabNextClicked() const;

private:
    Ui::RaptorSettingPage* _Ui = Q_NULLPTR;
    QButtonGroup* _TabGroup = Q_NULLPTR;
};

#endif // RAPTORSETTINGPAGE_H