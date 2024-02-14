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

#include "RaptorTimelineDelegate.h"

void RaptorTimelineDelegate::paint(QPainter* qPainter,
                                   const QStyleOptionViewItem& qStyleOption,
                                   const QModelIndex& qIndex) const
{
    QStyledItemDelegate::paint(qPainter, qStyleOption, qIndex);
    if (!qIndex.isValid())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorTimelineItem>();
    const auto qRect = qStyleOption.rect;
    qPainter->save();
    qPainter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    if (item._Active)
    {
        qPainter->setPen(QPen(QColor(199, 44, 63, 255), 2));
    }
    else
    {
        qPainter->setPen(QPen(QColor(136, 136, 136, 255), 2));
    }

    auto qCirclePath = QPainterPath();
    qCirclePath.addEllipse(qRect.x(), qRect.y(), 15, 15);
    qPainter->setClipPath(qCirclePath);
    qPainter->drawEllipse(qRect.x(), qRect.y(), 15, 15);
    qPainter->restore();

    qPainter->save();
    auto qDateFont = qApp->font();
    if (item._Active)
    {
        qDateFont.setPixelSize(14);
        qPainter->setPen(QPen(QColor(199, 44, 63, 255), 2));
    }
    else
    {
        qDateFont.setPixelSize(12);
        qPainter->setPen(QPen(QColor(136, 136, 136, 255), 2));
    }

    qPainter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    if (!item._Initial)
    {
        auto qLinePath = QPainterPath();
        qLinePath.addRect(qRect.x() + 7, qRect.y() + 15, 1, 65);
        qPainter->setClipPath(qLinePath);
        qPainter->drawRect(qRect.x() + 7, qRect.y() + 15, 1, 65);
    }

    qPainter->restore();
    qPainter->save();
    qPainter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    if (item._Active)
    {
        qPainter->setPen(QPen(QColor(199, 44, 63, 255)));
    }
    else
    {
        qPainter->setPen(QPen(QColor(136, 136, 136, 255)));
    }

    qPainter->setFont(qDateFont);
    qPainter->drawText(qRect.x() + 26, qRect.y() + 26, item._Date);
    qPainter->restore();
    qPainter->save();
    qPainter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    auto qSummaryFont = qApp->font();
    if (item._Active)
    {
        qPainter->setPen(QColor(159, 44, 63, 255));
        qSummaryFont.setPixelSize(19);
        qSummaryFont.setBold(true);
    }
    else
    {
        qPainter->setPen(QColor(136, 136, 136, 255));
        qSummaryFont.setPixelSize(13);
        qSummaryFont.setBold(false);
    }

    qPainter->setFont(qSummaryFont);
    qPainter->drawText(QRect(qRect.x() + 26, qRect.y() + 7, qRect.width(), qRect.height()),
                       item._Summary,
                       QTextOption(Qt::AlignVCenter));
    qPainter->restore();
}

QSize RaptorTimelineDelegate::sizeHint(const QStyleOptionViewItem& qStyleOption, const QModelIndex& qIndex) const
{
    return QSize(qStyleOption.widget->width(), _ItemHeight);
}