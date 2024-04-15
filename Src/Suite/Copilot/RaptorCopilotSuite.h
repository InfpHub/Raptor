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

#ifndef RAPTORCOPILOTSUITE_H
#define RAPTORCOPILOTSUITE_H

#include <QHostInfo>
#include <QObject>
#include <QPainter>
#include <QTimer>

#include <random>

#include <zint.h>

#include "../Http/RaptorHttpSuite.h"
#include "../Persistence/RaptorPersistenceSuite.h"
#include "../Store/RaptorStoreSuite.h"

class RaptorCopilotSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    explicit RaptorCopilotSuite();

    static RaptorCopilotSuite *invokeSingletonGet();

    Q_INVOKABLE void invokeStop() const;

    Q_INVOKABLE void invokeAriaHostParse() const;

private:
    void invokeInstanceInit();

    void invokeSlotInit() const;

Q_SIGNALS:
    Q_SIGNAL void itemCopyWriterHaveFound(const QVariant &qVariant) const;

    Q_SIGNAL void itemQrCodeEncoded(const QVariant &qVariant) const;

    Q_SIGNAL void itemAriaConnectTested(const QVariant &qVariant) const;

    Q_SIGNAL void itemProxyConnectTested(const QVariant &qVariant) const;

public Q_SLOT:
    Q_SLOT void onItemCopyWriterFinding() const;

    Q_SLOT void onItemNoticeFetching() const;

    Q_SLOT void onItemQrCodeEncoding(const QVariant &qVariant) const;

    Q_SLOT void onItemAriaConnectTesting(const QVariant &qVariant) const;

    Q_SLOT void onItemProxyConnectTesting(const QVariant &qVariant) const;

private Q_SLOTS:
    Q_SLOT void onHeartbeatTimerTimeout() const;

    Q_SLOT void onHeartbeatExploring(const QHostInfo &qHostInfo) const;

    Q_SLOT void onAriaHostParsing(const QHostInfo &qHostInfo) const;

private:
    QTimer *_HeartbeatTimer = Q_NULLPTR;
};

#endif // RAPTORCOPILOTSUITE_H
