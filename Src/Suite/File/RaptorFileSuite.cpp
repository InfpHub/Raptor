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

#include "RaptorFileSuite.h"

Q_GLOBAL_STATIC(RaptorFileSuite, _FileSuite)

RaptorFileSuite* RaptorFileSuite::invokeSingletonGet()
{
    return _FileSuite();
}

QVector<RaptorFileItem> RaptorFileSuite::invokeMapToItems(const QJsonArray& qArray)
{
    auto itens = QVector<RaptorFileItem>();
    for (auto item : qArray)
    {
        if (item["status"].toString() != "available")
        {
            continue;
        }

        auto iten = RaptorFileItem();
        iten._Id = item["file_id"].toString();
        iten._Parent = item["parent_file_id"].toString();
        iten._Name = item["name"].toString();
        iten._Space = item["drive_id"].toString();
        iten._Domain = item["domain_id"].toString();
        iten._Extension = item["file_extension"].toString();
        iten._Type = item["type"].toString();
        iten._SHA1 = item["content_hash"].toString();
        iten._Url = item["url"].toString();
        iten._Starred = item["starred"].toBool();
        iten._Mime = item["mime_type"].toString();
        if (item["type"].toString() == "file")
        {
            const auto size = item["size"].toVariant().toULongLong();
            iten._Byte = size;
            iten._Size = RaptorUtil::invokeStorageUnitConvert(size);
            iten._Icon = RaptorUtil::invokeIconMatch(iten._Name);
        }
        else
        {
            iten._Icon = RaptorUtil::invokeIconMatch(QString(), true);
        }

        auto qCreated = QDateTime::fromString(item["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qCreated.setTimeSpec(Qt::UTC);
        iten._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qUpdated = QDateTime::fromString(item["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qUpdated.setTimeSpec(Qt::UTC);
        iten._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        itens << iten;
    }

    return itens;
}

RaptorOutput RaptorFileSuite::invokeItemUrlFetch(const RaptorAuthenticationItem& item,
                                                 const QString& qId)
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v2/file/get_download_url";
    USE_HEADER_X_CANARY(qHttpPayload)
    USE_HEADER_REFERER(qHttpPayload)
    USE_HEADER_CUSTOM_X_DEVICE_ID(qHttpPayload, item._Session._Device)
    USE_HEADER_CUSTOM_X_SIGNATURE(qHttpPayload, item._Session._Signature)
    USE_HEADER_CUSTOM_AUTHORIZATION(qHttpPayload, item._AccessToken)
    USE_HEADER_APPLICATION_JSON(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = item._Space;
    qRow["file_id"] = qId;
    qRow["expire_sec"] = 115200;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        return output;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        return output;
    }

    const auto qUrl = qDocument["url"].toString();
    if (qUrl.isEmpty())
    {
        const auto qErros = QStringLiteral("地址刷新失败，可能文件违规: %1。").arg(qId);
        qCritical() << qErros;
        output._State = false;
        output._Message = qErros;
        return output;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QString>(qUrl);
    return output;
}

void RaptorFileSuite::onItemsByParentIdFetching(const QVariant& qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v3/file/list";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["parent_file_id"] = input._Parent;
    qRow["limit"] = 200;
    qRow["marker"] = input._Marker;
    qRow["type"] = input._Type;
    qRow["category"] = input._Category;
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
    input._Marker = qDocument["next_marker"].toString();
    const auto items = invokeMapToItems(itens);
    output._State = true;
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem>>>(qMakePair(input, items));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemsByIdFetching(const QVariant& qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v1/file/get_path";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["file_id"] = input._Id;
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

    input._Parent = qDocument["items"][0]["parent_file_id"].toString();
    onItemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorFileSuite::onItemsByConditionFetching(const QVariant& qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v3/file/search";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qCondition = QStringLiteral(R"(name match "%1")").arg(input._Name);
    if (!input._Category.isEmpty() && !input._Type.isEmpty())
    {
        qCondition = QStringLiteral(R"(name match "%1" and type = "%2" and category match "%3")").arg(input._Name, input._Type, input._Category);
    }
    else if (!input._Type.isEmpty())
    {
        qCondition = QStringLiteral(R"(name match "%1" and type = "%2")").arg(input._Name, input._Type);
    }
    else if (!input._Category.isEmpty())
    {
        qCondition = QStringLiteral(R"(name match "%1" and category match "%2")").arg(input._Name, input._Category);
    }

    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["query"] = qCondition;
    qRow["marker"] = input._Marker;
    qRow["limit"] = 100;
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

    const auto itens = qDocument["items"].toArray();
    input._Marker = qDocument["marker"].toString();
    const auto items = invokeMapToItems(itens);
    output._State = true;
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem>>>(qMakePair(input, items));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemCreating(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto items = input._Name.split("/");
    auto qParent = input._Parent;
    auto output = RaptorOutput();
    for (auto& item : items)
    {
        auto qHttpPayload = RaptorHttpPayload();
        qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v2/file/createWithFolders";
        USE_HEADER_DEFAULT(qHttpPayload)
        auto qRow = QJsonObject();
        qRow["check_name_mode"] = input._Type;
        qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
        qRow["parent_file_id"] = qParent;
        qRow["type"] = "folder";
        qRow["name"] = item;
        qHttpPayload._Body = QJsonDocument(qRow);
        const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
        if (!qError.isEmpty())
        {
            qCritical() << qError;
            output._State = false;
            output._Message = qError;
            Q_EMIT itemCreated(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        const auto qDocument = QJsonDocument::fromJson(qBody);
        if (qStatus != RaptorHttpStatus::Created)
        {
            qCritical() << qDocument.toJson();
            output._State = false;
            output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
            Q_EMIT itemCreated(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        qParent = qDocument["file_id"].toString();
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<quint8, QString>>(qMakePair(items.length(), input._Parent));
    Q_EMIT itemCreated(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemRenaming(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto item = input._Index.data(Qt::UserRole).value<RaptorFileItem>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v3/file/update";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["check_name_mode"] = "refuse";
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["file_id"] = item._Id;
    qRow["type"] = item._Type;
    qRow["name"] = input._Name;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemRenamed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemRenamed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<QModelIndex, QString>>(qMakePair(input._Index, input._Name));
    Q_EMIT itemRenamed(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemUrlFetching(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto item = input._Index.data(Qt::UserRole).value<RaptorFileItem>();
    if (const auto [_State, _Message, _Data] = invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), item._Id);
        _State)
    {
        item._Url = _Data.value<QString>();
        auto output = RaptorOutput();
        output._State = true;
        output._Data = QVariant::fromValue<QPair<QModelIndex, RaptorFileItem>>(qMakePair(input._Index, item));
        Q_EMIT itemUrlFetched(QVariant::fromValue<RaptorOutput>(output));
    }
    else
    {
        auto output = RaptorOutput();
        output._State = false;
        output._Message = _Message;
        Q_EMIT itemUrlFetched(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorFileSuite::onItemsMoving(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qArray = QJsonArray();
    const auto items = input._Variant.value<QVector<RaptorFileItem>>();
    for (auto& item : items)
    {
        auto qBody = QJsonObject();
        qBody["drive_id"] = input._Parent;
        qBody["file_id"] = item._Id;
        qBody["to_parent_file_id"] = input._Id;
        qBody["to_drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;

        auto qHeader = QJsonObject();
        qHeader["Content-Type"] = RaptorHttpHeader::ApplicationJson;

        auto qRow = QJsonObject();
        qRow["body"] = qBody;
        qRow["headers"] = qHeader;
        qRow["id"] = item._Id;
        qRow["method"] = "POST";
        qRow["url"] = "/file/move";
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
        Q_EMIT itemsMoved(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsMoved(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QString>(input._Id);
    Q_EMIT itemsMoved(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemsCopying(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qArray = QJsonArray();
    const auto items = input._Variant.value<QVector<RaptorFileItem>>();
    for (auto& item : items)
    {
        auto qBody = QJsonObject();
        qBody["drive_id"] = input._Parent;
        qBody["file_id"] = item._Id;
        qBody["to_parent_file_id"] = input._Id;
        qBody["to_drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
        auto qHeader = QJsonObject();
        qHeader["Content-Type"] = RaptorHttpHeader::ApplicationJson;
        auto qRow = QJsonObject();
        qRow["body"] = qBody;
        qRow["headers"] = qHeader;
        qRow["id"] = item._Id;
        qRow["method"] = "POST";
        qRow["url"] = "/file/copy";
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
        Q_EMIT itemsCopied(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsCopied(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QString>(input._Id);
    Q_EMIT itemsCopied(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemPreviewPlayInfoFetching(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto item = input._Index.data(Qt::UserRole).value<RaptorFileItem>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v2/file/get_video_preview_play_info";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["file_id"] = item._Id;
    if (const auto qStream = RaptorSettingSuite::invokeItemFind(Setting::Section::Play,
                                                                Setting::Play::Stream).toString();
        !qStream.isEmpty())
    {
        if (Setting::Play::Quick == qStream)
        {
            qRow["category"] = "quick_video";
        }
        else if (Setting::Play::Live == qStream)
        {
            qRow["category"] = "live_transcoding";
        }
    }

    qRow["get_subtitle_info"] = true;
    qRow["mode"] = "high_res";
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus == RaptorHttpStatus::OK)
    {
        const auto items = qDocument["video_preview_play_info"]["live_transcoding_task_list"].toArray();
        auto qUrl = QString();
        for (auto iten : items)
        {
            if (const auto qTemplateId = iten["template_id"].toString();
                input._Id == qTemplateId)
            {
                qUrl = iten["url"].toString();
                break;
            }
        }

        if (qUrl.isEmpty())
        {
            const auto [_State, _Message, _Data] = invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), item._Id);
            if (!_State)
            {
                output._State = false;
                output._Message = _Message;
                Q_EMIT itemPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
                return;
            }

            qUrl = _Data.value<QString>();
        }

        output._State = true;
        output._Data = QVariant::fromValue<QString>(qUrl);
        Q_EMIT itemPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
    }
    else
    {
        if (const auto qUserOrigin = RaptorSettingSuite::invokeItemFind(Setting::Section::Play,
                                                                        Setting::Play::UseOrigin).toBool(); qUserOrigin)
        {
            const auto [_State, _Message, _Data] = invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), item._Id);
            if (!_State)
            {
                output._State = false;
                output._Message = _Message;
                Q_EMIT itemPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
                return;
            }

            output._State = true;
            output._Data = _Data;
            Q_EMIT itemPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
    }
}
