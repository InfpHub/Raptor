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

#include "RaptorUploadingSuite.h"

Q_GLOBAL_STATIC(RaptorUploadingSuite, _UploadingSuite)

RaptorUploadingSuite::RaptorUploadingSuite()
{
    invokeInstanceInit();
    invokeSlotInit();
}


RaptorUploadingSuite* RaptorUploadingSuite::invokeSingletonGet()
{
    return _UploadingSuite();
}

void RaptorUploadingSuite::invokeStop() const
{
    _ItemsStatusTimer->stop();
    for (const auto& item : _ItemsQueue)
    {
        Q_EMIT itemPausing(QVariant::fromValue<RaptorTransferItem>(item));
    }
}

void RaptorUploadingSuite::invokeInstanceInit()
{
    _ItemsThreadPool = new QThreadPool(this);
    _ItemsStatusTimer = new QTimer(this);
    _ItemsStatusTimer->setInterval(500);
}

void RaptorUploadingSuite::invokeSlotInit() const
{
    connect(_ItemsStatusTimer,
            &QTimer::timeout,
            this,
            &RaptorUploadingSuite::onItemsStatusTimerTimeout);
}

QVector<RaptorTransferItem> RaptorUploadingSuite::invokeItemsByLimitSelect(const quint32& qLimit)
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        SELECT * FROM Uploading WHERE State = 0 AND UserId = :UserId LIMIT :LIMIT
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
    qSQLQuery.bindValue(":LIMIT", qLimit);
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
        item._WorkerId = qSQLQuery.value("WorkerId").toString();
        item._Parent = qSQLQuery.value("Parent").toString();
        item._Name = qSQLQuery.value("Name").toString();
        item._Space = qSQLQuery.value("Space").toString();
        item._Size = qSQLQuery.value("Size").toString();
        item._Created = qSQLQuery.value("Created").toString();
        item._Path = qSQLQuery.value("Path").toString();
        item._Byte = qSQLQuery.value("Byte").toULongLong();
        item._Rapid = qSQLQuery.value("Rapid").toInt() == 1;
        item._Icon = RaptorUtil::invokeIconMatch(item._Name);
        item._Status = item._Transferred == 0 ? QStringLiteral("队列中") : QStringLiteral("已暂停");
        items << item;
    }

    return items;
}

void RaptorUploadingSuite::invokeItemsSave(const QVector<RaptorTransferItem>& items)
{
    RaptorPersistenceSuite::invokeTransaction();
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        INSERT INTO Uploading ("LeafId", "WorkerId", "Name", "Space", "Size", "Created", "Path", "Transferred", "Byte", "Rapid", "UserId", "Parent", "State")
        VALUES (:LeafId, :WorkerId, :Name, :Space, :Size, :Created, :Path, :Transferred, :Byte, :Rapid, :UserId, :Parent, :State)
    )";
    const auto itens = RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                          Setting::Upload::Filter).value<QVector<QString>>();
    for (auto& item : items)
    {
        if (itens.contains(item._Name))
        {
            continue;
        }

        qSQLQuery.prepare(qSQL);
        qSQLQuery.bindValue(":LeafId", item._LeafId);
        qSQLQuery.bindValue(":WorkerId", item._WorkerId);
        qSQLQuery.bindValue(":Name", item._Name);
        qSQLQuery.bindValue(":Space", item._Space);
        qSQLQuery.bindValue(":Size", item._Size);
        qSQLQuery.bindValue(":Created", item._Created);
        qSQLQuery.bindValue(":Path", item._Path);
        qSQLQuery.bindValue(":Byte", item._Byte);
        qSQLQuery.bindValue(":Transferred", 0);
        qSQLQuery.bindValue(":Rapid", item._Rapid ? 1 : 0);
        qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
        qSQLQuery.bindValue(":Parent", item._Parent);
        qSQLQuery.bindValue(":State", 0);
        qSQLQuery.exec();
    }

    RaptorPersistenceSuite::invokeCommit();
}

