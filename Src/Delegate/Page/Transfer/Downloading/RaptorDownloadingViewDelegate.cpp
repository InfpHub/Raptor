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

#include "RaptorDownloadingViewDelegate.h"

void RaptorDownloadingViewDelegate::paint(QPainter* qPainter,
                                          const QStyleOptionViewItem& qStyleOption,
                                          const QModelIndex& qIndex) const
{
    QStyledItemDelegate::paint(qPainter, qStyleOption, qIndex);
    if (!qIndex.isValid())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
    if (const auto qColumn = qIndex.column();
        qColumn == 0)
    {
        if (!item._Icon.isNull())
        {
            _SvgRender->load(item._Icon);
            if (_SvgRender->isValid())
            {
                const auto qRect = qStyleOption.rect;
                _SvgRender->render(qPainter, QRect(qRect.x() + 5, qRect.y() + 3, 20, 20));
            }
        }
    }
    else if (qColumn == 3)
    {
        auto qFont = qApp->font();
        qFont.setPixelSize(10);
        const auto qFontMetrics = QFontMetrics(qFont);
        const auto qStatusHeight = qFontMetrics.height();
        const auto qStartX = qStyleOption.rect.x() + _ContentMargin;
        const auto qBackgroundWidth = qStyleOption.rect.width() - _ContentMargin - _ContentMargin;

        // 网速、剩余传输时间绘制
        qPainter->save();
        auto qTextOption = QTextOption();
        qTextOption.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        qPainter->setRenderHint(QPainter::TextAntialiasing);
        qPainter->setPen(QColor(136, 136, 136, 255));
        qPainter->setFont(qFont);
        qPainter->drawText(
            QRect(qStartX, qStyleOption.rect.y() + 1, qBackgroundWidth, qStatusHeight),
            item._Status,
            qTextOption
        );
        qPainter->restore();

        // 进度条背景
        qPainter->save();
        qPainter->setRenderHint(QPainter::Antialiasing);
        auto qBackgroundPath = QPainterPath();
        qBackgroundPath.addRoundedRect(
            qStartX,
            qStyleOption.rect.y() + 18,
            qBackgroundWidth,
            _ProgressBarHeight,
            _ProgressBarHeight / 2,
            _ProgressBarHeight / 2
        );

        qPainter->setClipPath(qBackgroundPath);
        qPainter->fillPath(qBackgroundPath, QColor(136, 136, 136, 255));
        qPainter->restore();
        if (item._Transferred == -1)
        {
            return;
        }

        // 进度条前景
        qPainter->save();
        auto qForegroundPath = QPainterPath();
        auto qForegroundWidth = static_cast<qreal>(item._Transferred) / static_cast<qreal>(item._Byte) * qBackgroundWidth;
        qForegroundWidth = qForegroundWidth > qBackgroundWidth ? qBackgroundWidth : qForegroundWidth;
        qForegroundPath.addRoundedRect(
            qStartX,
            qStyleOption.rect.y() + 18,
            qForegroundWidth,
            _ProgressBarHeight,
            _ProgressBarHeight / 2,
            _ProgressBarHeight / 2
        );

        qPainter->setClipPath(qForegroundPath.intersected(qBackgroundPath));
        qPainter->fillPath(qForegroundPath, QColor(0, 164, 255, 255));
        qPainter->restore();
    }
}