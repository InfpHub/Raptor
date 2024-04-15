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

#include "RaptorTableViewDelegate.h"

void RaptorTableViewDelegate::paint(QPainter *qPainter,
                                    const QStyleOptionViewItem &qStyleOption,
                                    const QModelIndex &qIndex) const
{
    QStyledItemDelegate::paint(qPainter, qStyleOption, qIndex);
    if (!qIndex.isValid())
    {
        return;
    }

    if (qIndex.column() == 0)
    {
        if (qIndex.data(Qt::UserRole).canConvert<RaptorFileItem>())
        {
            invokeIconPaint(qPainter,
                            qIndex.data(Qt::UserRole).value<RaptorFileItem>()._Icon,
                            qStyleOption.rect);
        } else if (qIndex.data(Qt::UserRole).canConvert<RaptorTransferItem>())
        {
            invokeIconPaint(qPainter,
                            qIndex.data(Qt::UserRole).value<RaptorTransferItem>()._Icon,
                            qStyleOption.rect);
        } else if (qIndex.data(Qt::UserRole).canConvert<RaptorShareItem>())
        {
            invokeIconPaint(qPainter,
                            qIndex.data(Qt::UserRole).value<RaptorShareItem>()._Icon,
                            qStyleOption.rect);
        } else if (qIndex.data(Qt::UserRole).canConvert<RaptorStarItem>())
        {
            invokeIconPaint(qPainter,
                            qIndex.data(Qt::UserRole).value<RaptorStarItem>()._Icon,
                            qStyleOption.rect);
        } else if (qIndex.data(Qt::UserRole).canConvert<RaptorTrashItem>())
        {
            invokeIconPaint(qPainter,
                            qIndex.data(Qt::UserRole).value<RaptorTrashItem>()._Icon,
                            qStyleOption.rect);
        } else if (qIndex.data(Qt::UserRole).canConvert<RaptorMediaItem>())
        {
            invokeIconPaint(qPainter,
                            qIndex.data(Qt::UserRole).value<RaptorMediaItem>()._Icon,
                            qStyleOption.rect);
        } else if (qIndex.data(Qt::UserRole).canConvert<RaptorSettingItem>())
        {
            invokeIconPaint(qPainter,
                            qIndex.data(Qt::UserRole).value<RaptorSettingItem>()._Icon,
                            qStyleOption.rect);
        } else if (qIndex.data(Qt::UserRole).canConvert<RaptorDeviceItem>())
        {
            invokeIconPaint(qPainter,
                            RaptorUtil::invokeIconMatch("Device", false, true),
                            qStyleOption.rect);
        }
    }
}

QWidget *RaptorTableViewDelegate::createEditor(QWidget *qParent,
                                               const QStyleOptionViewItem &qStyleOption,
                                               const QModelIndex &qIndex) const
{
    // RaptorRename
    if (qIndex.data(Qt::UserRole).canConvert<RaptorFileItem>())
    {
        if (qIndex.column() == 1)
        {
            const auto qLineEdit = new QLineEdit(qParent);
            qLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
            qLineEdit->setObjectName("QCellEdit");
            return qLineEdit;
        }
    }


    // RaptorSettingPage
    if (qIndex.data(Qt::UserRole).canConvert<RaptorSettingItem>())
    {
        if (qIndex.column() == 2)
        {
            const auto qLineEdit = new QLineEdit(qParent);
            qLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
            qLineEdit->setObjectName("QCellEdit");
            return qLineEdit;
        }
    }

    return QStyledItemDelegate::createEditor(qParent, qStyleOption, qIndex);
}

void RaptorTableViewDelegate::invokeIconPaint(QPainter *qPainter,
                                              const QString &qIcon,
                                              const QRect &qRect) const
{
    if (qIcon.isNull())
    {
        return;
    }

    _SvgRender->load(qIcon);
    if (_SvgRender->isValid())
    {
        _SvgRender->render(qPainter, QRect(qRect.x() + 5, qRect.y() + 3, 20, 20));
    }
}
