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

#include "RaptorCopilotSuite.h"

Q_GLOBAL_STATIC(RaptorCopilotSuite, _CopilotSuite)

RaptorCopilotSuite::RaptorCopilotSuite()
{
    invokeInstanceInit();
    invokeSlotInit();
}

RaptorCopilotSuite *RaptorCopilotSuite::invokeSingletonGet()
{
    return _CopilotSuite();
}

void RaptorCopilotSuite::invokeStop() const
{
    if (_HeartbeatTimer->isActive())
    {
        _HeartbeatTimer->stop();
    }
}

void RaptorCopilotSuite::invokeAriaHostParse() const
{
    QHostInfo::lookupHost(RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Download,
                                                                      Setting::Download::AriaHost).toString(), this, onAriaHostParsing);
}

void RaptorCopilotSuite::invokeInstanceInit()
{
    _HeartbeatTimer = new QTimer(this);
    _HeartbeatTimer->setInterval(10000);
    _HeartbeatTimer->start();
}

void RaptorCopilotSuite::invokeSlotInit() const
{
    connect(_HeartbeatTimer,
            &QTimer::timeout,
            this,
            &RaptorCopilotSuite::onHeartbeatTimerTimeout);
}

void RaptorCopilotSuite::onItemCopyWriterFinding() const
{
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    const auto qSQL = R"(SELECT Content FROM CopyWriter ORDER BY RANDOM() LIMIT 9)";
    qSQLQuery.exec(qSQL);
    auto itens = QVector<RaptorCopyWriter>();
    while (qSQLQuery.next())
    {
        auto iten = RaptorCopyWriter();
        iten._Content = qSQLQuery.value("Content").toString();
        itens << iten;
    }

    if (itens.empty())
    {
        return;
    }

    auto items = QStringList();
    items << "RaptorSpacePage" << "RaptorTransferPage" << "RaptorSharePage" << "RaptorStarPage" << "RaptorTrashPage" << "RaptorMediaPage" << "RaptorPlusPage" << "RaptorStoryPage" << "RaptorSettingPage";
    auto itos = QVector<quint32>(items.length());
    std::iota(itos.begin(), itos.end(), 1);
    std::shuffle(itos.begin(), itos.end(), std::mt19937(std::random_device()()));
    for (auto i = 0; i < items.length(); ++i)
    {
        itens[i]._Page = items[itos[i] - 1];
    }

    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QVector<RaptorCopyWriter> >(itens);
    Q_EMIT itemCopyWriterHaveFound(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorCopilotSuite::onItemNoticeFetching() const
{
    auto qHttpPayload = RaptorHttpPayload();
    qHttpPayload._Url = QStringLiteral("%1/blob/master/NOTICE").arg(GITHUB);
    const auto [qError, qStatus, qBody] = RaptorHttpSuite::invokeGet(qHttpPayload);
    if (!qError.isEmpty())
    {
        qWarning() << qError;
        return;
    }

    if (qStatus != RaptorHttpStatus::OK)
    {
        qWarning() << qError;
        return;
    }

    auto qYaml = YAML::Load(qBody.toStdString().c_str());
    const auto qNotice = qYaml["Notice"];
    const auto qId = qNotice["Id"].as<std::string>();
    const auto qType = qNotice["Type"].as<std::string>();
    auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
    auto qSQL = R"(SELECT State FROM Notice WHERE LeafId = :LeafId)";
    qSQLQuery.prepare(qSQL);
    qSQLQuery.bindValue(":LeafId", QString::fromStdString(qId));
    qSQLQuery.exec(qSQL);
    if (qSQLQuery.lastError().isValid())
    {
        const auto qErros = qSQLQuery.lastError().text();
        qCritical() << qErros;
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeCriticalEject",
                                  Qt::AutoConnection,
                                  Q_ARG(QString, qErros));
        return;
    }

    if (qSQLQuery.next())
    {
        // TODO 优化通知
        if (const auto qState = qSQLQuery.value("State").toInt();
            qState == 1)
        {
            // 该通知已读
            return;
        } else if (qState == -1 && qType == "NewVersion")
        {
            // 该通知已跳过
            return;
        }
    }

    const auto qContent = qNotice["Content"].as<std::string>();
    if (qType == "Notice")
    {
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeNoticeEject",
                                  Qt::AutoConnection,
                                  Q_ARG(bool, false),
                                  Q_ARG(QString, QString::fromStdString(qId)),
                                  Q_ARG(QString, QStringLiteral("通知")),
                                  Q_ARG(QString, QString::fromStdString(qContent)));
    } else if (qType == "NewVersion")
    {
        const auto qVersion = qYaml["Version"];
        const auto qMajor = qVersion["Major"].as<int>();
        const auto qMinor = qVersion["Minor"].as<int>();
        const auto qPatch = qVersion["Patch"].as<int>();
        if (qMajor != MAJOR_VERSION && qMinor != MINOR_VERSION && qPatch != PATCH_VERSION)
        {
            QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                      "invokeNoticeEject",
                                      Qt::AutoConnection,
                                      Q_ARG(bool, true),
                                      Q_ARG(QString, QString::fromStdString(qId)),
                                      Q_ARG(QString, QString(qCreativeTemplate).arg(QStringLiteral("Hi %1。新版本 %2.%3.%4 可用").arg(RaptorStoreSuite::invokeUserGet()._NickName, QString::number(qMajor), QString::number(qMinor)), QString::number(qPatch))),
                                      Q_ARG(QString, QString::fromStdString(qContent)));
        }
    }
}

