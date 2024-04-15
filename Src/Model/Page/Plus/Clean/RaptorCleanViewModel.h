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

#ifndef RAPTORCLEANVIEWMODEL_H
#define RAPTORCLEANVIEWMODEL_H

#include <QAbstractTableModel>
#include <QCollator>

#include "../../../../Common/RaptorDeclare.h"

class RaptorCleanViewModel Q_DECL_FINAL : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RaptorCleanViewModel(QObject *qParent = Q_NULLPTR);

    [[nodiscard]]
    QVariant headerData(int qSection,
                        Qt::Orientation qOrientation,
                        int qRole = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    [[nodiscard]]
    int rowCount(const QModelIndex &qIndex = QModelIndex()) const Q_DECL_OVERRIDE;

    [[nodiscard]]
    int columnCount(const QModelIndex &qIndex = QModelIndex()) const Q_DECL_OVERRIDE;

    [[nodiscard]]
    QVariant data(const QModelIndex &qIndex, int qRole = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    bool removeRow(int qRow, const QModelIndex &parent = QModelIndex());

    bool removeRows(int qRow,
                    int qCount,
                    const QModelIndex &qIndex = QModelIndex()) Q_DECL_OVERRIDE;

    void sort(int qColumn, Qt::SortOrder qOrder) Q_DECL_OVERRIDE;

    void invokeHeaderSet(const QVector<QString> &qHeader);

    void invokeColumnCountSet(const quint16 &qCount);

    void invokeItemAppend(const RaptorFileItem &item);

    void invokeItemsAppend(const QVector<RaptorFileItem> &items);

    void invokeItemsClear();

    QVector<RaptorFileItem> invokeItemsEject();

private:
    void invokeInstanceInit();

    [[nodiscard]]
    bool invokeItemsByNameAscSort(const RaptorFileItem &item, const RaptorFileItem &iten) const;

    [[nodiscard]]
    bool invokeItemsByNameDescSort(const RaptorFileItem &item, const RaptorFileItem &iten) const;

    [[nodiscard]]
    bool invokeItemsByDirAscSort(const RaptorFileItem &item, const RaptorFileItem &iten) const;

    [[nodiscard]]
    bool invokeItemsByDirDescSort(const RaptorFileItem &item, const RaptorFileItem &iten) const;

    [[nodiscard]]
    bool invokeItemsBySizeAscSort(const RaptorFileItem &item, const RaptorFileItem &iten) const;

    [[nodiscard]]
    bool invokeItemsBySizeDescSort(const RaptorFileItem &item, const RaptorFileItem &iten) const;

Q_SIGNALS:
    Q_SIGNAL void itemEdited(const QVariant &qVariant);

private:
    QVector<QString> _Headers;
    quint16 _ColumnCount;
    QVector<RaptorFileItem> _Items;
    QCollator qCollator;
};

#endif // RAPTORCLEANVIEWMODEL_H
