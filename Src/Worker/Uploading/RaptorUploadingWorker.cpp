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

#include "RaptorUploadingWorker.h"

RaptorUploadingWorker::RaptorUploadingWorker(RaptorTransferItem item) : _Item(std::move(item)),
                                                                        _LastTransferred(0),
                                                                        _Paused(false),
                                                                        _Cancel(false)
{
    invokeInstanceInit();
}

RaptorUploadingWorker::~RaptorUploadingWorker()
{
    if (_Paused)
    {
        qInfo() << QStringLiteral("%1 暂停上传。").arg(_Item._Name);
    } else if (_Cancel)
    {
        qInfo() << QStringLiteral("%1 取消上传。").arg(_Item._Name);
    }
}

void RaptorUploadingWorker::run()
{
    qInfo() << QStringLiteral("%1 开始上传。").arg(_Item._Name);
    if (_Item._Transferred == _Item._Byte)
    {
        // 在最近一次的上传中的计算指纹的时候执行了暂停
        invokeItemComplete(_Item);
        return;
    }

    if (_Item._Rapid)
    {
        invokeItemRapid();
    } else
    {
        invokeItemUpload();
    }
}

void RaptorUploadingWorker::invokeInstanceInit()
{
    setAutoDelete(true);
}

QPair<QString, RaptorFileItem> RaptorUploadingWorker::invokeItemByIdFetch(const QString &qId) const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v1/file/get_path";
    qUseHeaderDefault(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = _Item._Space;
    qRow["file_id"] = qId;
    qHttpPayload._Body = QJsonDocument(qRow);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    auto item = RaptorFileItem();
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qMakePair(qError, item);
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return qMakePair(QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString()), item);
    }

    const auto iten = qDocument["items"][0];
    item._Name = iten["name"].toString();
    item._Extension = iten["file_extension"].toString();
    item._Icon = RaptorUtil::invokeIconMatch(item._Name);
    auto qCreated = QDateTime::fromString(iten["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
    qCreated.setTimeSpec(Qt::UTC);
    item._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
    return qMakePair(QString(), item);
}

void RaptorUploadingWorker::invokeItemRapid()
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v2/file/createWithFolders";
    qUseHeaderDefault(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["check_name_mode"] = "auto_rename";
    qRow["create_scene"] = "file_upload";
    auto qFile = QFile(QString("%1/%2").arg(_Item._Path, _Item._Name));
    if (!qFile.open(QIODevice::ReadOnly))
    {
        const auto qError = QStringLiteral("无法打开文件: %1").arg(_Item._Name);
        qCritical() << qError;
        auto output = RaptorOutput();
        output._State = true;
        output._Message = qError;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto qHash = QCryptographicHash(QCryptographicHash::Sha1);
    auto qComputed = static_cast<quint64>(0);
    auto qContinue = true;
    while (!qFile.atEnd())
    {
        if (_Paused)
        {
            qContinue = false;
            _Item._Status = QStringLiteral("已暂停");
            Q_EMIT itemPaused(QVariant::fromValue<RaptorTransferItem>(_Item));
            break;
        }

        if (_Cancel)
        {
            qContinue = false;
            Q_EMIT itemCancelled(QVariant::fromValue<RaptorTransferItem>(_Item));
            break;
        }

        auto qData = qFile.read(64 * 1024);
        qComputed += qData.length();
        qHash.addData(qData);
        const auto qRadio = qRound(static_cast<float>(qComputed) / static_cast<float>(_Item._Byte) * 10000) / 100.0;
        _Item._Transferred = qComputed;
        _Item._Status = QStringLiteral("计算指纹 %1%").arg(qRadio, 0, 'f', 2);
        Q_EMIT itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(_Item));
    }

    if (!qContinue)
    {
        return;
    }

    qRow["content_hash"] = QString(qHash.result().toHex());
    const auto qMD5 = QCryptographicHash::hash(RaptorStoreSuite::invokeUserGet()._AccessToken.toUtf8(),
                                               QCryptographicHash::Md5).toHex()
            .left(16)
            .toULongLong(Q_NULLPTR, 16);
    const auto qOffset = _Item._Byte ? qMD5 % _Item._Byte : 0;
    qFile.seek(qOffset);
    const auto data = qFile.read(qMin(8 + qOffset, _Item._Byte) - qOffset);
    qFile.close();
    qRow["proof_code"] = QString::fromUtf8(data.toBase64());
    qRow["proof_version"] = "v1";
    qRow["content_hash_name"] = "sha1";
    qRow["device_name"] = QStringLiteral("%1 %2.%3.%4").arg(APPLICATION_NAME, QString::number(MAJOR_VERSION), QString::number(MINOR_VERSION), QString::number(PATCH_VERSION));
    qRow["size"] = QString::number(_Item._Byte);
    qRow["drive_id"] = _Item._Space;
    qRow["parent_file_id"] = _Item._Parent;
    qRow["type"] = "file";
    qRow["name"] = _Item._Name;
    qRow["part_info_list"] =
            RaptorUtil::invokeItemPartialCompute(QStringLiteral("%1/%2").arg(_Item._Path, _Item._Name));
    qHttpPayload._Body = QJsonDocument(qRow);
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = true;
        output._Message = qError;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::Created)
    {
        qCritical() << qDocument.toJson();
        output._State = true;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (qDocument["rapid_upload"].toBool())
    {
        const auto [qError, item] = invokeItemByIdFetch(qDocument["file_id"].toString());
        if (!qError.isEmpty())
        {
            qCritical() << qDocument.toJson();
            output._State = qError.isEmpty();
            output._Message = qError;
            output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
            Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        _Item._Icon = item._Icon;
        _Item._Created = item._Created;
        Q_EMIT itemCompleted(QVariant::fromValue<RaptorTransferItem>(_Item));
    } else
    {
        qCritical() << qDocument.toJson();
        output._State = true;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorUploadingWorker::invokeItemUpload()
{
    auto qFile = QFile(QStringLiteral("%1/%2").arg(_Item._Path, _Item._Name));
    if (!qFile.open(QIODevice::ReadOnly))
    {
        const auto qError = QStringLiteral("无法打开文件: %1").arg(_Item._Name);
        qCritical() << qError;
        auto output = RaptorOutput();
        output._State = true;
        output._Message = qError;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto [qError, items] = invokeItemUploadUrlFetch();
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        auto output = RaptorOutput();
        output._State = qError.isEmpty();
        output._Message = qError;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    _Item._Partials = items;
    const auto qChunk = RaptorUtil::invokeChunkCompute(qFile.size());
    auto qStart = 0;
    const auto [qErros, qCount] = invokeItemUploadedPartialFetch();
    if (!qErros.isEmpty())
    {
        qCritical() << qErros;
        auto output = RaptorOutput();
        output._State = qErros.isEmpty();
        output._Message = qErros;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (qCount != 0)
    {
        _Item._Transferred = qChunk * qCount;
        qStart = qCount;
    }

    for (auto i = qStart; i < _Item._Partials.length(); ++i)
    {
        if (_Paused)
        {
            break;
        }

        if (_Cancel)
        {
            break;
        }

        const auto item = _Item._Partials[i];
        const auto qOffset = i * qChunk;
        qFile.seek(qOffset);
        auto qContent = QByteArray();
        if (i == _Item._Partials.length() - 1)
        {
            // 最后一个分片
            qContent = qFile.readAll();
        } else
        {
            qContent = qFile.read(qChunk);
        }

        auto qHttpPayload = RaptorHttpPayload();
        qHttpPayload._Url = item._Url;
        qUseHeaderXCanary(qHttpPayload)
        qUseHeaderDefaultAuthorization(qHttpPayload)
        qHttpPayload._Content = qContent;
        if (const auto [qStatus, qBody] = invokeItemPartialUpload(qHttpPayload);
            qStatus == RaptorHttpStatus::BadRequest)
        {
            qWarning() << qBody;
            auto qCode = QString();
            auto qPartial = 0;
            auto qXmlStreamReader = QXmlStreamReader(qBody);
            while (!qXmlStreamReader.atEnd() && !qXmlStreamReader.hasError())
            {
                if (auto qToken = qXmlStreamReader.readNext();
                    qToken == QXmlStreamReader::StartElement)
                {
                    if (qXmlStreamReader.name().toString() == "Code")
                    {
                        qXmlStreamReader.readNext();
                        qCode = qXmlStreamReader.text().toString();
                    } else if (qXmlStreamReader.name().toString() == "PartNumber")
                    {
                        qXmlStreamReader.readNext();
                        qPartial = qXmlStreamReader.text().toUInt();
                    }
                }
            }

            if (qCode == "PartNotSequential")
            {
            }
        } else if (qStatus == RaptorHttpStatus::Conflict)
        {
            qWarning() << qBody;
            auto qCode = QString();
            auto qPartial = 0;
            auto qXmlStreamReader = QXmlStreamReader(qBody);
            while (!qXmlStreamReader.atEnd() && !qXmlStreamReader.hasError())
            {
                if (auto qToken = qXmlStreamReader.readNext();
                    qToken == QXmlStreamReader::StartElement)
                {
                    if (qXmlStreamReader.name().toString() == "Code")
                    {
                        qXmlStreamReader.readNext();
                        qCode = qXmlStreamReader.text().toString();
                    } else if (qXmlStreamReader.name().toString() == "PartNumber")
                    {
                        qXmlStreamReader.readNext();
                        qPartial = qXmlStreamReader.text().toUInt();
                    }
                }
            }

            if (qCode == "PartAlreadyExist")
            {
            }
        } else if (qStatus == RaptorHttpStatus::Forbidden)
        {
            qWarning() << qBody;
            _Item._Status = "刷新中";
            Q_EMIT itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(_Item));
            const auto [qError, items] = invokeItemUploadUrlFetch();
            if (!qError.isEmpty())
            {
                auto output = RaptorOutput();
                output._State = qError.isEmpty();
                output._Message = qError;
                output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
                Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
                return;
            }

            _Item._Partials = items;
            --i;
        }
    }

    if (!_Paused && !_Cancel && _Item._Transferred == qFile.size())
    {
        Q_EMIT itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(_Item));
        invokeItemComplete(_Item);
    }
}

QPair<QString, QQueue<RaptorPartial> > RaptorUploadingWorker::invokeItemUploadUrlFetch() const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/v2/file/get_upload_url";
    qUseHeaderDefault(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = _Item._Space;
    qRow["file_id"] = _Item._LeafId;
    qRow["upload_id"] = _Item._WorkerId;
    qRow["part_info_list"] =
            RaptorUtil::invokeItemPartialCompute(QStringLiteral("%1/%2").arg(_Item._Path, _Item._Name));
    qHttpPayload._Body = QJsonDocument(qRow);
    auto itens = QQueue<RaptorPartial>();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qMakePair(qError, itens);
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return qMakePair(QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString()), itens);
    }

    const auto items = qDocument["part_info_list"].toArray();
    for (const auto &item: items)
    {
        const auto iten = item.toObject();
        auto iteno = RaptorPartial();
        iteno._Number = iten["part_number"].toVariant().toUInt();
        iteno._Url = iten["upload_url"].toString();
        itens << iteno;
    }

    return qMakePair(QString(), itens);
}

QPair<QString, quint32> RaptorUploadingWorker::invokeItemUploadedPartialFetch() const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/v2/file/list_uploaded_parts";
    qUseHeaderDefault(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = _Item._Space;
    qRow["file_id"] = _Item._LeafId;
    qRow["upload_id"] = _Item._WorkerId;
    qHttpPayload._Body = QJsonDocument(qRow);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qMakePair(qError, 0);
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return qMakePair(QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString()), 0);
    }

    return qMakePair(QString(), qDocument["uploaded_parts"].toArray().size());
}

