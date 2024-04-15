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

#include "RaptorAriaSuite.h"

Q_GLOBAL_STATIC(RaptorAriaSuite, _AriaSuite)

RaptorAriaSuite::RaptorAriaSuite()
{
    invokeInstanceInit();
    invokeSlotInit();
}

RaptorAriaSuite *RaptorAriaSuite::invokeSingletonGet()
{
    return _AriaSuite();
}

void RaptorAriaSuite::invokeStop() const
{
    _ItemsTimer->stop();
    _ItemsStatusTimer->stop();
    if (_WebSocket->state() != QAbstractSocket::ConnectedState)
    {
        _WebSocket->abort();
        return;
    }

    auto qRow = QJsonObject();
    qRow["jsonrpc"] = "2.0";
    qRow["id"] = RaptorUtil::invokeUUIDGenerate();
    qRow["method"] = "aria2.forcePause";
    auto qParam = QJsonArray();
    if (!_Token.isEmpty())
    {
        qParam << QStringLiteral("token:%1").arg(_Token);
    }

    for (const auto &item: _ItemsQueue)
    {
        qParam << item._LeafId;
    }

    qRow["params"] = qParam;
    _WebSocket->sendTextMessage(QJsonDocument(qRow).toJson());
    _WebSocket->close();
}

void RaptorAriaSuite::invokeInstanceInit()
{
    _ItemsTimer = new QTimer(this);
    _ItemsTimer->setInterval(100);
    _ItemsStatusTimer = new QTimer(this);
    _ItemsStatusTimer->setInterval(500);
    _WebSocket = new QWebSocket();
    _WebSocket->setParent(this);

    auto qEndPoint = QString();
    const auto qAriaHost = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                              Setting::Download::AriaHost).toString();
    const auto qAriaPort = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                              Setting::Download::AriaPort).toString();
    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                           Setting::Download::AriaSSL).toBool())
    {
        qEndPoint = QStringLiteral("wss://%1:%2/jsonrpc").arg(qAriaHost, qAriaPort);
    } else
    {
        qEndPoint = QStringLiteral("ws://%1:%2/jsonrpc").arg(qAriaHost, qAriaPort);
    }

    if (const auto qError = RaptorHttpSuite::invokeItemWebSocketConnectTest(qEndPoint);
        !qError.isEmpty())
    {
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeAriaStatusSet", Q_ARG(QString, QStringLiteral(qCriticalTemplate).arg("Aria ⚯ 节点尚未在线")));
        return;
    }

    _WebSocket->open(QUrl(qEndPoint));
}

void RaptorAriaSuite::invokeSlotInit() const
{
    connect(_WebSocket,
            &QWebSocket::stateChanged,
            this,
            &RaptorAriaSuite::onWebSocketStateChanged);

    connect(_WebSocket,
            &QWebSocket::errorOccurred,
            this,
            &RaptorAriaSuite::onWebSocketErrorOccurred);

    connect(_WebSocket,
            &QWebSocket::textMessageReceived,
            this,
            &RaptorAriaSuite::onWebSocketTextMessageReceived);

    connect(_ItemsTimer,
            &QTimer::timeout,
            this,
            &RaptorAriaSuite::onItemsTimerTimeout);

    connect(_ItemsStatusTimer,
            &QTimer::timeout,
            this,
            &RaptorAriaSuite::onItemStatusTimerTimeout);
}

void RaptorAriaSuite::invokeItemDownload(const RaptorTransferItem &item)
{
    const auto [qError, qUrl] = RaptorFileSuite::invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), item._Id);
    if (!qError.isEmpty())
    {
        _ItemsQueue.removeOne(item);
        auto output = RaptorOutput();
        output._State = true;
        output._Message = qError;
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto qRow = QJsonObject();
    qRow["jsonrpc"] = "2.0";
    qRow["id"] = RaptorUtil::invokeUUIDGenerate();
    qRow["method"] = "aria2.addUri";
    auto qParam = QJsonArray();
    if (!_Token.isEmpty())
    {
        qParam << QStringLiteral("token:%1").arg(_Token);
    }

    qParam << (QJsonArray() << qUrl);
    auto qOption = QJsonObject();
    qOption["gid"] = item._LeafId;
    qOption["dir"] = item._Path;
    qOption["out"] = item._Name;
    qOption["max-connection-per-server"] = item._Parallel;
    qOption["split"] = item._Parallel;
    qOption["checksum"] = QStringLiteral("sha-1=%1").arg(item._SHA1);
    qParam << qOption;
    qRow["params"] = qParam;
    _WebSocket->sendTextMessage(QJsonDocument(qRow).toJson());
}

