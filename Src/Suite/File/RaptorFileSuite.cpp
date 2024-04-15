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

RaptorFileSuite *RaptorFileSuite::invokeSingletonGet()
{
    return _FileSuite();
}

QVector<RaptorFileItem> RaptorFileSuite::invokeMapToItems(const QJsonArray &qArray)
{
    auto itens = QVector<RaptorFileItem>();
    for (const auto &item: qArray)
    {
        const auto iten = item.toObject();
        if (!iten["status"].isUndefined() && iten["status"].toString() != "available")
        {
            continue;
        }

        auto iteo = RaptorFileItem();
        iteo._Id = iten["file_id"].toString();
        iteo._Parent = iten["parent_file_id"].toString();
        iteo._Name = iten["name"].toString();
        if (!iten["location"].isUndefined())
        {
            iteo._Dir = iten["location"].toString();
        }

        iteo._Space = iten["drive_id"].toString();
        iteo._Domain = iten["domain_id"].toString();
        iteo._Extension = iten["file_extension"].toString();
        iteo._Type = iten["type"].toString();
        iteo._SHA1 = iten["content_hash"].toString();
        iteo._Url = iten["url"].toString();
        iteo._Mime = iten["mime_type"].toString();
        if (iten["type"].toString() == "file")
        {
            const auto qSize = iten["size"].toVariant().toULongLong();
            iteo._Byte = qSize;
            iteo._Size = RaptorUtil::invokeStorageUnitConvert(qSize);
            iteo._Icon = RaptorUtil::invokeIconMatch(iteo._Name);
        } else
        {
            iteo._Icon = RaptorUtil::invokeIconMatch(QString(), true);
        }

        auto qCreated = QDateTime::fromString(iten["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qCreated.setTimeSpec(Qt::UTC);
        iteo._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qUpdated = QDateTime::fromString(iten["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qUpdated.setTimeSpec(Qt::UTC);
        iteo._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        itens << iteo;
    }

    return itens;
}

QPair<QString, QString> RaptorFileSuite::invokeItemUrlFetch(const RaptorAuthenticationItem &item,
                                                            const QString &qId)
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/v2/file/get_download_url";
    qUseHeaderXCanary(qHttpPayload)
    qUseHeaderReferer(qHttpPayload)
    qUseHeaderCustomXDeviceId(qHttpPayload, item._Session._Device)
    qUseHeaderCustomXSignature(qHttpPayload, item._Session._Signature)
    qUseHeaderCustomAuthorization(qHttpPayload, item._AccessToken)
    qUseHeaderApplicationJson(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = item._Space;
    qRow["file_id"] = qId;
    qRow["expire_sec"] = 115200;
    qHttpPayload._Body = QJsonDocument(qRow);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qMakePair(qError, QString());
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return qMakePair(QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString()), QString());
    }

    const auto qUrl = qDocument["url"].toString();
    if (qUrl.isEmpty())
    {
        const auto qErros = QStringLiteral("地址刷新失败，可能文件违规: %1。").arg(qId);
        qCritical() << qErros;
        return qMakePair(qErros, QString());
    }

    return qMakePair(QString(), qUrl);
}

void RaptorFileSuite::onItemsByParentIdFetching(const QVariant &qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v4/file/list";
    qUseHeaderDefault(qHttpPayload)
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
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem> > >(qMakePair(input, items));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemsByIdFetching(const QVariant &qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v1/file/get_path";
    qUseHeaderDefault(qHttpPayload)
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

void RaptorFileSuite::onItemsByConditionFetching(const QVariant &qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v3/file/search";
    qUseHeaderDefault(qHttpPayload)
    auto qCondition = QStringLiteral(R"(name match "%1")").arg(input._Name);
    if (!input._Category.isEmpty() && !input._Type.isEmpty())
    {
        qCondition = QStringLiteral(R"(name match "%1" and type = "%2" and category match "%3")").arg(
            input._Name, input._Type, input._Category);
    } else if (!input._Type.isEmpty())
    {
        qCondition = QStringLiteral(R"(name match "%1" and type = "%2")").arg(input._Name, input._Type);
    } else if (!input._Category.isEmpty())
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
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem> > >(qMakePair(input, items));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemCreating(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto items = input._Name.split("/");
    auto qParent = input._Parent;
    auto output = RaptorOutput();
    for (auto &item: items)
    {
        auto qHttpPayload = RaptorHttpPayload();
        qHttpPayload._Url = "https://api.alipan.com/adrive/v2/file/createWithFolders";
        qUseHeaderDefault(qHttpPayload)
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
    output._Data = QVariant::fromValue<QPair<quint8, QString> >(qMakePair(items.length(), input._Parent));
    Q_EMIT itemCreated(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemsRenaming(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto items = input._Variant.value<QVector<RaptorFileItem> >();
    auto qArray = QJsonArray();
    for (auto &item: items)
    {
        auto qBody = QJsonObject();
        qBody["check_name_mode"] = "refuse";
        qBody["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
        qBody["file_id"] = item._Id;
        qBody["name"] = item._Name;

        auto qHeader = QJsonObject();
        qHeader["Content-Type"] = RaptorHttpHeader::ApplicationJson;

        auto qRow = QJsonObject();
        qRow["body"] = qBody;
        qRow["headers"] = qHeader;
        qRow["id"] = item._Id;
        qRow["method"] = "POST";
        qRow["url"] = "/file/update";
        qArray << qRow;
    }

    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v4/batch";
    qUseHeaderDefault(qHttpPayload)
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
        Q_EMIT itemsRenamed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsRenamed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<QVector<RaptorFileItem>, QString> >(qMakePair(items, input._Parent));
    Q_EMIT itemsRenamed(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemUrlFetching(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto item = input._Index.data(Qt::UserRole).value<RaptorFileItem>();
    if (const auto [qError, qUrl] = invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), item._Id);
        qError.isEmpty())
    {
        item._Url = qUrl;
        auto output = RaptorOutput();
        output._State = true;
        output._Data = QVariant::fromValue<QPair<QModelIndex, RaptorFileItem> >(qMakePair(input._Index, item));
        Q_EMIT itemUrlFetched(QVariant::fromValue<RaptorOutput>(output));
    } else
    {
        auto output = RaptorOutput();
        output._State = false;
        output._Message = qError;
        Q_EMIT itemUrlFetched(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorFileSuite::onItemsCopying(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qArray = QJsonArray();
    const auto items = input._Variant.value<QVector<RaptorFileItem> >();
    for (auto &item: items)
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
    qHttpPayload._Url = "https://api.alipan.com/adrive/v4/batch";
    qUseHeaderDefault(qHttpPayload)
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

void RaptorFileSuite::onItemsMoving(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qArray = QJsonArray();
    const auto items = input._Variant.value<QVector<RaptorFileItem> >();
    for (auto &item: items)
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
    qHttpPayload._Url = "https://api.alipan.com/adrive/v4/batch";
    qUseHeaderDefault(qHttpPayload)
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

void RaptorFileSuite::onItemsDeleting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto [qError, qIndexList] = RaptorTrashSuite::invokeItemsDelete(input._Indexes);
    auto output = RaptorOutput();
    output._State = qError.isEmpty();
    output._Message = qError;
    output._Data = QVariant::fromValue<QModelIndexList>(qIndexList);
    Q_EMIT itemsDeleted(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorFileSuite::onItemVideoPreviewPlayInfoFetching(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto item = input._Index.data(Qt::UserRole).value<RaptorFileItem>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/v2/file/get_video_preview_play_info";
    qUseHeaderDefault(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["file_id"] = item._Id;
    if (const auto qStream = RaptorSettingSuite::invokeItemFind(Setting::Section::Video,
                                                                Setting::Video::Stream).toString();
        !qStream.isEmpty())
    {
        if (Setting::Video::Quick == qStream)
        {
            qRow["category"] = "quick_video";
        } else if (Setting::Video::Live == qStream)
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
        Q_EMIT itemVideoPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus == RaptorHttpStatus::OK)
    {
        const auto items = qDocument["video_preview_play_info"]["live_transcoding_task_list"].toArray();
        auto qUrl = QString();
        for (const auto &iten: items)
        {
            const auto iteo = iten.toObject();
            if (const auto qTemplateId = iteo["template_id"].toString();
                input._Id == qTemplateId)
            {
                qUrl = iteo["url"].toString();
                break;
            }
        }

        if (qUrl.isEmpty())
        {
            const auto [qError, qUrm] = invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), item._Id);
            if (!qError.isEmpty())
            {
                output._State = false;
                output._Message = qError;
                Q_EMIT itemVideoPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
                return;
            }

            qUrl = qUrm;
        }

        output._State = true;
        output._Data = QVariant::fromValue<QPair<RaptorFileItem, QString> >(qMakePair(item, qUrl));
        Q_EMIT itemVideoPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
    } else
    {
        if (const auto qUseOrigin = RaptorSettingSuite::invokeItemFind(Setting::Section::Video,
                                                                       Setting::Video::UseOrigin).toBool();
            qUseOrigin)
        {
            const auto [qError, qUrl] = invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), item._Id);
            if (!qError.isEmpty())
            {
                output._State = false;
                output._Message = qError;
                Q_EMIT itemVideoPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
                return;
            }

            output._State = true;
            output._Data = QVariant::fromValue<QPair<RaptorFileItem, QString> >(qMakePair(item, qUrl));
            Q_EMIT itemVideoPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemVideoPreviewPlayInfoFetched(QVariant::fromValue<RaptorOutput>(output));
    }
}