/**
 * \brief
 * \param item
 * \param qState
 *        -1: 已暂停
 *         1: 传输中
 *         0: 等待中
 */
void RaptorUploadingSuite::invokeItemUpdate(const RaptorTransferItem& item,
                                            const qint8& qState)
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        UPDATE Uploading SET Transferred = :Transferred, State = :State WHERE LeafId = :LeafId AND UserId = :UserId
    )";

    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":Transferred", item._Transferred);
    qSQLQuery.bindValue(":State", qState);
    qSQLQuery.bindValue(":LeafId", item._LeafId);
    qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
    qSQLQuery.exec();
    if (qSQLQuery.lastError().isValid())
    {
        qCritical() << qSQLQuery.lastError().text();
    }
}

void RaptorUploadingSuite::invokeItemDelete(const RaptorTransferItem& item,
                                            const bool& qDeleteServer)
{
    if (qDeleteServer)
    {
        auto qArray = QJsonArray();
        auto qBody = QJsonObject();
        qBody["drive_id"] = item._Space;
        qBody["file_id"] = item._LeafId;

        auto qHeader = QJsonObject();
        qHeader["Content-Type"] = RaptorHttpHeader::ApplicationJson;

        auto qDocument = QJsonObject();
        qDocument["body"] = qBody;
        qDocument["headers"] = qHeader;
        qDocument["id"] = item._LeafId;
        qDocument["method"] = "POST";
        qDocument["url"] = "/file/delete";
        qArray << qDocument;

        auto qHttpPayload = RaptorHttpPayload();
        qHttpPayload._Url = "https://api.alipan.com/adrive/v4/batch";
        qUseHeaderDefault(qHttpPayload)
        auto qRow = QJsonObject();
        qRow["requests"] = qArray;
        qRow["resource"] = "file";
        qHttpPayload._Body = QJsonDocument(qRow);
        RaptorHttpSuite::invokePost(qHttpPayload);
    }

    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        DELETE FROM Uploading WHERE LeafId = :LeafId AND UserId = :UserId
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

QString RaptorUploadingSuite::invokeItemCreate(RaptorTransferItem& item)
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = "https://api.alipan.com/adrive/v2/file/createWithFolders";
    qUseHeaderDefault(qHttpPayload)
    auto qRow = QJsonObject();
    qRow["check_name_mode"] = "refuse";
    qRow["drive_id"] = item._Space;
    qRow["parent_file_id"] = item._Parent;
    qRow["type"] = item._Type;
    qRow["name"] = item._Name;
    if (item._Type == "file")
    {
        qRow["check_name_mode"] = "auto_rename";
        qRow["create_scene"] = "file_upload";
        qRow["pre_hash"] = item._1024Hash;
        qRow["size"] = QString::number(item._Byte);
        qRow["part_info_list"] = RaptorUtil::invokeItemPartialCompute(QStringLiteral("%1/%2").arg(item._Path, item._Name));
    }

    qHttpPayload._Body = QJsonDocument(qRow);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokePost(qHttpPayload);
    if (!qError.isEmpty())
    {
        qCritical() << qError;
        return qError;
    }

    if (qStatus == RaptorHttpStatus::Created)
    {
        const auto qDocument = QJsonDocument::fromJson(qBody);
        item._LeafId = qDocument["file_id"].toString();
        item._Name = qDocument["file_name"].toString();
        item._Space = qDocument["drive_id"].toString();
        item._Created = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        if (item._Type == "file")
        {
            auto items = QQueue<RaptorPartial>();
            const auto iteos = qDocument["part_info_list"].toArray();
            for (const auto& iteo : iteos)
            {
                const auto itep = iteo.toObject();
                auto iteq = RaptorPartial();
                iteq._Number = itep["part_number"].toVariant().toUInt();
                iteq._Url = itep["upload_url"].toString();
                items << iteq;
            }

            item._WorkerId = qDocument["upload_id"].toString();
            item._Partials = items;
            item._Size = RaptorUtil::invokeStorageUnitConvert(item._Byte);
        }

        return QString();
    }

    const auto qDocument = QJsonDocument::fromJson(qBody);
    if (qStatus == RaptorHttpStatus::Conflict)
    {
        if (item._Type == "file")
        {
            item._LeafId = RaptorUtil::invokeUUIDGenerate();
            item._WorkerId = RaptorUtil::invokeUUIDGenerate();
            item._Created = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            item._Rapid = true;
            item._Type = "file";
            item._Size = RaptorUtil::invokeStorageUnitConvert(item._Byte);
        }

        return QString();
    }

    qCritical() << qDocument.toJson();
    return QString("%1: %2").arg(qDocument["code"].toString(), qDocument["message"].toString());
}

