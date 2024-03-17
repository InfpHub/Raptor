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
    } else if (_Cancel)
    {
        qInfo() << QStringLiteral("%1 取消下载。").arg(_Item._Name);
    }
}

void RaptorDownloadingWorker::run()
{
    qInfo() << QStringLiteral("%1 开始下载。").arg(_Item._Name);

#ifdef Q_OS_WIN
    if (_Item._Path.length() != 3)
    {
        // 非 Windows 磁盘根目录
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
#endif

    if (_Item._Transferred == _Item._Byte)
    {
        // 最近一次传输中在指纹校验阶段执行了暂停
        auto qCount = 0;
        for (const auto &item: _Item._Partials)
        {
            if (const auto qFileInfo = QFileInfo(QStringLiteral("%1/%2").arg(item._Path, item._Name));
                qFileInfo.exists())
            {
                if (qFileInfo.size() == item._End - item._Offset + 1)
                {
                    ++qCount;
                }
            }
        }

        if (qCount == _Item._Partials.length())
        {
            invokeItemComplete();
            return;
        }
    }

    const auto [qError, qUrl] = RaptorFileSuite::invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), _Item._Id);
    if (!qError.isEmpty())
    {
        const auto qErros = QStringLiteral("%1 地址刷新失败: %2").arg(_Item._Name, qError);
        qCritical() << qErros;
        auto output = RaptorOutput();
        output._State = true;
        output._Message = qErros;
        output._Data = QVariant::fromValue<RaptorTransferItem>(_Item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    _Item._Url = qUrl;
    if (_Item._Partials.isEmpty())
    {
        const auto items = RaptorUtil::invokeItemRangeCompute(_Item._Byte, _Item._Parallel);
        for (auto &item: items)
        {
            auto iten = RaptorPartial();
            iten._LeafId = RaptorUtil::invokeUUIDGenerate();
            iten._Name = QStringLiteral("%1.rpf").arg(iten._LeafId);
            iten._Offset = item.first;
            iten._End = item.second;
            const auto qDir = QDir(QStringLiteral("%1/%2").arg(_Item._Path, _Item._LeafId));
            if (!qDir.exists())
            {
                if (!qDir.mkpath(qDir.absolutePath()))
                {
                    qCritical() << QStringLiteral("无法创建目录: %1。").arg(qDir.absolutePath());
                    return;
                }
            }

            iten._Path = qDir.absolutePath();
            _Item._Partials.enqueue(iten);
        }
    }

    invokeItemDownload();
    if (!_Paused && !_Cancel)
    {
        invokeItemComplete();
    }
}

void RaptorDownloadingWorker::invokeInstanceInit()
{
    setAutoDelete(true);
    uv_loop_init(&_Loop);
    uv_timer_init(&_Loop, &_Timer);
    _Timer.data = this;
    _Multi = curl_multi_init();
}

void RaptorDownloadingWorker::invokeItemDownload()
{
    curl_multi_setopt(_Multi, CURLMOPT_SOCKETFUNCTION, invokeItemCommonSocketCallback);
    curl_multi_setopt(_Multi, CURLMOPT_SOCKETDATA, static_cast<void *>(this));
    curl_multi_setopt(_Multi, CURLMOPT_TIMERFUNCTION, invokeItemCommonTimerCallback);
    curl_multi_setopt(_Multi, CURLMOPT_TIMERDATA, static_cast<void *>(this));
    for (const auto &item: _Item._Partials)
    {
        auto qOffset = item._Offset;
        if (item._Transferred == item._End - item._Offset + 1)
        {
            continue;
        }

        if (item._Transferred != 0)
        {
            qOffset += item._Transferred;
        }

        const auto qCurl = curl_easy_init();
        curl_easy_setopt(qCurl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(qCurl, CURLOPT_URL, _Item._Url.toStdString().c_str());
        if (const auto qIPResolve = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                       Setting::Network::IPResolve).toString();
            qIPResolve == Setting::Network::Auto)
        {
            curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);
        } else if (qIPResolve == Setting::Network::IPV4)
        {
            curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
        } else if (qIPResolve == Setting::Network::IPV6)
        {
            curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
        }

        if (RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                               Setting::Network::Proxy).toBool())
        {
            if (const auto qProxyEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                             Setting::Network::ProxyEngine).toString();
                qProxyEngine == Setting::Network::HTTP)
            {
                curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
            } else if (qProxyEngine == Setting::Network::HTTP1_0)
            {
                curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
            } else if (qProxyEngine == Setting::Network::HTTPS)
            {
                curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
            } else if (qProxyEngine == Setting::Network::HTTPS2)
            {
                curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
            } else if (qProxyEngine == Setting::Network::SOCKS4)
            {
                curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
            } else if (qProxyEngine == Setting::Network::SOCKS5)
            {
                curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
            }

            const auto qProxyHost = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                       Setting::Network::ProxyHost).toString();
            const auto qProxyPort = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                       Setting::Network::ProxyPort).toUInt();
            curl_easy_setopt(qCurl, CURLOPT_PROXY, qProxyHost.toStdString().c_str());
            curl_easy_setopt(qCurl, CURLOPT_PROXYPORT, qProxyPort);
            if (const auto qProxyUserName = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                               Setting::Network::ProxyUserName).toString();
                !qProxyUserName.isNull())
            {
                curl_easy_setopt(qCurl, CURLOPT_PROXYUSERNAME, qProxyUserName.toStdString().c_str());
            }
            if (const auto qProxyPassword = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                               Setting::Network::ProxyPassword).toString();
                !qProxyPassword.isNull())
            {
                curl_easy_setopt(qCurl, CURLOPT_PROXYPASSWORD, qProxyPassword.toStdString().c_str());
            }
        }

        if (_Item._Limit > 1)
        {
            curl_easy_setopt(qCurl, CURLOPT_MAX_RECV_SPEED_LARGE, _Item._Limit / _Item._Partials.length() / 1024);
        }

        curl_easy_setopt(qCurl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(qCurl, CURLOPT_RANGE, QStringLiteral("%1-%2").arg(qOffset).arg(item._End).toStdString().c_str());
        curl_easy_setopt(qCurl, CURLOPT_USERAGENT, RaptorHttpHeader::UserAgent);
        const auto qPayload = new Payload();
        qPayload->_Curl = qCurl;
        qPayload->_Worker = this;
        qPayload->_Item = item;
        qPayload->_File.setFileName(QStringLiteral("%1/%2").arg(item._Path, item._Name));
        qPayload->_File.open(QIODevice::WriteOnly | QIODevice::Append);
        _Payloads.enqueue(qPayload);
        curl_easy_setopt(qCurl, CURLOPT_PRIVATE, static_cast<void *>(qPayload));
        curl_easy_setopt(qCurl, CURLOPT_WRITEDATA, static_cast<void *>(qPayload));
        curl_easy_setopt(qCurl, CURLOPT_WRITEFUNCTION, invokeItemCommonWriteCallback);
        curl_easy_setopt(qCurl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(qCurl, CURLOPT_XFERINFODATA, static_cast<void *>(qPayload));
        curl_easy_setopt(qCurl, CURLOPT_XFERINFOFUNCTION, invokeItemCommonProgressCallback);
        curl_multi_add_handle(_Multi, qCurl);
    }

    uv_run(&_Loop, UV_RUN_DEFAULT);
    curl_multi_cleanup(_Multi);
}

