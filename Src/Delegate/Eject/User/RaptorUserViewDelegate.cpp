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

#include "RaptorUserViewDelegate.h"

void RaptorUserViewDelegate::paint(QPainter* qPainter,
                                   const QStyleOptionViewItem& qStyleOption,
                                   const QModelIndex& qIndex) const
{
    QStyledItemDelegate::paint(qPainter, qStyleOption, qIndex);
    if (!qIndex.isValid())
    {
        return;
    }

    switch (qIndex.column())
    {
    case 0:
        {
            const auto item = qIndex.data(Qt::UserRole).value<RaptorAuthenticationItem>();
            const auto qRect = QRect{
                qStyleOption.rect.x() + 5,
                qStyleOption.rect.y() + 3,
                20,
                20
            };
            qPainter->save();
            qPainter->setPen(Qt::NoPen);
            qPainter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            auto qPainterPath = QPainterPath();
            qPainterPath.addEllipse(qRect.x(), qRect.y(), qRect.height(), qRect.height());
            qPainter->setClipPath(qPainterPath);
            auto qPixmap = QPixmap();
            qPixmap.loadFromData(item._Avatar);
            qPainter->drawPixmap(qRect, qPixmap);
            qPainter->restore();
        }
        break;
    case 2:
        {
            if (const auto item = qIndex.data(Qt::UserRole).value<RaptorAuthenticationItem>();
                item._Active)
            {
                qPainter->save();
                const auto qRect = QRect{
                    qStyleOption.rect.x() + 15,
                    qStyleOption.rect.y() + 8,
                    qStyleOption.rect.width() - 30,
                    qStyleOption.rect.height() - 16
                };
                qPainter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
                qPainter->setPen(Qt::NoPen);
                qPainter->setBrush(QColor(0, 164, 255, 255));
                qPainter->drawEllipse(qRect);
                qPainter->restore();
            }
        }
        break;
    default:
        break;
    }
}