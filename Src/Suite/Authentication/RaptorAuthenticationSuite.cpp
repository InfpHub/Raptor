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

#include "RaptorAuthenticationSuite.h"

Q_GLOBAL_STATIC(RaptorAuthenticationSuite, _AuthenticationSuite)

RaptorAuthenticationSuite::RaptorAuthenticationSuite()
{
    invokeInstanceInit();
    invokeSlotInit();
}

RaptorAuthenticationSuite *RaptorAuthenticationSuite::invokeSingletonGet()
{
    return _AuthenticationSuite();
}

void RaptorAuthenticationSuite::invokeStop() const
{
    if (_ItemSessionTimer->isActive())
    {
        _ItemSessionTimer->stop();
    }

    if (_ItemAccessTokenTimer->isActive())
    {
        _ItemAccessTokenTimer->stop();
    }
}

void RaptorAuthenticationSuite::invokeInstanceInit()
{
    _ItemSessionTimer = new QTimer(this);
    _ItemSessionTimer->setInterval(600000);
    _ItemAccessTokenTimer = new QTimer(this);
}

void RaptorAuthenticationSuite::invokeSlotInit() const
{
    connect(_ItemSessionTimer,
            &QTimer::timeout,
            this,
            &RaptorAuthenticationSuite::onItemSessionTimerTimeout);

    connect(_ItemAccessTokenTimer,
            &QTimer::timeout,
            this,
            &RaptorAuthenticationSuite::onItemAccessTokenTimerTimeout);
}

void RaptorAuthenticationSuite::invokeItemDelete(const QString &qId)
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        DELETE FROM User WHERE LeafId = :LeafId
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":LeafId", qId);
    qSQLQuery.exec();
    if (qSQLQuery.lastError().isValid())
    {
        qCritical() << qSQLQuery.lastError().text();
    }
}

QPair<QString, RaptorAuthenticationItem> RaptorAuthenticationSuite::invokeItemByActiveFind()
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    qSQLQuery.exec(R"(SELECT * FROM User WHERE State = 1)");
    if (qSQLQuery.lastError().isValid())
    {
        qCritical() << qSQLQuery.lastError().text();
        return qMakePair(qSQLQuery.lastError().text(), RaptorAuthenticationItem());
    }

    if (!qSQLQuery.next())
    {
        return qMakePair(QStringLiteral("尚未登录任何用户!"), RaptorAuthenticationItem());
    }

    auto item = RaptorAuthenticationItem();
    item._LeafId = qSQLQuery.value("LeafId").toString();
    item._Space = qSQLQuery.value("Space").toString();
    item._Private = qSQLQuery.value("Private").toString();
    item._Public = qSQLQuery.value("Public").toString();
    item._Nickname = qSQLQuery.value("Nickname").toString();
    item._AccessToken = qSQLQuery.value("AccessToken").toString();
    item._RefreshToken = qSQLQuery.value("RefreshToken").toString();
    item._Active = true;
    item._Session._Device = qSQLQuery.value("Device").toString();
    item._Session._PrivateKey = qSQLQuery.value("PrivateKey").toString();
    item._Session._PublicKey = qSQLQuery.value("PublicKey").toString();
    item._Session._Signature = qSQLQuery.value("Signature").toString();
    return qMakePair(QString(), item);
}

void RaptorAuthenticationSuite::invokeItemSave(const RaptorAuthenticationItem &item)
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    qSQLQuery.exec(R"(UPDATE User Set State = NULL)");
    const auto qSQL = R"(
        INSERT INTO User ("LeafId", "Space", "Device", "Nickname", "Avatar", "AccessToken", "RefreshToken", "State", "Private", "PrivateKey", "PublicKey", "Public", "Signature")
        VALUES (:LeafId, :Space, :Device, :Nickname, :Avatar, :AccessToken, :RefreshToken, :State, :Private, :PrivateKey, :PublicKey, :Public, :Signature)
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":LeafId", item._LeafId);
    qSQLQuery.bindValue(":Space", item._Space);
    qSQLQuery.bindValue(":Device", item._Session._Device);
    qSQLQuery.bindValue(":Nickname", item._Nickname);
    qSQLQuery.bindValue(":Avatar", item._Avatar);
    qSQLQuery.bindValue(":AccessToken", item._AccessToken);
    qSQLQuery.bindValue(":RefreshToken", item._RefreshToken);
    qSQLQuery.bindValue(":State", 1);
    qSQLQuery.bindValue(":Private", item._Private);
    qSQLQuery.bindValue(":PrivateKey", item._Session._PrivateKey);
    qSQLQuery.bindValue(":PublicKey", item._Session._PublicKey);
    qSQLQuery.bindValue(":Signature", item._Session._Signature);
    qSQLQuery.bindValue(":Public", item._Public);
    qSQLQuery.exec();
    if (qSQLQuery.lastError().isValid())
    {
        qCritical() << qSQLQuery.lastError().text();
    }
}

