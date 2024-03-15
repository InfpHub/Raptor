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

#include "RaptorDownloadingViewModel.h"

QVariant RaptorDownloadingViewModel::headerData(int qSection,
                                                Qt::Orientation qOrientation,
                                                int qRole) const
{
    if (qOrientation != Qt::Horizontal)
    {
        return QVariant();
    }

    switch (qRole)
    {
    case Qt::DisplayRole:
        if (qSection > 0 && qSection <= _Headers.length())
        {
            return _Headers[qSection - 1];
        }

        return QVariant();
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    default:
        return QVariant();
    }
}

int RaptorDownloadingViewModel::rowCount(const QModelIndex& qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _Items.length();
}

int RaptorDownloadingViewModel::columnCount(const QModelIndex& qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _ColumnCount;
}

QVariant RaptorDownloadingViewModel::data(const QModelIndex& qIndex, int qRole) const
{
    if (!qIndex.isValid())
    {
        return QVariant();
    }

    const auto item = _Items[qIndex.row()];
    switch (qRole)
    {
    case Qt::UserRole:
        return QVariant::fromValue<RaptorTransferItem>(item);
    case Qt::DisplayRole:
        {
            switch (qIndex.column())
            {
            case 1:
                return item._Name;
            case 2:
                return item._Transferred <= 0 ? item._Size : QStringLiteral("%1/%2").arg(RaptorUtil::invokeStorageUnitConvert(item._Transferred), item._Size);
            default:
                return QVariant();
            }
        }
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    default:
        return QVariant();
    }
}

bool RaptorDownloadingViewModel::setData(const QModelIndex& qIndex, const QVariant& qVariant, int qRole)
{
    if (!qIndex.isValid())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (qRole != Qt::EditRole)
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (!qVariant.canConvert<RaptorTransferItem>())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    const auto item = qVariant.value<RaptorTransferItem>();
    _Items.replace(qIndex.row(), item);
    Q_EMIT dataChanged(qIndex, qIndex);
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
}

bool RaptorDownloadingViewModel::removeRow(int qRow, const QModelIndex& qIndex)
{
    if (qRow < 0 || qRow > _Items.length())
    {
        return false;
    }

    beginRemoveRows(qIndex, qRow, qRow);
    _Items.removeAt(qRow);
    endRemoveRows();
    return true;
}

void RaptorDownloadingViewModel::invokeHeaderSet(const QVector<QString>& qHeader)
{
    _Headers = qHeader;
}

void RaptorDownloadingViewModel::invokeColumnCountSet(const quint16& qCount)
{
    _ColumnCount = qCount;
}

void RaptorDownloadingViewModel::invokeItemAppend(const RaptorTransferItem& item)
{
    beginInsertRows(QModelIndex(), _Items.length(), _Items.length());
    _Items << item;
    endInsertRows();
}

void RaptorDownloadingViewModel::invokeItemsAppend(const QVector<RaptorTransferItem>& items)
{
    if (items.isEmpty())
    {
        return;
    }

    beginInsertRows(QModelIndex(), _Items.length(), _Items.length() + items.length() - 1);
    _Items << items;
    endInsertRows();
}

void RaptorDownloadingViewModel::invokeItemsClear()
{
    beginResetModel();
    _Items.clear();
    endResetModel();
}

QVector<RaptorTransferItem> RaptorDownloadingViewModel::invokeItemsEject()
{
    return _Items;
}