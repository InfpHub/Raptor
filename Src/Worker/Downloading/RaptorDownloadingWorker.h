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

#ifndef RAPTORDOWNLOADINGWORKER_H
#define RAPTORDOWNLOADINGWORKER_H

#include <QCryptographicHash>
#include <QFile>
#include <QObject>
#include <QRunnable>

#include <curl/curl.h>
#include <uv.h>

#include "../../Common/RaptorDeclare.h"
#include "../../Suite/File/RaptorFileSuite.h"
#include "../../Suite/Http/RaptorHttpSuite.h"
#include "../../Suite/Store/RaptorStoreSuite.h"
#include "../../Util/RaptorUtil.h"

class RaptorDownloadingWorker Q_DECL_FINAL : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit RaptorDownloadingWorker(RaptorTransferItem item);

    ~RaptorDownloadingWorker() Q_DECL_OVERRIDE;

    void run() Q_DECL_OVERRIDE;

private:
    struct Context
    {
        uv_poll_t _Poll;
        curl_socket_t _SocketFD;
        RaptorDownloadingWorker *_Worker = Q_NULLPTR;
    };

    struct Payload
    {
        CURL *_Curl = Q_NULLPTR;
        RaptorDownloadingWorker *_Worker = Q_NULLPTR;
        RaptorPartial _Item;
        QFile _File;
        qint64 _LastTransferred = 0;

        bool operator==(const Payload &item) const
        {
            return _Item._LeafId == item._Item._LeafId;
        }
    };

    void invokeInstanceInit();

    void invokeItemDownload();

    void invokeItemComplete();

    static void invokeItemCommonPerformCallback(uv_poll_t *qReq,
                                                int qStatus,
                                                int qEvents);

    static void invokeItemCommonUVCloseCallback(uv_handle_t *qHandle);

    static int invokeItemCommonSocketCallback(CURL *qCurl,
                                              curl_socket_t qSocket,
                                              int qWhat,
                                              void *qData,
                                              void *socketp);

    static void invokeItemCommonTimeoutCallback(uv_timer_t *qTimer);

    static int invokeItemCommonTimerCallback(CURLM *multi,
                                             long qTimeout,
                                             void *qData);

    static size_t invokeItemCommonWriteCallback(char *qTarget,
                                                size_t qSize,
                                                size_t qNMemb,
                                                void *qData);

    static int invokeItemCommonProgressCallback(void *qData,
                                                curl_off_t dltotal,
                                                curl_off_t dlnow,
                                                curl_off_t ultotal,
                                                curl_off_t ulnow);

Q_SIGNALS:
    Q_SIGNAL void itemStatusChanged(const QVariant &qVariant) const;

    Q_SIGNAL void itemPaused(const QVariant &qVariant) const;

    Q_SIGNAL void itemCancelled(const QVariant &qVariant) const;

    Q_SIGNAL void itemCompleted(const QVariant &qVariant) const;

    Q_SIGNAL void itemErrored(const QVariant &qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemPausing(const QVariant &qVariant);

    Q_SLOT void onItemCanceling(const QVariant &qVariant);

private:
    CURLM *_Multi;
    uv_loop_t _Loop;
    uv_timer_t _Timer;
    QQueue<Payload *> _Payloads;
    RaptorTransferItem _Item;
    qint64 _LastTransferred;
    bool _Paused;
    bool _Cancel;
};

#endif // RAPTORDOWNLOADINGWORKER_H
