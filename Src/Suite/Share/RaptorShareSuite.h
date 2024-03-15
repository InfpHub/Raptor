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

#ifndef RAPTORSHARESUITE_H
#define RAPTORSHARESUITE_H

#include <QObject>

#include "../Http/RaptorHttpSuite.h"
#include "../Store/RaptorStoreSuite.h"
#include "../../Common/RaptorDeclare.h"
#include "../../Util/RaptorUtil.h"

class RaptorShareSuitePrivate;

class RaptorShareSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    static RaptorShareSuite *invokeSingletonGet();

private:
    static std::tuple<QString, RaptorInput, QVector<RaptorFileItem>> invokeItemsByTokenFetching(const RaptorInput &input);


Q_SIGNALS:
    Q_SIGNAL void itemsFetched(const QVariant &qVariant) const;

    Q_SIGNAL void itemsExported(const QVariant &qVariant) const;

    Q_SIGNAL void itemsCancelled(const QVariant &qVariant) const;

    Q_SIGNAL void itemCreated(const QVariant &qVariant) const;

    Q_SIGNAL void itemsRapidCreated(const QVariant &qVariant) const;

    Q_SIGNAL void itemsRapidImported(const QVariant &qVariant) const;

    Q_SIGNAL void itemsImported(const QVariant &qVariant) const;

    Q_SIGNAL void itemParsed(const QVariant &qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemsFetching(const QVariant &qVariant) const;

    Q_SLOT void onItemsExporting(const QVariant &qVariant) const;

    Q_SLOT void onItemsCancelling(const QVariant &qVariant) const;

    Q_SLOT void onItemCreating(const QVariant &qVariant) const;

    Q_SLOT void onItemsRapidCreating(const QVariant &qVariant) const;

    Q_SLOT void onItemsRapidFetching() const;

    Q_SLOT void onItemRapidImporting(const QVariant &qVariant) const;

    Q_SLOT void onItemParsing(const QVariant &qVariant) const;

    Q_SLOT void onItemsByParentIdFetching(const QVariant &qVariant) const;

    Q_SLOT void onItemImporting(const QVariant &qVariant) const;
};

#endif // RAPTORSHARESUITE_H