void RaptorDownloadingWorker::invokeItemComplete()
{
    auto qFile = QFile(QStringLiteral("%1/%2").arg(_Item._Path, _Item._Name));
    if (!qFile.open(QIODevice::ReadWrite))
    {
        qCritical() << QStringLiteral("文件打开失败: %1。合并文件无法继续!").arg(QStringLiteral("%1/%2").arg(_Item._Path, _Item._Name));
        return;
    }

    // 校验各个分片是否完整
    for (auto i = 0; i < _Item._Partials.length(); ++i)
    {
        auto item = _Item._Partials[i];
        _Item._Status = QStringLiteral("校验分片 %1/%2").arg(i).arg(_Item._Partials.length());
        Q_EMIT itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(_Item));
        if (item._Transferred != item._End - item._Offset + 1)
        {
            // 分片不完整
            item._Transferred = 0;
            _Item._Partials.replace(i, item);
            uv_loop_close(&_Loop);
            invokeItemDownload();
            return;
        }
    }

    // 合并各个分片
    auto qContinue = true;
    while (!_Item._Partials.isEmpty())
    {
        if (_Paused)
        {
            qContinue = false;
            qFile.close();
            qFile.remove();
            _Item._Status = QStringLiteral("已暂停");
            Q_EMIT itemPaused(QVariant::fromValue<RaptorTransferItem>(_Item));
            break;
        }

        if (_Cancel)
        {
            qContinue = false;
            qFile.close();
            qFile.remove();
            Q_EMIT itemCancelled(QVariant::fromValue<RaptorTransferItem>(_Item));
            break;
        }

        const auto item = _Item._Partials.dequeue();
        auto qFilf = QFile(QStringLiteral("%1/%2").arg(item._Path, item._Name));
        if (!qFilf.open(QIODevice::ReadOnly))
        {
            qCritical() << QStringLiteral("分片读取失败: %1。合并文件无法继续!").arg(QStringLiteral("%1/%2").arg(item._Path, item._Name));
            uv_loop_close(&_Loop);
            return;
        }

        while (!qFilf.atEnd())
        {
            qFile.write(qFilf.read(64 * 1024));
        }

        qFilf.close();
        qFilf.remove();
    }

    if (!qContinue)
    {
        uv_loop_close(&_Loop);
        return;
    }

    // 移除临时目录
    if (auto qDir = QDir(QStringLiteral("%1/%2").arg(_Item._Path, _Item._LeafId));
        qDir.exists())
    {
        qDir.removeRecursively();
    }

    // 最终指纹校验
    auto qHash = QCryptographicHash(QCryptographicHash::Sha1);
    auto qComputed = static_cast<quint64>(0);
    qFile.seek(0);
    while (!qFile.atEnd())
    {
        if (_Paused)
        {
            qContinue = false;
            qFile.close();
            _Item._Status = QStringLiteral("已暂停");
            Q_EMIT itemPaused(QVariant::fromValue<RaptorTransferItem>(_Item));
            break;
        }

        if (_Cancel)
        {
            qContinue = false;
            qFile.close();
            qFile.remove();
            Q_EMIT itemCancelled(QVariant::fromValue<RaptorTransferItem>(_Item));

            break;
        }

        const auto qData = qFile.read(64 * 1024);
        qComputed += qData.length();
        qHash.addData(qData);
        const auto qRadio = qRound(static_cast<float>(qComputed) / static_cast<float>(_Item._Byte) * 10000) / 100.0;
        _Item._Status = QStringLiteral("计算指纹 %1%").arg(qRadio, 0, 'f', 2);
        Q_EMIT itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(_Item));
    }

    if (!qContinue)
    {
        uv_loop_close(&_Loop);
        return;
    }

    if (_Item._SHA1.compare(qHash.result().toHex(), Qt::CaseInsensitive) != 0)
    {
        // 指纹校验失败
        qFile.close();
        qFile.remove();
        while (!_Item._Partials.isEmpty())
        {
            const auto item = _Item._Partials.dequeue();
            QFile::remove(QStringLiteral("%1/%2").arg(item._Path, item._Name));
        }

        run();
        return;
    }

    qInfo() << QStringLiteral("%1 完成下载。").arg(_Item._Name);
    Q_EMIT itemCompleted(QVariant::fromValue<RaptorTransferItem>(_Item));
    qFile.close();
    uv_loop_close(&_Loop);
}

