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

#include "RaptorSettingViewModel.h"

QVariant RaptorSettingViewModel::headerData(int qSection,
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

int RaptorSettingViewModel::rowCount(const QModelIndex& qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _Items.length();
}

int RaptorSettingViewModel::columnCount(const QModelIndex& qIndex) const
{
    if (qIndex.isValid())
    {
        return 0;
    }

    return _ColumnCount;
}

QVariant RaptorSettingViewModel::data(const QModelIndex& qIndex, int qRole) const
{
    if (!qIndex.isValid())
    {
        return QVariant();
    }

    const auto item = _Items[qIndex.row()];
    switch (qRole)
    {
    case Qt::UserRole:
        return QVariant::fromValue<RaptorSettingItem>(item);
    case Qt::DisplayRole:
    case Qt::EditRole:
        {
            switch (qIndex.column())
            {
            case 1:
                return item._Name;
            case 2:
                return item._Path;
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

bool RaptorSettingViewModel::setData(const QModelIndex& qIndex, const QVariant& qVariant, int qRole)
{
    if (!qIndex.isValid())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (qRole != Qt::EditRole)
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (!qVariant.canConvert<QString>())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    auto item = _Items[qIndex.row()];
    if (item._Path == qVariant.value<QString>())
    {
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    const auto qFileName = qVariant.value<QString>();
    const auto qFileInfo = QFileInfo(qFileName);
    auto output = RaptorOutput();
    if (!qFileInfo.isFile())
    {
        output._State = false;
        output._Message = QStringLiteral("请选择可执行文件!");
        Q_EMIT itemEdited(QVariant::fromValue<RaptorOutput>(output));
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    if (!qFileInfo.exists())
    {
        output._State = false;
        output._Message = QStringLiteral("可执行文件不存在，请重新选择!");
        Q_EMIT itemEdited(QVariant::fromValue<RaptorOutput>(output));
        return QAbstractTableModel::setData(qIndex, qVariant, qRole);
    }

    item._Path = qVariant.value<QString>();
    _Items.replace(qIndex.row(), item);
    output._State = true;
    output._Data = QVariant::fromValue<QModelIndex>(qIndex);
    Q_EMIT itemEdited(QVariant::fromValue<RaptorOutput>(output));
    Q_EMIT dataChanged(qIndex, qIndex);
    return true;
}

bool RaptorSettingViewModel::removeRow(int qRow, const QModelIndex& qIndex)
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

Qt::ItemFlags RaptorSettingViewModel::flags(const QModelIndex& qIndex) const
{
    if (qIndex.column() == 2)
    {
        return Qt::ItemIsEditable | QAbstractTableModel::flags(qIndex);
    }

    return QAbstractTableModel::flags(qIndex);
}

void RaptorSettingViewModel::invokeHeaderSet(const QVector<QString>& qHeader)
{
    _Headers = qHeader;
}

void RaptorSettingViewModel::invokeColumnCountSet(const quint16& qCount)
{
    _ColumnCount = qCount;
}

void RaptorSettingViewModel::invokeItemAppend(const RaptorSettingItem& item)
{
    beginInsertRows(QModelIndex(), _Items.length(), _Items.length());
    _Items << item;
    endInsertRows();
}

void RaptorSettingViewModel::invokeItemsAppend(const QVector<RaptorSettingItem>& items)
{
    if (items.empty())
    {
        return;
    }

    beginInsertRows(QModelIndex(), _Items.length(), _Items.length() + items.length() - 1);
    _Items << items;
    endInsertRows();
}

void RaptorSettingViewModel::invokeItemsClear()
{
    beginResetModel();
    _Items.clear();
    endResetModel();
}

QVector<RaptorSettingItem> RaptorSettingViewModel::invokeItemsEject()
{
    return _Items;
}
