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

#include "RaptorDownloadingSuite.h"

Q_GLOBAL_STATIC(RaptorDownloadingSuite, _DownloadingSuite)

RaptorDownloadingSuite::RaptorDownloadingSuite()
{
    invokeInstanceInit();
    invokeSlotInit();
}

RaptorDownloadingSuite *RaptorDownloadingSuite::invokeSingletonGet()
{
    return _DownloadingSuite();
}

QPair<QString, QVector<RaptorFileItem> > RaptorDownloadingSuite::invokeItemsWalk(const QString &qParent, const QString &qMarker)
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.aliyundrive.com/v2/file/walk";
    USE_HEADER_DEFAULT(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["drive_id"] = RaptorStoreSuite::invokeUserGet()._Space;
    qRow["parent_file_id"] = qParent;
    qRow["limit"] = 1000;
    qRow["marker"] = qMarker;
    qRow["url_expire_sec"] = 86400;
    qHttpPayload._Body = QJsonDocument(qRow);
    auto items = QVector<RaptorFileItem>();
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qMakePair(qError, items);
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus != RaptorHttpStatus::OK)
    {
        qCritical() << qDocument.toJson();
        return qMakePair(QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString()), items);
    }

    const auto itens = qDocument["items"].toArray();
    const auto qMarkes = qDocument["next_marker"].toString();
    for (const auto &iten: itens)
    {
        const auto iteo = iten.toObject();
        if (iteo["status"].toString() != "available")
        {
            continue;
        }

        auto item = RaptorFileItem();
        item._Id = iteo["file_id"].toString();
        item._Parent = iteo["parent_file_id"].toString();
        item._Name = iteo["name"].toString();
        item._Space = iteo["drive_id"].toString();
        item._Domain = iteo["domain_id"].toString();
        item._Extension = iteo["file_extension"].toString();
        item._Type = iteo["type"].toString();
        item._SHA1 = iteo["content_hash"].toString();
        if (iteo["type"].toString() == "file")
        {
            const auto size = iteo["size"].toVariant().toULongLong();
            item._Byte = size;
            item._Size = RaptorUtil::invokeStorageUnitConvert(size);
            item._Icon = RaptorUtil::invokeIconMatch(item._Name);
        } else
        {
            item._Icon = RaptorUtil::invokeIconMatch(QString(), true);
        }

        item._Url = iteo["url"].toString();
        item._Created = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        item._Updated = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        items << item;
    }

    if (!qMarkes.isEmpty())
    {
        const auto [qErros, qChildren] = invokeItemsWalk(qParent, qMarkes);
        if (!qErros.isEmpty())
        {
            return qMakePair(qErros, items);
        }

        items << qChildren;
    }

    return qMakePair(QString(), items);
}

void RaptorDownloadingSuite::invokeStop()
{
    _ItemsStatusTimer->stop();
    for (const auto &item: _ItemsQueue)
    {
        Q_EMIT itemPausing(QVariant::fromValue<RaptorTransferItem>(item));
    }
}

void RaptorDownloadingSuite::invokeInstanceInit()
{
    _ItemsThreadPool = new QThreadPool(this);
    _ItemsStatusTimer = new QTimer(this);
    _ItemsStatusTimer->setInterval(500);
}

void RaptorDownloadingSuite::invokeSlotInit() const
{
    connect(_ItemsStatusTimer,
            &QTimer::timeout,
            this,
            &RaptorDownloadingSuite::onItemStatusTimerTimeout);
}

QVector<RaptorTransferItem> RaptorDownloadingSuite::invokeItemsByLimitSelect(const quint32 &qLimit)
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        SELECT * FROM Downloading WHERE State = 0 AND UserId = :UserId LIMIT :Limit
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
    qSQLQuery.bindValue(":Limit", qLimit);
    qSQLQuery.exec();
    auto items = QVector<RaptorTransferItem>();
    if (qSQLQuery.lastError().isValid())
    {
        qCritical() << qSQLQuery.lastError().text();
        return items;
    }
    while (qSQLQuery.next())
    {
        auto item = RaptorTransferItem();
        item._LeafId = qSQLQuery.value("LeafId").toString();
        item._Id = qSQLQuery.value("Id").toString();
        item._Name = qSQLQuery.value("Name").toString();
        item._Size = qSQLQuery.value("Size").toString();
        item._Created = qSQLQuery.value("Created").toString();
        item._Path = qSQLQuery.value("Path").toString();
        item._Byte = qSQLQuery.value("Byte").toULongLong();
        item._SHA1 = qSQLQuery.value("SHA1").toString();
        item._Icon = RaptorUtil::invokeIconMatch(item._Name);
        item._Limit = qSQLQuery.value("Limit").toLongLong();
        item._Parallel = qSQLQuery.value("Parallel").toUInt();
        items << item;
    }

    return items;
}

