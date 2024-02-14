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

#include "RaptorUploadedSuite.h"

Q_GLOBAL_STATIC(RaptorUploadedSuite, _UploadedSuite)

RaptorUploadedSuite* RaptorUploadedSuite::invokeSingletonGet()
{
    return _UploadedSuite();
}

void RaptorUploadedSuite::onItemsLoading() const
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        SELECT * FROM Uploaded WHERE UserId = :UserId
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
        item._Name = qSQLQuery.value("Name").toString();
        item._Path = qSQLQuery.value("Path").toString();
        item._Created = qSQLQuery.value("Created").toString();
        item._Size = qSQLQuery.value("Size").toString();
        item._Icon = RaptorUtil::invokeIconMatch(item._Name);
        items << item;
    }

    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QVector<RaptorTransferItem>>(items);
    Q_EMIT itemsLoaded(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorUploadedSuite::onItemsClearing() const
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
        DELETE FROM Uploaded WHERE UserId = :UserId
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
        Q_EMIT itemsCleared(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    auto output = RaptorOutput();
    output._State = true;
    Q_EMIT itemsCleared(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorUploadedSuite::onItemsDeleting(const QVariant& qVariant) const
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    RaptorPersistenceSuite::invokeTransaction();
    const auto input = qVariant.value<RaptorInput>();
    const auto items = input._Variant.value<QVector<RaptorTransferItem>>();
    const auto qSQL = R"(
        DELETE FROM Uploaded WHERE LeafId = :LeafId AND UserId = :UserId
    )";

    auto qCount = quint32{0};
    for (auto& item : items)
    {
        qSQLQuery.prepare(qSQL);
        qSQLQuery.bindValue(":LeafId", item._LeafId);
        qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
        qSQLQuery.exec();
        if (qSQLQuery.lastError().isValid())
        {
            break;
        }

        ++qCount;
    }

    if (qCount != items.length())
    {
        const auto qError = qSQLQuery.lastError().text();
        qCritical() << qError;
        auto output = RaptorOutput();
        output._State = false;
        output._Message = qError;
        Q_EMIT itemsDeleted(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    RaptorPersistenceSuite::invokeCommit();
    auto output = RaptorOutput();
    output._State = true;
    Q_EMIT itemsDeleted(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorUploadedSuite::onItemCompleted(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto item = _Data.value<RaptorTransferItem>();
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(
            INSERT INTO Uploaded ("LeafId", "Name", "Path", "Created", "Size", "UserId")
            VALUES (:LeafId, :Name, :Path, :Created, :Size, :UserId)
    )";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":LeafId", item._LeafId);
    qSQLQuery.bindValue(":Name", item._Name);
    qSQLQuery.bindValue(":Path", item._Path);
    qSQLQuery.bindValue(":Created", item._Created);
    qSQLQuery.bindValue(":Size", item._Size);
    qSQLQuery.bindValue(":UserId", RaptorStoreSuite::invokeUserGet()._LeafId);
    qSQLQuery.exec();
    if (qSQLQuery.lastError().isValid())
    {
        qCritical() << qSQLQuery.lastError().text();
    }
}
