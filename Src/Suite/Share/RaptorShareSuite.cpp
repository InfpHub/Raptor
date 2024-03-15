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

#include "RaptorShareSuite.h"

Q_GLOBAL_STATIC(RaptorShareSuite, _ShareSuite)

RaptorShareSuite *RaptorShareSuite::invokeSingletonGet()
{
    return _ShareSuite();
}

std::tuple<QString, RaptorInput, QVector<RaptorFileItem> > RaptorShareSuite::invokeItemsByTokenFetching(const RaptorInput &input)
{
    auto inpus = input;
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v2/file/list_by_share";
    USE_HEADER_DEFAULT(qHttpPayload)
    USE_HEADER_X_SHARE_TOKEN(qHttpPayload, input._Token)
    auto qRow = QJsonObject();
    qRow["limit"] = 100;
    qRow["share_id"] = input._Id;
    qRow["parent_file_id"] = input._Parent;
    qRow["order_by"] = "name";
    qRow["order_direction"] = "DESC";
    qRow["marker"] = input._Marker;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto items = QVector<RaptorFileItem>();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return std::make_tuple(qError, input, items);
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return std::make_tuple(QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString()), input, items);
    }

    const auto itens = qDocument["items"].toArray();
    inpus._Marker = qDocument["next_marker"].toString();
    for (const auto &iten: itens)
    {
        const auto iteo = iten.toObject();
        auto item = RaptorFileItem();
        item._Id = iteo["file_id"].toString();
        item._Parent = iteo["parent_file_id"].toString();
        item._Name = iteo["name"].toString();
        item._Space = iteo["drive_id"].toString();
        item._Domain = iteo["domain_id"].toString();
        item._Extension = iteo["file_extension"].toString();
        item._Type = iteo["type"].toString();
        item._SHA1 = iteo["content_hash"].toString();
        item._Url = iteo["url"].toString();
        if (iteo["type"].toString() == "file")
        {
            const auto qSize = iteo["size"].toVariant().toULongLong();
            item._Byte = qSize;
            item._Size = RaptorUtil::invokeStorageUnitConvert(qSize);
            item._Icon = RaptorUtil::invokeIconMatch("Share", false, true);
        } else
        {
            item._Icon = RaptorUtil::invokeIconMatch(QString(), true);
        }

        auto qCreated = QDateTime::fromString(iteo["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qCreated.setTimeSpec(Qt::UTC);
        item._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qUpdated = QDateTime::fromString(iteo["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qUpdated.setTimeSpec(Qt::UTC);
        item._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        items << item;
    }

    return std::make_tuple(QString(), inpus, items);
}

void RaptorShareSuite::onItemsFetching(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v2/share_link/search";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qCondition = QString();
    if (!input._Name.isEmpty())
    {
        if (!input._Type.isEmpty())
        {
            qCondition = QStringLiteral(R"(status in ["%1"] and share_name_for_fuzzy match "%2")").arg(
                input._Type, input._Name);
        } else
        {
            if (input._Category.isEmpty())
            {
                qCondition = QStringLiteral(R"(share_name_for_fuzzy match "%1" and expired_time < "%2")").arg(
                    input._Name, QDateTime::currentDateTime().toUTC().toString("yyyy-MM-ddTHH:mm:ss"));
            } else
            {
                qCondition = QStringLiteral(
                    R"(share_name_for_fuzzy match "%1" and expired_time = "1970-01-01T00:00:00")").arg(
                    input._Type, input._Name);
            }
        }
    } else
    {
        if (!input._Type.isEmpty())
        {
            qCondition = QStringLiteral(R"(status in ["%1"])").arg(input._Type);
        } else
        {
            if (input._Category.isEmpty())
            {
                qCondition = QStringLiteral(R"(expired_time < "%1")").arg(
                    QDateTime::currentDateTime().toUTC().toString("yyyy-MM-ddTHH:mm:ss"));
            } else
            {
                qCondition = QStringLiteral(R"(expired_time = "1970-01-01T00:00:00")");
            }
        }
    }

    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["query"] = qCondition;
    qRow["limit"] = 100;
    qRow["marker"] = input._Marker;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
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

    auto items = QVector<RaptorShareItem>();
    const auto itens = qDocument["items"].toArray();
    const auto qMarker = qDocument["next_marker"].toString();
    for (const auto &iten: itens)
    {
        const auto iteo = iten.toObject();
        auto item = RaptorShareItem();
        item._Id = iteo["share_id"].toString();
        item._Name = iteo["share_name"].toString();
        item._Description = iteo["description"].toString();
        auto qCreated = QDateTime::fromString(iteo["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qCreated.setTimeSpec(Qt::UTC);
        item._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qUpdated = QDateTime::fromString(iteo["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qUpdated.setTimeSpec(Qt::UTC);
        item._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qExpired = QDateTime::fromString(iteo["expiration"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qExpired.setTimeSpec(Qt::UTC);
        item._Expired = qExpired.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        item._Preview = iteo["preview_count"].toVariant().toUInt();
        item._Save = iteo["save_count"].toVariant().toUInt();
        item._Download = iteo["download_count"].toVariant().toUInt();
        item._Url = iteo["share_url"].toString();
        item._Password = iteo["share_pwd"].toString();
        item._Icon = RaptorUtil::invokeIconMatch("Share", false, true);
        items << item;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<QVector<RaptorShareItem>, QString> >(qMakePair(items, qMarker));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorShareSuite::onItemsExporting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qText = QString();
    const auto qIndexList = input._Indexes;
    for (auto i = 0; i < qIndexList.length(); ++i)
    {
        if (const auto item = qIndexList[i].data(Qt::UserRole).value<RaptorShareItem>();
            item._Password.isEmpty())
        {
            qText.append(QStringLiteral("链接: %1").arg(item._Url));
        } else
        {
            qText.append(QStringLiteral("链接: %1 提取码: %2").arg(item._Url, item._Password));
        }

        if (i != qIndexList.length() - 1)
        {
            qText.append("\n");
        }
    }

    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QString>(qText);
    Q_EMIT itemsExported(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorShareSuite::onItemsCancelling(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto qIndexList = input._Indexes;
    auto qCount = 0;
    auto qMessage = QString();
    for (auto &qIndex: qIndexList)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorShareItem>();
        auto qHttpPayload = RaptorHttpPayload();
        qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v2/share_link/cancel";
        USE_HEADER_DEFAULT(qHttpPayload)
        USE_HEADER_APPLICATION_JSON(qHttpPayload)
        auto qRow = QJsonObject();
        qRow["share_id"] = item._Id;
        qHttpPayload._Body = QJsonDocument(qRow);
        const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
        if (!qError.isEmpty())
        {
            qCritical() << qError;
            continue;
        }

        const auto qDocument = QJsonDocument::fromJson(qBody);
        if (qStatus != RaptorHttpStatus::OK)
        {
            qMessage = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
            qCritical() << qDocument.toJson();
            continue;
        }

        ++qCount;
    }

    if (qCount == 0 || qCount < qIndexList.length())
    {
        auto output = RaptorOutput();
        output._State = false;
        output._Message = QStringLiteral("部分分享取消失败: %1").arg(qMessage);
        Q_EMIT itemsCancelled(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QModelIndexList>(qIndexList);
    Q_EMIT itemsCancelled(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorShareSuite::onItemCreating(const QVariant &qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v2/share_link/create";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["share_name"] = input._Name;
    qRow["description"] = input._Description;
    qRow["preview_limit"] = input._Preview;
    qRow["save_limit"] = input._Save;
    qRow["download_limit"] = input._Download;
    qRow["share_pwd"] = input._Password;
    qRow["expiration"] = input._Expire.isEmpty()
                             ? input._Expire
                             : QDateTime::fromString(input._Expire, "yyyy-MM-dd H:mm").toString(
                                 "yyyy-MM-ddTHH:mm:ss.zzzZ");

    auto qArray = QJsonArray();
    for (const auto &qIndex: input._Indexes)
    {
        qArray << qIndex.data(Qt::UserRole).value<RaptorFileItem>()._Id;
    }

    qRow["file_id_list"] = qArray;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus == RaptorHttpStatus::OK)
    {
        input._Name = qDocument["share_name"].toString();
        input._Description = qDocument["description"].toString();
        input._Url = qDocument["share_url"].toString();
        input._Password = qDocument["share_pwd"].toString();
        output._State = true;
        output._Data = QVariant::fromValue<RaptorInput>(input);
        Q_EMIT itemCreated(QVariant::fromValue<RaptorOutput>(output));
    } else if (qStatus == RaptorHttpStatus::Forbidden)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = qDocument["display_message"].toString();
        Q_EMIT itemCreated(QVariant::fromValue<RaptorOutput>(output));
    } else
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemCreated(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorShareSuite::onItemsRapidCreating(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto qIndexList = input._Indexes;
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v1/share/create";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    auto qArray = QJsonArray();
    for (auto &qIndex: qIndexList)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        auto qDocument = QJsonObject();
        qDocument["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
        qDocument["file_id"] = item._Id;
        qArray << qDocument;
    }

    qRow["drive_file_list"] = qArray;
    auto output = RaptorOutput();
    qHttpPayload._Body = QJsonDocument(qRow);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost((qHttpPayload));
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsRapidCreated(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsRapidCreated(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QString>(qDocument["share_url"].toString());
    Q_EMIT itemsRapidCreated(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorShareSuite::onItemsRapidFetching() const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v1/share/list";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["limit"] = 100;
    qRow["order_by"] = "browse_count";
    qRow["order_direction"] = "DESC";
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost((qHttpPayload));
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

    auto items = QVector<RaptorShareItem>();
    const auto itens = qDocument["items"].toArray();
    for (const auto &iten: itens)
    {
        const auto iteo = iten.toObject();
        auto item = RaptorShareItem();
        item._Id = iteo["share_id"].toString();
        item._Name = iteo["share_name"].toString();
        auto qCreated = QDateTime::fromString(iteo["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qCreated.setTimeSpec(Qt::UTC);
        item._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qUpdated = QDateTime::fromString(iteo["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qUpdated.setTimeSpec(Qt::UTC);
        item._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        auto qExpired = QDateTime::fromString(iteo["expiration"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
        qExpired.setTimeSpec(Qt::UTC);
        item._Expired = qExpired.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
        item._Url = iteo["share_url"].toString();
        item._Icon = RaptorUtil::invokeIconMatch("Share", false, true);
        items << item;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<QVector<RaptorShareItem>, QString> >(qMakePair(items, QString{}));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorShareSuite::onItemRapidImporting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto qId = input._Link.right(20);
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v1/share/getShareToken";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["share_id"] = qId;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost((qHttpPayload));
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsRapidImported(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsRapidImported(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto qHttpPayloae = RaptorHttpPayload();
    qHttpPayloae._Url = "https://api.aliyundrive.com/adrive/v1/share/saveFile";
    USE_HEADER_DEFAULT(qHttpPayloae)
    USE_HEADER_X_SHARE_TOKEN(qHttpPayloae, qDocument["share_token"].toString())
    auto qJsoo = QJsonObject();
    qJsoo["share_id"] = qId;
    qJsoo["to_drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qJsoo["to_parent_file_id"] = input._Id;
    qHttpPayloae._Body = QJsonDocument(qJsoo);
    const auto [qErros, qStatut, qBodz] = RaptorHttpSuite::invokePost((qHttpPayloae));
    if (!qErros.isEmpty())
    {
        qCritical() << qErros;
        output._State = false;
        output._Message = qErros;
        Q_EMIT itemsRapidImported(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocumenu = QJsonDocument::fromJson(qBodz);
    if (qStatut != RaptorHttpStatus::OK)
    {
        qCritical() << qDocumenu.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocumenu["code"].toString(), qDocumenu["message"].toString());
        Q_EMIT itemsRapidImported(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    Q_EMIT itemsRapidImported(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorShareSuite::onItemParsing(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto qId = input._Link.right(11);
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v3/share_link/get_share_by_anonymous";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["share_id"] = qId;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost((qHttpPayload));
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemParsed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }
    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemParsed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (qDocument["has_pwd"].toBool() && input._Password.isEmpty())
    {
        const auto qErros = QStringLiteral("此分享需要提取码!");
        qCritical() << qErros;
        output._State = false;
        output._Message = qErros;
        Q_EMIT itemParsed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto qHttpPayloae = RaptorHttpPayload();
    qHttpPayloae._Url = "https://api.aliyundrive.com/v2/share_link/get_share_token";
    USE_HEADER_DEFAULT(qHttpPayloae)
    auto qJsoo = QJsonObject();
    qJsoo["share_id"] = qId;
    qJsoo["share_pwd"] = input._Password;
    qHttpPayloae._Body = QJsonDocument(qJsoo);
    const auto [qErros, qStatut, qBodz] = RaptorHttpSuite::invokePost(qHttpPayloae);
    if (!qErros.isEmpty())
    {
        qCritical() << qErros;
        output._State = false;
        output._Message = qErros;
        Q_EMIT itemParsed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocumenu = QJsonDocument::fromJson(qBodz);
    if (qStatut != RaptorHttpStatus::OK)
    {
        qCritical() << qDocumenu.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocumenu["code"].toString(), qDocumenu["message"].toString());
        Q_EMIT itemParsed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto inpus = RaptorInput();
    inpus._Id = qId;
    inpus._Token = qDocumenu["share_token"].toString();
    inpus._Parent = input._Parent.isEmpty() ? "root" : input._Parent;
    const auto [qErrot, inpuu, items] = invokeItemsByTokenFetching(inpus);
    if (!qErrot.isEmpty())
    {
        output._State = false;
        output._Message = qErrot;
        Q_EMIT itemParsed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem> > >(qMakePair(inpuu, items));
    Q_EMIT itemParsed(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorShareSuite::onItemsByParentIdFetching(const QVariant &qVariant) const
{
    const auto [qError, input, items] = invokeItemsByTokenFetching(qVariant.value<RaptorInput>());
    auto output = RaptorOutput();
    output._State = qError.isEmpty();
    output._Message = qError;
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem> > >(qMakePair(input, items));
    Q_EMIT itemParsed(QVariant::fromValue<>(output));
}

void RaptorShareSuite::onItemImporting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto qId = input._Link.right(11);
    auto qArray = QJsonArray();
    const auto qIndexList = input._Indexes;
    for (auto i = 0; i < qIndexList.length(); ++i)
    {
        const auto item = qIndexList[i].data(Qt::UserRole).value<RaptorFileItem>();
        auto qBody = QJsonObject();
        qBody["file_id"] = item._Id;
        qBody["share_id"] = qId;
        qBody["auto_rename"] = true;
        qBody["to_parent_file_id"] = input._Id;
        qBody["to_drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;

        auto qHeader = QJsonObject();
        qHeader["Content-Type"] = RaptorHttpHeader::ApplicationJson;

        auto qRow = QJsonObject();
        qRow["body"] = qBody;
        qRow["headers"] = qHeader;
        qRow["id"] = i;
        qRow["method"] = "POST";
        qRow["url"] = "/file/copy";
        qArray << qRow;
    }

    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v4/batch";
    USE_HEADER_DEFAULT(qHttpPayload)
    USE_HEADER_X_SHARE_TOKEN(qHttpPayload, input._Token)
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
        Q_EMIT itemsImported(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsImported(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    Q_EMIT itemsImported(QVariant::fromValue<RaptorOutput>(output));
}