void RaptorDownloadingSuite::invokeItemsSave(QVector<RaptorTransferItem> &items)
{
    RaptorPersistenceSuite::invokeTransaction();
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        INSERT INTO Downloading ("Id", "Name", "Created", "Path", "Size", "Byte", "SHA1", "UserId", "Limit", "State", "Parallel")
        VALUES (:Id, :Name, :Created, :Path, :Size, :Byte, :SHA1, :UserId, :Limit, :State, :Parallel)
    )";
    for (auto &item: items)
    {
        if (const auto qFileInfo = QFileInfo(QStringLiteral("%1/%2").arg(item._Path, item._Name));
            qFileInfo.exists())
        {
            if (RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                   Setting::Download::AutoRename).toBool())
            {
                item._Name = QStringLiteral("RC[%1]-%2").arg(RaptorUtil::invokeUUIDGenerate().left(5), item._Name);
            } else
            {
                if (!QFile::remove(qFileInfo.absoluteFilePath()))
                {
                    qCritical() << QStringLiteral("无法移除文件，跳过 %1 下载!").arg(item._Name);
                    continue;
                }
            }
        }

        qSQLQuery.prepare(qSQL);
        qSQLQuery.bindValue(":Id", item._Id);
        qSQLQuery.bindValue(":Name", item._Name);
        qSQLQuery.bindValue(":Created", item._Created);
        qSQLQuery.bindValue(":Path", item._Path);
        qSQLQuery.bindValue(":Size", item._Size);
        qSQLQuery.bindValue(":Byte", item._Byte);
        qSQLQuery.bindValue(":SHA1", item._SHA1);
        qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
        qSQLQuery.bindValue(":Limit", item._Limit);
        qSQLQuery.bindValue(":State", 0);
        qSQLQuery.bindValue(":Parallel", item._Parallel);
        qSQLQuery.exec();
        if (qSQLQuery.lastError().isValid())
        {
            qCritical() << qSQLQuery.lastError().text();
        }

        qSQLQuery.next();
        item._LeafId = qSQLQuery.lastInsertId().toString();
    }

    RaptorPersistenceSuite::invokeCommit();
}

void RaptorDownloadingSuite::invokeItemDelete(const RaptorTransferItem &item)
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        DELETE FROM Downloading WHERE LeafId = :LeafId AND UserId = :UserId
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":LeafId", item._LeafId);
    qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
    qSQLQuery.exec();
    if (qSQLQuery.lastError().isValid())
    {
        qCritical() << qSQLQuery.lastError().text();
    }
}

/**
 * \brief
 * \param item
 * \param qState
 *        -1: 已暂停
 *         1: 传输中
 *         0: 等待中
 */
void RaptorDownloadingSuite::invokeItemUpdate(const RaptorTransferItem &item,
                                              const qint8 &qState)
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        UPDATE Downloading SET Partial = :Partial, State = :State WHERE LeafId = :LeafId AND UserId = :UserId
    )";

    auto qArray = QJsonArray();
    for (const auto &iten: item._Partials)
    {
        auto qBody = QJsonObject();
        qBody["LeafId"] = iten._LeafId;
        qBody["Name"] = iten._Name;
        qBody["Offset"] = iten._Offset;
        qBody["Transferred"] = iten._Transferred;
        qBody["End"] = iten._End;
        qBody["Path"] = iten._Path;
        qArray << qBody;
    }

    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":Partial", QJsonDocument(qArray).toJson());
    qSQLQuery.bindValue(":State", qState);
    qSQLQuery.bindValue(":LeafId", item._LeafId);
    qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
    qSQLQuery.exec();
    if (qSQLQuery.lastError().isValid())
    {
        qCritical() << qSQLQuery.lastError().text();
    }
}

/**
 * \brief 组装文件的完整路径、并创建所需文件夹
 * \param qId 顶级文件夹 Id
 * \param qDir 要保存的位置
 * \param items
 * \param qPathComponent
 * \return
 */