/**
 * \brief  递归创建目录
 * \param qSpace
 * \param qParent
 * \param items
 * \return
 */
QPair<QString, RaptorTransferItem> RaptorUploadingSuite::invokeItemRecurseCreate(const QString& qSpace,
                                                                                 const QString& qParent,
                                                                                 const QVector<QString>& items)
{
    auto qParenu = qParent;
    auto iten = RaptorTransferItem();
    for (auto i = 0; i < items.length(); ++i)
    {
        auto iteo = RaptorTransferItem();
        iteo._Parent = qParenu;
        iteo._Space = qSpace;
        iteo._Name = items[i];
        iteo._Type = "folder";
        iteo._Icon = RaptorUtil::invokeIconMatch(QString(), true);
        if (const auto qError = invokeItemCreate(iteo);
            !qError.isEmpty())
        {
            return qMakePair(qError, iteo);
        }

        qParenu = iteo._LeafId;
        if (i == items.length() - 1)
        {
            iten = iteo;
        }
    }

    return qMakePair(QString(), iten);
}

/**
 * \brief
 * \param qTopPath 递归组装文件/文件夹
 * \param qCurrentPath
 * \return
 */
QPair<QString, QVector<QVector<QString>>> RaptorUploadingSuite::invokeItemsAssemble(const QString& qTopPath,
                                                                                    const QVector<QString>& qCurrentPath)
{
    auto qDir = QDir(qTopPath);
    qDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    qDir.setSorting(QDir::DirsFirst);
    auto items = QVector<QVector<QString>>();
    const auto itens = qDir.entryInfoList();
    for (auto& iten : itens)
    {
        auto qPath = qCurrentPath;
        qPath << iten.fileName();
        items << qPath;
        if (iten.isDir())
        {
            const auto [qTopPati, iteos] = invokeItemsAssemble(iten.filePath(), qPath);
            items << iteos;
        }
    }

    return qMakePair(qTopPath, items);
}

void RaptorUploadingSuite::invokeItemUpload(const RaptorTransferItem& item)
{
    const auto qWorker = new RaptorUploadingWorker(item);
    connect(qWorker,
            &RaptorUploadingWorker::itemStatusChanged,
            this,
            &RaptorUploadingSuite::onItemStatusChanged);

    connect(this,
            &RaptorUploadingSuite::itemPausing,
            qWorker,
            &RaptorUploadingWorker::onItemPausing);

    connect(qWorker,
            &RaptorUploadingWorker::itemPaused,
            this,
            &RaptorUploadingSuite::onItemPaused);

    connect(this,
            &RaptorUploadingSuite::itemCancelling,
            qWorker,
            &RaptorUploadingWorker::onItemCanceling);

    connect(qWorker,
            &RaptorUploadingWorker::itemCancelled,
            this,
            &RaptorUploadingSuite::onItemCancelled);

    connect(qWorker,
            &RaptorUploadingWorker::itemCompleted,
            this,
            &RaptorUploadingSuite::onItemCompleted);

    connect(qWorker,
            &RaptorUploadingWorker::itemErrored,
            this,
            &RaptorUploadingSuite::onItemErrored);
    _ItemsThreadPool->start(qWorker);
}