void RaptorAuthenticationSuite::invokeItemUpdate(const RaptorAuthenticationItem &item)
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        UPDATE User SET Avatar = :Avatar, AccessToken = :AccessToken, RefreshToken = :RefreshToken
        WHERE LeafId = :LeafId
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":Avatar", item._Avatar);
    qSQLQuery.bindValue(":AccessToken", item._AccessToken);
    qSQLQuery.bindValue(":RefreshToken", item._RefreshToken);
    qSQLQuery.bindValue(":LeafId", item._LeafId);
    qSQLQuery.exec();
    if (qSQLQuery.lastError().isValid())
    {
        qCritical() << qSQLQuery.lastError().text();
    }
}

QString RaptorAuthenticationSuite::invokeItemAccessTokenRefresh(RaptorAuthenticationItem &item) const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v2/account/token";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["grant_type"] = "refresh_token";
    qRow["refresh_token"] = item._RefreshToken;
    qHttpPayload._Body = QJsonDocument(qRow);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qError;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
    }

    if (qDocument["status"].toString() != "enabled")
    {
        const auto qErros = QStringLiteral("用户 %1 已被禁用!").arg(item._Nickname);
        qCritical() << qErros;
        return qErros;
    }

    if (const auto qId = qDocument["user_id"].toString();
        RaptorStoreSuite::invokeUserGet()._LeafId == qId && !_ItemAccessTokenTimer->isActive())
    {
        const auto qExpire = qDocument["expires_in"].toInt();
        _ItemAccessTokenTimer->start(qExpire * 1000);
    }

    item._AccessToken = qDocument["access_token"].toString();
    item._RefreshToken = qDocument["refresh_token"].toString();
    invokeItemDetailFetch(item, qDocument["avatar"].toString());
    return QString();
}

QPair<QString, RaptorSession> RaptorAuthenticationSuite::invokeSessionInit(const RaptorAuthenticationItem &item)
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/users/v1/users/device/create_session";
    auto qDevice = item._Session._Device;
    auto qPrivateKey = item._Session._PrivateKey;
    auto qPublicKey = item._Session._PublicKey;
    auto qSignature = item._Session._Signature;
    if (qDevice.isEmpty() || qPrivateKey.isEmpty() || qPublicKey.isEmpty() || qSignature.isEmpty())
    {
        const auto qSession = RaptorUtil::invokeDeviceSignature(item);
        qDevice = qSession._Device;
        qPrivateKey = qSession._PrivateKey;
        qPublicKey = qSession._PublicKey;
        qSignature = qSession._Signature;
    }

    USE_HEADER_X_CANARY(qHttpPayload)
    USE_HEADER_CUSTOM_X_DEVICE_ID(qHttpPayload, qDevice)
    USE_HEADER_CUSTOM_X_SIGNATURE(qHttpPayload, qSignature)
    USE_HEADER_CUSTOM_AUTHORIZATION(qHttpPayload, item._AccessToken)
    USE_HEADER_APPLICATION_JSON(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["pubKey"] = qPublicKey;
    qRow["deviceName"] = QStringLiteral("%1 %2.%3.%4").arg(APPLICATION_NAME, QString::number(MAJOR_VERSION), QString::number(MINOR_VERSION), QString::number(PATCH_VERSION));
    qRow["modelName"] = QStringLiteral("Windows X Ultimate");
    qHttpPayload._Body = QJsonDocument(qRow);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    auto qSession = RaptorSession();
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qMakePair(qError, qSession);
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return qMakePair(QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString()), qSession);
    }

    if (!qDocument["success"].toBool())
    {
        return qMakePair(QStringLiteral("初始化会话失败!"), qSession);
    }

    qSession._Device = qDevice;
    qSession._Signature = qSignature;
    qSession._PrivateKey = qPrivateKey;
    qSession._PublicKey = qPublicKey;
    return qMakePair(QString(), qSession);
}