void RaptorDownloadingWorker::invokeItemCommonPerformCallback(uv_poll_t *qReq,
                                                              int qStatus,
                                                              int qEvents)
{
    auto qStillRunning = static_cast<int>(0);
    auto qFlags = static_cast<int>(0);
    auto qContext = static_cast<Context *>(Q_NULLPTR);
    if (qEvents & UV_READABLE)
    {
        qFlags |= CURL_CSELECT_IN;
    }

    if (qEvents & UV_WRITABLE)
    {
        qFlags |= CURL_CSELECT_OUT;
    }

    qContext = static_cast<Context *>(qReq->data);
    curl_multi_socket_action(qContext->_Worker->_Multi, qContext->_SocketFD, qFlags, &qStillRunning);
    auto qMessage = static_cast<CURLMsg *>(Q_NULLPTR);
    auto qPending = static_cast<int>(0);
    while ((qMessage = curl_multi_info_read(qContext->_Worker->_Multi, &qPending)))
    {
        switch (qMessage->msg)
        {
            case CURLMSG_DONE:
            {
                const auto qCurl = qMessage->easy_handle;
                auto qPayload = static_cast<void *>(Q_NULLPTR);
                curl_easy_getinfo(qCurl, CURLINFO_PRIVATE, &qPayload);
                static_cast<Payload *>(qPayload)->_File.close();
                curl_multi_remove_handle(qContext->_Worker->_Multi, qCurl);
                curl_easy_cleanup(qCurl);
                break;
            }
            default:
                break;
        }
    }
}

void RaptorDownloadingWorker::invokeItemCommonUVCloseCallback(uv_handle_t *qHandle)
{
    auto qContext = static_cast<Context *>(qHandle->data);
    FREE(qContext);
}