void RaptorCopilotSuite::onItemQrCodeEncoding(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto qSize = input._Variant.value<QSize>();
    const auto qSymbol = ZBarcode_Create();
    qSymbol->symbology = BARCODE_QRCODE;
    ZBarcode_Encode_and_Buffer_Vector(qSymbol, reinterpret_cast<unsigned char *>(input._Url.toUtf8().data()), input._Url.toUtf8().length(), 0);
    auto qImage = QImage(qSize, QImage::Format_RGB888);
    auto qPainter = QPainter(&qImage);
    const auto qRect = QRect(4, 4, qSize.width() - 8, qSize.height() - 8);
    qPainter.save();
    qPainter.fillRect(qImage.rect(), Qt::white);
    qPainter.setClipRect(qRect, Qt::IntersectClip);
    auto qTranslateX = qRect.x();
    auto qTranslateY = qRect.y();
    auto qScale = 0.;
    const auto qGlobalWidth = qSymbol->vector->width;
    const auto qGlobalHeight = qSymbol->vector->height;
    if (qRect.width() / qGlobalWidth < qRect.height() / qGlobalHeight)
    {
        qScale = qRect.width() / qGlobalWidth;
    } else
    {
        qScale = qRect.height() / qGlobalHeight;
    }

    qTranslateX += (qRect.width() - qGlobalWidth * qScale) / 2.0;
    qTranslateY += (qRect.height() - qGlobalHeight * qScale) / 2.0;
    qPainter.translate(qTranslateX, qTranslateY);
    qPainter.scale(qScale, qScale);
    if (Q_DECL_CONSTEXPR auto qBackgroundColor = QColor(255, 255, 255, 255);
        qBackgroundColor.alpha() != 0)
    {
        qPainter.fillRect(QRectF(0, 0, qGlobalWidth, qGlobalHeight), QBrush(qBackgroundColor));
    }

    if (auto qRecv = qSymbol->vector->rectangles)
    {
        auto qMaxRight = -1, qMaxBottom = -1;
        for (; qRecv; qRecv = qRecv->next)
        {
            if (qRecv->x + qRecv->width > qMaxRight)
            {
                qMaxRight = qRecv->x + qRecv->width;
            }
            if (qRecv->y + qRecv->height > qMaxBottom)
            {
                qMaxBottom = qRecv->y + qRecv->height;
            }
        }

        qRecv = qSymbol->vector->rectangles;
        auto qBrush = QBrush(Qt::SolidPattern);
        while (qRecv)
        {
            qBrush.setColor(QColor(0, 0, 0, 255));
            const auto qFudgeWidth = qRecv->x + qRecv->width == qMaxRight ? 0.1F : 0.0F;
            const auto qFudgHeight = qRecv->y + qRecv->height == qMaxBottom ? 0.1F : 0.0F;
            qPainter.fillRect(QRectF(qRecv->x, qRecv->y, qRecv->width + qFudgeWidth, qRecv->height + qFudgHeight), qBrush);
            qRecv = qRecv->next;
        }
    }

    qPainter.restore();
    ZBarcode_Delete(qSymbol);
    auto output = RaptorOutput();
    output._State = true;
    output._Data = QVariant::fromValue<QPixmap>(QPixmap::fromImage(qImage));
    Q_EMIT itemQrCodeEncoded(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorCopilotSuite::onItemAriaConnectTesting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto [qAriaHost, qAriaPort] = input._Variant.value<QPair<QString, QString> >();
    auto qEndPoint = QString();
    if (input._State)
    {
        qEndPoint = QStringLiteral("wss://%1:%2/jsonrpc").arg(qAriaHost, qAriaPort);
    } else
    {
        qEndPoint = QStringLiteral("ws://%1:%2/jsonrpc").arg(qAriaHost, qAriaPort);
    }

    auto output = RaptorOutput();
    if (const auto qError = RaptorHttpSuite::invokeItemWebSocketConnectTest(qEndPoint);
        !qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemAriaConnectTested(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    Q_EMIT itemAriaConnectTested(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorCopilotSuite::onItemProxyConnectTesting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto [qHost, qPort] = input._Variant.value<QPair<QString, QString> >();
    auto output = RaptorOutput();
    if (const auto [qError, qStatus] = RaptorHttpSuite::invokeItemProxyConnectTest(input._Type,
                                                                                   input._Name,
                                                                                   input._Password,
                                                                                   qHost,
                                                                                   qPort.toUInt());
        !qError.isEmpty())
    {
        qCritical() << qError;
        output._State = false;
        output._Message = qError;
        Q_EMIT itemProxyConnectTested(QVariant::fromValue<RaptorOutput>(output));
        return;
    }

    output._State = true;
    Q_EMIT itemProxyConnectTested(QVariant::fromValue<RaptorOutput>(output));
}

void RaptorCopilotSuite::onHeartbeatExploring(const QHostInfo &qHostInfo) const
{
    if (qHostInfo.error() != QHostInfo::NoError)
    {
        qCritical() << QStringLiteral("通讯阵列损毁!");
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeCriticalEject",
                                  Qt::AutoConnection,
                                  Q_ARG(QString, QStringLiteral("通讯阵列损毁!")));
    }
}

void RaptorCopilotSuite::onAriaHostParsing(const QHostInfo &qHostInfo) const
{
    for (const auto &item: qHostInfo.addresses())
    {
        if (item == QHostAddress::LocalHost)
        {
            RaptorStoreSuite::invokeAriaIsLocalHostSet(true);
            break;
        }
    }
}

void RaptorCopilotSuite::onHeartbeatTimerTimeout() const
{
    QHostInfo::lookupHost("alipan.com", this, onHeartbeatExploring);
}