void RaptorUploadingSuite::onItemsUploading(const QVariant& qVariant)
{
    const auto input = qVariant.value<RaptorInput>();
    const auto items = input._Variant.value<QVector<RaptorTransferItem>>();
    auto output = RaptorOutput();
    auto itess = QVector<RaptorTransferItem>();
    for (auto& item : items)
    {
        // 文件夹
        if (item._Type == "folder")
        {
            // 创建顶级目录
            auto iten = RaptorTransferItem();
            iten._Parent = input._Parent;
            iten._Space = input._Space;
            iten._Name = item._Name;
            iten._Type = "folder";
            iten._Icon = RaptorUtil::invokeIconMatch(QString(), true);
            if (const auto qError = invokeItemCreate(iten);
                !qError.isEmpty())
            {
                output._State = false;
                output._Message = qError;
                Q_EMIT itemsQueuing(QVariant::fromValue<RaptorOutput>(output));
                continue;
            }

            // 创建顶级目录下的多级子目录
            const auto [qTopPath, itens] = invokeItemsAssemble(item._Path);
            for (auto& iteo : itens)
            {
                auto qAbsolutePath = QStringLiteral("%1/").arg(qTopPath);
                for (auto qIterator = iteo.begin(); qIterator != iteo.end(); ++qIterator)
                {
                    qAbsolutePath.append(*qIterator);
                    if (std::next(qIterator) != iteo.end())
                    {
                        qAbsolutePath.append("/");
                    }
                }

                // 是目录、递归创建
                if (const auto qFileInfo = QFileInfo(qAbsolutePath);
                    qFileInfo.isDir())
                {
                    invokeItemRecurseCreate(input._Space, iten._LeafId, iteo);
                }
                else if (qFileInfo.isFile())
                {
                    // 是文件且是顶级目录下的直系文件
                    if (iteo.length() == 1)
                    {
                        auto iteq = RaptorTransferItem();
                        iteq._Name = qFileInfo.fileName();
                        iteq._Parent = iten._LeafId;
                        iteq._Space = input._Space;
                        iteq._Type = "file";
                        iteq._Byte = qFileInfo.size();
                        iteq._Path = qFileInfo.path();
                        iteq._Icon = RaptorUtil::invokeIconMatch(iteq._Name);
                        iteq._1024Hash = RaptorUtil::invoke1024SHA1Compute(QStringLiteral("%1/%2").arg(iteq._Path, iteq._Name));
                        iteq._Status = QStringLiteral("队列中");
                        invokeItemCreate(iteq);
                        if (RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                               Setting::Upload::SkipNoRapid).toBool())
                        {
                            if (!iteq._Rapid)
                            {
                                invokeItemDelete(iteq);
                                continue;
                            }
                        }

                        itess << iteq;
                    }
                    else
                    {
                        // 是文件且是多级目录下的文件
                        auto itep = iteo;
                        itep.removeLast();
                        auto [qErros, iter] = invokeItemRecurseCreate(input._Space, iten._LeafId, itep);
                        if (!qErros.isEmpty())
                        {
                            output._State = false;
                            output._Message = qErros;
                            Q_EMIT itemsQueuing(QVariant::fromValue<RaptorOutput>(output));
                            continue;
                        }

                        iter._Name = qFileInfo.fileName();
                        iter._Parent = iter._LeafId;
                        iter._Space = input._Space;
                        iter._Type = "file";
                        iter._Byte = qFileInfo.size();
                        iter._Path = qFileInfo.path();
                        iter._Icon = RaptorUtil::invokeIconMatch(iter._Name);
                        iter._1024Hash = RaptorUtil::invoke1024SHA1Compute(QStringLiteral("%1/%2").arg(iter._Path, iter._Name));
                        iter._Status = QStringLiteral("队列中");
                        invokeItemCreate(iter);
                        if (RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                               Setting::Upload::SkipNoRapid).toBool())
                        {
                            if (!iter._Rapid)
                            {
                                invokeItemDelete(iter);
                                continue;
                            }
                        }

                        itess << iter;
                    }
                }
            }
        }
        else
        {
            // 文件
            auto iten = RaptorTransferItem();
            auto qFileInfo = QFileInfo(QStringLiteral("%1/%2").arg(item._Path, item._Name));
            iten._Parent = input._Parent;
            iten._Space = input._Space;
            iten._Name = item._Name;
            iten._Type = "file";
            iten._Byte = qFileInfo.size();
            iten._Path = item._Path;
            iten._Icon = RaptorUtil::invokeIconMatch(iten._Name);
            iten._1024Hash = RaptorUtil::invoke1024SHA1Compute(qFileInfo.absoluteFilePath());
            iten._Status = QStringLiteral("队列中");
            invokeItemCreate(iten);
            if (RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                   Setting::Upload::SkipNoRapid).toBool())
            {
                if (!iten._Rapid)
                {
                    invokeItemDelete(iten);
                    continue;
                }
            }

            itess << iten;
        }
    }

    invokeItemsSave(itess);
    output._State = true;
    output._Data = QVariant::fromValue<QVector<RaptorTransferItem>>(itess);
    Q_EMIT itemsQueuing(QVariant::fromValue<RaptorOutput>(output));
    if (const auto qConcurrent = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Upload,
                                                                    Setting::Upload::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        const auto qLimit = qConcurrent - _ItemsQueue.length();
        const auto itens = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
        if (!itens.isEmpty() && !_ItemsStatusTimer->isActive())
        {
            _ItemsStatusTimer->start();
        }

        for (auto& iten : itens)
        {
            if (_ItemsQueue.contains(iten))
            {
                continue;
            }

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemUpload(iten);
        }
    }
}