int RaptorUploadingWorker::invokeItemPartialUploadProgressCallback(const curl_off_t &ulnow)
{
    if (_Paused)
    {
        const auto qChunk = RaptorUtil::invokeChunkCompute(_Item._Byte);
        _Item._Transferred = qFloor(_Item._Transferred / qChunk) * qChunk;
        _Item._Status = QStringLiteral("已暂停");
        Q_EMIT itemPaused(QVariant::fromValue<RaptorTransferItem>(_Item));
        return CURLE_ABORTED_BY_CALLBACK;
    }

    if (_Cancel)
    {
        Q_EMIT itemCancelled(QVariant::fromValue<RaptorTransferItem>(_Item));
        return CURLE_ABORTED_BY_CALLBACK;
    }

    if (ulnow != _LastTransferred)
    {
        if (const auto qDifference = static_cast<qint64>(ulnow - _LastTransferred);
            qDifference > 0)
        {
            _Item._Transferred += qDifference;
        }

        _LastTransferred = ulnow;
    }

    auto qSpeed = static_cast<curl_off_t>(0);
    curl_easy_getinfo(_Curl, CURLINFO_SPEED_UPLOAD_T, &qSpeed);
    _Item._Speed = QVariant::fromValue<QString>(RaptorUtil::invokeStorageUnitConvert(qSpeed));
    if (qSpeed > 0)
    {
        const auto qEtr = RaptorUtil::invokeTimeUnitConvert((_Item._Byte - _Item._Transferred) / qSpeed);
        _Item._ETR = qEtr;
        _Item._Status = QStringLiteral("%1 - %2").arg(_Item._Speed.toString(), qEtr);
    } else
    {
        _Item._ETR = "计算中";
        _Item._Status = "处理中";
    }

    Q_EMIT itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(_Item));
    return CURLE_OK;
}

