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

#include "RaptorContributor.h"

void RaptorContributor::paintEvent(QPaintEvent* qPaintEvent)
{
    Q_UNUSED(qPaintEvent)
    if (!_Contributor.first.isEmpty())
    {
        auto qPainter = QPainter(this);
        qPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        auto qPixmap = QPixmap(_Contributor.first);
        if (qPixmap.isNull())
        {
            return;
        }

        qPixmap = qPixmap.scaled(size(),
                                 Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
        auto qPainterPath = QPainterPath();
        qPainterPath.addRect(0, 0, width(), height());
        qPainter.setClipPath(qPainterPath);
        qPainter.drawPixmap(0, 0, width(), height(), qPixmap);
    }
}

void RaptorContributor::invokeContributorSet(const QPair<QString, QString>& qContributor)
{
    _Contributor = qContributor;
}

QPair<QString, QString> RaptorContributor::invokeContributorGet()
{
    return _Contributor;
}