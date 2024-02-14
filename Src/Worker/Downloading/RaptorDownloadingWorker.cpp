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

#include "RaptorDownloadingWorker.h"

RaptorDownloadingWorker::RaptorDownloadingWorker(RaptorTransferItem item) : _Item(std::move(item)),
                                                                            _LastTransferred(0),
                                                                            _Paused(false),
                                                                            _Cancel(false)
{
    invokeInstanceInit();
}

RaptorDownloadingWorker::~RaptorDownloadingWorker()
{
    if (_Paused)
    {
        qInfo() << QStringLiteral("%1 暂停下载。").arg(_Item._Name);
    }
    else if (_Cancel)
    {
        qInfo() << QStringLiteral("%1 取消下载。").arg(_Item._Name);
    }
}

void RaptorDownloadingWorker::run()
{
    qInfo() << QStringLiteral("%1 开始下载。").arg(_Item._Name);
    if (_Item._Path.length() != 3)
    {
        // Windows 磁盘根目录特殊处理
        if (const auto qDir = QDir(_Item._Path);
            !qDir.exists())
        {
            if (const auto qState = qDir.mkpath(_Item._Path);
                !qState)
            {
                const auto qError = QStringLiteral("无法创建文件夹: %1").arg(_Item._Name);
                qCritical() << qError;
                auto output = RaptorOutput();
                output._State = true;
                output._Message = qError;
                output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
                Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
                return;
            }
        }
    }


    _File.setFileName(QStringLiteral("%1/%2").arg(_Item._Path, _Item._Name));
    if (_Item._Transferred == _Item._Byte)
    {
        // 最近一次传输中在指纹校验阶段执行了暂停
        if (!_File.exists())
        {
            const auto qError = QStringLiteral("文件可能被移动或删除: %1，无法校验指纹!").arg(_Item._Name);
            qCritical() << qError;
            auto output = RaptorOutput();
            output._State = true;
            output._Message = qError;
            output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
            Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        invokeItemComplete();
        return;
    }

    if (!_File.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        const auto qError = QStringLiteral("无法打开文件: %1/%2").arg(_Item._Path, _Item._Name);
        qCritical() << qError;
        auto output = RaptorOutput();
        output._State = true;
        output._Message = qError;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    if (_Item._Transferred != 0)
    {
        if (_Item._Transferred == _File.size())
        {
            _Item._Offset = _Item._Transferred;
        }
        else
        {
            // 文件被外部修改
            _File.seek(0);
        }
    }

    _Item._End = _Item._Byte - 1;
    const auto [_State, _Message, _Data] = RaptorFileSuite::invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), _Item._Id);
    if (!_State)
    {
        const auto qError = QStringLiteral("地址刷新失败: ").arg(_Message);
        qCritical() << qError;
        auto output = RaptorOutput();
        output._State = true;
        output._Message = qError;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    _Item._Url = _Data.value<QString>();
    if (invokeItemDownload() == RaptorHttpStatus::PartialContent && !_Paused && !_Cancel)
    {
        invokeItemComplete();
    }
}

void RaptorDownloadingWorker::invokeInstanceInit()
{
    setAutoDelete(true);
}

quint16 RaptorDownloadingWorker::invokeItemDownload()
{
    _Curl = curl_easy_init();
    curl_easy_setopt(_Curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(_Curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(_Curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(_Curl, CURLOPT_URL, _Item._Url.toStdString().data());
    if (_Item._Limit > 0)
    {
        curl_easy_setopt(_Curl, CURLOPT_MAX_RECV_SPEED_LARGE, _Item._Limit / 1024);
    }

    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                           Setting::Network::Proxy).toBool())
    {
        if (const auto qProxyEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                         Setting::Network::ProxyEngine).toString();
            qProxyEngine == Setting::Network::HTTP)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        }
        else if (qProxyEngine == Setting::Network::HTTP1_0)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
        }
        else if (qProxyEngine == Setting::Network::HTTPS)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
        }
        else if (qProxyEngine == Setting::Network::HTTPS2)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
        }
        else if (qProxyEngine == Setting::Network::SOCKS4)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
        }
        else if (qProxyEngine == Setting::Network::SOCKS5)
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        }

        const auto qProxyHost = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyHost).toString();
        const auto qProxyPort = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyPort).toUInt();
        curl_easy_setopt(_Curl, CURLOPT_PROXY, qProxyHost.toStdString().data());
        curl_easy_setopt(_Curl, CURLOPT_PROXYPORT, qProxyPort);
        if (const auto qProxyUserName = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyUserName).toString();
            !qProxyUserName.isNull())
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYUSERNAME, qProxyUserName.toStdString().data());
        }
        if (const auto qProxyPassword = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyPassword).toString();
            !qProxyPassword.isNull())
        {
            curl_easy_setopt(_Curl, CURLOPT_PROXYPASSWORD, qProxyPassword.toStdString().data());
        }
    }

    curl_easy_setopt(_Curl, CURLOPT_RANGE, QStringLiteral("%1-%2").arg(_Item._Offset).arg(_Item._End).toStdString().data());
    curl_easy_setopt(_Curl, CURLOPT_USERAGENT, RaptorHttpHeader::UserAgent);
    curl_easy_setopt(_Curl, CURLOPT_WRITEDATA, static_cast<void*>(this));
    curl_easy_setopt(_Curl, CURLOPT_WRITEFUNCTION, invokeItemDownloadCommonWriteCallback);
    curl_easy_setopt(_Curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(_Curl, CURLOPT_XFERINFODATA, static_cast<void*>(this));
    curl_easy_setopt(_Curl, CURLOPT_XFERINFOFUNCTION, invokeItemDownloadCommonProgressCallback);

    auto qStatus = quint32();
    curl_easy_perform(_Curl);
    curl_easy_getinfo(_Curl, CURLINFO_RESPONSE_CODE, &qStatus);
    curl_easy_cleanup(_Curl);
    return qStatus;
}