void RaptorAriaSuite::onItemsLoading() const
{
    const auto [qError, items] = RaptorDownloadingSuite::invokeItemsLoad(false);
    auto output = RaptorOutput();
    output._State = qError.isEmpty();
    output._Message = qError;
    output._Data = QVariant::fromValue<QVector<RaptorTransferItem> >(items);
    Q_EMIT itemsLoaded(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorAriaSuite::onItemsDownloading(const QVariant &qVariant)
{
    if (_WebSocket->state() != QAbstractSocket::ConnectedState)
    {
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeCriticalEject", Q_ARG(QString, QStringLiteral(qCriticalTemplate).arg("Aria ⚯ 节点尚未在线")));
        return;
    }

    const auto input = qVariant.value<RaptorInput>();
    auto output = RaptorOutput();
    auto [qError, items] = RaptorDownloadingSuite::invokeItemsAssemble(qVariant);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsQueuing(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    for (auto &item: items)
    {
        if (item._Type != "folder")
        {
            item._Created = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item._Limit = input._Speed;
            item._Parallel = input._Parallel;
            continue;
        }

        if (RaptorStoreSuite::invokeAriaIsLocalHostGet())
        {
            if (auto qDir = QDir(item._Path);
                !qDir.exists())
            {
                Q_UNUSED(qDir.mkpath(item._Path))
            }
        }

        items.removeOne(item);
    }

    RaptorDownloadingSuite::invokeItemsSave(items, false);
    output._State = true;
    output._Data = QVariant::fromValue<QVector<RaptorTransferItem> >(items);
    Q_EMIT itemsQueuing(QVariant::fromValue<RaptorOutput>(output));
    if (!items.isEmpty() && !_ItemsTimer->isActive() && !_ItemsStatusTimer->isActive())
    {
        _ItemsTimer->start();
        _ItemsStatusTimer->start();
    }

    if (_ItemsQueue.length() < _Concurrent)
    {
        const auto qLimit = _ItemsQueue.length() - _Concurrent;
        const auto itens = RaptorDownloadingSuite::invokeItemsByLimitSelect(qLimit < 0 ? _Concurrent - _ItemsQueue.length() : qLimit, false);
        if (_ItemsQueue.isEmpty() && itens.isEmpty())
        {
            _ItemsStatusTimer->stop();
            return;
        }

        for (const auto &iten: itens)
        {
            if (_ItemsQueue.contains(iten))
            {
                continue;
            }

            RaptorDownloadingSuite::invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemDownload(iten);
        }
    }
}

void RaptorAriaSuite::onItemsPausing(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    for (const auto &qIndex: input._Indexes)
    {
        auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        if (_ItemsQueue.contains(item))
        {
            auto qRow = QJsonObject();
            qRow["jsonrpc"] = "2.0";
            qRow["id"] = RaptorUtil::invokeUUIDGenerate();
            qRow["method"] = "aria2.forcePause";
            auto qParam = QJsonArray();
            if (!_Token.isEmpty())
            {
                qParam << QStringLiteral("token:%1").arg(_Token);
            }

            qParam << item._LeafId;
            qRow["params"] = qParam;
            _WebSocket->sendTextMessage(QJsonDocument(qRow).toJson());
            continue;
        }

        item._Status = QStringLiteral("已暂停");
        RaptorDownloadingSuite::invokeItemUpdate(item, -1);
        auto output = RaptorOutput();
        output._State = true;
        output._Data = QVariant::fromValue<RaptorTransferItem>(item);
        Q_EMIT itemPaused(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorAriaSuite::onItemsResuming(const QVariant &qVariant)
{
    const auto input = qVariant.value<RaptorInput>();
    if (!input._Indexes.isEmpty() && !_ItemsTimer->isActive() && !_ItemsStatusTimer->isActive())
    {
        _ItemsTimer->start();
        _ItemsStatusTimer->start();
    }

    for (const auto &qIndex: input._Indexes)
    {
        auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        RaptorDownloadingSuite::invokeItemUpdate(item, 0);
        if (_ItemsQueue.length() >= _Concurrent)
        {
            continue;
        }

        if (_ItemsQueue.contains(item))
        {
            continue;
        }

        _ItemsQueue.enqueue(item);
        auto qRow = QJsonObject();
        qRow["jsonrpc"] = "2.0";
        qRow["id"] = RaptorUtil::invokeUUIDGenerate();
        qRow["method"] = "aria2.changeUri";
        auto qParam = QJsonArray();
        if (!_Token.isEmpty())
        {
            qParam << QStringLiteral("token:%1").arg(_Token);
        }

        const auto [qError, qUrl] = RaptorFileSuite::invokeItemUrlFetch(RaptorStoreSuite::invokeUserGet(), item._Id);
        if (!qError.isEmpty())
        {
            _ItemsQueue.removeOne(item);
            const auto qErros = QStringLiteral("%1 地址刷新失败: %2").arg(item._Name, qError);
            qCritical() << qErros;
            auto output = RaptorOutput();
            output._State = true;
            output._Message = qError;
            Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
            continue;
        }

        qParam << item._LeafId << 1 << QJsonArray() << (QJsonArray() << qUrl);
        qRow["params"] = qParam;
        _WebSocket->sendTextMessage(QJsonDocument(qRow).toJson());
        qRow["id"] = RaptorUtil::invokeUUIDGenerate();
        qRow["method"] = "aria2.unpause";
        qParam = QJsonArray();
        if (!_Token.isEmpty())
        {
            qParam << QStringLiteral("token:%1").arg(_Token);
        }

        qParam << item._LeafId;
        qRow["params"] = qParam;
        _WebSocket->sendTextMessage(QJsonDocument(qRow).toJson());
    }
}

void RaptorAriaSuite::onItemCancelling(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto item = input._Variant.value<RaptorTransferItem>();
    if (_ItemsQueue.contains(item))
    {
        auto qRow = QJsonObject();
        qRow["jsonrpc"] = "2.0";
        qRow["id"] = RaptorUtil::invokeUUIDGenerate();
        qRow["method"] = "aria2.forceRemove";
        auto qParam = QJsonArray();
        if (!_Token.isEmpty())
        {
            qParam << QStringLiteral("token:%1").arg(_Token);
        }

        qParam << item._LeafId;
        qRow["params"] = qParam;
        _WebSocket->sendTextMessage(QJsonDocument(qRow).toJson());
        return;
    }

    RaptorDownloadingSuite::invokeItemDelete(item);
    if (RaptorStoreSuite::invokeAriaIsLocalHostGet())
    {
        QFile::remove(QStringLiteral("%1/%2").arg(item._Path, item._Name));
    }
}

void RaptorAriaSuite::onWebSocketStateChanged(const QAbstractSocket::SocketState &qState)
{
    switch (qState)
    {
        case QAbstractSocket::ConnectedState:
        {
            QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                      "invokeAriaStatusSet", Q_ARG(QString, QStringLiteral(qSuccessTemplate).arg(QStringLiteral("Aria ⚯ %1").arg(RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Download,
                                                                       Setting::Download::AriaHost).toString()))));
            _Token = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                        Setting::Download::AriaToken).toString();
            auto qRow = QJsonObject();
            qRow["jsonrpc"] = "2.0";
            qRow["id"] = RaptorUtil::invokeUUIDGenerate();
            qRow["method"] = "aria2.getGlobalOption";
            auto qParam = QJsonArray();
            if (!_Token.isEmpty())
            {
                qParam << QStringLiteral("token:%1").arg(_Token);
            }

            qRow["params"] = qParam;
            _WebSocket->sendTextMessage(QJsonDocument(qRow).toJson());
            break;
        }
        case QAbstractSocket::UnconnectedState:
        {
            QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                      "invokeAriaStatusSet", Q_ARG(QString, QStringLiteral(qCriticalTemplate).arg("Aria ⚯ 节点尚未在线")));
            break;
        }
        default:
            break;
    }
}

void RaptorAriaSuite::onWebSocketErrorOccurred(const QAbstractSocket::SocketError &qError) const
{
    if (const auto qErros = QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(qError);
        qErros)
    {
        qCritical() << qErros;
        auto output = RaptorOutput();
        output._State = true;
        output._Message = qErros;
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorAriaSuite::onWebSocketTextMessageReceived(const QString &qBody)
{
    if (const auto qDocument = QJsonDocument::fromJson(qBody.toUtf8());
        !qDocument["method"].isUndefined())
    {
        if (const auto qMethod = qDocument["method"].toString();
            qMethod == "aria2.onDownloadStart")
        {
        } else if (qMethod == "aria2.onDownloadPause")
        {
            const auto qParam = qDocument["params"];
            if (qParam.isUndefined())
            {
                return;
            }

            if (!qParam.isArray())
            {
                return;
            }

            if (qParam.toArray().isEmpty())
            {
                return;
            }

            for (auto i = 0; i < _ItemsQueue.length(); ++i)
            {
                for (const auto &item: qParam.toArray())
                {
                    const auto qId = item.toObject()["gid"].toString();
                    if (auto iten = _ItemsQueue[i];
                        iten._LeafId == qId)
                    {
                        iten._Status = QStringLiteral("已暂停");
                        _ItemsQueue.removeOne(iten);
                        RaptorDownloadingSuite::invokeItemUpdate(iten, -1);
                        auto output = RaptorOutput();
                        output._State = true;
                        output._Data = QVariant::fromValue<RaptorTransferItem>(iten);
                        Q_EMIT itemPaused(QVariant::fromValue<RaptorOutput>(output));
                        break;
                    }
                }
            }

            if (_ItemsQueue.isEmpty())
            {
                _ItemsTimer->stop();
                _ItemsStatusTimer->stop();
            }
        } else if (qMethod == "aria2.onDownloadError")
        {
            qDebug() << qBody;
            if (_ItemsQueue.isEmpty())
            {
                _ItemsTimer->stop();
                _ItemsStatusTimer->stop();
            }
        } else if (qMethod == "aria2.onDownloadStop")
        {
            const auto qParam = qDocument["params"];
            if (qParam.isUndefined())
            {
                return;
            }

            if (!qParam.isArray())
            {
                return;
            }

            if (qParam.toArray().isEmpty())
            {
                return;
            }

            for (auto i = 0; i < _ItemsQueue.length(); ++i)
            {
                for (const auto &item: qParam.toArray())
                {
                    const auto qId = item.toObject()["gid"].toString();
                    if (const auto iten = _ItemsQueue[i];
                        iten._LeafId == qId)
                    {
                        _ItemsQueue.removeOne(iten);
                        RaptorDownloadingSuite::invokeItemDelete(iten);
                        if (RaptorStoreSuite::invokeAriaIsLocalHostGet())
                        {
                            QFile::remove(QStringLiteral("%1/%2").arg(iten._Path, iten._Name));
                        }

                        break;
                    }
                }
            }

            if (_ItemsQueue.isEmpty())
            {
                _ItemsTimer->stop();
                _ItemsStatusTimer->stop();
            }
        } else if (qMethod == "aria2.onDownloadComplete")
        {
            const auto qParam = qDocument["params"];
            if (qParam.isUndefined())
            {
                return;
            }

            if (!qParam.isArray())
            {
                return;
            }

            if (qParam.toArray().isEmpty())
            {
                return;
            }

            for (auto i = 0; i < _ItemsQueue.length(); ++i)
            {
                for (const auto &item: qParam.toArray())
                {
                    const auto qId = item.toObject()["gid"].toString();
                    if (const auto iten = _ItemsQueue[i]; iten._LeafId == qId)
                    {
                        _ItemsQueue.removeOne(iten);
                        RaptorDownloadingSuite::invokeItemDelete(iten);
                        auto output = RaptorOutput();
                        output._State = true;
                        output._Data = QVariant::fromValue<RaptorTransferItem>(iten);
                        Q_EMIT itemCompleted(QVariant::fromValue<RaptorOutput>(output));
                        break;
                    }
                }
            }

            if (_ItemsQueue.length() < _Concurrent)
            {
                const auto qLimit = _ItemsQueue.length() - _Concurrent;
                const auto items = RaptorDownloadingSuite::invokeItemsByLimitSelect(qLimit < 0 ? _Concurrent - _ItemsQueue.length() : qLimit);
                if (_ItemsQueue.isEmpty() && items.isEmpty())
                {
                    _ItemsStatusTimer->stop();
                    return;
                }

                for (auto &iten: items)
                {
                    if (_ItemsQueue.contains(iten))
                    {
                        continue;
                    }

                    RaptorDownloadingSuite::invokeItemUpdate(iten, 1);
                    _ItemsQueue.enqueue(iten);
                    invokeItemDownload(iten);
                }
            }

            if (_ItemsQueue.isEmpty())
            {
                _ItemsTimer->stop();
                _ItemsStatusTimer->stop();
            }
        }
    } else if (const auto qResult = qDocument["result"];
        !qResult.isUndefined())
    {
        if (qResult.isArray())
        {
            if (qResult.toArray().isEmpty())
            {
                return;
            }

            for (auto i = 0; i < _ItemsQueue.length(); ++i)
            {
                for (const auto &item: qResult.toArray())
                {
                    const auto iten = item.toObject();
                    const auto qId = iten["gid"].toString();
                    const auto qSpeed = iten["downloadSpeed"].toVariant().toULongLong();
                    if (qSpeed == 0)
                    {
                        continue;
                    }

                    if (auto iteo = _ItemsQueue[i];
                        iteo._LeafId == qId)
                    {
                        iteo._Transferred = iten["completedLength"].toVariant().toULongLong();
                        iteo._Speed = QVariant::fromValue<QString>(RaptorUtil::invokeStorageUnitConvert(qSpeed));
                        iteo._ETR = RaptorUtil::invokeTimeUnitConvert((iteo._Byte - iteo._Transferred) / qSpeed);
                        iteo._Status = QStringLiteral("%1 - %2").arg(iteo._Speed.value<QString>(), iteo._ETR);
                        _ItemsQueue.replace(i, iteo);
                    }
                }
            }
        } else if (qResult.isObject())
        {
            _Concurrent = qResult.toObject()["max-concurrent-downloads"].toString().toUInt();
        }
    } else if (!qDocument["error"].isUndefined())
    {
        const auto qError = qDocument["error"].toObject()["message"].toString();
        qCritical() << qError;
        const auto qRegularExpression = QRegularExpression(R"(\b([a-fA-F0-9]{16})\b)");
        auto item = RaptorTransferItem();
        if (const auto qRegularExpressionMatch = qRegularExpression.match(qError);
            qRegularExpressionMatch.hasMatch())
        {
            const auto qId = qRegularExpressionMatch.captured(1);
            for (auto i = 0; i < _ItemsQueue.length(); ++i)
            {
                if (const auto iten = _ItemsQueue[i];
                    iten._LeafId == qId)
                {
                    item = iten;
                    _ItemsQueue.removeAt(i);
                    break;
                }
            }
        }

        auto output = RaptorOutput();
        output._State = true;
        output._Message = qError;
        output._Data = QVariant::fromValue<RaptorTransferItem>(item);
        Q_EMIT itemErrored(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorAriaSuite::onItemsTimerTimeout() const
{
    auto qRow = QJsonObject();
    qRow["jsonrpc"] = "2.0";
    qRow["id"] = RaptorUtil::invokeUUIDGenerate();
    qRow["method"] = "aria2.tellActive";
    _WebSocket->sendTextMessage(QJsonDocument(qRow).toJson());
}

void RaptorAriaSuite::onItemStatusTimerTimeout() const
{
    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QQueue<RaptorTransferItem> >(_ItemsQueue);
    Q_EMIT itemsStatusChanged(QVariant::fromValue<RaptorOutput>(output));
}