QString RaptorAuthenticationSuite::invokeItemDetailFetch(RaptorAuthenticationItem &item, const QString &qUrl)
{
    if (qUrl.isEmpty())
    {
        if (auto qFile = QFile(RaptorUtil::invokeIconMatch("aDrive", false, true));
            qFile.open(QIODevice::ReadOnly))
        {
            item._Avatar = qFile.readAll();
            qFile.close();
        }
    }else
    {
        auto qHttpPayload = RaptorHttpPayload();
        qHttpPayload._Url = qUrl;
        const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokeGet(qHttpPayload);
        if (!qError.isEmpty())
        {
            qCritical() << qError;
            return qError;
        }

        if (qStatus != RaptorHttpStatus::OK)
        {
            qCritical() << qError;
            return qError;
        }

        item._Avatar = qBody;
    }


    auto qHttpPayloae = RaptorHttpPayload();
    qHttpPayloae._Url = "https://user.aliyundrive.com/v2/user/get";
    USE_HEADER_X_CANARY(qHttpPayloae)
    USE_HEADER_REFERER(qHttpPayloae)
    USE_HEADER_CUSTOM_AUTHORIZATION(qHttpPayloae, item._AccessToken)
    USE_HEADER_CUSTOM_X_DEVICE_ID(qHttpPayloae, item._Session._Device)
    USE_HEADER_CUSTOM_X_SIGNATURE(qHttpPayloae, item._Session._Signature)
    USE_HEADER_APPLICATION_JSON(qHttpPayloae)
    const auto [qErros, qStatut, qBodz] = RaptorHttpSuite::invokePost(qHttpPayloae);
    if (!qErros.isEmpty())
    {
        qCritical() << qErros;
        return qErros;
    }

    const auto qDocument = QJsonDocument::fromJson(qBodz);
    if (qStatut != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
    }

    item._VIP = qDocument["vip_identity"] != "member";
    item._Description = qDocument["description"].toString();
    item._Public = qDocument["resource_drive_id"].toString();
    const auto qCreated = qDocument["created_at"].toVariant().toLongLong();
    const auto qDifference = QDateTime::currentMSecsSinceEpoch() - qCreated;
    item._Meet = RaptorUtil::invokeTimestampConvert(qDifference / 1000);
    return QString();
}

QString RaptorAuthenticationSuite::invokeItemCapacityFetch(RaptorAuthenticationItem &item)
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/adrive/v1/user/driveCapacityDetails";
    USE_HEADER_X_CANARY(qHttpPayload)
    USE_HEADER_REFERER(qHttpPayload)
    USE_HEADER_CUSTOM_AUTHORIZATION(qHttpPayload, item._AccessToken)
    USE_HEADER_CUSTOM_X_DEVICE_ID(qHttpPayload, item._Session._Device)
    USE_HEADER_CUSTOM_X_SIGNATURE(qHttpPayload, item._Session._Signature)
    USE_HEADER_APPLICATION_JSON(qHttpPayload)
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qError;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
    }

    const auto qUsed = qDocument["drive_used_size"].toVariant().toLongLong();
    const auto qTotal = qDocument["drive_total_size"].toVariant().toLongLong();
    const auto qAlbum = qDocument["album_drive_used_size"].toVariant().toLongLong() + qDocument[
                            "share_album_drive_used_size"].toVariant().toLongLong();
    const auto qNote = qDocument["note_drive_used_size"].toVariant().toLongLong();
    item._Capacity._Radio = QString::number((static_cast<double>(qUsed) / static_cast<double>(qTotal)) * 100, 'f', 2) +
                            "%";
    item._Capacity._Used = RaptorUtil::invokeStorageUnitConvert(qUsed);
    item._Capacity._Total = RaptorUtil::invokeStorageUnitConvert(qTotal);
    item._Capacity._Album = RaptorUtil::invokeStorageUnitConvert(qAlbum);
    item._Capacity._Note = RaptorUtil::invokeStorageUnitConvert(qNote);
    return QString();
}