void RaptorDownloadingWorker::invokeItemComplete()
{
    _File.seek(0);
    auto qHash = QCryptographicHash(QCryptographicHash::Sha1);
    auto qComputed = quint64{0};
    auto qContinue = true;
    while (!_File.atEnd())
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
            _File.remove();
            Q_EMIT itemCancelled(QVariant::fromValue<RaptorTransferItem>(_Item));
            break;
        }

        auto qData = _File.read(64 * 1024);
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

    if (_Item._SHA1.compare(qHash.result().toHex(), Qt::CaseInsensitive) != 0)
    {
        // 指纹校验失败
        _File.close();
        _File.remove();
        _Item._Offset = 0;
        _Item._Transferred = 0;
        const auto [_State, _Message, _Data] = RaptorFileSuite::invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), _Item._Id);
        if (!_State)
        {
            auto output = RaptorOutput();
            output._State = true;
            output._Message = QStringLiteral("地址刷新失败: ").arg(_Message);
            output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
            Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
            return;
        }

        _Item._Url = _Data.value<QString>();
        run();
        return;
    }

    qInfo() << QStringLiteral("%1 完成下载。").arg(_Item._Name);
    Q_EMIT itemCompleted(QVariant::fromValue<RaptorTransferItem>(_Item));
    _File.close();
}

size_t RaptorDownloadingWorker::invokeItemDownloadWriteCallback(char* qTarget,
                                                                size_t qSize,
                                                                size_t nmemb)
{
    if (_Paused)
    {
        _File.close();
        _Item._Status = QStringLiteral("已暂停");
        Q_EMIT itemPaused(QVariant::fromValue<RaptorTransferItem>(_Item));
        return CURLE_ABORTED_BY_CALLBACK;
    }

    if (_Cancel)
    {
        _File.close();
        _File.remove();
        Q_EMIT itemCancelled(QVariant::fromValue<RaptorTransferItem>(_Item));
        return CURLE_ABORTED_BY_CALLBACK;
    }

    const auto qLength = qSize * nmemb;
    if (_File.isOpen())
    {
        if (_File.write(qTarget, qLength) == -1)
        {
            _File.close();
            const auto qError = QStringLiteral("数据写入失败: %1").arg(_Item._Name);
            qCritical() << qError;
            auto output = RaptorOutput();
            output._State = true;
            output._Message = qError;
            output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
            Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
            return CURLE_ABORTED_BY_CALLBACK;
        }

        _File.flush();
    }

    return qLength;
}

int RaptorDownloadingWorker::invokeItemDownloadProgressCallback(curl_off_t dltotal,
                                                                curl_off_t dlnow)
{
    Q_UNUSED(dltotal)
    if (!_Paused && !_Cancel)
    {
        if (dlnow != _LastTransferred)
        {
            if (const auto qDifference = static_cast<qint64>(dlnow - _LastTransferred);
                qDifference > 0)
            {
                _Item._Transferred += qDifference;
            }

            _LastTransferred = dlnow;
        }

        auto qSpeed = qint64{0};
        curl_easy_getinfo(_Curl, CURLINFO_SPEED_DOWNLOAD_T, &qSpeed);
        _Item._Speed = QVariant::fromValue<QString>(RaptorUtil::invokeStorageUnitConvert(qSpeed));
        if (qSpeed > 0)
        {
            const auto qEtr = RaptorUtil::invokeTimeUnitConvert(
                (_Item._Byte - _Item._Transferred) / qSpeed);
            _Item._ETR = qEtr;
            _Item._Status = QStringLiteral("%1 - %2").arg(_Item._Speed.toString(), qEtr);
        }
        else
        {
            _Item._ETR = QStringLiteral("计算中");
            _Item._Status = QStringLiteral("处理中");
        }

        Q_EMIT itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(_Item));
    }

    return CURLE_OK;
}

size_t RaptorDownloadingWorker::invokeItemDownloadCommonWriteCallback(char* qTarget,
                                                                      size_t size,
                                                                      size_t nmemb,
                                                                      void* qData)
{
    if (qTarget == Q_NULLPTR)
    {
        return 0;
    }

    return static_cast<RaptorDownloadingWorker*>(qData)->invokeItemDownloadWriteCallback(qTarget, size, nmemb);
}

int RaptorDownloadingWorker::invokeItemDownloadCommonProgressCallback(void* qData,
                                                                      curl_off_t dltotal,
                                                                      curl_off_t dlnow,
                                                                      curl_off_t ultotal,
                                                                      curl_off_t ulnow)
{
    Q_UNUSED(ultotal)
    Q_UNUSED(ulnow)
    return static_cast<RaptorDownloadingWorker*>(qData)->invokeItemDownloadProgressCallback(dltotal, dlnow);
}

void RaptorDownloadingWorker::onItemPausing(const QVariant& qVariant)
{
    if (const auto item = qVariant.value<RaptorTransferItem>();
        _Item == item)
    {
        _Paused = true;
    }
}

void RaptorDownloadingWorker::onItemCanceling(const QVariant& qVariant)
{
    if (const auto item = qVariant.value<RaptorTransferItem>();
        _Item == item)
    {
        _Cancel = true;
    }
}
