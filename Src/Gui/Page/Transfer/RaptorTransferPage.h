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

#ifndef RAPTORTRANSFERPAGE_H
#define RAPTORTRANSFERPAGE_H

#include <QButtonGroup>

#include "Downloaded/RaptorDownloadedPage.h"
#include "Downloading/RaptorDownloadingPage.h"
#include "Uploaded/RaptorUploadedPage.h"
#include "Uploading/RaptorUploadingPage.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorTransferPage;
}

QT_END_NAMESPACE

class RaptorTransferPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorTransferPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorTransferPage() Q_DECL_OVERRIDE;

    RaptorDownloadingPage* invokeDownloadingPageGet() const;

    RaptorDownloadedPage* invokeDownloadedPageGet() const;

    RaptorUploadingPage* invokeUploadingPageGet() const;

    RaptorUploadedPage* invokeUploadedPageGet() const;

private:
    void invokeInstanceInit();

    void invokeUiInit() const;

    void invokeSlotInit() const;

    static void invokeItemsLocate(const QModelIndexList& qIndexList);

public Q_SLOTS:
    Q_SLOT void onItemCopyWriterHaveFound(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onTabPrevClicked() const;

    Q_SLOT void onTabDownloadingToggled(const bool& qChecked) const;

    Q_SLOT void onTabDownloadedToggled(const bool& qChecked) const;

    Q_SLOT void onTabUploadingToggled(const bool& qChecked) const;

    Q_SLOT void onTabUploadedToggled(const bool& qChecked) const;

    Q_SLOT void onTabNextClicked() const;

    Q_SLOT void onResumeClicked() const;

    Q_SLOT void onPauseClicked() const;

    Q_SLOT void onLocateClicked() const;

    Q_SLOT void onCancelClicked() const;

    Q_SLOT void onClearClicked() const;

    Q_SLOT void onDeleteClicked() const;

    Q_SLOT void onBodyChanged(const int& qIndex) const;

private:
    Ui::RaptorTransferPage* _Ui = Q_NULLPTR;
    QButtonGroup* _TabGroup = Q_NULLPTR;
};

#endif // RAPTORTRANSFERPAGE_H