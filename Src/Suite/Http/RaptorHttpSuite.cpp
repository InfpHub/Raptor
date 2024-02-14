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

RaptorHttpSuite* RaptorHttpSuite::invokeSingletonGet()
{
    return _HttpSuite();
}

std::tuple<QString, quint32, QByteArray> RaptorHttpSuite::invokeGet(const RaptorHttpPayload& qHttpPayload)
{
    const auto qCurl = curl_easy_init();
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    const auto qCurlUrl = curl_url();
    curl_url_set(qCurlUrl, CURLUPART_URL, qHttpPayload._Url.toStdString().data(), 0L);
    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                           Setting::Network::Proxy).toBool())
    {
        if (const auto qProxyEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                         Setting::Network::ProxyEngine).toString();
            qProxyEngine == Setting::Network::HTTP)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        }
        else if (qProxyEngine == Setting::Network::HTTP1_0)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
        }
        else if (qProxyEngine == Setting::Network::HTTPS)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
        }
        else if (qProxyEngine == Setting::Network::HTTPS2)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
        }
        else if (qProxyEngine == Setting::Network::SOCKS4)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
        }
        else if (qProxyEngine == Setting::Network::SOCKS5)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        }

        const auto qProxyHost = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyHost).toString();
        const auto qProxyPort = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyPort).toUInt();
        curl_easy_setopt(qCurl, CURLOPT_PROXY, qProxyHost.toStdString().data());
        curl_easy_setopt(qCurl, CURLOPT_PROXYPORT, qProxyPort);
        if (const auto qProxyUserName = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyUserName).toString();
            !qProxyUserName.isNull())
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYUSERNAME, qProxyUserName.toStdString().data());
        }
        if (const auto qProxyPassword = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyPassword).toString();
            !qProxyPassword.isNull())
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYPASSWORD, qProxyPassword.toStdString().data());
        }
    }

    for (auto iterator = qHttpPayload._Query.begin(); iterator != qHttpPayload._Query.end(); ++iterator)
    {
        auto query = iterator.key().toStdString() + "=" + iterator.value().toString().toStdString();
        curl_url_set(qCurlUrl, CURLUPART_QUERY, query.data(), CURLU_APPENDQUERY);
    }

    auto qBegin = static_cast<curl_httppost*>(Q_NULLPTR);
    auto qEnd = static_cast<curl_httppost*>(Q_NULLPTR);
    for (auto qIterator = qHttpPayload._Form.begin(); qIterator != qHttpPayload._Form.end(); ++qIterator)
    {
        curl_formadd(&qBegin,
                     &qEnd,
                     CURLFORM_COPYNAME,
                     qIterator.key().toStdString().data(),
                     CURLFORM_COPYCONTENTS,
                     qIterator.value().toString().toStdString().data());
    }

    curl_easy_setopt(qCurl, CURLOPT_HTTPGET, qBegin);
    auto qUrl = static_cast<char*>(Q_NULLPTR);
    curl_url_get(qCurlUrl, CURLUPART_URL, &qUrl, 0L);
    curl_easy_setopt(qCurl, CURLOPT_URL, qUrl);
    curl_easy_setopt(qCurl, CURLOPT_USERAGENT, RaptorHttpHeader::UserAgent);
    auto qHeader = static_cast<curl_slist*>(Q_NULLPTR);
    for (auto qIterator = qHttpPayload._Header.begin(); qIterator != qHttpPayload._Header.end(); ++qIterator)
    {
        qHeader = curl_slist_append(qHeader, (qIterator.key().toStdString() + ": " + qIterator.value().toString().toStdString()).data());
    }

    curl_easy_setopt(qCurl, CURLOPT_HTTPHEADER, qHeader);
    auto qBody = QByteArray();
    curl_easy_setopt(qCurl, CURLOPT_WRITEDATA, &qBody);
    curl_easy_setopt(qCurl, CURLOPT_WRITEFUNCTION, invokeCommonWriteCallback);
    auto qStatus = quint32();
    if (const auto qCode = curl_easy_perform(qCurl);
        qCode != CURLE_OK)
    {
        return std::make_tuple(QString::fromStdString(curl_easy_strerror(qCode)), qStatus, qBody);
    }

    curl_easy_getinfo(qCurl, CURLINFO_RESPONSE_CODE, &qStatus);
    curl_slist_free_all(qHeader);
    curl_url_cleanup(qCurlUrl);
    curl_easy_cleanup(qCurl);
    qInfo() << qStatus << " -> " << qHttpPayload._Url;
    if (qStatus == RaptorHttpStatus::TooManyRequests)
    {
        return invokeGet(qHttpPayload);
    }

    return std::make_tuple(QString(), qStatus, qBody);
}

