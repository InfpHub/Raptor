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

#include "RaptorLogSuite.h"

Q_GLOBAL_STATIC(RaptorLogSuite, _LogSuite)

RaptorLogSuite::RaptorLogSuite()
{
    invokeInstanceInit();
}

RaptorLogSuite::~RaptorLogSuite()
{
    _Log.close();
}

RaptorLogSuite* RaptorLogSuite::invokeSingletonGet()
{
    return _LogSuite();
}

void RaptorLogSuite::invokeInstanceInit()
{
    _Log.setFileName(QStringLiteral("%1.log").arg(APPLICATION_NAME));
}

void RaptorLogSuite::onItemCallback(const QtMsgType& qType,
                                    const QMessageLogContext& qContext,
                                    const QString& qMessage)
{
    _Mutex.lock();
    const auto qDate = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    auto qLevel = QString();
    switch (qType)
    {
    case QtDebugMsg:
        qLevel = Log::Level::Debug;
        break;
    case QtInfoMsg:
        qLevel = Log::Level::Info;
        break;
    case QtWarningMsg:
        qLevel = Log::Level::Warning;
        break;
    case QtCriticalMsg:
        qLevel = Log::Level::Error;
        break;
    case QtFatalMsg:
        qLevel = Log::Level::Fatal;
        break;
    default:
        break;
    }

    auto qPid = QString::number(qApp->applicationPid());
    if (qPid.toULongLong() < 1000)
    {
        qPid = "00" + qPid;
    }
    else if (qPid.toULongLong() < 10000)
    {
        qPid = "0" + qPid;
    }

    auto qThread = QStringLiteral("%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    if (qThread.toULongLong() < 1000)
    {
        qThread = QStringLiteral("00%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    }
    else if (qThread.toULongLong() < 10000)
    {
        qThread = QStringLiteral("0%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    }

    auto qFunction = QRegularExpression(Log::Pattern::Function).match(qContext.function).captured();
    if (qFunction.length() == 0)
    {
        for (auto i = 0; i < Log::Pattern::MaxFunctionLength; ++i)
        {
            qFunction.append("-");
        }
    }
    else if (qFunction.length() < Log::Pattern::MaxFunctionLength)
    {
        qFunction.append(QString(Log::Pattern::MaxFunctionLength - qFunction.length(), ' '));
    }
    else if (qFunction.length() > Log::Pattern::MaxFunctionLength)
    {
        qFunction = qFunction.left(Log::Pattern::MaxFunctionLength - 3).append("...");
    }

    auto qLine = QString();
    if (qContext.line < 1000)
    {
        if (qContext.line < 100)
        {
            if (qContext.line < 10)
            {
                qLine = qLine.append(QString::number(qContext.line)).append(' ').append(' ').append(' ');
            }
            else
            {
                qLine = qLine.append(QString::number(qContext.line)).append(' ').append(' ');
            }
        }
        else
        {
            qLine = qLine.append(QString::number(qContext.line)).append(' ');
        }
    }
    else
    {
        qLine = QString::number(qContext.line);
    }

    const auto qContent = QStringLiteral(R"(%1 %2 %3 --- [%4] %5 : %6%7)").arg(qDate,
                                                                               qLevel,
                                                                               qPid,
                                                                               qThread,
                                                                               qFunction,
                                                                               qLine,
                                                                               qMessage);

#ifdef QT_DEBUG
    std::cout << qContent.toStdString() << std::endl;
#endif

    if (!_Log.exists())
    {
        if (!_Log.isOpen())
        {
            _Log.open(QIODevice::WriteOnly | QIODevice::Append);
        }
    }
    else
    {
        if (!_Log.isOpen())
        {
            _Log.open(QIODevice::WriteOnly | QIODevice::Append);
        }
    }

    auto qTextStream = QTextStream(&_Log);
    qTextStream.setCodec(QTextCodec::codecForName("UTF-8"));
    if (qLevel != Log::Level::Debug)
    {
        qTextStream << qContent << Qt::endl;
    }

    _Mutex.unlock();
}