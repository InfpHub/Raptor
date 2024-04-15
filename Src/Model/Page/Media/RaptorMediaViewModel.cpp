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

#include "RaptorMediaViewModel.h"

QVariant RaptorMediaViewModel::headerData(int qSection,
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

int RaptorMediaViewModel::rowCount(const QModelIndex &qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _Items.length();
}

int RaptorMediaViewModel::columnCount(const QModelIndex &qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _ColumnCount;
}

QVariant RaptorMediaViewModel::data(const QModelIndex &qIndex, int qRole) const
{
    if (!qIndex.isValid())
    {
        return QVariant();
    }

    const auto item = _Items[qIndex.row()];
    switch (qRole)
    {
        case Qt::UserRole:
            return QVariant::fromValue<RaptorMediaItem>(item);
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            switch (qIndex.column())
            {
                case 1:
                    return item._Name;
                case 2:
                    return item._Size;
                case 3:
                    return item._Updated.split(' ')[0];
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

bool RaptorMediaViewModel::removeRow(int qRow, const QModelIndex &parent)
{
    if (qRow < 0 || qRow > _Items.length())
    {
        return false;
    }

    beginRemoveRows(parent, qRow, qRow);
    _Items.removeAt(qRow);
    endRemoveRows();
    return true;
}

bool RaptorMediaViewModel::removeRows(int qRow,
                                      int qCount,
                                      const QModelIndex &qIndex)
{
    if (qRow < 0 || qRow > _Items.length())
    {
        return false;
    }

    beginRemoveRows(qIndex, qRow, qRow + qCount - 1);
    _Items.removeAt(qRow);
    endRemoveRows();
    return true;
}

Qt::ItemFlags RaptorMediaViewModel::flags(const QModelIndex &qIndex) const
{
    if (qIndex.column() == 1)
    {
        return Qt::ItemIsEditable | QAbstractTableModel::flags(qIndex);
    }

    return QAbstractTableModel::flags(qIndex);
}

void RaptorMediaViewModel::invokeHeaderSet(const QVector<QString> &qHeader)
{
    _Headers = qHeader;
}

void RaptorMediaViewModel::invokeColumnCountSet(const quint16 &qCount)
{
    _ColumnCount = qCount;
}

void RaptorMediaViewModel::invokeItemAppend(const RaptorMediaItem &item)
{
    beginInsertRows(QModelIndex(), _Items.length(), _Items.length());
    _Items << item;
    endInsertRows();
}

void RaptorMediaViewModel::invokeItemsAppend(const QVector<RaptorMediaItem> &items)
{
    if (items.empty())
    {
        return;
    }

    beginInsertRows(QModelIndex(), _Items.length(), _Items.length() + items.length() - 1);
    _Items << items;
    endInsertRows();
}

void RaptorMediaViewModel::invokeItemsClear()
{
    beginResetModel();
    _Items.clear();
    endResetModel();
}

QVector<RaptorMediaItem> RaptorMediaViewModel::invokeItemsEject()
{
    return _Items;
}
