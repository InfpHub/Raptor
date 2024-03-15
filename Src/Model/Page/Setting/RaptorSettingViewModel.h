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

#ifndef RAPTORSETTINGVIEWMODEL_H
#define RAPTORSETTINGVIEWMODEL_H

#include <QAbstractTableModel>

#include "../../../Util/RaptorUtil.h"

class RaptorSettingViewModel Q_DECL_FINAL : public QAbstractTableModel
{
    Q_OBJECT

public:
    using QAbstractTableModel::QAbstractTableModel;

    QVariant headerData(int qSection,
                        Qt::Orientation qOrientation,
                        int qRole = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    [[nodiscard]]
    int rowCount(const QModelIndex& qIndex = QModelIndex()) const Q_DECL_OVERRIDE;

    [[nodiscard]]
    int columnCount(const QModelIndex& qIndex = QModelIndex()) const Q_DECL_OVERRIDE;

    [[nodiscard]]
    QVariant data(const QModelIndex& qIndex, int qRole = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    bool setData(const QModelIndex& qIndex,
                 const QVariant& qVariant,
                 int qRole = Qt::EditRole) Q_DECL_OVERRIDE;

    bool removeRow(int qRow, const QModelIndex& qIndex = QModelIndex());

    [[nodiscard]]
    Qt::ItemFlags flags(const QModelIndex& qIndex) const Q_DECL_OVERRIDE;

    void invokeHeaderSet(const QVector<QString>& qHeader);

    void invokeColumnCountSet(const quint16& qCount);

    void invokeItemAppend(const RaptorSettingItem& item);

    void invokeItemsAppend(const QVector<RaptorSettingItem>& items);

    void invokeItemsClear();

    QVector<RaptorSettingItem> invokeItemsEject();

Q_SIGNALS:
    Q_SIGNAL void itemEdited(const QVariant& qVariant);

private:
    QVector<QString> _Headers;
    quint16 _ColumnCount;
    QVector<RaptorSettingItem> _Items;
};

#endif // RAPTORSETTINGVIEWMODEL_H