void RaptorAuthenticationSuite::onItemsLoading() const
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(SELECT * FROM User)";
    qSQLQuery.exec(qSQL);
    if (qSQLQuery.lastError().isValid())
    {
        const auto qError = qSQLQuery.lastError().text();
        qCritical() << qError;
        auto output = RaptorOutput();
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsLoaded(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto items = QVector<RaptorAuthenticationItem>();
    while (qSQLQuery.next())
    {
        auto item = RaptorAuthenticationItem();
        item._LeafId = qSQLQuery.value("LeafId").toString();
        item._Space = qSQLQuery.value("Space").toString();
        item._Private = qSQLQuery.value("Private").toString();
        item._Public = qSQLQuery.value("Public").toString();
        item._Nickname = qSQLQuery.value("Nickname").toString();
        item._Avatar = qSQLQuery.value("Avatar").toByteArray();
        item._AccessToken = qSQLQuery.value("AccessToken").toString();
        item._RefreshToken = qSQLQuery.value("RefreshToken").toString();
        item._Active = qSQLQuery.value("State").toInt() == 1;
        item._Session._Device = qSQLQuery.value("Device").toString();
        item._Session._PrivateKey = qSQLQuery.value("PrivateKey").toString();
        item._Session._PublicKey = qSQLQuery.value("PublicKey").toString();
        item._Session._Signature = qSQLQuery.value("Signature").toString();
        items << item;
    }

    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QVector<RaptorAuthenticationItem> >(items);
    Q_EMIT itemsLoaded(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemLogouting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto item = input._Variant.value<RaptorAuthenticationItem>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/users/v1/users/device_logout";
    USE_HEADER_X_CANARY(qHttpPayload)
    USE_HEADER_REFERER(qHttpPayload)
    USE_HEADER_CUSTOM_AUTHORIZATION(qHttpPayload, item._AccessToken)
    USE_HEADER_CUSTOM_X_DEVICE_ID(qHttpPayload, item._Session._Device)
    USE_HEADER_CUSTOM_X_SIGNATURE(qHttpPayload, item._Session._Signature)
    USE_HEADER_APPLICATION_JSON(qHttpPayload)
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemLogouted(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemLogouted(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (!qDocument["success"].toBool())
    {
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemLogouted(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    invokeItemDelete(item._LeafId);
    if (item == RaptorStoreSuite::invokeUserGet())
    {
        if (_ItemSessionTimer->isActive())
        {
            _ItemSessionTimer->stop();
        }

        // 注销的是当前用户，则选择表中第一个用户为当前用户（如果有）
        auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
        auto qSQL = R"(
            SELECT * FROM User LIMIT 1
        )";
        qSQLQuery.exec(qSQL);
        if (qSQLQuery.next())
        {
            qSQL = R"(
                UPDATE USER SET State = 1 WHERE ROWID = 1
            )";
            qSQLQuery.exec(qSQL);
            onItemAccessTokenRefreshing();
        }
    }

    output._State = true;
    output._Data = QVariant::fromValue<RaptorAuthenticationItem>(item);
    Q_EMIT itemLogouted(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemSwitching(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    auto item = input._Variant.value<RaptorAuthenticationItem>();
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    auto qSQL = R"(
        UPDATE User SET State = 0
    )";

    qSQLQuery.exec(qSQL);
    qSQLQuery.clear();
    qSQL = R"(
        UPDATE User SET State = 1 WHERE LeafId = :LeafId
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":LeafId", item._LeafId);
    qSQLQuery.exec();
    const auto qError = invokeItemAccessTokenRefresh(item);
    auto output = RaptorOutput();
    output._State = qError.isEmpty();
    output._Message = qError;
    output._Data = QVariant::fromValue<RaptorAuthenticationItem>(item);
    Q_EMIT itemSwitched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemCapacityRefreshing() const
{
    auto item = RaptorStoreSuite::invokeUserGet();
    const auto qError = invokeItemCapacityFetch(item);
    auto output = RaptorOutput();
    output._State = qError.isEmpty();
    output._Message = qError;
    output._Data = QVariant::fromValue<RaptorCapacity>(item._Capacity);
    RaptorStoreSuite::invokeUserSet(item);
    Q_EMIT itemCapacityRefreshed(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemQrCodeGenerating() const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://passport.aliyundrive.com/newlogin/qrcode/generate.do";
    USE_HEADER_X_CANARY(qHttpPayload)
    USE_HEADER_REFERER(qHttpPayload)
    USE_HEADER_APPLICATION_JSON(qHttpPayload)
    USE_QUERY_APPLICATION_NAME(qHttpPayload)
    USE_QUERY_FROM_SITE(qHttpPayload)
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokeGet(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemQrCodeGenerated(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemQrCodeGenerated(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qData = qDocument["content"]["data"];
    auto item = RaptorQrCode();
    item._Timestamp = qData["t"].toVariant().toULongLong();
    item._Code = qData["codeContent"].toString();
    item._Cookie = qData["ck"].toString();
    output._State = true;
    output._Data = QVariant::fromValue<RaptorQrCode>(item);
    Q_EMIT itemQrCodeGenerated(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemQrCodeStatusFetching(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto [_Timestamp, _Code, _Cookie] = input._Variant.value<RaptorQrCode>();
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://passport.aliyundrive.com/newlogin/qrcode/query.do";
    USE_HEADER_X_CANARY(qHttpPayload)
    USE_HEADER_REFERER(qHttpPayload)
    USE_HEADER_APPLICATION_URL_ENCODED(qHttpPayload)
    USE_QUERY_APPLICATION_NAME(qHttpPayload)
    USE_QUERY_FROM_SITE(qHttpPayload)
    USE_QUERY_CUSTOM_TIMESTAMP(qHttpPayload, _Timestamp)
    USE_QUERY_CUSTOM_COOKIE(qHttpPayload, _Cookie)
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemQrCodeStatusFetched(QVariant::fromValue<RaptorOutput>(output));
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemQrCodeStatusFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto qContent = qDocument["content"];
    if (auto qrCodeStatus = qContent["data"]["qrCodeStatus"];
        qrCodeStatus == "NEW")
    {
        output._State = true;
        output._Data = QVariant::fromValue<RaptorQrCodeStatus>(RaptorQrCodeStatus::New);
        Q_EMIT itemQrCodeStatusFetched(QVariant::fromValue<RaptorOutput>(output));
    } else if (qrCodeStatus == "SCANED")
    {
        output._State = true;
        output._Data = QVariant::fromValue<RaptorQrCodeStatus>(RaptorQrCodeStatus::Scanned);
        Q_EMIT itemQrCodeStatusFetched(QVariant::fromValue<RaptorOutput>(output));
    } else if (qrCodeStatus == "CONFIRMED")
    {
        const auto qBizExt = QJsonDocument::fromJson(
            QString::fromLocal8Bit(QByteArray::fromBase64(qContent["data"]["bizExt"].toString().toUtf8())).toUtf8())[
            "pds_login_result"];
        if (qBizExt["status"].toString() != "enabled")
        {
            const auto qErros = QStringLiteral("该用户已被禁用!");
            qCritical() << qErros;
            output._State = false;
            output._Message = qErros;
            Q_EMIT itemQrCodeStatusFetched(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        auto item = RaptorAuthenticationItem();
        item._LeafId = qBizExt["userId"].toString();
        item._Private = qBizExt["defaultDriveId"].toString();
        item._Space = item._Private;
        item._Nickname = qBizExt["nickName"].toString();
        item._AccessToken = qBizExt["accessToken"].toString();
        item._RefreshToken = qBizExt["refreshToken"].toString();
        if (const auto qErros = invokeItemCapacityFetch(item);
            !qErros.isEmpty())
        {
            qCritical() << qErros;
            output._State = false;
            output._Message = qErros;
            Q_EMIT itemAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        if (const auto qErrot = invokeItemDetailFetch(item, qBizExt["avatar"].toString());
            !qErrot.isEmpty())
        {
            qCritical() << qErrot;
            output._State = false;
            output._Message = qErrot;
            Q_EMIT itemAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        const auto [qError, qSession] = invokeSessionInit(item);
        if (!qError.isEmpty())
        {
            qCritical() << qError;
            output._State = false;
            output._Message = qError;
            Q_EMIT itemAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        const auto [qDevice, qSignature, qPrivateKey, qPublicKey] = qSession;
        item._Session._Device = qDevice;
        item._Session._Signature = qSignature;
        item._Session._PrivateKey = qPrivateKey;
        item._Session._PublicKey = qPublicKey;
        _ItemSessionTimer->start();
        invokeItemSave(item);
        RaptorStoreSuite::invokeUserSet(item);
        output._State = true;
        output._Data = QVariant::fromValue<RaptorQrCodeStatus>(RaptorQrCodeStatus::Confirmed);
        Q_EMIT itemQrCodeStatusFetched(QVariant::fromValue<RaptorOutput>(output));
        output._Data = QVariant::fromValue<RaptorAuthenticationItem>(item);
        Q_EMIT itemAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
    } else if (qrCodeStatus == "CANCELED")
    {
        output._State = true;
        output._Data = QVariant::fromValue<RaptorQrCodeStatus>(RaptorQrCodeStatus::Canceled);
        Q_EMIT itemQrCodeStatusFetched(QVariant::fromValue<RaptorOutput>(output));
    } else if (qrCodeStatus == "EXPIRED")
    {
        output._State = true;
        output._Data = QVariant::fromValue<RaptorQrCodeStatus>(RaptorQrCodeStatus::Expired);
        Q_EMIT itemQrCodeStatusFetched(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorAuthenticationSuite::onItemAccessTokenRefreshing() const
{
    auto output = RaptorOutput();
    auto [qError, item] = invokeItemByActiveFind();
    if (!qError.isEmpty())
    {
        output._State = false;
        output._Message = qError;
        Q_EMIT itemAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (const auto qErros = invokeItemAccessTokenRefresh(item);
        !qErros.isEmpty())
    {
        output._State = false;
        output._Message = qErros;
        Q_EMIT itemAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto [qErros, qSession] = invokeSessionInit(item);
    if (!qErros.isEmpty())
    {
        output._State = qErros.isEmpty();
        output._Message = qErros;
        Q_EMIT itemAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto [qDevice, qSignature, qPrivateKey, qPublicKey] = qSession;
    item._Session._Device = qDevice;
    item._Session._Signature = qSignature;
    item._Session._PrivateKey = qPrivateKey;
    item._Session._PublicKey = qPublicKey;
    _ItemSessionTimer->start();
    invokeItemCapacityFetch(item);
    invokeItemUpdate(item);
    RaptorStoreSuite::invokeUserSet(item);
    _ItemSessionTimer->start();
    output._State = true;
    Q_EMIT itemAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemsAccessTokenRefreshing(const QVariant &qVariant) const
{
    auto qPair = qVariant.value<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> >();
    auto output = RaptorOutput();
    if (const auto qError = invokeItemAccessTokenRefresh(qPair.first);
        !qError.isEmpty())
    {
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (const auto [qError, qSession] = invokeSessionInit(qPair.first);
        !qError.isEmpty())
    {
        output._State = qError.isEmpty();
        output._Message = qError;
        Q_EMIT itemsAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (const auto qErros = invokeItemAccessTokenRefresh(qPair.second);
        !qErros.isEmpty())
    {
        output._State = false;
        output._Message = qErros;
        Q_EMIT itemsAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (const auto [qError, qSession] = invokeSessionInit(qPair.second);
        !qError.isEmpty())
    {
        output._State = qError.isEmpty();
        output._Message = qError;
        Q_EMIT itemsAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    output._Data = QVariant::fromValue<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> >(qMakePair(qPair.first, qPair.second));
    Q_EMIT itemsAccessTokenRefreshed(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemDevicesFetching() const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/users/v2/users/device_list";
    USE_HEADER_DEFAULT(qHttpPayload)
    qHttpPayload._Body = QJsonDocument(QJsonObject());
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemDevicesFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemDevicesFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qMax = qDocument["result"]["maxDeviceCount"].toInt();
    const auto items = qDocument["result"]["devices"].toArray();
    const auto qUsed = items.size();
    auto itens = QVector<RaptorDeviceItem>();
    for (const auto &item: items)
    {
        const auto iten = item.toObject();
        auto iteo = RaptorDeviceItem();
        iteo._Name = iten["deviceName"].toString();
        iteo._City = iten["city"].toString();
        iteo._Logined = iten["loginTime"].toString();
        itens << iteo;
    }

    output._State = true;
    output._Data = QVariant::fromValue<std::tuple<quint8, quint8, QVector<RaptorDeviceItem> > >(std::make_tuple(qMax, qUsed, itens));
    Q_EMIT itemDevicesFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemSignInInfoFetching() const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://member.aliyundrive.com/v2/activity/sign_in_info";
    USE_HEADER_DEFAULT(qHttpPayload)
    qHttpPayload._Body = QJsonDocument(QJsonObject());
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemSignInInfoFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemSignInInfoFetched(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qBlessing = qDocument["result"]["subtitle"].toString();
    output._State = true;
    output._Data = QVariant::fromValue<QString>(qBlessing);
    Q_EMIT itemSignInInfoFetched(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemSigningIn() const
{
    auto output = RaptorOutput();
    if (RaptorStoreSuite::invokeUserGet()._SignIn.invokeItemSignInConfirm())
    {
        output._State = true;
        output._Message = RaptorStoreSuite::invokeUserGet()._SignIn._Goods;
        Q_EMIT itemSignedIn(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://member.aliyundrive.com/v1/activity/sign_in_list";
    USE_HEADER_DEFAULT(qHttpPayload)
    qHttpPayload._Body = QJsonDocument(QJsonObject());
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemSignedIn(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemSignedIn(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qSignInCount = qDocument["result"]["signInCount"].toVariant().toUInt();
    auto qHttpPayloae = RaptorHttpPayload();
    qHttpPayloae._Url = "https://member.aliyundrive.com/v1/activity/sign_in_reward";
    USE_HEADER_DEFAULT(qHttpPayloae)
    auto qRow = QJsonObject();
    qRow["signInDay"] = QString::number(qSignInCount);
    qHttpPayloae._Body = QJsonDocument(qRow);
    const auto [qErros, qStatut, qBodz] = RaptorHttpSuite::invokePost(qHttpPayloae);
    if (!qErros.isEmpty())
    {
        qCritical() << qErros;
        output._State = false;
        output._Message = qErros;
        Q_EMIT itemSignedIn(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocumenu = QJsonDocument::fromJson(qBodz);
    if (qStatut != RaptorHttpStatus::OK)
    {
        qCritical() << qDocumenu.toJson();
        output._State = false;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        Q_EMIT itemSignedIn(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qName = qDocumenu["result"]["name"].toString();
    output._State = true;
    const auto qGoods = QStringLiteral("Hi %1。本月累计签到%2次。本次签到获得【%3】").arg(RaptorStoreSuite::invokeUserGet()._Nickname)
            .arg(qSignInCount)
            .arg(qName);
    if (!RaptorStoreSuite::invokeUserGet()._SignIn.invokeItemSignInConfirm())
    {
        auto item = RaptorStoreSuite::invokeUserGet();
        item._SignIn._LeafId = item._LeafId;
        item._SignIn._Goods = qGoods;
        RaptorStoreSuite::invokeUserSet(item);
    }

    output._Message = qGoods;
    if (auto qDescription = qDocumenu["result"]["description"].toString();
        !qDescription.isEmpty())
    {
        const auto qGoodt = QStringLiteral("Hi %1。本月累计签到%2次。本次签到获得【%3】 - %4")
                .arg(RaptorStoreSuite::invokeUserGet()._Nickname)
                .arg(qSignInCount)
                .arg(qName, qDescription);
        if (!RaptorStoreSuite::invokeUserGet()._SignIn.invokeItemSignInConfirm())
        {
            auto item = RaptorStoreSuite::invokeUserGet();
            item._SignIn._LeafId = item._LeafId;
            item._SignIn._Goods = qGoodt;
            RaptorStoreSuite::invokeUserSet(item);
        }

        output._Message = qGoodt;
    }

    Q_EMIT itemSignedIn(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAuthenticationSuite::onItemSessionTimerTimeout() const
{
    if (const auto [qError, qSession] = invokeSessionInit(RaptorStoreSuite::invokeUserGet());
        !qError.isEmpty())
    {
        qCritical() << QStringLiteral("会话初始化失败: %1").arg(qError);
    }
}

void RaptorAuthenticationSuite::onItemAccessTokenTimerTimeout() const
{
    auto item = RaptorStoreSuite::invokeUserGet();
    if (const auto qError = invokeItemAccessTokenRefresh(item);
        !qError.isEmpty())
    {
        qCritical() << QStringLiteral("短令牌刷新失败: %1").arg(qError);
        return;
    }

    RaptorStoreSuite::invokeUserSet(item);
    invokeItemUpdate(item);
}
