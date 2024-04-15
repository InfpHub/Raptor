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

#include "RaptorMediaSuite.h"

Q_GLOBAL_STATIC(RaptorMediaSuite, _MediaSuite)

RaptorMediaSuite *RaptorMediaSuite::invokeSingletonGet()
{
    return _MediaSuite();
}

QVector<RaptorMediaItem> RaptorMediaSuite::invokeMapToItems(const QJsonArray &qArray)
{
    auto itens = QVector<RaptorMediaItem>();
    std::for_each(qArray.begin(), qArray.end(), [&](const QJsonValueConstRef &item) -> void
    {
        const auto iten = item.toObject();
        auto iteo = RaptorMediaItem();
        if (iten["category"].toString().isEmpty())
        {
            iteo._CompilationId = iten["compilation_id"].toString();
            iteo._Name = iten["name"].toString();
            iteo._Id = iten["default_file_id"].toString();
            iteo._Thumbnail = iten["thumbnail"].toString();
            iteo._Number = iten["video_nums"].toInt();
            iteo._Icon = RaptorUtil::invokeIconMatch(QString(), true);
            auto qCreated = QDateTime::fromString(iten["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
            qCreated.setTimeSpec(Qt::UTC);
            iteo._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
            auto qUpdated = QDateTime::fromString(iten["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
            qUpdated.setTimeSpec(Qt::UTC);
            iteo._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
            itens << iteo;
        }
    });

    std::for_each(qArray.begin(), qArray.end(), [&](const QJsonValueConstRef &item) -> void
    {
        const auto iten = item.toObject();
        auto iteo = RaptorMediaItem();
        if (!iten["category"].toString().isEmpty())
        {
            iteo._Id = iten["file_id"].toString();
            iteo._CompilationId = iten["compilation_id"].toString();
            iteo._Name = iten["name"].toString();
            iteo._Space = iten["drive_id"].toString();
            iteo._Domain = iten["domain_id"].toString();
            iteo._Extension = iten["file_extension"].toString();
            iteo._Type = iten["type"].toString();
            iteo._SHA1 = iten["content_hash"].toString();
            iteo._Mime = iten["mime_type"].toString();
            const auto size = iten["size"].toVariant().toULongLong();
            iteo._Byte = size;
            iteo._Size = RaptorUtil::invokeStorageUnitConvert(size);
            iteo._Icon = RaptorUtil::invokeIconMatch(iteo._Name);
            iteo._Duration = iten["duration"].toDouble();
            iteo._Description = iten["description"].toString();
            auto qCreated = QDateTime::fromString(iten["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
            qCreated.setTimeSpec(Qt::UTC);
            iteo._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
            auto qUpdated = QDateTime::fromString(iten["updated_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
            qUpdated.setTimeSpec(Qt::UTC);
            iteo._Updated = qUpdated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
            itens << iteo;
        }
    });

    return itens;
}

void RaptorMediaSuite::onItemsFetching(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v2/video/list";
    qUseHeaderDefault(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["limit"] = 100;
    qRow["duration"] = 0;
    qRow["drive_id_list"] = QJsonArray() << RaptorStoreSuite::invokeUserGet()._Private << RaptorStoreSuite::invokeUserGet()._Public;
    qRow["marker"] = input._Marker;
    qRow["order_by"] = "created_at desc";
    qRow["hidden_type"] = input._Type;
    qRow["use_compilation"] = true;
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

    const auto itens = qDocument["items"].toArray();
    const auto items = invokeMapToItems(itens);
    const auto qMarker = qDocument["next_marker"].toString();
    output._State = true;
    output._Data = QVariant::fromValue<QPair<QVector<RaptorMediaItem>, QString> >(qMakePair(items, qMarker));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}
