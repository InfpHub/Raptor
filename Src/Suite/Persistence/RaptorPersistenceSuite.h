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

#ifndef RAPTORPERSISTENCESUITE_H
#define RAPTORPERSISTENCESUITE_H

#include <QApplication>
#include <QDir>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>

#include "../../Common/RaptorDeclare.h"

class RaptorPersistenceSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    explicit RaptorPersistenceSuite();

    ~RaptorPersistenceSuite() Q_DECL_OVERRIDE;

    static RaptorPersistenceSuite* invokeSingletonGet();

    static QSqlQuery invokeQueryGenerate();

    static void invokeTransaction();

    static void invokeCommit();

private:
    void invokeInstanceInit();

    QSqlDatabase invokeConnectionGet();

private:
    QSqlDatabase _SqlDatabase;
};

#endif // RAPTORPERSISTENCESUITE_H