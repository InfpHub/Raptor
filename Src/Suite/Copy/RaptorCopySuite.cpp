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

#include "RaptorCopySuite.h"

Q_GLOBAL_STATIC(RaptorCopySuite, _CopySuite)

RaptorCopySuite* RaptorCopySuite::invokeSingletonGet()
{
    return _CopySuite();
}

RaptorOutput RaptorCopySuite::invokeItemRapid(const RaptorAuthenticationItem& item,
                                              const RaptorFileItem& iten)
{
    const auto qMD5 = QCryptographicHash::hash(item._AccessToken.toUtf8(), QCryptographicHash::Md5)
                      .toHex()
                      .left(16)
                      .toULongLong(Q_NULLPTR, 16);
    const auto qByte = iten._Byte;
    const auto qOffset = qByte ? qMD5 % qByte : 0;
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = iten._Url;
    qHttpPayload._Header.insert("Range", QStringLiteral("bytes=%1-%2").arg(qOffset).arg(qMin(8 + qOffset, qByte) - 1));
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokeGet(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        return output;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::PartialContent)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        return output;
    }

    const auto qProofCode = QString::fromUtf8(qBody.toBase64());
    auto qHttpPayloae = RaptorHttpPayload();
    qHttpPayloae._Url = "https://api.aliyundrive.com/adrive/v2/file/createWithFolders";
    USE_HEADER_X_CANARY(qHttpPayloae)
    USE_HEADER_REFERER(qHttpPayloae)
    USE_HEADER_CUSTOM_X_DEVICE_ID(qHttpPayloae, item._Session._Device)
    USE_HEADER_CUSTOM_X_SIGNATURE(qHttpPayloae, item._Session._Signature)
    USE_HEADER_CUSTOM_AUTHORIZATION(qHttpPayloae, item._AccessToken)
    USE_HEADER_APPLICATION_JSON(qHttpPayloae)
    auto qRow = QJsonObject();
    qRow["parent_file_id"] = iten._Parent;
    qRow["check_name_mode"] = "auto_rename";
    qRow["content_hash"] = iten._SHA1;
    qRow["content_hash_name"] = "sha1";
    qRow["create_scene"] = "file_upload";
    qRow["device_name"] = QStringLiteral("%1 %2.%3.%4")
                          .arg(APPLICATION_NAME)
                          .arg(MAJOR_VERSION)
                          .arg(MINOR_VERSION)
                          .arg(PATCH_VERSION);
    qRow["drive_id"] = item._Space;
    qRow["name"] = iten._Name;
    qRow["proof_code"] = qProofCode;
    qRow["proof_version"] = "v1";
    qRow["size"] = QString::number(iten._Byte);
    qRow["type"] = "file";
    qHttpPayloae._Body = QJsonDocument(qRow);
    const auto [qErros, qStatut, qBodz] = RaptorHttpSuite::invokePost(qHttpPayloae);
    if (!qErros.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qErros;
        return output;
    }

    const auto qDocumenu = QJsonDocument::fromJson(qBodz);
    if (qStatut != RaptorHttpStatus::Created)
    {
        qCritical() << qDocumenu.toJson();
        output._State = false;
        output._Message = qDocumenu["message"].toString();
        return output;
    }

    if (qDocumenu["rapid_upload"].toBool())
    {
        output._State = true;
        output._Data = QVariant::fromValue<bool>(true);
    }
    else
    {
        output._State = false;
        output._Data = QVariant::fromValue<bool>(false);
    }

    return output;
}

