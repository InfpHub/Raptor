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

#ifndef RAPTORDEFINE_H
#define RAPTORDEFINE_H

#define APPLICATION_NAME "Raptor"

#define APPLICATION_ID "25dzX3vbYqktVxyX"

#define MAJOR_VERSION 2024
#define MINOR_VERSION 3
#define PATCH_VERSION 0

#define GITHUB "https://github.com/InfpHub/Raptor"

#define qInformationTemplate R"(<span style="font-size: 12px; color: rgba(112, 192, 232, 1);">%1</span>)"
#define qSuccessTemplate R"(<span style="font-size: 12px; color: rgba(99, 226, 183, 1);">%1</span>)"
#define qWarningTemplate R"(<span style="font-size: 12px; color: rgba(242, 201, 125, 1);">%1</span>)"
#define qCriticalTemplate R"(<span style="font-size: 12px; color: rgba(232, 128, 128, 1);">%1</span>)"
#define qCreativeTemplate R"(<span style="font-size: 12px; color: rgba(184, 112, 232, 1);">%1</span>)"

#define qFree(qPointer) \
        if (qPointer) \
        { \
            delete qPointer; \
            qPointer = Q_NULLPTR; \
        }

#define qUseHeaderXCanary(qHttpPayload) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::XCanary, "client=windows,app=adrive,version=v4.13.1"); \
        } while(0);

#define qUseHeaderReferer(qHttpPayload) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::Referer, "https://www.alipan.com/"); \
        } while(0);

#define qUseHeaderXShareToken(qHttpPayload, qValue) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::XShareToken, qValue); \
        } while(0);

#define qUseHeaderDefaultXDeviceId(qHttpPayload) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::XDeviceId, RaptorStoreSuite::invokeUserGet()._Session._Device); \
        } while(0);

#define qUseHeaderXSignature(qHttpPayload) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::XSignature, RaptorStoreSuite::invokeUserGet()._Session._Signature); \
        } while(0);

#define qUseHeaderDefaultAuthorization(qHttpPayload) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::Authorization, RaptorHttpHeader::Bearer + RaptorStoreSuite::invokeUserGet()._AccessToken); \
        } while(0);

#define qUseHeaderApplicationJson(qHttpPayload) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::ContentType, RaptorHttpHeader::ApplicationJson); \
        } while(0);

#define qUseHeaderApplicationUrlEncoded(qHttpPayload) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::ContentType, RaptorHttpHeader::ApplicationUrlEncoded); \
        } while(0);

#define qUseHeaderDefault(qHttpPayload) \
        qUseHeaderXCanary(qHttpPayload) \
        qUseHeaderReferer(qHttpPayload) \
        qUseHeaderDefaultXDeviceId(qHttpPayload) \
        qUseHeaderXSignature(qHttpPayload) \
        qUseHeaderDefaultAuthorization(qHttpPayload) \
        qUseHeaderApplicationJson(qHttpPayload)

#define qUseHeaderCustomXDeviceId(qHttpPayload, qValue) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::XDeviceId, qValue); \
        } while(0);

#define qUseHeaderCustomXSignature(qHttpPayload, qValue) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::XSignature, qValue); \
        } while(0);

#define qUseHeaderCustomAuthorization(qHttpPayload, qValue) \
        do \
        { \
            qHttpPayload._Header.insert(RaptorHttpHeader::Authorization, RaptorHttpHeader::Bearer + qValue); \
        } while(0);

#define qUseQueryApplicationName(qHttpPayload) \
        do \
        { \
           qHttpPayload._Query.insert("appName", "aliyun_drive"); \
        } while(0);

#define qUseQueryFromSite(qHttpPayload) \
        do \
        { \
           qHttpPayload._Query.insert("fromSite", "52"); \
        } while(0);

#define qUseQueryCustomTimestamp(qHttpPayload, qValue) \
        do \
        { \
           qHttpPayload._Query.insert("t", qValue); \
        } while(0);

#define qUseQueryCustomCookie(qHttpPayload, qValue) \
        do \
        { \
           qHttpPayload._Query.insert("ck", qValue); \
        } while(0);

namespace RaptorHttpHeader
{
    Q_DECL_CONSTEXPR auto UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36";
    Q_DECL_CONSTEXPR auto Referer = "Referer";
    Q_DECL_CONSTEXPR auto Authorization = "Authorization";
    Q_DECL_CONSTEXPR auto Bearer = "Bearer ";
    Q_DECL_CONSTEXPR auto XShareToken = "X-Share-Token";
    Q_DECL_CONSTEXPR auto ContentType = "Content-Type";
    Q_DECL_CONSTEXPR auto XDeviceId = "X-Device-Id";
    Q_DECL_CONSTEXPR auto XSignature = "X-Signature";
    Q_DECL_CONSTEXPR auto XCanary = "X-Canary";
    Q_DECL_CONSTEXPR auto Range = "Range";
    Q_DECL_CONSTEXPR auto ApplicationJson = "application/json";
    Q_DECL_CONSTEXPR auto ApplicationUrlEncoded = "application/x-www-query-urlencoded";
}

namespace RaptorHttpStatus
{
    Q_DECL_CONSTEXPR auto Continue = 100;
    Q_DECL_CONSTEXPR auto SwitchingProtocols = 101;
    Q_DECL_CONSTEXPR auto OK = 200;
    Q_DECL_CONSTEXPR auto Created = 201;
    Q_DECL_CONSTEXPR auto Accepted = 202;
    Q_DECL_CONSTEXPR auto PartialContent = 206;
    Q_DECL_CONSTEXPR auto BadRequest = 400;
    Q_DECL_CONSTEXPR auto Forbidden = 403;
    Q_DECL_CONSTEXPR auto Conflict = 409;
    Q_DECL_CONSTEXPR auto TooManyRequests = 429;
}

#endif // RAPTORDEFINE_H
