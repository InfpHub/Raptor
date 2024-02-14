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

#ifndef RAPTORLOGSUITE_H
#define RAPTORLOGSUITE_H

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QMutex>
#include <QRegularExpression>
#include <QThread>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTextStream>

#include <iostream>

#include "../../Common/RaptorDefine.h"

QT_BEGIN_NAMESPACE

namespace Log
{
    namespace Level
    {
        Q_DECL_CONSTEXPR auto Debug = "  DEBUG";
        Q_DECL_CONSTEXPR auto Info = "   INFO";
        Q_DECL_CONSTEXPR auto Warning = "WARNING";
        Q_DECL_CONSTEXPR auto Error = "  ERROR";
        Q_DECL_CONSTEXPR auto Fatal = "  FATAL";
    }

    namespace Pattern
    {
        Q_DECL_CONSTEXPR auto Function = "Raptor[a-zA-Z_:]+::~?[a-zA-Z_:]+";
        Q_DECL_CONSTEXPR auto MaxFunctionLength = 55;
    }
}

QT_END_NAMESPACE

class RaptorLogSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    explicit RaptorLogSuite();

    ~RaptorLogSuite() Q_DECL_OVERRIDE;

    static RaptorLogSuite* invokeSingletonGet();

private:
    void invokeInstanceInit();

public Q_SLOTS:
    Q_SLOT void onItemCallback(const QtMsgType& qType,
                               const QMessageLogContext& qContext,
                               const QString& qMessage);

private:
    QFile _Log;
    QMutex _Mutex;
};

#endif // RAPTORLOGSUITE_H