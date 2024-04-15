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

#include "RaptorSpaceViewModel.h"

RaptorSpaceViewModel::RaptorSpaceViewModel(QObject *qParent) : QAbstractTableModel(qParent)
{
    invokeInstanceInit();
}

QVariant RaptorSpaceViewModel::headerData(int qSection,
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

int RaptorSpaceViewModel::rowCount(const QModelIndex &qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _Items.length();
}

int RaptorSpaceViewModel::columnCount(const QModelIndex &qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _ColumnCount;
}

QVariant RaptorSpaceViewModel::data(const QModelIndex &qIndex, int qRole) const
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

bool RaptorSpaceViewModel::setData(const QModelIndex &qIndex, const QVariant &qVariant, int qRole)
{
    if (!qIndex.isValid())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (qRole != Qt::EditRole)
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (qVariant.canConvert<QString>())
    {
        const auto qName = qVariant.value<QString>();
        auto item = _Items[qIndex.row()];
        if (item._Name == qName)
        {
            return QAbstractTableModel::setData(qIndex, qVariant, qRole);
        }

        item._Name = qName;
        _Items.replace(qIndex.row(), item);
        auto output = RaptorOutput();
        output._State = true;
        output._Data = QVariant::fromValue<QModelIndex>(qIndex);
        Q_EMIT itemEdited(QVariant::fromValue<RaptorOutput>(output));
        Q_EMIT dataChanged(qIndex, qIndex);
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (qVariant.canConvert<RaptorFileItem>())
    {
        const auto item = qVariant.value<RaptorFileItem>();
        _Items.replace(qIndex.row(), item);
        Q_EMIT dataChanged(qIndex, qIndex);
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    return QAbstractTableModel::setData(qIndex, qVariant, qRole);
}

bool RaptorSpaceViewModel::removeRow(int qRow, const QModelIndex &parent)
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

bool RaptorSpaceViewModel::removeRows(int qRow,
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

Qt::ItemFlags RaptorSpaceViewModel::flags(const QModelIndex &qIndex) const
{
    if (qIndex.column() == 1)
    {
        return Qt::ItemIsEditable | QAbstractTableModel::flags(qIndex);
    }

    return QAbstractTableModel::flags(qIndex);
}

void RaptorSpaceViewModel::sort(int qColumn, Qt::SortOrder qOrder)
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
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsBySizeAscSort, this, std::placeholders::_1, std::placeholders::_2));
            } else
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsBySizeDescSort, this, std::placeholders::_1, std::placeholders::_2));
            }
            break;
        }
        case 3:
        {
            if (qOrder == Qt::AscendingOrder)
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByUpdatedAscSort, this, std::placeholders::_1, std::placeholders::_2));
            } else
            {
                std::sort(_Items.begin(), _Items.end(), std::bind(invokeItemsByUpdatedDescSort, this, std::placeholders::_1, std::placeholders::_2));
            }
            break;
        }
        default:
            break;
    }

    endResetModel();
}

void RaptorSpaceViewModel::invokeHeaderSet(const QVector<QString> &qHeader)
{
    _Headers = qHeader;
}

void RaptorSpaceViewModel::invokeColumnCountSet(const quint16 &qCount)
{
    _ColumnCount = qCount;
}

void RaptorSpaceViewModel::invokeItemAppend(const RaptorFileItem &item)
{
    beginInsertRows(QModelIndex(), _Items.length(), _Items.length());
    _Items << item;
    endInsertRows();
}

void RaptorSpaceViewModel::invokeItemsAppend(const QVector<RaptorFileItem> &items)
{
    if (items.empty())
    {
        return;
    }

    beginInsertRows(QModelIndex(), _Items.length(), _Items.length() + items.length() - 1);
    _Items << items;
    endInsertRows();
}

void RaptorSpaceViewModel::invokeItemsClear()
{
    beginResetModel();
    _Items.clear();
    endResetModel();
}

QVector<RaptorFileItem> RaptorSpaceViewModel::invokeItemsEject()
{
    return _Items;
}

void RaptorSpaceViewModel::invokeInstanceInit()
{
    qCollator = QCollator(QLocale::Chinese);
    qCollator.setNumericMode(false);
    qCollator.setIgnorePunctuation(true);
}

bool RaptorSpaceViewModel::invokeItemsByNameAscSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return qCollator.compare(item._Name, iten._Name) < 0;
}

bool RaptorSpaceViewModel::invokeItemsByNameDescSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return qCollator.compare(item._Name, iten._Name) > 0;
}

bool RaptorSpaceViewModel::invokeItemsBySizeAscSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return item._Byte < iten._Byte;
}

bool RaptorSpaceViewModel::invokeItemsBySizeDescSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return item._Byte > iten._Byte;
}

bool RaptorSpaceViewModel::invokeItemsByUpdatedAscSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return qCollator.compare(item._Updated, iten._Updated) < 0;
}

bool RaptorSpaceViewModel::invokeItemsByUpdatedDescSort(const RaptorFileItem &item, const RaptorFileItem &iten) const
{
    return qCollator.compare(item._Updated, iten._Updated) > 0;
}