size_t RaptorUploadingWorker::invokeItemPartialUploadWriteCallback(char *qTarget,
                                                                   size_t qSize,
                                                                   size_t nmemb,
                                                                   void *qData)
{
    if (qTarget == Q_NULLPTR)
    {
        return 0;
    }

    const auto qByteArray = static_cast<QByteArray *>(qData);
    qByteArray->append(qTarget, qSize * nmemb);
    return qSize * nmemb;
}

size_t RaptorUploadingWorker::invokeItemPartialUploadReadCallback(char *qTarget,
                                                                  size_t qSize,
                                                                  size_t nmemb,
                                                                  void *qData)
{
    _ReadMutex.lock();
    const auto qStream = static_cast<Stream *>(qData);
    if (qStream->_Length == 0)
    {
        _ReadMutex.unlock();
        return 0;
    }

    if ((qSize == 0) || (nmemb == 0) || ((qSize * nmemb) < 1))
    {
        _ReadMutex.unlock();
        return 0;
    }

    auto qLength = qStream->_Length;
    if (qLength > qSize * nmemb)
    {
        qLength = qSize * nmemb;
    }

    std::copy_n(qStream->_Data, qLength, qTarget);
    qStream->_Data += qLength;
    qStream->_Length -= qLength;
    _ReadMutex.unlock();
    return qLength;
}

