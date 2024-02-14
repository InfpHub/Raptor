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

#ifndef RAPTORDOWNLOADEDSUITE_H
#define RAPTORDOWNLOADEDSUITE_H

#include <QObject>

#include "../Persistence/RaptorPersistenceSuite.h"
#include "../Store/RaptorStoreSuite.h"
#include "../../Common/RaptorDeclare.h"
#include "../../Util/RaptorUtil.h"

class RaptorDownloadedSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    static RaptorDownloadedSuite* invokeSingletonGet();

Q_SIGNALS:
    Q_SIGNAL void itemsLoaded(const QVariant& qVariant) const;

    Q_SIGNAL void itemsDeleted(const QVariant& qVariant) const;

    Q_SIGNAL void itemsCleared(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemsLoading() const;

    Q_SLOT void onItemsClearing() const;

    Q_SLOT void onItemsDeleting(const QVariant& qVariant) const;

    Q_SLOT void onItemCompleted(const QVariant& qVariant) const;
};

#endif // RAPTORDOWNLOADEDSUITE_H