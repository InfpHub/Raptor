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

#include "RaptorUserViewHeader.h"

void RaptorUserViewHeader::paintSection(QPainter* qPainter,
                                        const QRect& qRect,
                                        int qLogicalIndex) const
{
    if (qLogicalIndex == 0 || qLogicalIndex == 1)
    {
        qPainter->setRenderHints(QPainter::TextAntialiasing);
        qPainter->setPen(QColor(0, 164, 255, 255));
        qPainter->setFont(qApp->font());
        const auto qHeader = model()->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();
        qPainter->drawText(sectionPosition(0) + 6,
                           qRect.y(),
                           sectionSize(0) + sectionSize(1),
                           qRect.height(),
                           Qt::AlignVCenter,
                           qHeader);
    }
}