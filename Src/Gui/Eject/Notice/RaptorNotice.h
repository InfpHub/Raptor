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

#ifndef RAPTORUPDATE_H
#define RAPTORUPDATE_H

#include <QClipboard>
#include <QDesktopServices>
#include <QRegularExpression>

#include "../Toast/RaptorToast.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Suite/Setting/RaptorSettingSuite.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorNotice;
}

QT_END_NAMESPACE

class RaptorNotice Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    explicit RaptorNotice(QWidget* qParent = Q_NULLPTR);

    ~RaptorNotice() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeEject(const QVariant& qVariant = QVariant()) Q_DECL_OVERRIDE;

private:
    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

    void invokeIconDrawing() const;

private Q_SLOTS:
    Q_SLOT void onCloseClicked();

    Q_SLOT void onDownloadClicked();

    Q_SLOT void onSkipClicked();

private:
    Ui::RaptorNotice* _Ui = Q_NULLPTR;
    QString _Id; // 通知 Id
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());
};

#endif // RAPTORUPDATE_H