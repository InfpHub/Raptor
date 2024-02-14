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

#include "RaptorTableViewHeader.h"

void RaptorTableViewHeader::paintSection(QPainter* qPainter,
                                         const QRect& qRect,
                                         int qLogicalIndex) const
{
    if (qLogicalIndex == 0)
    {
        if (!_SvgRender.isNull())
        {
            if (_SvgRender->isValid())
            {
                _SvgRender->render(qPainter, QRect(qRect.x() + 5, qRect.y() + 3, 20, 20));
            }
        }
    }
    else
    {
        QHeaderView::paintSection(qPainter, qRect, qLogicalIndex);
    }
}

void RaptorTableViewHeader::invokeIconSet(const QString& qUrl) const
{
    _SvgRender->load(qUrl);
}