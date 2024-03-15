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

#ifndef RAPTORPLUSPAGE_H
#define RAPTORPLUSPAGE_H

#include <QButtonGroup>
#include <QWidget>

#include "./Copy/RaptorCopyPage.h"
#include "./Clean/RaptorCleanPage.h"
#include "../../Eject/Toast/RaptorToast.h"
#include "../../../Suite/Store/RaptorStoreSuite.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorPlusPage;
}

QT_END_NAMESPACE

class RaptorPlusPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorPlusPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorPlusPage() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    [[nodiscard]]
    RaptorCopyPage* invokeCopyPageGet() const;

    [[nodiscard]]
    RaptorCleanPage* invokeCleanPageGet() const;

private:
    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

Q_SIGNALS:
    Q_SIGNAL void itemsCopying(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemCopyWriterHaveFound(const QVariant& qVariant) const;

    Q_SLOT void onItemLogouting(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onTabPrevClicked() const;

    Q_SLOT void onTabCopyToggled() const;

    Q_SLOT void onTabCleanToggled() const;

    Q_SLOT void onTabNextClicked() const;

    Q_SLOT void onSearchClicked() const;

    Q_SLOT void onExecuteClicked() const;

    Q_SLOT void onRefreshClicked() const;

    Q_SLOT void onCancelClicked() const;

    Q_SLOT void onBodyChanged(const int& qIndex) const;

private:
    QButtonGroup* _TabGroup = Q_NULLPTR;
    Ui::RaptorPlusPage* _Ui = Q_NULLPTR;
};

#endif // RAPTORPLUSPAGE_H