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

#ifndef RAPTORUPLOADINGSUITE_H
#define RAPTORUPLOADINGSUITE_H

#include <QDir>
#include <QObject>
#include <QThreadPool>
#include <QTimer>

#include "../Http/RaptorHttpSuite.h"
#include "../Persistence/RaptorPersistenceSuite.h"
#include "../Store/RaptorStoreSuite.h"
#include "../../Common/RaptorDeclare.h"
#include "../../Util/RaptorUtil.h"
#include "../../Worker/Uploading/RaptorUploadingWorker.h"

class RaptorUploadingSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    explicit RaptorUploadingSuite();

    static RaptorUploadingSuite* invokeSingletonGet();

    Q_INVOKABLE void invokeStop() const;

private:
    void invokeInstanceInit();

    void invokeSlotInit() const;

    static QVector<RaptorTransferItem> invokeItemsByLimitSelect(const quint32& qLimit);

    static void invokeItemsSave(const QVector<RaptorTransferItem>& items);

    static void invokeItemUpdate(const RaptorTransferItem& item, const qint8& qState);

    static void invokeItemDelete(const RaptorTransferItem& item, const bool& qDeleteServer = true);

    static QString invokeItemCreate(RaptorTransferItem& item);

    static QPair<QString, RaptorTransferItem> invokeItemRecurseCreate(const QString& qSpace,
                                                                      const QString& qParent,
                                                                      const QVector<QString>& items);

    static QPair<QString, QVector<QVector<QString>>> invokeItemsAssemble(const QString& qTopPath,
                                                                         const QVector<QString>& qCurrentPath = QVector<QString>());

    void invokeItemUpload(const RaptorTransferItem& item);


Q_SIGNALS:
    Q_SIGNAL void itemsQueuing(const QVariant& qVariant) const;

    Q_SIGNAL void itemsLoaded(const QVariant& qVariant) const;

    Q_SIGNAL void itemPausing(const QVariant& qVariant) const;

    Q_SIGNAL void itemPaused(const QVariant& qVariant) const;

    Q_SIGNAL void itemCancelling(const QVariant& qVariant) const;

    Q_SIGNAL void itemsStatusChanged(const QVariant& qVariant) const;

    Q_SIGNAL void itemCompleted(const QVariant& qVariant) const;

    Q_SIGNAL void itemErrored(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemsUploading(const QVariant& qVariant);

    Q_SLOT void onItemsLoading() const;

    Q_SLOT void onItemsPausing(const QVariant& qVariant) const;

    Q_SLOT void onItemsResuming(const QVariant& qVariant);

    Q_SLOT void onItemCancelling(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onItemStatusChanged(const QVariant& qVariant);

    Q_SLOT void onItemPaused(const QVariant& qVariant);

    Q_SLOT void onItemCancelled(const QVariant& qVariant);

    Q_SLOT void onItemCompleted(const QVariant& qVariant);

    Q_SLOT void onItemErrored(const QVariant& qVariant);

    Q_SLOT void onItemsStatusTimerTimeout() const;

private:
    QThreadPool* _ItemsThreadPool = Q_NULLPTR;
    QTimer* _ItemsStatusTimer = Q_NULLPTR;
    QQueue<RaptorTransferItem> _ItemsQueue;
};

#endif // RAPTORUPLOADINGSUITE_H
