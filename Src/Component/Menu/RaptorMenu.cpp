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

#include "RaptorMenu.h"

#include "ui_RaptorMenu.h"

RaptorMenu::RaptorMenu(QWidget *qParent) : QWidget(qParent),
                                           _Ui(new Ui::RaptorMenu)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
}

RaptorMenu::~RaptorMenu()
{
    qFree(_Ui)
}

void RaptorMenu::invokeItemAdd(const QString &qText,
                               const QString &qIcon,
                               const QKeySequence &qKeySequence,
                               const std::function<void()> &qCallback)
{
    const auto qPushButton = new RaptorIndicatorButton(_Ui->_Stage);
    qPushButton->invokeIconSet(qIcon);
    qPushButton->invokeIndicatorHeightSet(16);
    if (qKeySequence.isEmpty())
    {
        qPushButton->invokeIndicatorVisibleSet(false);
    } else
    {
        qPushButton->invokeIndicatorTextSet(qKeySequence.toString());
        qPushButton->setShortcutEnabled(true);
        qPushButton->setShortcut(qKeySequence);
    }

    qPushButton->setContentsMargins(6, 0, 6, 0);
    qPushButton->setIconSize(QSize(22, 22));
    qPushButton->setText(qText);
    qPushButton->setFixedHeight(26);
    connect(qPushButton,
            &RaptorIndicatorButton::clicked,
            [=]() -> void
            {
                if (qCallback)
                {
                    qCallback();
                }

                close();
            });

    _Layout->addWidget(qPushButton);
}

void RaptorMenu::invokeEject()
{
    show();
}

void RaptorMenu::invokeInstanceInit()
{
    _Layout = new QVBoxLayout(_Ui->_Stage);
    _Layout->setSpacing(6);
    _Layout->setContentsMargins(6, 6, 6, 6);
    _Ui->_Stage->setLayout(_Layout);
}

void RaptorMenu::invokeUiInit()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
}
