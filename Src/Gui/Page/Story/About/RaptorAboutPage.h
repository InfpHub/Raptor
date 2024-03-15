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

#ifndef RAPTORABOUTPAGE_H
#define RAPTORABOUTPAGE_H

#include <QDesktopServices>
#include <QPropertyAnimation>
#include <QSvgRenderer>
#include <QTimer>

#include "../../../../Common/RaptorDeclare.h"
#include "../../../../Delegate/Page/Story/About/RaptorTimelineDelegate.h"
#include "../../../../Suite/Store/RaptorStoreSuite.h"
#include "../../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorAboutPage;
}

QT_END_NAMESPACE

class RaptorAboutPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorAboutPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorAboutPage() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeNavigate() const;

private:
    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

    void invokeAvatarPaint() const;

public Q_SLOTS:
    Q_SLOT void onItemAccessTokenRefreshed(const QVariant& qVariant);

    Q_SLOT void onItemSignInInfoFetched(const QVariant& qVariant) const;

    Q_SLOT void onItemLogouting(const QVariant& qVariant);

private Q_SLOTS:
    Q_SLOT void onThanksQtClicked() const;

    Q_SLOT void onThanksCurlClicked() const;

    Q_SLOT void onThanksSassClicked() const;

    Q_SLOT void onThanksSECP256K1Clicked() const;

    Q_SLOT void onThanksUVClicked() const;

    Q_SLOT void onThanksYamlClicked() const;

    Q_SLOT void onThanksZIntClicked() const;

    Q_SLOT void onAvatarAnimationValueChanged(const QVariant& qVariant);

    Q_SLOT void onAvatarTimerTimeout();

private:
    Ui::RaptorAboutPage* _Ui = Q_NULLPTR;
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());
    QPixmap _Avatar;
    quint16 _AvatarAlpha;
    quint16 _AvatarRadius;
    QPropertyAnimation* _AvatarAnimation = Q_NULLPTR;
    QTimer* _AvatarTimer = Q_NULLPTR;
    QStandardItemModel* _TimelineModel = Q_NULLPTR;
    RaptorTimelineDelegate* _TimelineDelegate = Q_NULLPTR;
};

#endif //RAPTORABOUTPAGE_H