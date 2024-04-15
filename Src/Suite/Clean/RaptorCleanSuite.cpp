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

#include "RaptorCleanSuite.h"

Q_GLOBAL_STATIC(RaptorCleanSuite, _CleanSuite)

RaptorCleanSuite *RaptorCleanSuite::invokeSingletonGet()
{
    return _CleanSuite();
}

void RaptorCleanSuite::onItemsFetching(const QVariant &qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v1/file/listToClean";
    qUseHeaderDefault(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["limit"] = 200;
    qRow["marker"] = input._Marker;
    qHttpPayload._Body = QJsonDocument(qRow);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    auto output = RaptorOutput();
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto itens = qDocument["items"].toArray();
    const auto items = RaptorFileSuite::invokeMapToItems(itens);
    output._State = true;
    output._Data = QVariant::fromValue<QPair<QVector<RaptorFileItem>, QString> >(qMakePair(items, qDocument["next_marker"].toString()));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorCleanSuite::onItemsCleaning(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto [qError, qIndexList] = RaptorTrashSuite::invokeItemsTrash(input._Indexes);
    auto output = RaptorOutput();
    output._State = qError.isEmpty();
    output._Message = qError;
    output._Data = QVariant::fromValue<QModelIndexList>(qIndexList);
    Q_EMIT itemsCleaned(QVariant::fromValue<RaptorOutput>(output));
}
