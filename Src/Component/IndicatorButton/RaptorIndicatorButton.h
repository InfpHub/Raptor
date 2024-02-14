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

#ifndef RAPTORINDICATORBUTTON_H
#define RAPTORINDICATORBUTTON_H

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QSpacerItem>
#include <QSvgWidget>

class RaptorIndicatorButton Q_DECL_FINAL : public QPushButton
{
    Q_OBJECT

public:
    explicit RaptorIndicatorButton(QWidget* qParent = Q_NULLPTR);

    void invokeIconSet(const QString& qIcon);

    void invokeIndicatorTextSet(const QString& qText) const;

    void invokeIndicatorHeightSet(const int& qValue) const;

    void invokeIndicatorWidthSet(const int& qValue) const;

    void invokeIndicatorVisibleSet(const bool& qValue) const;

    void setContentsMargins(int left, int top, int right, int bottom);

    void setText(const QString& qText);

    void setIconSize(const QSize& qSize);

private:
    void invokeInstanceInit();

    void invokeUiInit();

private:
    QString _Icon;
    QLabel* _Content = Q_NULLPTR;
    QLabel* _Indicator = Q_NULLPTR;
    QSpacerItem* _SpacerItem = Q_NULLPTR;
    QHBoxLayout* _Layout = Q_NULLPTR;
    QSvgWidget* _SvgWidget = Q_NULLPTR;
};

#endif // RAPTORINDICATORBUTTON_H