int RaptorDownloadingWorker::invokeItemCommonSocketCallback(CURL *qCurl,
                                                            curl_socket_t qSocket,
                                                            int qWhat,
                                                            void *qData,
                                                            void *socketp)
{
    Q_UNUSED(qCurl)
    const auto qInstance = static_cast<RaptorDownloadingWorker *>(qData);
    auto qContext = static_cast<Context *>(Q_NULLPTR);
    auto qEvents = static_cast<int>(0);
    switch (qWhat)
    {
        case CURL_POLL_IN:
        case CURL_POLL_OUT:
        case CURL_POLL_INOUT:
            if (socketp)
            {
                qContext = static_cast<Context *>(socketp);
            } else
            {
                qContext = new Context();
                qContext->_SocketFD = qSocket;
                uv_poll_init_socket(&qInstance->_Loop, &qContext->_Poll, qSocket);
                qContext->_Poll.data = qContext;
                qContext->_Worker = qInstance;
            }

            curl_multi_assign(qInstance->_Multi, qSocket, qContext);
            if (qWhat != CURL_POLL_IN)
            {
                qEvents |= UV_WRITABLE;
            }

            if (qWhat != CURL_POLL_OUT)
            {
                qEvents |= UV_READABLE;
            }

            uv_poll_start(&qContext->_Poll, qEvents, invokeItemCommonPerformCallback);
            break;
        case CURL_POLL_REMOVE:
            if (socketp)
            {
                uv_poll_stop(&(static_cast<Context *>(socketp))->_Poll);
                qContext = static_cast<Context *>(socketp);
                uv_close(reinterpret_cast<uv_handle_t *>(&qContext->_Poll), invokeItemCommonUVCloseCallback);
                curl_multi_assign(qInstance->_Multi, qSocket, Q_NULLPTR);
            }
            break;
        default:
            abort();
    }

    return 0;
}

void RaptorDownloadingWorker::invokeItemCommonTimeoutCallback(uv_timer_t *qTimer)
{
    const auto qInstance = static_cast<RaptorDownloadingWorker *>(qTimer->data);
    auto qStillRunning = static_cast<int>(0);
    curl_multi_socket_action(qInstance->_Multi, CURL_SOCKET_TIMEOUT, 0, &qStillRunning);
    auto qMessage = static_cast<CURLMsg *>(Q_NULLPTR);
    auto qPending = static_cast<int>(0);
    while ((qMessage = curl_multi_info_read(qInstance->_Multi, &qPending)))
    {
        switch (qMessage->msg)
        {
            case CURLMSG_DONE:
            {
                const auto qCurl = qMessage->easy_handle;
                curl_multi_remove_handle(qInstance->_Multi, qCurl);
                curl_easy_cleanup(qCurl);
                break;
            }
            default:
                break;
        }
    }
}

int RaptorDownloadingWorker::invokeItemCommonTimerCallback(CURLM *multi,
                                                           long qTimeout,
                                                           void *qData)
{
    const auto qInstance = static_cast<RaptorDownloadingWorker *>(qData);
    if (qTimeout < 0)
    {
        uv_timer_stop(&qInstance->_Timer);
    } else
    {
        if (qTimeout == 0)
        {
            qTimeout = 1;
        }

        uv_timer_start(&qInstance->_Timer, invokeItemCommonTimeoutCallback, qTimeout, 0);
    }

    return 0;
}

