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

#ifndef RAPTORSHARE_H
#define RAPTORSHARE_H

#include <QClipboard>

#include "../Toast/RaptorToast.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Suite/Setting/RaptorSettingSuite.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorShare;
}

QT_END_NAMESPACE

class RaptorShare Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    explicit RaptorShare(QWidget* qParent = Q_NULLPTR);

    ~RaptorShare() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeEject(const QVariant& qVariant = QVariant()) Q_DECL_OVERRIDE;

private:
    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

    void invokeIconDrawing(QWidget* qWidget, const bool& qTip = false) const;

Q_SIGNALS:
    Q_SIGNAL void itemCreating(const QVariant& qVariant);

public Q_SLOTS:
    Q_SLOT void onItemCreated(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onCloseClicked();

    Q_SLOT void onGenerateClicked() const;

    Q_SLOT void onNeverExpireStateChanged(const int& qChecked) const;

    Q_SLOT void onCreateClicked();

    Q_SLOT void onCancelClicked();

    Q_SLOT void onCopyClicked();

private:
    Ui::RaptorShare* _Ui = Q_NULLPTR;
    QVariant _Variant;
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());
};

#endif // RAPTORSHARE_H