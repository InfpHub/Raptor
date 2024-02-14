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

#ifndef RAPTORTABLEVIEW_H
#define RAPTORTABLEVIEW_H

#include <QApplication>
#include <QHeaderView>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QSvgRenderer>
#include <QTableView>

#include "../../Suite/Setting/RaptorSettingSuite.h"
#include "../../Util/RaptorUtil.h"

class RaptorTableView Q_DECL_FINAL : public QTableView
{
    Q_OBJECT

public:
    explicit RaptorTableView(QWidget* qParent = Q_NULLPTR);

    enum Code
    {
        Forbidden = 403,
        NotFound = 404,
        InternalError = 500
    };

    void invokeServerCodeSet(const Code& code);

    void invokeBackgroundPaintableSet(const bool& qValue);

    void invokeTitleSet(const QString& qText);

    void invokeSummarySet(const QString& qText);

    void invokeIndicatorSet(const QString& qText) const;

protected:
    void paintEvent(QPaintEvent* qPaintEvent) Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    void invokeSlotInit() const;

    void invokeBackgroundPaint();

Q_SIGNALS:
    Q_SIGNAL void indicatorClicked(const Code& qCode);

private Q_SLOTS:
    Q_SLOT void onIndicatorClicked();

private:
    Code _Code;
    bool _Value = false;
    QString _Title;
    QString _Summary;
    QPushButton* _Indicator = Q_NULLPTR;
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());
};

#endif // RAPTORTABLEVIEW_H