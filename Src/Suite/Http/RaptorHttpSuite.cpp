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

#include "RaptorHttpSuite.h"

Q_GLOBAL_STATIC(RaptorHttpSuite, _HttpSuite)

RaptorHttpSuite *RaptorHttpSuite::invokeSingletonGet()
{
    return _HttpSuite();
}

std::tuple<QString, long, QByteArray> RaptorHttpSuite::invokeGet(const RaptorHttpPayload &qHttpPayload)
{
    const auto qCurl = curl_easy_init();
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    if (const auto qIPResolve = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::IPResolve).toString();
        qIPResolve == Setting::Network::Auto)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);
    } else if (qIPResolve == Setting::Network::IPV4)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    } else if (qIPResolve == Setting::Network::IPV6)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
    }

    const auto qCurlUrl = curl_url();
    curl_url_set(qCurlUrl, CURLUPART_URL, qHttpPayload._Url.toStdString().c_str(), 0L);
    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                           Setting::Network::Proxy).toBool())
    {
        if (const auto qProxyEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                         Setting::Network::ProxyEngine).toString();
            qProxyEngine == Setting::Network::HTTP)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        } else if (qProxyEngine == Setting::Network::HTTP1_0)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
        } else if (qProxyEngine == Setting::Network::HTTPS)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
        } else if (qProxyEngine == Setting::Network::HTTPS2)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
        } else if (qProxyEngine == Setting::Network::SOCKS4)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
        } else if (qProxyEngine == Setting::Network::SOCKS5)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        }

        const auto qProxyHost = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyHost).toString();
        const auto qProxyPort = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyPort).toUInt();
        curl_easy_setopt(qCurl, CURLOPT_PROXY, qProxyHost.toStdString().c_str());
        curl_easy_setopt(qCurl, CURLOPT_PROXYPORT, qProxyPort);
        if (const auto qProxyUsername = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyUsername).toString();
            !qProxyUsername.isEmpty())
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYUSERNAME, qProxyUsername.toStdString().c_str());
        }
        if (const auto qProxyPassword = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyPassword).toString();
            !qProxyPassword.isEmpty())
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYPASSWORD, qProxyPassword.toStdString().c_str());
        }
    }

    for (auto iterator = qHttpPayload._Query.begin(); iterator != qHttpPayload._Query.end(); ++iterator)
    {
        auto query = iterator.key().toStdString() + "=" + iterator.value().toString().toStdString();
        curl_url_set(qCurlUrl, CURLUPART_QUERY, query.data(), CURLU_APPENDQUERY);
    }

    const auto qMime = curl_mime_init(qCurl);
    for (auto qIterator = qHttpPayload._Form.begin(); qIterator != qHttpPayload._Form.end(); ++qIterator)
    {
        const auto qPart = curl_mime_addpart(qMime);
        curl_mime_name(qPart, qIterator.key().toStdString().c_str());
        curl_mime_data(qPart, qIterator.value().toString().toStdString().c_str(), CURL_ZERO_TERMINATED);
    }

    curl_easy_setopt(qCurl, CURLOPT_HTTPGET, 1L);
    auto qUrl = static_cast<char *>(Q_NULLPTR);
    curl_url_get(qCurlUrl, CURLUPART_URL, &qUrl, 0L);
    curl_easy_setopt(qCurl, CURLOPT_URL, qUrl);
    curl_easy_setopt(qCurl, CURLOPT_USERAGENT, RaptorHttpHeader::UserAgent);
    auto qHeader = static_cast<curl_slist *>(Q_NULLPTR);
    for (auto qIterator = qHttpPayload._Header.begin(); qIterator != qHttpPayload._Header.end(); ++qIterator)
    {
        qHeader = curl_slist_append(qHeader, (qIterator.key().toStdString() + ": " + qIterator.value().toString().toStdString()).c_str());
    }

    curl_easy_setopt(qCurl, CURLOPT_HTTPHEADER, qHeader);
    auto qBody = QByteArray();
    curl_easy_setopt(qCurl, CURLOPT_WRITEDATA, &qBody);
    curl_easy_setopt(qCurl, CURLOPT_WRITEFUNCTION, invokeCommonWriteCallback);
    auto qStatus = long();
    if (const auto qCode = curl_easy_perform(qCurl);
        qCode != CURLE_OK)
    {
        curl_mime_free(qMime);
        curl_slist_free_all(qHeader);
        curl_url_cleanup(qCurlUrl);
        curl_easy_cleanup(qCurl);
        return std::make_tuple(QString::fromStdString(curl_easy_strerror(qCode)), qStatus, qBody);
    }

    curl_easy_getinfo(qCurl, CURLINFO_RESPONSE_CODE, &qStatus);
    curl_slist_free_all(qHeader);
    curl_url_cleanup(qCurlUrl);
    curl_easy_cleanup(qCurl);
    qInfo() << qStatus << " -> " << qHttpPayload._Url;
    if (qStatus == RaptorHttpStatus::TooManyRequests)
    {
        curl_slist_free_all(qHeader);
        curl_url_cleanup(qCurlUrl);
        curl_easy_cleanup(qCurl);
        return invokeGet(qHttpPayload);
    }

    return std::make_tuple(QString(), qStatus, qBody);
}

