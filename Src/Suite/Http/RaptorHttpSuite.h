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

#ifndef RAPTORHTTPSUITE_H
#define RAPTORHTTPSUITE_H

#include <curl/curl.h>

#include "../Setting/RaptorSettingSuite.h"
#include "../../Common/RaptorDeclare.h"

class RaptorHttpSuite Q_DECL_FINAL
{
public:
    static RaptorHttpSuite *invokeSingletonGet();

    static std::tuple<QString, long, QByteArray> invokeGet(const RaptorHttpPayload &qHttpPayload);

    static std::tuple<QString, long, QByteArray> invokePost(const RaptorHttpPayload &qHttpPayload);

    static std::pair<QString, long> invokeItemProxyConnectTest(const QString &qType,
                                                               const QString &qUsername,
                                                               const QString &qPassword,
                                                               const QString &qHost,
                                                               const quint32 &qPort);

    static QString invokeItemWebSocketConnectTest(const QString &qEndPoint);

private:
    static size_t invokeCommonWriteCallback(char *qTarget,
                                            size_t qSize,
                                            size_t nmemb,
                                            void *qData);
};

#endif // RAPTORHTTPSUITE_H
