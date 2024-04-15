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

#ifndef RAPTORARIASUITE_H
#define RAPTORARIASUITE_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QWebSocket>

#include "../Downloading/RaptorDownloadingSuite.h"
#include "../Setting/RaptorSettingSuite.h"

class RaptorAriaSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public :
    explicit RaptorAriaSuite();

    static RaptorAriaSuite *invokeSingletonGet();

    Q_INVOKABLE void invokeStop() const;

private:
    void invokeInstanceInit();

    void invokeSlotInit() const;

    void invokeItemDownload(const RaptorTransferItem& item);

Q_SIGNALS:
    Q_SIGNAL void itemsLoaded(const QVariant &qVariant) const;

    Q_SIGNAL void itemsQueuing(const QVariant &qVariant) const;

    Q_SIGNAL void itemPaused(const QVariant &qVariant) const;

    Q_SIGNAL void itemsStatusChanged(const QVariant &qVariant) const;

    Q_SIGNAL void itemCompleted(const QVariant &qVariant) const;

    Q_SIGNAL void itemErrored(const QVariant &qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemsLoading() const;

    Q_SLOT void onItemsDownloading(const QVariant &qVariant);

    Q_SLOT void onItemsPausing(const QVariant &qVariant) const;

    Q_SLOT void onItemsResuming(const QVariant &qVariant);

    Q_SLOT void onItemCancelling(const QVariant &qVariant) const;

private Q_SLOTS:
    Q_SLOT void onWebSocketStateChanged(const QAbstractSocket::SocketState &qState);

    Q_SLOT void onWebSocketErrorOccurred(const QAbstractSocket::SocketError &qError) const;

    Q_SLOT void onWebSocketTextMessageReceived(const QString &qBody);

    Q_SLOT void onItemsTimerTimeout() const;

    Q_SLOT void onItemStatusTimerTimeout() const;

private:
    QQueue<RaptorTransferItem> _ItemsQueue;
    QTimer *_ItemsTimer = Q_NULLPTR;
    QTimer *_ItemsStatusTimer = Q_NULLPTR;
    QWebSocket *_WebSocket = Q_NULLPTR;
    QString _Token;
    quint16 _Concurrent;
};


#endif // RAPTORARIASUITE_H
