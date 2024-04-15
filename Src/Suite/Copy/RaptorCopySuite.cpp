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

RaptorCopySuite *RaptorCopySuite::invokeSingletonGet()
{
    return _CopySuite();
}


QPair<QString, QString> RaptorCopySuite::invokeItemCreate(const RaptorAuthenticationItem &item,
                                                          const QString &qParent,
                                                          const QString &qName)
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v2/file/createWithFolders";
    qUseHeaderXCanary(qHttpPayload)
    qUseHeaderReferer(qHttpPayload)
    qUseHeaderCustomXDeviceId(qHttpPayload, item._Session._Device)
    qUseHeaderCustomXSignature(qHttpPayload, item._Session._Signature)
    qUseHeaderCustomAuthorization(qHttpPayload, item._AccessToken)
    qUseHeaderApplicationJson(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["check_name_mode"] = "refuse";
    qRow["drive_id"] = item._Space;
    qRow["parent_file_id"] = qParent;
    qRow["type"] = "folder";
    qRow["name"] = qName;
    qHttpPayload._Body = QJsonDocument(qRow);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qMakePair(qError, QString());
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::Created)
    {
        qCritical() << qDocument.toJson();
        return qMakePair(QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString()), QString());
    }

    return qMakePair(QString(), qDocument["file_id"].toString());
}

QPair<QString, bool> RaptorCopySuite::invokeItemRapid(const RaptorAuthenticationItem &item,
                                                      const RaptorFileItem &iten)
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
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokeGet(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qMakePair(qError, false);
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::PartialContent)
    {
        qCritical() << qDocument.toJson();
        return qMakePair(QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString()), false);
    }

    const auto qProofCode = QString::fromUtf8(qBody.toBase64());
    auto qHttpPayloae = RaptorHttpPayload();
    qHttpPayloae._Url = "https://api.alipan.com/adrive/v2/file/createWithFolders";
    qUseHeaderXCanary(qHttpPayloae)
    qUseHeaderReferer(qHttpPayloae)
    qUseHeaderCustomXDeviceId(qHttpPayloae, item._Session._Device)
    qUseHeaderCustomXSignature(qHttpPayloae, item._Session._Signature)
    qUseHeaderCustomAuthorization(qHttpPayloae, item._AccessToken)
    qUseHeaderApplicationJson(qHttpPayloae)
    auto qRow = QJsonObject();
    qRow["parent_file_id"] = iten._Parent;
    qRow["check_name_mode"] = "auto_rename";
    qRow["content_hash"] = iten._SHA1;
    qRow["content_hash_name"] = "sha1";
    qRow["create_scene"] = "file_upload";
    qRow["device_name"] = QStringLiteral("%1 %2.%3.%4").arg(APPLICATION_NAME, QString::number(MAJOR_VERSION), QString::number(MINOR_VERSION), QString::number(PATCH_VERSION));
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
        qCritical() << qErros;
        return qMakePair(qErros, false);
    }

    const auto qDocumenu = QJsonDocument::fromJson(qBodz);
    if (qStatut != RaptorHttpStatus::Created)
    {
        qCritical() << qDocumenu.toJson();
        return qMakePair(qDocumenu["message"].toString(), false);
    }

    if (!qDocumenu["rapid_upload"].toBool())
    {
        return qMakePair(QStringLiteral("秒传失败"), false);
    }

    return qMakePair(QString(), true);
}

void RaptorCopySuite::invokeItemsTreeBuild(const QVector<RaptorFileItem> &items, const QString &qParent)
{
    for (const auto &item: items)
    {
        const auto iten = new Node();
        iten->_Item = item;
        _Items << iten;
    }

    for (const auto item: _Items)
    {
        if (item->_Item._Id == qParent || item->_Item._Parent == qParent)
        {
            _Item = item;
            break;
        }
    }

    for (const auto item: _Items)
    {
        if (item == _Item)
        {
            continue;
        }

        for (const auto iten: _Items)
        {
            if (iten->_Item._Id == item->_Item._Parent)
            {
                iten->_Children << item;
                break;
            }
        }
    }
}

