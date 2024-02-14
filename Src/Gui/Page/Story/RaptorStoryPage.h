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

#ifndef RAPTORSTORYPAGE_H
#define RAPTORSTORYPAGE_H

#include <QButtonGroup>
#include <QDesktopServices>
#include <QPropertyAnimation>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>
#include <QUrl>
#include <QWidget>

#include "../../../Common/RaptorDeclare.h"
#include "../../../Util/RaptorUtil.h"
#include "About/RaptorAboutPage.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorStoryPage;
}

QT_END_NAMESPACE

class RaptorStoryPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorStoryPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorStoryPage() Q_DECL_OVERRIDE;

    RaptorAboutPage* invokeAboutPageGet() const;

private:
    void invokeInstanceInit();

    void invokeUiInit() const;

    void invokeSlotInit() const;

public Q_SLOTS:
    Q_SLOT void onItemCopyWriterHaveFound(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onTabPrevClicked() const;

    Q_SLOT void onTabAboutToggled() const;

    Q_SLOT void onTabAboutQtToggled() const;

    Q_SLOT void onTabFAQToggled() const;

    Q_SLOT void onTabNextClicked() const;

    Q_SLOT void onGitHubClicked() const;

private:
    Ui::RaptorStoryPage* _Ui = Q_NULLPTR;
    QButtonGroup* _TabGroup = Q_NULLPTR;
};

#endif // RAPTORSTORYPAGE_H