size_t RaptorDownloadingWorker::invokeItemCommonWriteCallback(char *qTarget,
                                                              size_t qSize,
                                                              size_t qNMemb,
                                                              void *qData)
{
    _WriteMutex.lock();
    if (qTarget == Q_NULLPTR)
    {
        _WriteMutex.unlock();
        return 0;
    }

    const auto qPayload = static_cast<Payload *>(qData);
    if (qPayload->_Worker->_Paused)
    {
        qPayload->_File.close();
        qPayload->_Worker->_Payloads.removeOne(qPayload);
        if (qPayload->_Worker->_Payloads.length() == 1)
        {
            // 最后一个分片暂停
            qPayload->_Worker->_Item._Status = QStringLiteral("已暂停");
            Q_EMIT qPayload->_Worker->itemPaused(QVariant::fromValue<RaptorTransferItem>(qPayload->_Worker->_Item));
        }

        _WriteMutex.unlock();
        return CURLE_ABORTED_BY_CALLBACK;
    }

    if (qPayload->_Worker->_Cancel)
    {
        qPayload->_File.close();
        qPayload->_File.remove();
        qPayload->_Worker->_Payloads.removeOne(qPayload);
        if (qPayload->_Worker->_Payloads.length() == 1)
        {
            // 最后一个分片取消
            Q_EMIT qPayload->_Worker->itemCancelled(QVariant::fromValue<RaptorTransferItem>(qPayload->_Worker->_Item));
        }

        _WriteMutex.unlock();
        return CURLE_ABORTED_BY_CALLBACK;
    }

    const auto qLength = qSize * qNMemb;
    if (qPayload->_File.isOpen())
    {
        if (qPayload->_File.write(qTarget, qLength) == -1)
        {
            qPayload->_File.close();
            const auto qError = QStringLiteral("数据写入失败: %1").arg(qPayload->_Worker->_Item._Name);
            qCritical() << qError;
            auto output = RaptorOutput();
            output._State = true;
            output._Message = qError;
            output._Data = QVariant::fromValue<RaptorTransferItem>(qPayload->_Worker->_Item);
            Q_EMIT qPayload->_Worker->itemErrored(QVariant::fromValue<RaptorOutput>(output));
            _WriteMutex.unlock();
            return CURLE_ABORTED_BY_CALLBACK;
        }

        qPayload->_File.flush();
    }

    _WriteMutex.unlock();
    return qLength;
}

int RaptorDownloadingWorker::invokeItemCommonProgressCallback(void *qData,
                                                              curl_off_t dltotal,
                                                              curl_off_t dlnow,
                                                              curl_off_t ultotal,
                                                              curl_off_t ulnow)
{
    Q_UNUSED(dlnow)
    Q_UNUSED(dltotal)
    Q_UNUSED(ulnow)
    Q_UNUSED(ultotal)
    _ProgressMutex.lock();
    if (const auto qPayload = static_cast<Payload *>(qData);
        !qPayload->_Worker->_Paused && !qPayload->_Worker->_Cancel)
    {
        if (dlnow != qPayload->_LastTransferred)
        {
            if (const auto qDifference = dlnow - qPayload->_LastTransferred;
                qDifference > 0)
            {
                qPayload->_Item._Transferred += qDifference;
                auto qTransferred = static_cast<qint64>(0);
                for (auto i = 0; i < qPayload->_Worker->_Item._Partials.length(); ++i)
                {
                    auto item = qPayload->_Worker->_Item._Partials[i];
                    if (qPayload->_Item == item)
                    {
                        item._Transferred = qPayload->_Item._Transferred;
                        qPayload->_Worker->_Item._Partials.replace(i, item);
                    }

                    qTransferred += item._Transferred;
                }

                if (const auto items = curl_multi_get_handles(qPayload->_Worker->_Multi);
                    items)
                {
                    auto qSpeed = static_cast<qint64>(0);
                    for (auto i = 0; items[i]; ++i)
                    {
                        auto qSpeee = static_cast<curl_off_t>(0);
                        const auto item = items[i];
                        curl_easy_getinfo(item, CURLINFO_SPEED_DOWNLOAD_T, &qSpeee);
                        qSpeed += qSpeee;
                    }

                    qPayload->_Worker->_Item._Transferred = qPayload->_Worker->_LastTransferred + qTransferred;
                    if (qSpeed > 0)
                    {
                        qPayload->_Worker->_Item._Speed = QVariant::fromValue<QString>(RaptorUtil::invokeStorageUnitConvert(qSpeed));
                        qPayload->_Worker->_Item._ETR = RaptorUtil::invokeTimeUnitConvert((qPayload->_Worker->_Item._Byte - qPayload->_Worker->_Item._Transferred) / qSpeed);
                        qPayload->_Worker->_Item._Status = QStringLiteral("%1 - %2").arg(qPayload->_Worker->_Item._Speed.value<QString>(), qPayload->_Worker->_Item._ETR);
                    }

                    Q_EMIT qPayload->_Worker->itemStatusChanged(QVariant::fromValue<RaptorTransferItem>(qPayload->_Worker->_Item));
                    curl_free(items);
                }
            }
        }

        qPayload->_LastTransferred = dlnow;
    }

    _ProgressMutex.unlock();
    return CURLE_OK;
}

void RaptorDownloadingWorker::onItemPausing(const QVariant &qVariant)
{
    if (const auto item = qVariant.value<RaptorTransferItem>();
        _Item == item)
    {
        _Paused = true;
    }
}

void RaptorDownloadingWorker::onItemCanceling(const QVariant &qVariant)
{
    if (const auto item = qVariant.value<RaptorTransferItem>();
        _Item == item)
    {
        _Cancel = true;
    }
}
