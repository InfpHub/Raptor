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

#ifndef RAPTORUIPAGE_H
#define RAPTORUIPAGE_H

#include <QButtonGroup>
#include <QTimer>
#include <QWidget>

#include "../../../Eject/Font/RaptorFont.h"
#include "../../../../Common/RaptorDeclare.h"
#include "../../../../Suite/Setting/RaptorSettingSuite.h"
#include "../../../../Suite/Store/RaptorStoreSuite.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorUiPage;
}

QT_END_NAMESPACE

class RaptorUiPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorUiPage(QWidget *qParent = Q_NULLPTR);

    ~RaptorUiPage() Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    void invokeUiInit() const;

    void invokeSlotInit() const;

private Q_SLOTS:
    Q_SLOT void onDebounceTimerTimeout() const;

    Q_SLOT void onThemeAutoClicked(const bool &qChecked) const;

    Q_SLOT void onThemeLightClicked(const bool &qChecked) const;

    Q_SLOT void onThemeDarkClicked(const bool &qChecked) const;

    Q_SLOT void onFontClicked() const;

    Q_SLOT void onNoticeStateChanged(const int &qState) const;

    Q_SLOT void onAutoSignStateChanged(const int &qState) const;

    Q_SLOT void onContextMenuStateChanged(const int &qState) const;

    Q_SLOT void onTrayIconStateChanged(const int &qState) const;

    Q_SLOT void onMinimizeToTrayStateChanged(const int &qState) const;

private:
    Ui::RaptorUiPage *_Ui = Q_NULLPTR;
    QButtonGroup *_ThemeGroup = Q_NULLPTR;
    RaptorFont *_Font = Q_NULLPTR;
    QTimer *_DebounceTimer = Q_NULLPTR;
};

#endif // RAPTORUIPAGE_H
