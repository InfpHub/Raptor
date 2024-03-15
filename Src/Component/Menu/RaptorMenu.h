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
#ifndef RAPTORMENU_H
#define RAPTORMENU_H

#include <QVBoxLayout>
#include <QWidget>

#include "../../Common/RaptorDefine.h"
#include "../IndicatorButton/RaptorIndicatorButton.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorMenu;
}

QT_END_NAMESPACE

class RaptorMenu Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorMenu(QWidget *qParent = Q_NULLPTR);

    ~RaptorMenu() Q_DECL_OVERRIDE;

    void invokeItemAdd(const QString &qText,
                       const QString &qIcon,
                       const QKeySequence &qKeySequence = QKeySequence(),
                       const std::function<void()> &qCallback = Q_NULLPTR);

    void invokeEject();

private:
    void invokeInstanceInit();

    void invokeUiInit();

private:
    Ui::RaptorMenu *_Ui = Q_NULLPTR;
    QVBoxLayout *_Layout = Q_NULLPTR;
};


#endif // RAPTORMENU_H
