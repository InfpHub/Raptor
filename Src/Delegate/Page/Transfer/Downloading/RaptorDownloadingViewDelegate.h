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

#ifndef RAPTORDOWNLOADINGVIEWDELEGATE_H
#define RAPTORDOWNLOADINGVIEWDELEGATE_H

#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QProgressBar>
#include <QStyledItemDelegate>
#include <QSvgRenderer>

#include "../../../../Common/RaptorDeclare.h"

class RaptorDownloadingViewDelegate Q_DECL_FINAL : public QStyledItemDelegate
{
    Q_OBJECT

public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* qPainter,
               const QStyleOptionViewItem& qStyleOption,
               const QModelIndex& qIndex) const Q_DECL_OVERRIDE;

private:
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());

    static Q_DECL_CONSTEXPR auto _ContentMargin = 4.; // 内容边距

    static Q_DECL_CONSTEXPR auto _ProgressBarHeight = 4.; // 进度条高度
};

#endif // RAPTORDOWNLOADINGVIEWDELEGATE_H