std::tuple<QString, quint32, QByteArray> RaptorHttpSuite::invokePost(const RaptorHttpPayload& qHttpPayload)
{
    const auto qCurl = curl_easy_init();
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    const auto qCurlUrl = curl_url();
    curl_url_set(qCurlUrl, CURLUPART_URL, qHttpPayload._Url.toStdString().data(), 0L);
    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                           Setting::Network::Proxy).toBool())
    {
        if (const auto qProxyEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                         Setting::Network::ProxyEngine).toString();
            qProxyEngine == Setting::Network::HTTP)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        }
        else if (qProxyEngine == Setting::Network::HTTP1_0)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
        }
        else if (qProxyEngine == Setting::Network::HTTPS)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
        }
        else if (qProxyEngine == Setting::Network::HTTPS2)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
        }
        else if (qProxyEngine == Setting::Network::SOCKS4)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
        }
        else if (qProxyEngine == Setting::Network::SOCKS5)
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        }

        const auto qProxyHost = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyHost).toString();
        const auto qProxyPort = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                   Setting::Network::ProxyPort).toUInt();
        curl_easy_setopt(qCurl, CURLOPT_PROXY, qProxyHost.toStdString().data());
        curl_easy_setopt(qCurl, CURLOPT_PROXYPORT, qProxyPort);
        if (const auto qProxyUserName = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyUserName).toString();
            !qProxyUserName.isNull())
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYUSERNAME, qProxyUserName.toStdString().data());
        }
        if (const auto qProxyPassword = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                           Setting::Network::ProxyPassword).toString();
            !qProxyPassword.isNull())
        {
            curl_easy_setopt(qCurl, CURLOPT_PROXYPASSWORD, qProxyPassword.toStdString().data());
        }
    }

    for (auto qIterator = qHttpPayload._Query.begin(); qIterator != qHttpPayload._Query.end(); ++qIterator)
    {
        const auto qQuery = qIterator.key().toStdString() + "=" + qIterator.value().toString().toStdString();
        curl_url_set(qCurlUrl, CURLUPART_QUERY, qQuery.data(), CURLU_APPENDQUERY);
    }

    auto qBegin = static_cast<curl_httppost*>(Q_NULLPTR);
    auto qEnd = static_cast<curl_httppost*>(Q_NULLPTR);
    for (auto qIterator = qHttpPayload._Form.begin(); qIterator != qHttpPayload._Form.end(); ++qIterator)
    {
        curl_formadd(&qBegin,
                     &qEnd,
                     CURLFORM_COPYNAME,
                     qIterator.key().toStdString().data(),
                     CURLFORM_COPYCONTENTS,
                     qIterator.value().toString().toStdString().data());
    }

    curl_easy_setopt(qCurl, CURLOPT_HTTPPOST, qBegin);
    const auto qRow = qHttpPayload._Body.toJson().toStdString();
    curl_easy_setopt(qCurl, CURLOPT_POSTFIELDS, qRow.data());
    auto qUrl = static_cast<char*>(Q_NULLPTR);
    curl_url_get(qCurlUrl, CURLUPART_URL, &qUrl, 0L);
    curl_easy_setopt(qCurl, CURLOPT_URL, qUrl);
    curl_easy_setopt(qCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(qCurl, CURLOPT_USERAGENT, RaptorHttpHeader::UserAgent);
    auto qHeader = static_cast<curl_slist*>(Q_NULLPTR);
    for (auto qIterator = qHttpPayload._Header.begin(); qIterator != qHttpPayload._Header.end(); ++qIterator)
    {
        qHeader = curl_slist_append(
            qHeader, (qIterator.key().toStdString() + ": " + qIterator.value().toString().toStdString()).data());
    }

    curl_easy_setopt(qCurl, CURLOPT_HTTPHEADER, qHeader);
    curl_easy_setopt(qCurl, CURLOPT_WRITEFUNCTION, invokeCommonWriteCallback);
    auto qBody = QByteArray();
    curl_easy_setopt(qCurl, CURLOPT_WRITEDATA, &qBody);
    auto qStatus = quint32();
    if (const auto qCode = curl_easy_perform(qCurl);
        qCode != CURLE_OK)
    {
        return std::make_tuple(QString::fromStdString(curl_easy_strerror(qCode)), qStatus, qBody);
    }

    curl_easy_getinfo(qCurl, CURLINFO_RESPONSE_CODE, &qStatus);
    curl_slist_free_all(qHeader);
    curl_url_cleanup(qCurlUrl);
    curl_easy_cleanup(qCurl);
    qInfo() << qStatus << " -> " << qHttpPayload._Url;
    if (qStatus == RaptorHttpStatus::TooManyRequests)
    {
        return invokePost(qHttpPayload);
    }

    return std::make_tuple(QString(), qStatus, qBody);
}