int RaptorUploadingWorker::invokeItemPartialUploadCommonProgressCallback(void *qData,
                                                                         curl_off_t dltotal,
                                                                         curl_off_t dlnow,
                                                                         curl_off_t ultotal,
                                                                         curl_off_t ulnow)
{
    Q_UNUSED(dltotal)
    Q_UNUSED(dlnow)
    Q_UNUSED(ultotal)
    _ProgressMutex.lock();
    const auto qInstance = static_cast<RaptorUploadingWorker *>(qData);
    const auto qLength = qInstance->invokeItemPartialUploadProgressCallback(ulnow);
    _ProgressMutex.unlock();
    return qLength;
}

QPair<quint16, QByteArray> RaptorUploadingWorker::invokeItemPartialUpload(const RaptorHttpPayload &qHttpPayload)
{
    _Curl = curl_easy_init();
    curl_easy_setopt(_Curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(_Curl, CURLOPT_SSL_VERIFYHOST, 0L);
    if (const auto qIPResolve = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::IPResolve).toString();
        qIPResolve == Setting::Network::Auto)
    {
        curl_easy_setopt(_Curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);
    } else if (qIPResolve == Setting::Network::IPV4)
    {
        curl_easy_setopt(_Curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    } else if (qIPResolve == Setting::Network::IPV6)
    {
        curl_easy_setopt(_Curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
    }

    const auto qCurlUrl = curl_url();
    curl_url_set(qCurlUrl, CURLUPART_URL, qHttpPayload._Url.toStdString().c_str(), 0L);
    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                           Setting::Network::Proxy).toBool())
    {
        if (const auto qProxyEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                         Setting::Network::ProxyEngine).toString();
            qProxyEngine == Setting::Network::HTTP)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        } else if (qProxyEngine == Setting::Network::HTTP1_0)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
        } else if (qProxyEngine == Setting::Network::HTTPS)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
        } else if (qProxyEngine == Setting::Network::HTTPS2)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
        } else if (qProxyEngine == Setting::Network::SOCKS4)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
        } else if (qProxyEngine == Setting::Network::SOCKS5)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        }

        const auto qProxyHost = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyHost).toString();
        const auto qProxyPort = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyPort).toUInt();
        curl_easy_setopt(_Curl, CURLOPT_PROXY, qProxyHost.toStdString().c_str());
        curl_easy_setopt(_Curl, CURLOPT_PROXYPORT, qProxyPort);
        if (const auto qProxyUsername = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyUsername).toString();
            !qProxyUsername.isEmpty())
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYUSERNAME, qProxyUsername.toStdString().c_str());
        }
        if (const auto qProxyPassword = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyPassword).toString();
            !qProxyPassword.isEmpty())
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYPASSWORD, qProxyPassword.toStdString().c_str());
        }
    }

    for (auto qIterator = qHttpPayload._Query.begin(); qIterator != qHttpPayload._Query.end(); ++qIterator)
    {
        const auto qQuery = qIterator.key().toStdString() + "=" + qIterator.value().toString().toStdString();
        curl_url_set(qCurlUrl, CURLUPART_QUERY, qQuery.data(), CURLU_APPENDQUERY);
    }

    const auto qRow = qHttpPayload._Body.toJson().toStdString();
    curl_easy_setopt(_Curl, CURLOPT_POSTFIELDS, qRow.c_str());
    auto qUrl = static_cast<char *>(Q_NULLPTR);
    curl_url_get(qCurlUrl, CURLUPART_URL, &qUrl, 0L);
    curl_easy_setopt(_Curl, CURLOPT_URL, qUrl);

    auto qStream = static_cast<Stream *>(Q_NULLPTR);
    auto qContent = qHttpPayload._Content.toStdString();
    if (!qContent.empty())
    {
        qStream = new Stream();
        qStream->_Data = qContent.data();
        qStream->_Length = qContent.length();
        curl_easy_setopt(_Curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(_Curl, CURLOPT_READDATA, qStream);
        curl_easy_setopt(_Curl, CURLOPT_READFUNCTION, invokeItemPartialUploadReadCallback);
        curl_easy_setopt(_Curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(_Curl, CURLOPT_XFERINFODATA, static_cast<void *>(this));
        curl_easy_setopt(_Curl, CURLOPT_XFERINFOFUNCTION, invokeItemPartialUploadCommonProgressCallback);
        curl_easy_setopt(_Curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(qContent.length()));
        if (_Item._Limit > 0)
        {
            curl_easy_setopt(_Curl, CURLOPT_MAX_SEND_SPEED_LARGE, _Item._Limit);
        }
    }

    curl_easy_setopt(_Curl, CURLOPT_USERAGENT, RaptorHttpHeader::UserAgent);
    auto qHeader = static_cast<curl_slist *>(Q_NULLPTR);
    for (auto qIterator = qHttpPayload._Header.begin(); qIterator != qHttpPayload._Header.end(); ++qIterator)
    {
        qHeader = curl_slist_append(qHeader, (qIterator.key().toStdString() + ": " + qIterator.value().toString().toStdString()).data());
    }

    curl_easy_setopt(_Curl, CURLOPT_HTTPHEADER, qHeader);
    auto qBody = QByteArray();
    curl_easy_setopt(_Curl, CURLOPT_WRITEDATA, &qBody);
    curl_easy_setopt(_Curl, CURLOPT_WRITEFUNCTION, &RaptorUploadingWorker::invokeItemPartialUploadWriteCallback);
    auto qStatus = quint32();
    curl_easy_perform(_Curl);
    curl_easy_getinfo(_Curl, CURLINFO_RESPONSE_CODE, &qStatus);
    qFree(qStream)
    curl_slist_free_all(qHeader);
    curl_url_cleanup(qCurlUrl);
    curl_easy_cleanup(_Curl);
    return qMakePair(qStatus, qBody);
}

void RaptorUploadingWorker::invokeItemComplete(RaptorTransferItem &item)
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/v2/file/complete";
    qUseHeaderDefault(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = item._Space;
    qRow["upload_id"] = item._WorkerId;
    qRow["file_id"] = item._LeafId;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto qFile = QFile(QString("%1/%2").arg(_Item._Path, _Item._Name));
    if (!qFile.open(QIODevice::ReadOnly))
    {
        const auto qError = QStringLiteral("无法打开文件: %1").arg(_Item._Name);
        qCritical() << qError;
        auto output = RaptorOutput();
        output._State = true;
        output._Message = qError;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto qHash = QCryptographicHash{QCryptographicHash::Sha1};
    auto qComputed = quint64{0};
    auto qContinue = true;
    while (!qFile.atEnd())
    {
        if (_Paused)
        {
            qContinue = false;
            _Item._Status = QStringLiteral("已暂停");
            qFile.close();
            Q_EMIT itemPaused(QVariant::fromValue<RaptorTransferItem>(_Item));
            break;
        }

        if (_Cancel)
        {
            qContinue = false;
            qFile.close();
            Q_EMIT itemCancelled(QVariant::fromValue<RaptorTransferItem>(_Item));
            break;
        }

        auto qData = qFile.read(64 * 1024);
        qComputed += qData.length();
        qHash.addData(qData);
        const auto qRadio = qRound(static_cast<float>(qComputed) / static_cast<float>(_Item._Byte) * 10000) / 100.0;
        _Item._Status = QStringLiteral("计算指纹 %1%").arg(qRadio, 0, 'f', 2);
        Q_EMIT itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(_Item));
    }

    if (!qContinue)
    {
        return;
    }

    _Item._Status = "合并分片";
    Q_EMIT itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(_Item));
    auto output = RaptorOutput();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = true;
        output._Message = qError;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        output._State = true;
        output._Message = QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (qDocument["content_hash"].toString().compare(qHash.result().toHex(), Qt::CaseInsensitive) != 0)
    {
        const auto qErros = QStringLiteral("指纹不匹配: %1").arg(QStringLiteral("%1/%2").arg(_Item._Path, _Item._Name));
        qCritical() << qErros;
        output._State = true;
        output._Message = qErros;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (qDocument["size"].toVariant().toULongLong() != item._Transferred)
    {
        const auto qErros = QStringLiteral("大小不匹配: %1").arg(QStringLiteral("%1/%2").arg(_Item._Path, _Item._Name));
        qCritical() << qErros;
        output._State = true;
        output._Message = qErros;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    qInfo() << QStringLiteral("%1 完成上传。").arg(_Item._Name);
    auto qCreated = QDateTime::fromString(qDocument["created_at"].toString(), "yyyy-MM-ddTHH:mm:ss.zzzZ");
    qCreated.setTimeSpec(Qt::UTC);
    item._Created = qCreated.toLocalTime().toString("yyyy-MM-dd hh:mm:ss");
    Q_EMIT itemCompleted(QVariant::fromValue<RaptorTransferItem>(item));
}

void RaptorUploadingWorker::onItemPausing(const QVariant &qVariant)
{
    if (const auto item = qVariant.value<RaptorTransferItem>();
        _Item == item)
    {
        _Paused = true;
    }
}

void RaptorUploadingWorker::onItemCanceling(const QVariant &qVariant)
{
    if (const auto item = qVariant.value<RaptorTransferItem>();
        _Item == item)
    {
        _Cancel = true;
    }
}