std::tuple<QString, long, QByteArray> RaptorHttpSuite::invokePost(const RaptorHttpPayload &qHttpPayload)
{
    const auto qCurl = curl_easy_init();
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    if (const auto qIPResolve = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::IPResolve).toString();
        qIPResolve == Setting::Network::Auto)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);
    } else if (qIPResolve == Setting::Network::IPV4)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    } else if (qIPResolve == Setting::Network::IPV6)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
    }

    const auto qCurlUrl = curl_url();
    curl_url_set(qCurlUrl, CURLUPART_URL, qHttpPayload._Url.toStdString().c_str(), 0L);
    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                           Setting::Network::Proxy).toBool())
    {
        if (const auto qProxyEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                         Setting::Network::ProxyEngine).toString();
            qProxyEngine == Setting::Network::HTTP)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        } else if (qProxyEngine == Setting::Network::HTTP1_0)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
        } else if (qProxyEngine == Setting::Network::HTTPS)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
        } else if (qProxyEngine == Setting::Network::HTTPS2)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
        } else if (qProxyEngine == Setting::Network::SOCKS4)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
        } else if (qProxyEngine == Setting::Network::SOCKS5)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        }

        const auto qProxyHost = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyHost).toString();
        const auto qProxyPort = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyPort).toUInt();
        curl_easy_setopt(qCurl, CURLOPT_PROXY, qProxyHost.toStdString().c_str());
        curl_easy_setopt(qCurl, CURLOPT_PROXYPORT, qProxyPort);
        if (const auto qProxyUsername = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyUsername).toString();
            !qProxyUsername.isEmpty())
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYUSERNAME, qProxyUsername.toStdString().c_str());
        }
        if (const auto qProxyPassword = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyPassword).toString();
            !qProxyPassword.isEmpty())
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYPASSWORD, qProxyPassword.toStdString().c_str());
        }
    }

    for (auto qIterator = qHttpPayload._Query.begin(); qIterator != qHttpPayload._Query.end(); ++qIterator)
    {
        const auto qQuery = qIterator.key().toStdString() + "=" + qIterator.value().toString().toStdString();
        curl_url_set(qCurlUrl, CURLUPART_QUERY, qQuery.data(), CURLU_APPENDQUERY);
    }

    const auto qMime = curl_mime_init(qCurl);
    for (auto qIterator = qHttpPayload._Form.begin(); qIterator != qHttpPayload._Form.end(); ++qIterator)
    {
        const auto qPart = curl_mime_addpart(qMime);
        curl_mime_name(qPart, qIterator.key().toStdString().c_str());
        curl_mime_data(qPart, qIterator.value().toString().toStdString().c_str(), CURL_ZERO_TERMINATED);
    }

    curl_easy_setopt(qCurl, CURLOPT_MIMEPOST, qMime);
    const auto qRow = qHttpPayload._Body.toJson().toStdString();
    curl_easy_setopt(qCurl, CURLOPT_POSTFIELDS, qRow.c_str());
    auto qUrl = static_cast<char *>(Q_NULLPTR);
    curl_url_get(qCurlUrl, CURLUPART_URL, &qUrl, 0L);
    curl_easy_setopt(qCurl, CURLOPT_URL, qUrl);
    curl_easy_setopt(qCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(qCurl, CURLOPT_USERAGENT, RaptorHttpHeader::UserAgent);
    auto qHeader = static_cast<curl_slist *>(Q_NULLPTR);
    for (auto qIterator = qHttpPayload._Header.begin(); qIterator != qHttpPayload._Header.end(); ++qIterator)
    {
        qHeader = curl_slist_append(qHeader, (qIterator.key().toStdString() + ": " + qIterator.value().toString().toStdString()).c_str());
    }

    curl_easy_setopt(qCurl, CURLOPT_HTTPHEADER, qHeader);
    curl_easy_setopt(qCurl, CURLOPT_WRITEFUNCTION, invokeCommonWriteCallback);
    auto qBody = QByteArray();
    curl_easy_setopt(qCurl, CURLOPT_WRITEDATA, &qBody);
    auto qStatus = long();
    if (const auto qCode = curl_easy_perform(qCurl);
        qCode != CURLE_OK)
    {
        curl_mime_free(qMime);
        curl_slist_free_all(qHeader);
        curl_url_cleanup(qCurlUrl);
        curl_easy_cleanup(qCurl);
        return std::make_tuple(QString::fromStdString(curl_easy_strerror(qCode)), qStatus, qBody);
    }

    curl_easy_getinfo(qCurl, CURLINFO_RESPONSE_CODE, &qStatus);
    curl_mime_free(qMime);
    curl_slist_free_all(qHeader);
    curl_url_cleanup(qCurlUrl);
    curl_easy_cleanup(qCurl);
    qInfo() << qStatus << " -> " << qHttpPayload._Url;
    if (qStatus == RaptorHttpStatus::TooManyRequests)
    {
        curl_slist_free_all(qHeader);
        curl_url_cleanup(qCurlUrl);
        curl_easy_cleanup(qCurl);
        return invokePost(qHttpPayload);
    }

    return std::make_tuple(QString(), qStatus, qBody);
}

