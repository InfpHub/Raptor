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

#ifndef RAPTORWORLD_H
#define RAPTORWORLD_H

#include <QCloseEvent>
#include <QMouseEvent>
#include <QThreadPool>

#include "../Eject/Login/RaptorLogin.h"
#include "../Eject/User/RaptorUser.h"
#include "../Page/Plus/RaptorPlusPage.h"
#include "../Page/Setting/RaptorSettingPage.h"
#include "../Page/Setting/Network/RaptorNetworkPage.h"
#include "../Page/Share/RaptorSharePage.h"
#include "../Page/Space/RaptorSpacePage.h"
#include "../Page/Star/RaptorStarPage.h"
#include "../Page/Story/RaptorStoryPage.h"
#include "../Page/Transfer/RaptorTransferPage.h"
#include "../Page/Trash/RaptorTrashPage.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorWorld;
}

QT_END_NAMESPACE

class RaptorWorld Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorWorld(QWidget* qParent = Q_NULLPTR);

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

protected:
    void paintEvent(QPaintEvent* qPaintEvent) Q_DECL_OVERRIDE;

    auto show() -> void;

public:
    void invokeRender();

    RaptorUser* invokeUserUiGet() const;

    RaptorLogin* invokeLoginUiGet() const;

    RaptorSpacePage* invokeSpacePageGet() const;

    RaptorFolder* invokeFolderUiGet() const;

    RaptorUpload* invokeUploadUiGet() const;

    RaptorImport* invokeImportUiGet() const;

    RaptorDownload* invokeDownloadUiGet() const;

    RaptorShare* invokeShareUiGet() const;

    RaptorTransferPage* invokeTransferPageGet() const;

    RaptorDownloadingPage* invokeDownloadingPageGet() const;

    RaptorDownloadedPage* invokeDownloadedPageGet() const;

    RaptorUploadingPage* invokeUploadingPageGet() const;

    RaptorUploadedPage* invokeUploadedPageGet() const;

    RaptorSharePage* invokeSharePageGet() const;

    RaptorStarPage* invokeStarPageGet() const;

    RaptorTrashPage* invokeTrashPageGet() const;

    RaptorPlusPage* invokePlusPageGet() const;

    RaptorCopyPage* invokeCopyPageGet() const;

    RaptorCopyUser* invokeCopyUserUiGet() const;

    RaptorStoryPage* invokeStoryPageGet() const;

    RaptorAboutPage* invokeAboutPageGet() const;

    RaptorSettingPage* invokeSettingPageGet() const;

    RaptorNetworkPage* invokeNetworkPageGet() const;

    Q_INVOKABLE void invokeToastSuccessEject(const QString& qMessage) const;

    Q_INVOKABLE void invokeToastCriticalEject(const QString& qMessage) const;

    Q_INVOKABLE void invokeLoginEject() const;

private:
    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

    void invokeLogoPaint() const;

    void invokeBackgroundPaint();

    void invokeAvatarPaint() const;

Q_SIGNALS:
    Q_SIGNAL void itemAccessTokenRefreshing() const;

    Q_SIGNAL void itemCopyWritingFinding() const;

    Q_SIGNAL void itemLogouting(const QVariant& qVariant) const;

    Q_SIGNAL void itemSpaceChanging() const;

    Q_SIGNAL void itemSwitching(const QVariant& qVariant) const;

    Q_SIGNAL void itemsLoading() const;

public Q_SLOTS:
    Q_SLOT void onItemAccessTokenRefreshed(const QVariant& qVariant);

    Q_SLOT void onItemLogouting(const QVariant& qVariant) const;

    Q_SLOT void onItemLogoutd(const QVariant& qVariant);

    Q_SLOT void onItemSwitched(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onDebounceTimerTimeout() const;

    Q_SLOT void onAdoreClicked() const;

    Q_SLOT void onPrivateClicked(const bool& qChecked) const;

    Q_SLOT void onPublicClicked(const bool& qChecked) const;

    Q_SLOT void onSpaceClicked() const;

    Q_SLOT void onTransferClicked() const;

    Q_SLOT void onShareClicked() const;

    Q_SLOT void onStarClicked() const;

    Q_SLOT void onTrashClicked() const;

    Q_SLOT void onExtraClicked() const;

    Q_SLOT void onStoryClicked() const;

    Q_SLOT void onSettingClicked() const;

    Q_SLOT void onPoweredClicked() const;

    Q_SLOT void onStartAnimationGroupFinished() const;

    Q_SLOT void onAvatarClicked() const;

    Q_SLOT void onMinimizeClicked() const;

    Q_SLOT void onMinimizeAnimationFinished();

    Q_SLOT void onMaximizeClicked();

    Q_SLOT void onMaximizeAnimationValueChanged(const QVariant& qVariant);

    Q_SLOT void onMaximizeAnimationFinished() const;

    Q_SLOT void onCloseClicked();

    Q_SLOT void onCloseAnimationFinished();

private:
    static Q_DECL_CONSTEXPR auto _WorldLayoutMargin = 12;
    Ui::RaptorWorld* _Ui = Q_NULLPTR;
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());
    RaptorUser* _User = Q_NULLPTR;
    RaptorLogin* _Login = Q_NULLPTR;
    QPixmap _Avatar;
    QPropertyAnimation* _StartAnimation = Q_NULLPTR;
    QPropertyAnimation* _StartAnimatioo = Q_NULLPTR;
    QParallelAnimationGroup* _StartAnimationGroup = Q_NULLPTR;
    QPropertyAnimation* _MinimizeAnimation = Q_NULLPTR;
    QPropertyAnimation* _MaximizeAnimation = Q_NULLPTR;
    QPropertyAnimation* _CloseAnimation = Q_NULLPTR;
    QPropertyAnimation* _VergeAnimation = Q_NULLPTR;
    bool _Maximized;
    bool _MousePressed;
    QPoint _MousePoint;
    QPixmap _Shadow;
    QButtonGroup* _NavigationGroup = Q_NULLPTR;
    QButtonGroup* _SpaceGroup = Q_NULLPTR;
    QTimer* _DebounceTimer = Q_NULLPTR;
};

#endif // RAPTORWORLD_H