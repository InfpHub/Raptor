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

#ifndef RAPTORLOGIN_H
#define RAPTORLOGIN_H

#include <QCloseEvent>
#include <QObject>
#include <QTimer>
#include <QWidget>

#include "../Toast/RaptorToast.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorLogin;
}

QT_END_NAMESPACE

class RaptorLogin Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    explicit RaptorLogin(QWidget* qParent = Q_NULLPTR);

    ~RaptorLogin() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeEject(const QVariant& qVariant = QVariant()) Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit() Q_DECL_OVERRIDE;

    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

Q_SIGNALS:
    Q_SIGNAL void itemQrCodeGenerating() const;

    Q_SIGNAL void itemQrCodeEncoding(const QVariant& qVariant) const;

    Q_SIGNAL void itemQrCodeStatusFetching(const QVariant& qVariant) const;

public Q_SLOTS:
    Q_SLOT void onItemQrCodeGenerated(const QVariant& qVariant);

    Q_SLOT void onItemQrCodeEncoded(const QVariant& qVariant) const;

    Q_SLOT void onItemQrCodeStatusFetched(const QVariant& qVariant) const;

    Q_SLOT void onItemAccessTokenRefreshed(const QVariant& qVariant);

private Q_SLOTS:
    Q_SLOT void onCloseClicked();

    Q_SLOT void onrQrCodeStatusTimerTimeout() const;

private:
    Ui::RaptorLogin* _Ui = Q_NULLPTR;
    QTimer* _QrCodeStatusTimer = Q_NULLPTR;
    QVariant _Variant;
    RaptorQrCode _QrCode;
};

#endif // RAPTORLOGIN_H