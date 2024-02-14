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

#include "RaptorTrashSuite.h"

Q_GLOBAL_STATIC(RaptorTrashSuite, _TrashSuite)

RaptorTrashSuite* RaptorTrashSuite::invokeSingletonGet()
{
    return _TrashSuite();
}

void RaptorTrashSuite::onItemsFetching(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v2/recyclebin/list";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["limit"] = 100;
    qRow["marker"] = input._Marker;
    qRow["order_by"] = "name";
    qRow["order_direction"] = "DESC";
    qRow["marker"] = input._Marker;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
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
        output._State = true;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto items = QVector<RaptorTrashItem>();
    const auto itens = qDocument["items"].toArray();
    const auto qMarker = qDocument["next_marker"].toString();
    for (auto iten : qAsConst(itens))
    {
        auto item = RaptorTrashItem();
        item._Id = iten["file_id"].toString();
        item._Parent = iten["parent_file_id"].toString();
        item._Name = iten["name"].toString();
        item._Space = iten["drive_id"].toString();
        item._Domain = iten["domain_id"].toString();
        item._Extension = iten["file_extension"].toString();
        item._Type = iten["type"].toString();
        item._SHA1 = iten["content_hash"].toString();
        if (iten["type"].toString() == "file")
        {
            item._Size = RaptorUtil::invokeStorageUnitConvert(iten["size"].toVariant().toULongLong());
            item._Icon = RaptorUtil::invokeIconMatch(item._Name);
        }
        else
        {
            item._Icon = RaptorUtil::invokeIconMatch(QString{}, true);
        }

        item._Url = iten["url"].toString();
        auto qCreated = QDateTime::fromString(iten["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qCreated.setTimeSpec(Qt::UTC);
        item._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qUpdated = QDateTime::fromString(iten["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qUpdated.setTimeSpec(Qt::UTC);
        item._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qTrashed = QDateTime::fromString(iten["trashed_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qTrashed.setTimeSpec(Qt::UTC);
        item._Trashed = qTrashed.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        items << item;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<QVector<RaptorTrashItem>, QString>>(qMakePair(items, qMarker));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorTrashSuite::onItemsTrashing(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qArray = QJsonArray();
    for (auto& qIndex : qAsConst(input._Indexes))
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        auto qBody = QJsonObject();
        qBody["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
        qBody["file_id"] = item._Id;

        auto qHeader = QJsonObject();
        qHeader["Content-Type"] = RaptorHttpHeader::ApplicationJson;

        auto qRow = QJsonObject();
        qRow["body"] = qBody;
        qRow["headers"] = qHeader;
        qRow["id"] = item._Id;
        qRow["method"] = "POST";
        qRow["url"] = "/recyclebin/trash";
        qArray << qRow;
    }

    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v3/batch";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["requests"] = qArray;
    qRow["resource"] = "file";
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsTrashed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsTrashed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QModelIndexList>(input._Indexes);
    Q_EMIT itemsTrashed(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorTrashSuite::onItemsRestoring(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qArray = QJsonArray();
    for (auto& qIndex : qAsConst(input._Indexes))
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorTrashItem>();
        auto qBody = QJsonObject();
        qBody["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
        qBody["file_id"] = item._Id;

        auto qHeader = QJsonObject();
        qHeader["Content-Type"] = RaptorHttpHeader::ApplicationJson;

        auto qRow = QJsonObject();
        qRow["body"] = qBody;
        qRow["header"] = qHeader;
        qRow["id"] = item._Id;
        qRow["method"] = "POST";
        qRow["url"] = "/recyclebin/restore";
        qArray << qRow;
    }

    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v3/batch";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["requests"] = qArray;
    qRow["resource"] = "file";
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsRestored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsRestored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QModelIndexList>(input._Indexes);
    Q_EMIT itemsRestored(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorTrashSuite::onItemsDeleting(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qArray = QJsonArray();
    for (auto& qIndex : qAsConst(input._Indexes))
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorTrashItem>();
        auto qBody = QJsonObject();
        qBody["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
        qBody["file_id"] = item._Id;

        auto qHeader = QJsonObject();
        qHeader["Content-Type"] = RaptorHttpHeader::ApplicationJson;

        auto qRow = QJsonObject();
        qRow["body"] = qBody;
        qRow["headers"] = qHeader;
        qRow["id"] = item._Id;
        qRow["method"] = "POST";
        qRow["url"] = "/file/delete";
        qArray << qRow;
    }

    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v3/batch";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["requests"] = qArray;
    qRow["resource"] = "file";
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsDeleted(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsDeleted(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QModelIndexList>(input._Indexes);
    Q_EMIT itemsDeleted(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorTrashSuite::onItemsClearing() const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v2/recyclebin/clear";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsCleared(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::Accepted)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsCleared(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    Q_EMIT itemsCleared(QVariant::fromValue<RaptorOutput>(output));
}