QPair<QString, QVector<RaptorTransferItem> > RaptorDownloadingSuite::invokeItemsBuild(const QString &qId,
                                                                                      const QString &qDir,
                                                                                      const QVector<RaptorFileItem> &items,
                                                                                      const QStringList &qPathComponent)
{
    auto itens = QVector<RaptorTransferItem>();
    for (auto &item: items)
    {
        if (item._Parent == qId)
        {
            auto qPathComponenu = qPathComponent;
            qPathComponenu << item._Name;
            if (item._Type == "file")
            {
                auto qSuffix = qPathComponenu;
                qSuffix.removeLast();
                auto iten = RaptorTransferItem();
                iten._Id = item._Id;
                iten._Name = item._Name;
                if (RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                       Setting::Download::FullPath).toBool())
                {
                    iten._Path = QDir(qDir + "/" + qSuffix.join("/")).path();
                } else
                {
                    iten._Path = qDir;
                }

                iten._Icon = item._Icon;
                iten._Byte = item._Byte;
                iten._SHA1 = item._SHA1;
                iten._Type = item._Type;
                iten._Size = item._Size;
                iten._Status = QStringLiteral("队列中");
                itens << iten;
            }

            const auto [qError, qChildren] = invokeItemsBuild(item._Id, qDir, items, qPathComponenu);
            if (!qError.isEmpty())
            {
                return qMakePair(qError, itens);
            }

            itens << qChildren;
        }
    }

    return qMakePair(QString(), itens);
}

QPair<QString, QVector<RaptorTransferItem> > RaptorDownloadingSuite::invokeItemsAssemble(const QVariant &qVariant)
{
    auto input = qVariant.value<RaptorInput>();
    const auto qIndexList = input._Indexes;
    auto items = QVector<RaptorTransferItem>();
    for (const auto &qIndex: qIndexList)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        input._Parent = item._Id;
        if (item._Type == "folder")
        {
            const auto [qError, itens] = invokeItemsWalk(input._Parent, input._Marker);
            if (!qError.isEmpty())
            {
                return qMakePair(qError, items);
            }

#ifdef Q_OS_WIN
            if (input._Dir.length() == 3)
            {
                // Windows 磁盘根目录特殊处理
                input._Dir = input._Dir.left(2);
            }
#endif
            const auto [qErros, iteos] = invokeItemsBuild(item._Id, QStringLiteral("%1/%2").arg(input._Dir, item._Name), itens);
            if (!qErros.isEmpty())
            {
                return qMakePair(qError, iteos);
            }

            items << iteos;
        } else
        {
            auto iten = RaptorTransferItem();
            iten._Id = item._Id;
            if (!input._Name.isEmpty() && input._Name.compare(item._Name, Qt::CaseSensitive) != 0)
            {
                iten._Name = input._Name;
            } else
            {
                iten._Name = item._Name;
            }

            if (input._Dir.length() == 3)
            {
                input._Dir = input._Dir.left(2);
            }
            iten._Path = input._Dir;
            iten._Icon = item._Icon;
            iten._Byte = item._Byte;
            iten._SHA1 = item._SHA1;
            iten._Type = item._Type;
            iten._Size = RaptorUtil::invokeStorageUnitConvert(item._Byte);
            iten._Status = QStringLiteral("队列中");
            items << iten;
        }
    }

    return qMakePair(QString(), items);
}

void RaptorDownloadingSuite::invokeItemDownload(const RaptorTransferItem &item)
{
    const auto qWorker = new RaptorDownloadingWorker(item);
    connect(qWorker,
            &RaptorDownloadingWorker::itemStatusChanged,
            this,
            &RaptorDownloadingSuite::onItemStatusChanged);

    connect(this,
            &RaptorDownloadingSuite::itemPausing,
            qWorker,
            &RaptorDownloadingWorker::onItemPausing);

    connect(qWorker,
            &RaptorDownloadingWorker::itemPaused,
            this,
            &RaptorDownloadingSuite::onItemPaused);

    connect(this,
            &RaptorDownloadingSuite::itemCancelling,
            qWorker,
            &RaptorDownloadingWorker::onItemCanceling);

    connect(qWorker,
            &RaptorDownloadingWorker::itemCancelled,
            this,
            &RaptorDownloadingSuite::onItemCancelled);

    connect(qWorker,
            &RaptorDownloadingWorker::itemCompleted,
            this,
            &RaptorDownloadingSuite::onItemCompleted);

    connect(qWorker,
            &RaptorDownloadingWorker::itemErrored,
            this,
            &RaptorDownloadingSuite::onItemErrored);

    _ItemsThreadPool->start(qWorker);
}

