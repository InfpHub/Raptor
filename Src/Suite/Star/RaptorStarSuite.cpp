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

#include "RaptorStarSuite.h"

Q_GLOBAL_STATIC(RaptorStarSuite, _StarSuite)

RaptorStarSuite* RaptorStarSuite::invokeSingletonGet()
{
    return _StarSuite();
}

void RaptorStarSuite::onItemsFetching(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v2/file/list_by_custom_index_key";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["custom_index_key"] = "starred_yes";
    qRow["limit"] = 100;
    qRow["marker"] = input._Marker;
    qRow["category"] = input._Category;
    qRow["type"] = input._Type;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = true;
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

    auto items = QVector<RaptorStarItem>();
    const auto itens = qDocument["items"].toArray();
    const auto qMarker = qDocument["next_marker"].toString();
    for (auto iten : itens)
    {
        auto item = RaptorStarItem();
        item._Id = iten["file_id"].toString();
        item._Name = iten["name"].toString();
        if (iten["type"].toString() == "file")
        {
            item._Size = RaptorUtil::invokeStorageUnitConvert(iten["size"].toVariant().toULongLong());
            item._Icon = RaptorUtil::invokeIconMatch(item._Name);
        }
        else
        {
            item._Icon = RaptorUtil::invokeIconMatch(QString{}, true);
        }

        auto qCreated = QDateTime::fromString(iten["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qCreated.setTimeSpec(Qt::UTC);
        item._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qUpdated = QDateTime::fromString(iten["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qUpdated.setTimeSpec(Qt::UTC);
        item._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        items << item;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<QVector<RaptorStarItem>, QString>>(qMakePair(items, qMarker));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorStarSuite::onItemsByConditionFetching(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v3/file/search";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qCondition = QStringLiteral(R"(name match "%1" and starred match "true")").arg(input._Name);
    if (!input._Category.isEmpty() && !input._Type.isEmpty())
    {
        qCondition = QStringLiteral(R"(name match "%1" and type = "%2" and category match "%3" and starred match "true")").arg(input._Name, input._Type, input._Category);
    }
    else if (!input._Type.isEmpty())
    {
        qCondition = QStringLiteral(R"(name match "%1" and type = "%2" and starred match "true")").arg(input._Name, input._Type);
    }
    else if (!input._Category.isEmpty())
    {
        qCondition = QStringLiteral(R"(name match "%1" and category match "%2" and starred match "true")").arg(input._Name, input._Category);
    }

    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["query"] = qCondition;
    qRow["marker"] = input._Marker;
    qRow["total_count"] = input._Type;
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
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto items = QVector<RaptorStarItem>();
    const auto itens = qDocument["items"].toArray();
    const auto qMarker = qDocument["marker"].toString();
    for (auto iten : itens)
    {
        auto item = RaptorStarItem();
        item._Id = iten["file_id"].toString();
        item._Name = iten["name"].toString();
        if (iten["type"].toString() == "file")
        {
            item._Size = RaptorUtil::invokeStorageUnitConvert(iten["size"].toVariant().toULongLong());
            item._Icon = RaptorUtil::invokeIconMatch(item._Name);
        }
        else
        {
            item._Icon = RaptorUtil::invokeIconMatch(QString{}, true);
        }

        auto qCreated = QDateTime::fromString(iten["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qCreated.setTimeSpec(Qt::UTC);
        item._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qUpdated = QDateTime::fromString(iten["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qUpdated.setTimeSpec(Qt::UTC);
        item._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        items << item;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<QVector<RaptorStarItem>, QString>>(qMakePair(items, qMarker));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorStarSuite::onItemsStarring(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qArray = QJsonArray();
    for (auto& qIndex : qAsConst(input._Indexes))
    {
        auto qId = QString();
        if (qIndex.data(Qt::UserRole).canConvert<RaptorFileItem>())
        {
            qId = qIndex.data(Qt::UserRole).value<RaptorFileItem>()._Id;
        }
        else if (qIndex.data(Qt::UserRole).canConvert<RaptorStarItem>())
        {
            qId = qIndex.data(Qt::UserRole).value<RaptorStarItem>()._Id;
        }

        auto qBody = QJsonObject();
        qBody["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
        qBody["file_id"] = qId;
        qBody["starred"] = input._State;
        qBody["custom_index_key"] = input._State ? "starred_yes" : "starred_no";

        auto qHeader = QJsonObject();
        qHeader["Content-Type"] = RaptorHttpHeader::ApplicationJson;

        auto qRow = QJsonObject();
        qRow["body"] = qBody;
        qRow["headers"] = qHeader;
        qRow["id"] = qId;
        qRow["method"] = "POST";
        qRow["url"] = "/file/update";
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
        Q_EMIT itemsStarred(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsStarred(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<QModelIndexList, bool>>(qMakePair(input._Indexes, input._State));
    Q_EMIT itemsStarred(QVariant::fromValue<RaptorOutput>(output));
}