void RaptorCopySuite::invokeItemPreorderTraversal(const Node *qRoot,
                                                  const QString &qParent,
                                                  const QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> &qUser) const
{
    if (!qRoot)
    {
        return;
    }

    auto item = qRoot->_Item;
    qDebug() << item._Name;
    auto qParenu = qParent;
    auto output = RaptorOutput();
    if (item._Type == "folder")
    {
        const auto [qError, qParenv] = invokeItemCreate(qUser.second, qParent, item._Name);
        if (!qError.isEmpty())
        {
            output._State = false;
            output._Message = qError;
            output._Data = QVariant::fromValue<RaptorFileItem>(item);
            Q_EMIT itemCopied(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        qParenu = qParenv;
    } else if (item._Type == "file")
    {
        const auto [qError, qUrl] = RaptorFileSuite::invokeItemUrlFetch(qUser.first, item._Id);
        if (!qError.isEmpty())
        {
            output._State = qError.isEmpty();
            output._Message = qError;
            output._Data = QVariant::fromValue<RaptorFileItem>(item);
            Q_EMIT itemCopied(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        item._Url = qUrl;
        item._Parent = qParenu;
        const auto [qErros, qState] = invokeItemRapid(qUser.second, item);
        if (!qErros.isEmpty())
        {
            output._State = false;
            output._Message = qErros;
            output._Data = QVariant::fromValue<RaptorFileItem>(item);
            Q_EMIT itemCopied(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        if (!qState)
        {
            output._State = false;
            output._Message = qErros;
            output._Data = QVariant::fromValue<RaptorFileItem>(item);
            Q_EMIT itemCopied(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        output._State = true;
        output._Data = QVariant::fromValue<RaptorFileItem>(item);
        Q_EMIT itemCopied(QVariant::fromValue<RaptorOutput>(output));
    }

    for (const auto iten: qRoot->_Children)
    {
        if (_Cancel)
        {
            output._State = true;
            Q_EMIT itemsCancelled(QVariant::fromValue<RaptorOutput>(output));
            break;
        }

        invokeItemPreorderTraversal(iten, qParenu, qUser);
    }
}

void RaptorCopySuite::onItemsByParentIdFetching(const QVariant &qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v3/file/list";
    qUseHeaderXCanary(qHttpPayload)
    qUseHeaderReferer(qHttpPayload)
    qUseHeaderCustomXDeviceId(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Device)
    qUseHeaderCustomXSignature(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Signature)
    qUseHeaderCustomAuthorization(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._AccessToken)
    qUseHeaderApplicationJson(qHttpPayload)
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
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem> > >(qMakePair(input, items));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorCopySuite::onItemsByIdFetching(const QVariant &qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v1/file/get_path";
    qUseHeaderXCanary(qHttpPayload)
    qUseHeaderReferer(qHttpPayload)
    qUseHeaderCustomXDeviceId(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Device)
    qUseHeaderCustomXSignature(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Signature)
    qUseHeaderCustomAuthorization(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._AccessToken)
    qUseHeaderApplicationJson(qHttpPayload)
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

void RaptorCopySuite::onItemsByConditionFetching(const QVariant &qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v3/file/search";
    qUseHeaderXCanary(qHttpPayload)
    qUseHeaderReferer(qHttpPayload)
    qUseHeaderCustomXDeviceId(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Device)
    qUseHeaderCustomXSignature(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._Session._Signature)
    qUseHeaderCustomAuthorization(qHttpPayload, input._Variant.value<RaptorAuthenticationItem>()._AccessToken)
    qUseHeaderApplicationJson(qHttpPayload)
    const auto qCondition = QStringLiteral(R"(name match "%1")").arg(input._Name);
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
    output._Data = QVariant::fromValue<QPair<RaptorInput, QVector<RaptorFileItem> > >(qMakePair(input, items));
    Q_EMIT itemsFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorCopySuite::onItemsCopying(const QVariant &qVariant)
{
    const auto input = qVariant.value<RaptorInput>();
    const auto qUser = input._Variant.value<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> >();
    auto output = RaptorOutput();
    for (const auto &qIndex: input._Indexes)
    {
        if (_Cancel)
        {
            output._State = true;
            Q_EMIT itemsCancelled(QVariant::fromValue<RaptorOutput>(output));
            break;
        }

        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        auto [qError, items] = RaptorDownloadingSuite::invokeItemsWalk(item._Id);
        if (!qError.isEmpty())
        {
            output._State = true;
            output._Message = qError;
            Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
            continue;
        }

        items.insert(0, item);
        invokeItemsTreeBuild(items, item._Id);
        invokeItemPreorderTraversal(_Item, input._Id, qUser);
        for (auto iten: _Items)
        {
            qFree(iten)
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