void RaptorDownloadingSuite::onItemsLoading() const
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        SELECT * FROM Downloading WHERE UserId = :UserId
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
    qSQLQuery.exec();
    if (qSQLQuery.lastError().isValid())
    {
        auto qError = qSQLQuery.lastError().text();
        qCritical() << qError;
        auto output = RaptorOutput();
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsLoaded(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto items = QVector<RaptorTransferItem>();
    while (qSQLQuery.next())
    {
        auto item = RaptorTransferItem();
        item._LeafId = qSQLQuery.value("LeafId").toString();
        item._Id = qSQLQuery.value("Id").toString();
        item._Name = qSQLQuery.value("Name").toString();
        item._Created = qSQLQuery.value("Created").toString();
        item._Size = qSQLQuery.value("Size").toString();
        item._Path = qSQLQuery.value("Path").toString();
        item._Byte = qSQLQuery.value("Byte").toULongLong();
        auto Transferred = static_cast<qint64>(0);
        const auto qPartial = QJsonDocument::fromJson(qSQLQuery.value("Partial").toString().toUtf8()).array();
        for (const auto &iten: qPartial)
        {
            const auto iteo = iten.toObject();
            auto itep = RaptorPartial();
            itep._LeafId = iteo["LeafId"].toString();
            itep._Name = iteo["Name"].toString();
            itep._Offset = iteo["Offset"].toVariant().toLongLong();
            itep._End = iteo["End"].toVariant().toLongLong();
            itep._Transferred = iteo["Transferred"].toVariant().toLongLong();
            itep._Path = iteo["Path"].toString();
            item._Partials.enqueue(itep);
            Transferred += itep._Transferred;
        }

        item._Transferred = Transferred;
        item._SHA1 = qSQLQuery.value("SHA1").toString();
        item._Icon = RaptorUtil::invokeIconMatch(item._Name);
        item._Status = item._Transferred == 0 ? QStringLiteral("队列中") : QStringLiteral("已暂停");
        item._Limit = qSQLQuery.value("Limit").toLongLong();
        item._Parallel = qSQLQuery.value("Parallel").toUInt();
        items << item;
    }

    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QVector<RaptorTransferItem> >(items);
    Q_EMIT itemsLoaded(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorDownloadingSuite::onItemsDownloading(const QVariant &qVariant)
{
    const auto input = qVariant.value<RaptorInput>();
    auto output = RaptorOutput();
    auto [qError, items] = invokeItemsAssemble(qVariant);
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
            item._Limit = input._Speed;
            item._Parallel = input._Parallel;
            item._Created = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            continue;
        }

        if (auto qDir = QDir(item._Path);
            !qDir.exists())
        {
            static_cast<void>(qDir.mkpath(item._Path));
        }

        items.removeOne(item);
    }

    invokeItemsSave(items);
    output._State = true;
    output._Data = QVariant::fromValue<QVector<RaptorTransferItem> >(items);
    Q_EMIT itemsQueuing(QVariant::fromValue<RaptorOutput>(output));
    if (!items.isEmpty() && !_ItemsStatusTimer->isActive())
    {
        _ItemsStatusTimer->start();
    }

    if (const auto qConcurrent = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                    Setting::Download::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        const auto qLimit = _ItemsQueue.length() - qConcurrent;
        const auto itens = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
        if (_ItemsQueue.isEmpty() && itens.isEmpty())
        {
            _ItemsStatusTimer->stop();
            return;
        }

        for (auto &iten: itens)
        {
            if (_ItemsQueue.contains(iten))
            {
                continue;
            }

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemDownload(iten);
        }
    }
}

void RaptorDownloadingSuite::onItemsPausing(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    for (const auto &qIndex: input._Indexes)
    {
        auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        if (_ItemsQueue.contains(item))
        {
            Q_EMIT itemPausing(QVariant::fromValue<RaptorTransferItem>(item));
            continue;
        }

        item._Status = QStringLiteral("已暂停");
        invokeItemUpdate(item, -1);
        auto output = RaptorOutput();
        output._State = true;
        output._Data = QVariant::fromValue<RaptorTransferItem>(item);
        Q_EMIT itemPaused(QVariant::fromValue<RaptorOutput>(output));
    }
}

void RaptorDownloadingSuite::onItemsResuming(const QVariant &qVariant)
{
    const auto input = qVariant.value<RaptorInput>();
    if (!_ItemsStatusTimer->isActive() && !input._Indexes.isEmpty())
    {
        _ItemsStatusTimer->start();
    }

    for (const auto &qIndex: input._Indexes)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        invokeItemUpdate(item, 0);
        if (const auto qConcurrent = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                        Setting::Download::Concurrent).toInt();
            _ItemsQueue.length() >= qConcurrent)
        {
            continue;
        }

        if (_ItemsQueue.contains(item))
        {
            continue;
        }

        _ItemsQueue.enqueue(item);
        invokeItemDownload(item);
    }
}

