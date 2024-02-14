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

#include "RaptorIndicatorButton.h"

RaptorIndicatorButton::RaptorIndicatorButton(QWidget* qParent) : QPushButton(qParent)
{
    invokeInstanceInit();
    invokeUiInit();
}

void RaptorIndicatorButton::invokeIconSet(const QString& qIcon)
{
    _Icon = qIcon;
    _SvgWidget->load(_Icon);
}

void RaptorIndicatorButton::invokeIndicatorTextSet(const QString& qText) const
{
    _Indicator->setText(qText);
}

void RaptorIndicatorButton::invokeIndicatorHeightSet(const int& qValue) const
{
    _Indicator->setFixedHeight(qValue);
}

void RaptorIndicatorButton::invokeIndicatorWidthSet(const int& qValue) const
{
    _Indicator->setFixedWidth(qValue);
}

void RaptorIndicatorButton::invokeIndicatorVisibleSet(const bool& qValue) const
{
    _Indicator->setVisible(qValue);
}

void RaptorIndicatorButton::setContentsMargins(int left, int top, int right, int bottom)
{
    _Layout->setContentsMargins(left, top, right, bottom);
}

void RaptorIndicatorButton::setText(const QString& qText)
{
    _Content->setText(qText);
}

void RaptorIndicatorButton::setIconSize(const QSize& qSize)
{
    _SvgWidget->setFixedSize(qSize);
}

void RaptorIndicatorButton::invokeInstanceInit()
{
    _SvgWidget = new QSvgWidget(this);
    _Content = new QLabel(this);
    _Content->setObjectName("QText");
    _Indicator = new QLabel(this);
    _Indicator->setObjectName("QIndicator");
    _SpacerItem = new QSpacerItem{20, 26, QSizePolicy::Expanding, QSizePolicy::Expanding};
    _Layout = new QHBoxLayout(this);
}

void RaptorIndicatorButton::invokeUiInit()
{
    _SvgWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    _Content->setAttribute(Qt::WA_TransparentForMouseEvents);
    _Indicator->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    _Layout->setSpacing(6);
    _Layout->addWidget(_SvgWidget);
    _Layout->addWidget(_Content);
    _Layout->addItem(_SpacerItem);
    _Layout->addWidget(_Indicator);
    setLayout(_Layout);
}