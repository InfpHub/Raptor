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

#ifndef RAPTORFILESUITE_H
#define RAPTORFILESUITE_H

#include <QObject>

#include "../Http/RaptorHttpSuite.h"
#include "../Store/RaptorStoreSuite.h"
#include "../../Common/RaptorDeclare.h"
#include "../../Util/RaptorUtil.h"

class RaptorFileSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    static RaptorFileSuite* invokeSingletonGet();

    static QVector<RaptorFileItem> invokeMapToItems(const QJsonArray& qArray);

    static RaptorOutput invokeItemUrlFetch(const RaptorAuthenticationItem& item,
                                           const QString& qId);

Q_SIGNALS:
    Q_SIGNAL void itemsFetched(const QVariant& qVariant) const;

    Q_SIGNAL void itemCreated(const QVariant& qVariant) const;

    Q_SIGNAL void itemRenamed(const QVariant& qVariant) const;

    Q_SIGNAL void itemUrlFetched(const QVariant& qVariant) const;

    Q_SIGNAL void itemRapided(const QVariant& qVariant) const;

    Q_SIGNAL void itemsMoved(const QVariant& qVariant) const;

    Q_SIGNAL void itemsCopied(const QVariant& qVariant) const;

    Q_SIGNAL void itemContentFetched(const QVariant& qVariant) const;

    Q_SIGNAL void itemPreviewPlayInfoFetched(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemsByParentIdFetching(const QVariant& qVariant) const;

    Q_SLOT void onItemsByIdFetching(const QVariant& qVariant) const;

    Q_SLOT void onItemsByConditionFetching(const QVariant& qVariant) const;

    Q_SLOT void onItemCreating(const QVariant& qVariant) const;

    Q_SLOT void onItemRenaming(const QVariant& qVariant) const;

    Q_SLOT void onItemUrlFetching(const QVariant& qVariant) const;

    Q_SLOT void onItemsMoving(const QVariant& qVariant) const;

    Q_SLOT void onItemsCopying(const QVariant& qVariant) const;

    Q_SLOT void onItemPreviewPlayInfoFetching(const QVariant& qVariant) const;
};

#endif // RAPTORFILESUITE_H