void RaptorDownloadingSuite::onItemCancelling(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto item = input._Variant.value<RaptorTransferItem>();
    if (_ItemsQueue.contains(item))
    {
        Q_EMIT itemCancelling(QVariant::fromValue<RaptorTransferItem>(item));
        invokeItemDelete(item);
        return;
    }

    invokeItemDelete(item);
    QFile::remove(QStringLiteral("%1/%2").arg(item._Path, item._Name));
    if (auto qDir = QDir(QStringLiteral("%1/%2").arg(item._Path, item._LeafId));
        qDir.exists())
    {
        qDir.removeRecursively();
    }
}

void RaptorDownloadingSuite::onItemStatusChanged(const QVariant &qVariant)
{
    const auto item = qVariant.value<RaptorTransferItem>();
    for (auto i = 0; i < _ItemsQueue.length(); ++i)
    {
        if (item == _ItemsQueue[i])
        {
            _ItemsQueue.replace(i, item);
            break;
        }
    }
}

void RaptorDownloadingSuite::onItemPaused(const QVariant &qVariant)
{
    const auto item = qVariant.value<RaptorTransferItem>();
    _ItemsQueue.removeOne(item);
    invokeItemUpdate(item, -1);
    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<RaptorTransferItem>(item);
    Q_EMIT itemPaused(QVariant::fromValue<RaptorOutput>(output));
    if (!RaptorStoreSuite::invokeEngineStateGet())
    {
        return;
    }

    if (const auto qConcurrent = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                    Setting::Download::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        auto qLimit = _ItemsQueue.length() - qConcurrent;
        const auto items = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
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

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemDownload(iten);
        }
    }
}

void RaptorDownloadingSuite::onItemCancelled(const QVariant &qVariant)
{
    const auto item = qVariant.value<RaptorTransferItem>();
    _ItemsQueue.removeOne(item);
    QFile::remove(QStringLiteral("%1/%2").arg(item._Path, item._Name));
    if (auto qDir = QDir(QStringLiteral("%1/%2").arg(item._Path, item._LeafId));
        qDir.exists())
    {
        qDir.removeRecursively();
    }

    if (!RaptorStoreSuite::invokeEngineStateGet())
    {
        return;
    }

    if (const auto qConcurrent = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                    Setting::Download::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        const auto qLimit = _ItemsQueue.length() - qConcurrent;
        const auto items = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
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

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemDownload(iten);
        }
    }
}

void RaptorDownloadingSuite::onItemCompleted(const QVariant &qVariant)
{
    const auto item = qVariant.value<RaptorTransferItem>();
    _ItemsQueue.removeOne(item);
    invokeItemDelete(item);
    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<RaptorTransferItem>(item);
    Q_EMIT itemCompleted(QVariant::fromValue<RaptorOutput>(output));
    if (!RaptorStoreSuite::invokeEngineStateGet())
    {
        return;
    }

    if (const auto qConcurrent = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                    Setting::Download::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        const auto qLimit = _ItemsQueue.length() - qConcurrent;
        const auto items = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
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

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemDownload(iten);
        }
    }
}

void RaptorDownloadingSuite::onItemErrored(const QVariant &qVariant)
{
    Q_EMIT itemErrored(qVariant);
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    const auto item = _Data.value<RaptorTransferItem>();
    _ItemsQueue.removeOne(item);
    if (!RaptorStoreSuite::invokeEngineStateGet())
    {
        return;
    }

    if (const auto qConcurrent = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                    Setting::Download::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        const auto qLimit = _ItemsQueue.length() - qConcurrent;
        const auto items = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
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

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemDownload(iten);
        }
    }
}

void RaptorDownloadingSuite::onItemStatusTimerTimeout() const
{
    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QQueue<RaptorTransferItem> >(_ItemsQueue);
    Q_EMIT itemsStatusChanged(QVariant::fromValue<RaptorOutput>(output));
}
