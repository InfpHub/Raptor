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

#ifndef RAPTORFOLDER_H
#define RAPTORFOLDER_H

#include <QButtonGroup>

#include "../Toast/RaptorToast.h"
#include "../../../Common/RaptorDeclare.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Suite/Setting/RaptorSettingSuite.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorFolder;
}

QT_END_NAMESPACE

class RaptorFolder Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    explicit RaptorFolder(QWidget* qParent = Q_NULLPTR);

    ~RaptorFolder() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeEject(const QVariant& qVariant = QVariant()) Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit() Q_DECL_OVERRIDE;

    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

    void invokeIconDrawing() const;

    QString invokeRuleFilter() const;

Q_SIGNALS:
    Q_SIGNAL void itemCreating(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onCloseClicked();

    Q_SLOT void onCancelClicked();

    Q_SLOT void onCreateClicked();

private:
    Ui::RaptorFolder* _Ui = Q_NULLPTR;
    QButtonGroup* _RuleGroup = Q_NULLPTR;
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());
    QVariant _Variant;
};

#endif // RAPTORFOLDER_H