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

#include "RaptorPersistenceSuite.h"

Q_GLOBAL_STATIC(RaptorPersistenceSuite, _PersistenceSuite)

RaptorPersistenceSuite::RaptorPersistenceSuite()
{
    invokeInstanceInit();
}

RaptorPersistenceSuite::~RaptorPersistenceSuite()
{
    _SqlDatabase.close();
}

RaptorPersistenceSuite* RaptorPersistenceSuite::invokeSingletonGet()
{
    return _PersistenceSuite();
}

QSqlQuery RaptorPersistenceSuite::invokeQueryGenerate()
{
    return QSqlQuery(invokeSingletonGet()->invokeConnectionGet());
}

void RaptorPersistenceSuite::invokeTransaction()
{
    invokeSingletonGet()->invokeConnectionGet().transaction();
}

void RaptorPersistenceSuite::invokeCommit()
{
    invokeSingletonGet()->invokeConnectionGet().commit();
}

QSqlDatabase RaptorPersistenceSuite::invokeConnectionGet()
{
    return _SqlDatabase;
}

void RaptorPersistenceSuite::invokeInstanceInit()
{
    _SqlDatabase = QSqlDatabase::addDatabase("QSQLITE", QStringLiteral("%1 %2.%3.%4 Persistence Connection").arg(APPLICATION_NAME)
                                                                                                            .arg(MAJOR_VERSION)
                                                                                                            .arg(MINOR_VERSION)
                                                                                                            .arg(PATCH_VERSION));
    _SqlDatabase.setDatabaseName(QStringLiteral("%1.db").arg(APPLICATION_NAME));
    if (!_SqlDatabase.open())
    {
        qCritical() << QStringLiteral("无法打开数据库: %1").arg(_SqlDatabase.lastError().text());
    }
}