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

#include "RaptorCleanViewModel.h"

RaptorCleanViewModel::RaptorCleanViewModel(QObject *qParent) : QAbstractTableModel(qParent)
{
    invokeInstanceInit();
}

QVariant RaptorCleanViewModel::headerData(int qSection,
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

int RaptorCleanViewModel::rowCount(const QModelIndex &qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _Items.length();
}

int RaptorCleanViewModel::columnCount(const QModelIndex &qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _ColumnCount;
}

QVariant RaptorCleanViewModel::data(const QModelIndex &qIndex, int qRole) const
{
    if (!qIndex.isValid())
    {
        return QVariant();
    }

    const auto item = _Items[qIndex.row()];
    switch (qRole)
    {
        case Qt::UserRole:
            return QVariant::fromValue<RaptorFileItem>(item);
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            switch (qIndex.column())
            {
                case 1:
                    return item._Name;
                case 2:
                    return item._Dir;
                case 3:
                    return item._Size;
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

bool RaptorCleanViewModel::removeRow(int qRow, const QModelIndex &parent)
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

bool RaptorCleanViewModel::removeRows(int qRow,
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

void RaptorCleanViewModel::sort(int qColumn, Qt::SortOrder qOrder)
{
    if (qColumn == 0)
    {
        return;
    }

    beginResetModel();
    switch (qColumn)
    {
        case 1:
        {
            if (qOrder == Qt::AscendingOrder)
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByNameAscSort, this, std::placeholders::_1, std::placeholders::_2));
            } else
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByNameDescSort, this, std::placeholders::_1, std::placeholders::_2));
            }
            break;
        }
        case 2:
        {
            if (qOrder == Qt::AscendingOrder)
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByDirAscSort, this, std::placeholders::_1, std::placeholders::_2));
            } else
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByDirDescSort, this, std::placeholders::_1, std::placeholders::_2));
            }
            break;
        }
        case 3:
        {
            if (qOrder == Qt::AscendingOrder)
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsBySizeAscSort, this, std::placeholders::_1, std::placeholders::_2));
            } else
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsBySizeDescSort, this, std::placeholders::_1, std::placeholders::_2));
            }
            break;
        }
        default:
            break;
    }

    endResetModel();
}

void RaptorCleanViewModel::invokeHeaderSet(const QVector<QString> &qHeader)
{
    _Headers = qHeader;
}

void RaptorCleanViewModel::invokeColumnCountSet(const quint16 &qCount)
{
    _ColumnCount = qCount;
}

void RaptorCleanViewModel::invokeItemAppend(const RaptorFileItem &item)
{
    beginInsertRows(QModelIndex(), _Items.length(), _Items.length());
    _Items << item;
    endInsertRows();
}

void RaptorCleanViewModel::invokeItemsAppend(const QVector<RaptorFileItem> &items)
{
    if (items.empty())
    {
        return;
    }

    beginInsertRows(QModelIndex(), _Items.length(), _Items.length() + items.length() - 1);
    _Items << items;
    endInsertRows();
}

void RaptorCleanViewModel::invokeItemsClear()
{
    beginResetModel();
    _Items.clear();
    endResetModel();
}

QVector<RaptorFileItem> RaptorCleanViewModel::invokeItemsEject()
{
    return _Items;
}

void RaptorCleanViewModel::invokeInstanceInit()
{
    qCollator = QCollator(QLocale::Chinese);
    qCollator.setNumericMode(false);
    qCollator.setIgnorePunctuation(true);
}

bool RaptorCleanViewModel::invokeItemsByNameAscSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return qCollator.compare(item._Name, iten._Name) < 0;
}

bool RaptorCleanViewModel::invokeItemsByNameDescSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return qCollator.compare(item._Name, iten._Name) > 0;
}

bool RaptorCleanViewModel::invokeItemsByDirAscSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return qCollator.compare(item._Dir, iten._Dir) < 0;
}

bool RaptorCleanViewModel::invokeItemsByDirDescSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return qCollator.compare(item._Dir, iten._Dir) > 0;
}

bool RaptorCleanViewModel::invokeItemsBySizeAscSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return item._Byte < iten._Byte;
}

bool RaptorCleanViewModel::invokeItemsBySizeDescSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return item._Byte > iten._Byte;
}
