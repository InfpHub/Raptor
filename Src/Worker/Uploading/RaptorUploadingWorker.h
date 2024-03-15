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

#ifndef RAPTORUPLOADINGWORKER_H
#define RAPTORUPLOADINGWORKER_H

#include <QCryptographicHash>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QRunnable>
#include <QtMath>
#include <QXmlStreamReader>

#include <curl/curl.h>

#include "../../Common/RaptorDeclare.h"
#include "../../Suite/File/RaptorFileSuite.h"
#include "../../Suite/Http/RaptorHttpSuite.h"
#include "../../Suite/Store/RaptorStoreSuite.h"
#include "../../Util/RaptorUtil.h"

class RaptorUploadingWorker Q_DECL_FINAL : public QObject, public QRunnable
{
    Q_OBJECT

public:
    struct Stream
    {
        char* _Data;
        quint64 _Length;
    };

    explicit RaptorUploadingWorker(RaptorTransferItem item);

    ~RaptorUploadingWorker() Q_DECL_OVERRIDE;

    void run() Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit();

    [[nodiscard]]
    QPair<QString, RaptorFileItem> invokeItemByIdFetch(const QString& qId) const;

    void invokeItemRapid();

    void invokeItemUpload();

    [[nodiscard]]
    QPair<QString, QQueue<RaptorPartial>> invokeItemUploadUrlFetch() const;

    [[nodiscard]]
    QPair<QString, quint32> invokeItemUploadedPartialFetch() const;

    int invokeItemPartialUploadProgressCallback(const curl_off_t& ulnow);

    static size_t invokeItemPartialUploadWriteCallback(char* qTarget,
                                                       size_t qSize,
                                                       size_t nmemb,
                                                       void* qData);

    static size_t invokeItemPartialUploadReadCallback(char* qTarget,
                                                      size_t qSize,
                                                      size_t nmemb,
                                                      void* qData);

    static int invokeItemPartialUploadCommonProgressCallback(void* qData,
                                                             curl_off_t dltotal,
                                                             curl_off_t dlnow,
                                                             curl_off_t ultotal,
                                                             curl_off_t ulnow);

    QPair<quint16, QByteArray> invokeItemPartialUpload(const RaptorHttpPayload& qHttpPayload);

    void invokeItemComplete(RaptorTransferItem& item);

Q_SIGNALS:
    Q_SIGNAL void itemStatusChanged(const QVariant& qVariant) const;

    Q_SIGNAL void itemPaused(const QVariant& qVariant) const;

    Q_SIGNAL void itemCancelled(const QVariant& qVariant) const;

    Q_SIGNAL void itemCompleted(const QVariant& qVariant) const;

    Q_SIGNAL void itemErrored(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemPausing(const QVariant& qVariant);

    Q_SLOT void onItemCanceling(const QVariant& qVariant);

private:
    CURL* _Curl = Q_NULLPTR;
    RaptorTransferItem _Item;
    quint64 _LastTransferred;
    inline static QMutex _ReadMutex;
    inline static QMutex _ProgressMutex;
    bool _Paused;
    bool _Cancel;
};

#endif // RAPTORUPLOADINGWORKER_H