void RaptorCopySuite::onItemsByParentIdFetching(const QVariant& qVariant)
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v3/file/list";
    USE_HEADER_X_CANARY(qHttpPayload)
    USE_HEADER_REFERER(qHttpPayload)
    USE_HEADER_CUSTOM_X_DEVICE_ID(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Device)
    USE_HEADER_CUSTOM_X_SIGNATURE(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Signature)
    USE_HEADER_CUSTOM_AUTHORIZATION(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._AccessToken)
    USE_HEADER_APPLICATION_JSON(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = input._Variant.value<RaptorAuthenticationItem>()._Space;
    qRow["parent_file_id"] = input._Parent;
    qRow["limit"] = 200;
    qRow["type"] = input._Type;
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
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto itens = qDocument["items"].toArray();
    input._Marker = qDocument["next_marker"].toString();
    const auto items = RaptorFileSuite::invokeMapToItems(itens);
    output._State = true;
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem>>>(qMakePair(input, items));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorCopySuite::onItemsByIdFetching(const QVariant& qVariant)
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v1/file/get_path";
    USE_HEADER_X_CANARY(qHttpPayload)
    USE_HEADER_REFERER(qHttpPayload)
    USE_HEADER_CUSTOM_X_DEVICE_ID(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Device)
    USE_HEADER_CUSTOM_X_SIGNATURE(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Signature)
    USE_HEADER_CUSTOM_AUTHORIZATION(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._AccessToken)
    USE_HEADER_APPLICATION_JSON(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = input._Variant.value<RaptorAuthenticationItem>()._Space;
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

void RaptorCopySuite::onItemsByConditionFetching(const QVariant& qVariant)
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v3/file/search";
    USE_HEADER_X_CANARY(qHttpPayload)
    USE_HEADER_REFERER(qHttpPayload)
    USE_HEADER_CUSTOM_X_DEVICE_ID(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Device)
    USE_HEADER_CUSTOM_X_SIGNATURE(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Signature)
    USE_HEADER_CUSTOM_AUTHORIZATION(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._AccessToken)
    USE_HEADER_APPLICATION_JSON(qHttpPayload)
    auto qCondition = QStringLiteral(R"(name match "%1")").arg(input._Name);
    auto qRow = QJsonObject();
    qRow["drive_id"] = input._Variant.value<RaptorAuthenticationItem>()._Space;
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
    const auto items = RaptorFileSuite::invokeMapToItems(itens);
    output._State = true;
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem>>>(qMakePair(input, items));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorCopySuite::onItemsCopying(const QVariant& qVariant)
{
    const auto input = qVariant.value<RaptorInput>();
    const auto qPair = input._Variant.value<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem>>();
    const auto qIndexList = input._Indexes;
    auto output = RaptorOutput();
    for (auto i = 0; i < qIndexList.length(); ++i)
    {
        if (!RaptorStoreSuite::invokeUserIsValidConfirm())
        {
            break;
        }

        if (_Cancel)
        {
            output._State = true;
            Q_EMIT itemsCancelled(QVariant::fromValue<RaptorOutput>(output));
            break;
        }

        const auto qIndex = qIndexList[i];
        auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        const auto [_State, _Message, _Data] = RaptorFileSuite::invokeItemUrlFetch(qPair.first, item._Id);
        if (!_State)
        {
            output._State = false;
            output._Message = _Message;
            Q_EMIT itemCopied(QVariant::fromValue<RaptorOutput>(output));
            continue;
        }

        item._Url = _Data.value<QString>();
        item._Parent = input._Id;
        const auto [_Statf, _Messagf, _Datb] = invokeItemRapid(qPair.second, item);
        if (!_Statf)
        {
            output._State = false;
            output._Message = _Messagf;
            output._Data = QVariant::fromValue<RaptorFileItem>(item);
            Q_EMIT itemCopied(QVariant::fromValue<RaptorOutput>(output));
            continue;
        }

        if (!_Data.value<bool>())
        {
            output._State = false;
            output._Message = _Messagf;
            output._Data = QVariant::fromValue<RaptorFileItem>(item);
            Q_EMIT itemCopied(QVariant::fromValue<RaptorOutput>(output));
            continue;
        }

        output._State = true;
        output._Data = QVariant::fromValue<RaptorFileItem>(item);
        Q_EMIT itemCopied(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorCopySuite::onItemsCancelling()
{
    _Cancel = true;
}