void RaptorUploadingSuite::onItemsLoading() const
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        SELECT * FROM Uploading WHERE UserId = :UserId
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
    qSQLQuery.exec();
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

    auto items = QVector<RaptorTransferItem>();
    while (qSQLQuery.next())
    {
        auto item = RaptorTransferItem();
        item._LeafId = qSQLQuery.value("LeafId").toString();
        item._WorkerId = qSQLQuery.value("WorkerId").toString();
        item._Name = qSQLQuery.value("Name").toString();
        item._Created = qSQLQuery.value("Created").toString();
        item._Space = qSQLQuery.value("Space").toString();
        item._Size = qSQLQuery.value("Size").toString();
        item._Path = qSQLQuery.value("Path").toString();
        item._Byte = qSQLQuery.value("Byte").toULongLong();
        item._Transferred = qSQLQuery.value("Transferred").toULongLong();
        item._Rapid = qSQLQuery.value("Rapid").toInt() == 1;
        item._Icon = RaptorUtil::invokeIconMatch(item._Name);
        item._Status = item._Transferred == 0 ? QStringLiteral("队列中") : QStringLiteral("已暂停");
        items << item;
    }

    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QVector<RaptorTransferItem>>(items);
    Q_EMIT itemsLoaded(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorUploadingSuite::onItemsPausing(const QVariant& qVariant) const
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

void RaptorUploadingSuite::onItemsResuming(const QVariant& qVariant)
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
        if (const auto qConcurrent = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Upload,
                                                                        Setting::Upload::Concurrent).toInt();
            _ItemsQueue.length() >= qConcurrent)
        {
            continue;
        }
        if (_ItemsQueue.contains(item))
        {
            continue;
        }

        _ItemsQueue.enqueue(item);
        invokeItemUpload(item);
    }
}

