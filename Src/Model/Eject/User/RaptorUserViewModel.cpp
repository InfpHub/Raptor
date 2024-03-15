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

#include "RaptorUserViewModel.h"

QVariant RaptorUserViewModel::headerData(int qSection,
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
        if (qSection >= 0 && qSection < _Headers.length())
        {
            return _Headers[qSection];
        }

        return QVariant();
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    default:
        return QVariant();
    }
}

int RaptorUserViewModel::rowCount(const QModelIndex& qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _Items.length();
}

int RaptorUserViewModel::columnCount(const QModelIndex& qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _ColumnCount;
}

QVariant RaptorUserViewModel::data(const QModelIndex& qIndex, int qRole) const
{
    if (!qIndex.isValid())
    {
        return QVariant();
    }

    const auto item = _Items[qIndex.row()];
    switch (qRole)
    {
    case Qt::UserRole:
        return QVariant::fromValue<RaptorAuthenticationItem>(item);

    case Qt::DisplayRole:
        {
            switch (qIndex.column())
            {
            case 1:
                return item._Nickname;
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

bool RaptorUserViewModel::setData(const QModelIndex& qIndex, const QVariant& qVariant, int qRole)
{
    if (!qIndex.isValid())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (qRole != Qt::EditRole)
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (!qVariant.canConvert<RaptorAuthenticationItem>())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    const auto item = qVariant.value<RaptorAuthenticationItem>();
    _Items.replace(qIndex.row(), item);
    Q_EMIT dataChanged(qIndex, qIndex);
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
}

bool RaptorUserViewModel::removeRow(int qRow, const QModelIndex& qIndex)
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

void RaptorUserViewModel::invokeHeaderSet(const QVector<QString>& qHeader)
{
    _Headers = qHeader;
}

void RaptorUserViewModel::invokeColumnCountSet(const quint16& qCount)
{
    _ColumnCount = qCount;
}

void RaptorUserViewModel::invokeItemAppend(const RaptorAuthenticationItem& item)
{
    beginInsertRows(QModelIndex(), _Items.length(), _Items.length());
    _Items << item;
    endInsertRows();
}

void RaptorUserViewModel::invokeItemsAppend(const QVector<RaptorAuthenticationItem>& items)
{
    if (items.isEmpty())
    {
        return;
    }

    beginInsertRows(QModelIndex(), _Items.length(), _Items.length() + items.length() - 1);
    _Items << items;
    endInsertRows();
}

void RaptorUserViewModel::invokeItemsClear()
{
    beginResetModel();
    _Items.clear();
    endResetModel();
}

QVector<RaptorAuthenticationItem> RaptorUserViewModel::invokeItemsEject()
{
    return _Items;
}