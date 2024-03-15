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

#include "../Downloading/RaptorDownloadingSuite.h"
#include "../File/RaptorFileSuite.h"
#include "../Http/RaptorHttpSuite.h"
#include "../Store/RaptorStoreSuite.h"
#include "../../Common/RaptorDeclare.h"

class RaptorCopySuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    static RaptorCopySuite *invokeSingletonGet();

private:
    struct Node
    {
        RaptorFileItem _Item;
        QVector<Node *> _Children;

        [[nodiscard]]
        bool isEmpty() const
        {
            return _Item._Id.isEmpty();
        }

        bool operator==(const Node &item) const
        {
            return _Item._Id == item._Item._Id;
        }

        bool operator!=(const Node &item) const
        {
            return _Item._Id != item._Item._Id;
        }
    };

    static QPair<QString, QString> invokeItemCreate(const RaptorAuthenticationItem &item,
                                                    const QString &qParent,
                                                    const QString &qName);

    static QPair<QString, bool> invokeItemRapid(const RaptorAuthenticationItem &item,
                                                const RaptorFileItem &iten);

    void invokeItemsTreeBuild(const QVector<RaptorFileItem> &items, const QString &qParent);

    void invokeItemPreorderTraversal(const Node *qRoot,
                                     const QString &qParent,
                                     const QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> &qUser) const;

Q_SIGNALS:
    Q_SIGNAL void itemsFetched(const QVariant &qVariant) const;

    Q_SIGNAL void itemCopied(const QVariant &qVariant) const;

    Q_SIGNAL void itemsCancelled(const QVariant &qVariant) const;

    Q_SIGNAL void itemErrored(const QVariant &qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemsByParentIdFetching(const QVariant &qVariant) const;

    Q_SLOT void onItemsByIdFetching(const QVariant &qVariant) const;

    Q_SLOT void onItemsByConditionFetching(const QVariant &qVariant) const;

    Q_SLOT void onItemsCopying(const QVariant &qVariant);

    Q_SLOT void onItemsCancelling();

private:
    bool _Cancel;
    QVector<Node *> _Items; // Children
    Node *_Item; // Root
};

#endif // RAPTORCOPYSUITE_H