std::pair<QString, quint32> RaptorHttpSuite::invokeItemProxyConnectTest(const QString& qType,
                                                                    const QString& qUsername,
                                                                    const QString& qPassword,
                                                                    const QString& qHost,
                                                                    const quint32& qPort)
{
    const auto qCurl = curl_easy_init();
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(qCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(qCurl, CURLOPT_URL, "https://aliyundrive.com");
    curl_easy_setopt(qCurl, CURLOPT_CONNECT_ONLY, 2L);
    if (qType == Setting::Network::HTTP)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
    }
    else if (qType == Setting::Network::HTTP1_0)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP_1_0);
    }
    else if (qType == Setting::Network::HTTPS)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS);
    }
    else if (qType == Setting::Network::HTTPS2)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTPS2);
    }
    else if (qType == Setting::Network::SOCKS4)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
    }
    else if (qType == Setting::Network::SOCKS5)
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
    }

    curl_easy_setopt(qCurl, CURLOPT_PROXY, qHost.toStdString().data());
    curl_easy_setopt(qCurl, CURLOPT_PROXYPORT, qPort);
    if (!qUsername.isNull())
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYUSERNAME, qUsername.toStdString().data());
    }
    if (!qPassword.isNull())
    {
        curl_easy_setopt(qCurl, CURLOPT_PROXYPASSWORD, qPassword.toStdString().data());
    }

    auto qStatus = quint32();
    if (const auto qCode = curl_easy_perform(qCurl);
        qCode != CURLE_OK)
    {
        return std::make_pair(QString::fromStdString(curl_easy_strerror(qCode)), qStatus);
    }

    curl_easy_getinfo(qCurl, CURLINFO_RESPONSE_CODE, &qStatus);
    curl_easy_cleanup(qCurl);
    return std::make_pair(QString(), qStatus);
}

size_t RaptorHttpSuite::invokeCommonWriteCallback(char* qTarget, size_t qSize, size_t nmemb, void* qData)
{
    if (qTarget == Q_NULLPTR)
    {
        return 0;
    }

    const auto qByteArray = static_cast<QByteArray*>(qData);
    qByteArray->append(qTarget, qSize * nmemb);
    return qSize * nmemb;
}