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

#ifndef RAPTORCOPYSUITE_H
#define RAPTORCOPYSUITE_H
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

#include "../File/RaptorFileSuite.h"
#include "../Http/RaptorHttpSuite.h"
#include "../Store/RaptorStoreSuite.h"
#include "../../Common/RaptorDeclare.h"

class RaptorCopySuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    static RaptorCopySuite* invokeSingletonGet();

private:
    static RaptorOutput invokeItemRapid(const RaptorAuthenticationItem& item,
                                        const RaptorFileItem& iten);


Q_SIGNALS:
    Q_SIGNAL void itemsFetched(const QVariant& qVariant);

    Q_SIGNAL void itemCopied(const QVariant& qVariant);

    Q_SIGNAL void itemsCancelled(const QVariant& qVariant);

public Q_SLOTS:
    Q_SLOT void onItemsByParentIdFetching(const QVariant& qVariant);

    Q_SLOT void onItemsByIdFetching(const QVariant& qVariant);

    Q_SLOT void onItemsByConditionFetching(const QVariant& qVariant);

    Q_SLOT void onItemsCopying(const QVariant& qVariant);

    Q_SLOT void onItemsCancelling();

private:
    bool _Cancel;
};

#endif // RAPTORCOPYSUITE_H