void RaptorUploadingSuite::onItemCancelling(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto item = input._Variant.value<RaptorTransferItem>();
    if (_ItemsQueue.contains(item))
    {
        Q_EMIT itemCancelling(QVariant::fromValue<RaptorTransferItem>(item));
        return;
    }

    invokeItemDelete(item);
}

void RaptorUploadingSuite::onItemStatusChanged(const QVariant& qVariant)
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

void RaptorUploadingSuite::onItemPaused(const QVariant& qVariant)
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

    if (const auto qConcurrent = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Upload,
                                                                    Setting::Upload::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        const auto qLimit = _ItemsQueue.length() - qConcurrent;
        const auto items = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
        if (_ItemsQueue.isEmpty() && items.isEmpty())
        {
            _ItemsStatusTimer->stop();
            return;
        }

        for (auto& iten : items)
        {
            if (_ItemsQueue.contains(iten))
            {
                continue;
            }

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemUpload(iten);
        }
    }
}

void RaptorUploadingSuite::onItemCancelled(const QVariant& qVariant)
{
    const auto item = qVariant.value<RaptorTransferItem>();
    _ItemsQueue.removeOne(item);
    if (!RaptorStoreSuite::invokeEngineStateGet())
    {
        return;
    }

    if (const auto qConcurrent = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Upload,
                                                                    Setting::Upload::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        const auto qLimit = _ItemsQueue.length() - qConcurrent;
        const auto items = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
        if (_ItemsQueue.isEmpty() && items.isEmpty())
        {
            _ItemsStatusTimer->stop();
            return;
        }

        for (auto& iten : items)
        {
            if (_ItemsQueue.contains(iten))
            {
                continue;
            }

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemUpload(iten);
        }
    }
}

void RaptorUploadingSuite::onItemCompleted(const QVariant& qVariant)
{
    const auto item = qVariant.value<RaptorTransferItem>();
    _ItemsQueue.removeOne(item);
    invokeItemDelete(item, false);
    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<RaptorTransferItem>(item);
    Q_EMIT itemCompleted(QVariant::fromValue<RaptorOutput>(output));
    if (!RaptorStoreSuite::invokeEngineStateGet())
    {
        return;
    }

    if (const auto qConcurrent = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Upload,
                                                                    Setting::Upload::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        const auto qLimit = _ItemsQueue.length() - qConcurrent;
        const auto items = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
        if (_ItemsQueue.isEmpty() && items.isEmpty())
        {
            _ItemsStatusTimer->stop();
            return;
        }

        for (auto& iten : items)
        {
            if (_ItemsQueue.contains(iten))
            {
                continue;
            }

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemUpload(iten);
        }
    }
}

void RaptorUploadingSuite::onItemErrored(const QVariant& qVariant)
{
    Q_EMIT itemErrored(qVariant);
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    const auto item = _Data.value<RaptorTransferItem>();
    _ItemsQueue.removeOne(item);
    if (!RaptorStoreSuite::invokeEngineStateGet())
    {
        return;
    }

    if (const auto qConcurrent = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Upload,
                                                                    Setting::Upload::Concurrent).toInt();
        _ItemsQueue.length() < qConcurrent)
    {
        const auto qLimit = _ItemsQueue.length() - qConcurrent;
        const auto items = invokeItemsByLimitSelect(qLimit < 0 ? qConcurrent - _ItemsQueue.length() : qLimit);
        if (_ItemsQueue.isEmpty() && items.isEmpty())
        {
            _ItemsStatusTimer->stop();
            return;
        }

        for (auto& iten : items)
        {
            if (_ItemsQueue.contains(iten))
            {
                continue;
            }

            invokeItemUpdate(iten, 1);
            _ItemsQueue.enqueue(iten);
            invokeItemUpload(iten);
        }
    }
}

void RaptorUploadingSuite::onItemsStatusTimerTimeout() const
{
    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QQueue<RaptorTransferItem>>(_ItemsQueue);
    Q_EMIT itemsStatusChanged(QVariant::fromValue<RaptorOutput>(output));
}