std::pair<QString, long> RaptorHttpSuite::invokeItemProxyConnectTest(const QString &qType,
                                                                     const QString &qUsername,
                                                                     const QString &qPassword,
                                                                     const QString &qHost,
                                                                     const quint32 &qPort)
{
    const auto qCurl = curl_easy_init();
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(qCurl, CURLOPT_URL, "https://aliyundrive.com");
    curl_easy_setopt(qCurl, CURLOPT_CONNECT_ONLY, 2L);
    if (const auto qIPResolve = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::IPResolve).toString();
        qIPResolve == Setting::Network::Auto)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);
    } else if (qIPResolve == Setting::Network::IPV4)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    } else if (qIPResolve == Setting::Network::IPV6)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
    }

    if (qType == Setting::Network::HTTP)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
    } else if (qType == Setting::Network::HTTP1_0)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
    } else if (qType == Setting::Network::HTTPS)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
    } else if (qType == Setting::Network::HTTPS2)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
    } else if (qType == Setting::Network::SOCKS4)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
    } else if (qType == Setting::Network::SOCKS5)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
    }

    curl_easy_setopt(qCurl, CURLOPT_PROXY, qHost.toStdString().c_str());
    curl_easy_setopt(qCurl, CURLOPT_PROXYPORT, qPort);
    if (!qUsername.isEmpty())
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYUSERNAME, qUsername.toStdString().c_str());
    }

    if (!qPassword.isEmpty())
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYPASSWORD, qPassword.toStdString().c_str());
    }

    auto qStatus = long();
    if (const auto qCode = curl_easy_perform(qCurl);
        qCode != CURLE_OK)
    {
        curl_easy_cleanup(qCurl);
        return std::make_pair(QString::fromStdString(curl_easy_strerror(qCode)), qStatus);
    }

    curl_easy_getinfo(qCurl, CURLINFO_RESPONSE_CODE, &qStatus);
    curl_easy_cleanup(qCurl);
    return std::make_pair(QString(), qStatus);
}

QString RaptorHttpSuite::invokeItemWebSocketConnectTest(const QString &qEndPoint)
{
    const auto qCurl = curl_easy_init();
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(qCurl, CURLOPT_URL, qEndPoint.toStdString().c_str());
    curl_easy_setopt(qCurl, CURLOPT_CONNECT_ONLY, 2L);
    curl_easy_setopt(qCurl, CURLOPT_CONNECTTIMEOUT_MS, 750);
    if (const auto qIPResolve = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::IPResolve).toString();
        qIPResolve == Setting::Network::Auto)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);
    } else if (qIPResolve == Setting::Network::IPV4)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    } else if (qIPResolve == Setting::Network::IPV6)
    {
        curl_easy_setopt(qCurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
    }

    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                           Setting::Network::Proxy).toBool())
    {
        if (const auto qProxyEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                         Setting::Network::ProxyEngine).toString();
            qProxyEngine == Setting::Network::HTTP)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        } else if (qProxyEngine == Setting::Network::HTTP1_0)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
        } else if (qProxyEngine == Setting::Network::HTTPS)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
        } else if (qProxyEngine == Setting::Network::HTTPS2)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
        } else if (qProxyEngine == Setting::Network::SOCKS4)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
        } else if (qProxyEngine == Setting::Network::SOCKS5)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        }

        const auto qProxyHost = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyHost).toString();
        const auto qProxyPort = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyPort).toUInt();
        curl_easy_setopt(qCurl, CURLOPT_PROXY, qProxyHost.toStdString().c_str());
        curl_easy_setopt(qCurl, CURLOPT_PROXYPORT, qProxyPort);
    }

    auto qSend = size_t();
    if (const auto qCode = curl_easy_perform(qCurl);
        qCode != CURLE_OK)
    {
        curl_ws_send(qCurl, Q_NULLPTR, 0, &qSend, 0, CURLWS_CLOSE);
        curl_easy_cleanup(qCurl);
        return QString::fromStdString(curl_easy_strerror(qCode));
    }

    curl_ws_send(qCurl, Q_NULLPTR, 0, &qSend, 0, CURLWS_CLOSE);
    curl_easy_cleanup(qCurl);
    return QString();
}

size_t RaptorHttpSuite::invokeCommonWriteCallback(char *qTarget, size_t qSize, size_t nmemb, void *qData)
{
    if (qTarget == Q_NULLPTR)
    {
        return 0;
    }

    const auto qByteArray = static_cast<QByteArray *>(qData);
    qByteArray->append(qTarget, qSize * nmemb);
    return qSize * nmemb;
}
