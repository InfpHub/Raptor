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

#include "RaptorWorld.h"
#include "ui_RaptorWorld.h"

RaptorWorld::RaptorWorld(QWidget *qParent) : QWidget(qParent),
                                             _Ui(new Ui::RaptorWorld),
                                             _Maximized(false),
                                             _MousePressed(false)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

bool RaptorWorld::eventFilter(QObject *qObject, QEvent *qEvent)
{
    if (qObject == this)
    {
        // 关闭事件拦截
        if (qEvent->type() == QEvent::Close)
        {
            if (const auto qCloseEvent = static_cast<QCloseEvent *>(qEvent);
                windowOpacity() == 0.)
            {
                qCloseEvent->accept();
            } else
            {
                auto qOperate = RaptorMessageBox::Nothing;
                const auto qTrayState = RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                                           Setting::Ui::TrayIcon).toBool() &&
                                        RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                                           Setting::Ui::MinimizeToTray).toBool();
                if (qTrayState)
                {
                    qOperate = RaptorMessageBox::invokeInformationEject(QStringLiteral("最小化到托盘"),
                                                                        QStringLiteral("最小化 %1 到托盘。是否继续?").arg(
                                                                            APPLICATION_NAME),
                                                                        QStringLiteral("最小化到托盘"),
                                                                        QStringLiteral("直接退出"));
                } else
                {
                    qOperate = RaptorMessageBox::invokeWarningEject(QStringLiteral("退出 %1").arg(APPLICATION_NAME),
                                                                    QStringLiteral("即将退出 %1。是否继续?").arg(
                                                                        APPLICATION_NAME));
                }

                if (qOperate == RaptorMessageBox::Yes)
                {
                    if (qTrayState)
                    {
                        _TrayAnimation->setStartValue(1);
                        _TrayAnimation->setEndValue(0);
                        _TrayAnimation->setDuration(350);
                        _TrayAnimation->setEasingCurve(QEasingCurve::InOutExpo);
                        _TrayAnimation->start();
                    } else
                    {
                        RaptorStoreSuite::invokeEngineStateSet(false);
                        QMetaObject::invokeMethod(RaptorStoreSuite::invokeEngineGet(),
                                                  "invokeStop",
                                                  Qt::AutoConnection);
                        qCloseEvent->ignore();
                        _CloseAnimation->setStartValue(1);
                        _CloseAnimation->setEndValue(0);
                        _CloseAnimation->setDuration(350);
                        _CloseAnimation->setEasingCurve(QEasingCurve::InOutExpo);
                        _CloseAnimation->start();
                    }
                } else if (qOperate == RaptorMessageBox::No)
                {
                    if (qTrayState)
                    {
                        RaptorStoreSuite::invokeEngineStateSet(false);
                        QMetaObject::invokeMethod(RaptorStoreSuite::invokeEngineGet(),
                                                  "invokeStop",
                                                  Qt::AutoConnection);
                        qCloseEvent->ignore();
                        _CloseAnimation->setStartValue(1);
                        _CloseAnimation->setEndValue(0);
                        _CloseAnimation->setDuration(350);
                        _CloseAnimation->setEasingCurve(QEasingCurve::InOutExpo);
                        _CloseAnimation->start();
                    }
                }

                qCloseEvent->ignore();
            }

            return true;
        }

        // 任务栏还原到正常状态
        if (qEvent->type() == QEvent::WindowStateChange)
        {
            if (const auto qWindowStateChangeEvent = static_cast<QWindowStateChangeEvent *>(qEvent);
                qWindowStateChangeEvent->oldState() == Qt::WindowMinimized)
            {
                if (_Maximized)
                {
                    showMaximized();
                }

                _MinimizeAnimation->setStartValue(0);
                _MinimizeAnimation->setEndValue(1);
                _MinimizeAnimation->setDuration(350);
                _MinimizeAnimation->setEasingCurve(QEasingCurve::OutBounce);
                _MinimizeAnimation->start();
                qWindowStateChangeEvent->ignore();
                return true;
            }
        }

        // 取消贴边
        if (qEvent->type() == QEvent::Enter)
        {
            if (y() == -(_Ui->_Stage->minimumHeight() + _WorldLayoutMargin))
            {
                _VergeAnimation->setStartValue(QPoint{x(), y()});
                _VergeAnimation->setEndValue(QPoint{x(), -_WorldLayoutMargin});
                _VergeAnimation->setDuration(200);
                _VergeAnimation->start();
                return true;
            }
        }
    }

    // 无边框移动和标题区域双击
    if (qObject == _Ui->_Header)
    {
        if (qEvent->type() == QEvent::MouseButtonPress)
        {
            if (const auto qMouseEvent = static_cast<QMouseEvent *>(qEvent);
                qMouseEvent->button() == Qt::LeftButton && qMouseEvent->pos().y() <= _Ui->_Header->height())
            {
                _MousePressed = true;
                _MousePoint = qMouseEvent->globalPosition().toPoint() - pos();
                return true;
            }
        } else if (qEvent->type() == QEvent::MouseButtonRelease)
        {
            RaptorStoreSuite::invokeViewPaintableSet(true);
            _MousePressed = false;
            return true;
        } else if (qEvent->type() == QEvent::MouseMove)
        {
            const auto qMouseEvent = static_cast<QMouseEvent *>(qEvent);
            if (_MousePressed && !_Maximized)
            {
                RaptorStoreSuite::invokeViewPaintableSet(false);
                move(qMouseEvent->globalPosition().toPoint() - _MousePoint);
                return true;
            }
        } else if (qEvent->type() == QEvent::MouseButtonDblClick)
        {
            onMaximizeClicked();
            return true;
        }
        // 窗体贴边
        if (qEvent->type() == QEvent::Leave)
        {
            if (y() <= -_WorldLayoutMargin && !_Maximized)
            {
                _VergeAnimation->setStartValue(QPoint{x(), y()});
                _VergeAnimation->setEndValue(QPoint{
                    x(),
                    -(_Ui->_Stage->minimumHeight() + _WorldLayoutMargin)
                });
                _VergeAnimation->setDuration(200);
                _VergeAnimation->start();
                return true;
            }
        }
    }

    if (qObject == _Ui->_Logo)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeLogoPaint();
            return true;
        }
    }

    if (qObject == _Ui->_Avatar)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeAvatarPaint();
            return true;
        }
    }

    if (qObject == _Ui->_Page)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeBackgroundPaint();
            return true;
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

void RaptorWorld::paintEvent(QPaintEvent *qPaintEvent)
{
    if (!_Maximized)
    {
        if (_Shadow.isNull())
        {
            _Shadow = RaptorBlurUtil::invokeShadowGenerate(size(), _WorldLayoutMargin, 6, QColor(0, 0, 0, 155));
        }

        auto qPainter = QPainter(this);
        qPainter.drawPixmap(rect(), _Shadow);
    }
}

void RaptorWorld::invokeRender()
{
    Q_EMIT itemCopyWriteFinding();
    _StartAnimation->setStartValue(0);
    _StartAnimation->setEndValue(1);
    _StartAnimation->setDuration(750);
    _StartAnimation->setEasingCurve(QEasingCurve::InOutExpo);
    const auto qSize = RaptorUtil::invokePrimaryDesktopGeometryCompute();
    _StartAnimatioo->setStartValue(
        QPoint(qSize.width() / 2 - width() / 2 - 220, qSize.height() / 2 - height() / 2));
    _StartAnimatioo->setEndValue(QPoint(qSize.width() / 2 - width() / 2, qSize.height() / 2 - height() / 2));
    _StartAnimatioo->setDuration(750);
    _StartAnimatioo->setEasingCurve(QEasingCurve::InOutExpo);
    _StartAnimationGroup->start();
    show();
}

RaptorUser *RaptorWorld::invokeUserUiGet() const
{
    return _User;
}

RaptorLogin *RaptorWorld::invokeLoginUiGet() const
{
    return _Login;
}

RaptorSpacePage *RaptorWorld::invokeSpacePageGet() const
{
    return _Ui->_SpacePage;
}

RaptorDevice *RaptorWorld::invokeDeviceUiGet() const
{
    return _Device;
}

RaptorFolder *RaptorWorld::invokeFolderUiGet() const
{
    return _Ui->_SpacePage->invokeFolderUiGet();
}

RaptorUpload *RaptorWorld::invokeUploadUiGet() const
{
    return _Ui->_SpacePage->invokeUploadUiGet();
}

RaptorImport *RaptorWorld::invokeImportUiGet() const
{
    return _Ui->_SpacePage->invokeImportUiGet();
}

RaptorDownload *RaptorWorld::invokeDownloadUiGet() const
{
    return _Ui->_SpacePage->invokeDownloadUiGet();
}

RaptorShare *RaptorWorld::invokeShareUiGet() const
{
    return _Ui->_SpacePage->invokeShareUiGet();
}

RaptorRename *RaptorWorld::invokeRenameUiGet() const
{
    return _Ui->_SpacePage->invokeRenameUiGet();
}

RaptorTransferPage *RaptorWorld::invokeTransferPageGet() const
{
    return _Ui->_TransferPage;
}

RaptorDownloadingPage *RaptorWorld::invokeDownloadingPageGet() const
{
    return invokeTransferPageGet()->invokeDownloadingPageGet();
}

RaptorDownloadedPage *RaptorWorld::invokeDownloadedPageGet() const
{
    return invokeTransferPageGet()->invokeDownloadedPageGet();
}

RaptorUploadingPage *RaptorWorld::invokeUploadingPageGet() const
{
    return invokeTransferPageGet()->invokeUploadingPageGet();
}

RaptorUploadedPage *RaptorWorld::invokeUploadedPageGet() const
{
    return invokeTransferPageGet()->invokeUploadedPageGet();
}

RaptorSharePage *RaptorWorld::invokeSharePageGet() const
{
    return _Ui->_SharePage;
}

RaptorStarPage *RaptorWorld::invokeStarPageGet() const
{
    return _Ui->_StarPage;
}

RaptorTrashPage *RaptorWorld::invokeTrashPageGet() const
{
    return _Ui->_TrashPage;
}

RaptorPlusPage *RaptorWorld::invokePlusPageGet() const
{
    return _Ui->_PlusPage;
}

RaptorCopyPage *RaptorWorld::invokeCopyPageGet() const
{
    return invokePlusPageGet()->invokeCopyPageGet();
}

RaptorCleanPage *RaptorWorld::invokeCleanPageGet() const
{
    return invokePlusPageGet()->invokeCleanPageGet();
}

RaptorCopyUser *RaptorWorld::invokeCopyUserUiGet() const
{
    return invokeCopyPageGet()->invokeCopyUserUiGet();
}

RaptorStoryPage *RaptorWorld::invokeStoryPageGet() const
{
    return _Ui->_StoryPage;
}

RaptorAboutPage *RaptorWorld::invokeAboutPageGet() const
{
    return _Ui->_StoryPage->invokeAboutPageGet();
}

RaptorSettingPage *RaptorWorld::invokeSettingPageGet() const
{
    return _Ui->_SettingPage;
}

RaptorNetworkPage *RaptorWorld::invokeNetworkPageGet() const
{
    return invokeSettingPageGet()->invokeNetworkPageGet();
}

void RaptorWorld::invokeSuccessEject(const QString &qMessage) const
{
    if (windowOpacity() != 1.)
    {
        return;
    }

    RaptorToast::invokeSuccessEject(qMessage);
}

void RaptorWorld::invokeWarningEject(const QString &qMessage) const
{
    if (windowOpacity() != 1.)
    {
        return;
    }

    RaptorToast::invokeWarningEject(qMessage);
}

void RaptorWorld::invokeCriticalEject(const QString &qMessage) const
{
    if (windowOpacity() != 1.)
    {
        return;
    }

    RaptorToast::invokeCriticalEject(qMessage);
}

void RaptorWorld::invokeLoginEject() const
{
    _Login->invokeEject();
}

void RaptorWorld::invokeNoticeEject(const bool &qNewVersion,
                                    const QString &qId,
                                    const QString &qTitle,
                                    const QString &qContent) const
{
    if (const auto qWorld = dynamic_cast<RaptorWorld *>(qApp->activeWindow());
        qWorld && qWorld == this)
    {
        _Notice->invokeEject(QVariant::fromValue<std::tuple<bool, QString, QString, QString> >(std::make_tuple(qNewVersion, qId, qTitle, qContent)));
    }
}

void RaptorWorld::invokeGoToSpacePage() const
{
    if (_Ui->_Space->isChecked())
    {
        return;
    }

    _Ui->_Space->setChecked(true);
    onSpaceClicked();
}

void RaptorWorld::invokeInstanceInit()
{
    _NavigationGroup = new QButtonGroup(this);
    _NavigationGroup->setExclusive(true);
    _NavigationGroup->addButton(_Ui->_Space);
    _NavigationGroup->addButton(_Ui->_Transfer);
    _NavigationGroup->addButton(_Ui->_Share);
    _NavigationGroup->addButton(_Ui->_Star);
    _NavigationGroup->addButton(_Ui->_Trash);
    _NavigationGroup->addButton(_Ui->_Plus);
    _NavigationGroup->addButton(_Ui->_Story);
    _NavigationGroup->addButton(_Ui->_Setting);
    _Ui->_Space->setChecked(true);
    _Ui->_Page->setCurrentWidget(_Ui->_SpacePage);
    _SpaceGroup = new QButtonGroup(this);
    _SpaceGroup->setExclusive(true);
    _SpaceGroup->addButton(_Ui->_Private);
    _SpaceGroup->addButton(_Ui->_Public);
    _DebounceTimer = new QTimer(this);
    _DebounceTimer->setSingleShot(true);
    _DebounceTimer->setInterval(500);

    _Device = new RaptorDevice(this);
    _Login = new RaptorLogin(this);
    _Notice = new RaptorNotice(this);
    _StartAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    _StartAnimatioo = new QPropertyAnimation(this, "pos", this);
    _StartAnimationGroup = new QParallelAnimationGroup(this);
    _StartAnimationGroup->addAnimation(_StartAnimation);
    _StartAnimationGroup->addAnimation(_StartAnimatioo);
    _MinimizeAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    _MaximizeAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    _CloseAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    _TrayAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    _VergeAnimation = new QPropertyAnimation(this, "pos", this);
    _TrayIcon = new QSystemTrayIcon(this);
    _User = new RaptorUser(this);
}

void RaptorWorld::invokeUiInit()
{
    if (const auto qTheme = RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                               Setting::Ui::Theme).toString();
        qTheme == Setting::Ui::Auto)
    {
        if (RaptorUtil::invokeSystemDarkThemeConfirm())
        {
            setStyleSheet(RaptorUtil::invokeStyleSheetLoad(Setting::Ui::Dark));
        } else
        {
            setStyleSheet(RaptorUtil::invokeStyleSheetLoad(Setting::Ui::Light));
        }
    } else
    {
        setStyleSheet(RaptorUtil::invokeStyleSheetLoad(qTheme));
    }

    qApp->setFont(RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                     Setting::Ui::Font).toString());
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle(QStringLiteral("%1 %2.%3.%4").arg(APPLICATION_NAME, QString::number(MAJOR_VERSION), QString::number(MINOR_VERSION), QString::number(PATCH_VERSION)));
    installEventFilter(this);
    _Ui->_Logo->installEventFilter(this);
    _Ui->_Name->setText(windowTitle());
    _Ui->_Adore->setIcon(QIcon(RaptorUtil::invokeIconMatch("Adore", false, true)));
    _Ui->_Adore->setIconSize(QSize(16, 16));
    _Ui->_Header->installEventFilter(this);
    _Ui->_Avatar->installEventFilter(this);
    _Ui->_Avatar->setEnabled(false);
    _Ui->_Minimize->setIcon(QIcon(RaptorUtil::invokeIconMatch("Ninimize", false, true)));
    _Ui->_Minimize->setIconSize(QSize(12, 12));
    _Ui->_Maximize->setIcon(QIcon(RaptorUtil::invokeIconMatch("Maximize", false, true)));
    _Ui->_Maximize->setIconSize(QSize(12, 12));
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(12, 12));
    _Ui->_Private->setText(QStringLiteral("备份盘"));
    _Ui->_Private->setChecked(true);
    _Ui->_Public->setText(QStringLiteral("资源盘"));
    _Ui->_CorePanelTip->setText(QStringLiteral("核心"));
    _Ui->_Space->invokeIconSet(RaptorUtil::invokeIconMatch("Space", false, true));
    _Ui->_Space->setContentsMargins(6, 0, 6, 0);
    _Ui->_Space->setIconSize(QSize(22, 22));
    _Ui->_Space->setText(QStringLiteral("空间"));
    _Ui->_Space->setChecked(true);
    _Ui->_Transfer->invokeIconSet(RaptorUtil::invokeIconMatch("Transfer", false, true));
    _Ui->_Transfer->setContentsMargins(6, 0, 6, 0);
    _Ui->_Transfer->setIconSize(QSize(22, 22));
    _Ui->_Transfer->setText(QStringLiteral("传输"));
    _Ui->_Share->invokeIconSet(RaptorUtil::invokeIconMatch("Share", false, true));
    _Ui->_Share->setContentsMargins(6, 0, 6, 0);
    _Ui->_Share->setIconSize(QSize(22, 22));
    _Ui->_Share->setText(QStringLiteral("分享"));
    _Ui->_Star->invokeIconSet(RaptorUtil::invokeIconMatch("Star", false, true));
    _Ui->_Star->setContentsMargins(6, 0, 6, 0);
    _Ui->_Star->setIconSize(QSize(22, 22));
    _Ui->_Star->setText(QStringLiteral("收藏"));
    _Ui->_Trash->invokeIconSet(RaptorUtil::invokeIconMatch("Trash", false, true));
    _Ui->_Trash->setContentsMargins(6, 0, 6, 0);
    _Ui->_Trash->setIconSize(QSize(22, 22));
    _Ui->_Trash->setText(QStringLiteral("回收站"));
    _Ui->_Plus->invokeIconSet(RaptorUtil::invokeIconMatch("Plus", false, true));
    _Ui->_Plus->setContentsMargins(6, 0, 6, 0);
    _Ui->_Plus->setIconSize(QSize(22, 22));
    _Ui->_Plus->setText(QStringLiteral("Plus"));
    _Ui->_ExtraPanelTip->setText(QStringLiteral("附加"));
    _Ui->_Story->invokeIconSet(RaptorUtil::invokeIconMatch("Story", false, true));
    _Ui->_Story->setContentsMargins(6, 0, 6, 0);
    _Ui->_Story->setIconSize(QSize(22, 22));
    _Ui->_Story->setText(QStringLiteral("故事"));
    _Ui->_Setting->invokeIconSet(RaptorUtil::invokeIconMatch("Setting", false, true));
    _Ui->_Setting->setContentsMargins(6, 0, 6, 0);
    _Ui->_Setting->setIconSize(QSize(22, 22));
    _Ui->_Setting->setText(QStringLiteral("设置"));
    _Ui->_Page->installEventFilter(this);
    _Ui->_Page->setCurrentWidget(_Ui->_SpacePage);
    _Ui->_Capacity->invokeIconSet(RaptorUtil::invokeIconMatch("Capacity", false, true));
    _Ui->_Capacity->invokeIndicatorVisibleSet(false);
    _Ui->_Capacity->setContentsMargins(6, 0, 6, 0);
    _Ui->_Capacity->setIconSize(QSize(22, 22));
    _Ui->_Capacity->invokeIndicatorHeightSet(16);
    _Ui->_Capacity->setText(QStringLiteral("容量使用"));
    _Ui->_AlbumCapacity->invokeIconSet(RaptorUtil::invokeIconMatch("Album", false, true));
    _Ui->_AlbumCapacity->invokeIndicatorVisibleSet(false);
    _Ui->_AlbumCapacity->setContentsMargins(6, 0, 6, 0);
    _Ui->_AlbumCapacity->setIconSize(QSize(22, 22));
    _Ui->_AlbumCapacity->invokeIndicatorHeightSet(16);
    _Ui->_AlbumCapacity->setText(QStringLiteral("相册使用"));
    _Ui->_NoteCapacity->invokeIconSet(RaptorUtil::invokeIconMatch("Note", false, true));
    _Ui->_NoteCapacity->invokeIndicatorVisibleSet(false);
    _Ui->_NoteCapacity->setContentsMargins(6, 0, 6, 0);
    _Ui->_NoteCapacity->setIconSize(QSize(22, 22));
    _Ui->_NoteCapacity->invokeIndicatorHeightSet(16);
    _Ui->_NoteCapacity->setText(QStringLiteral("笔记使用"));
    _Ui->_Meet->invokeIconSet(RaptorUtil::invokeIconMatch("Meet", false, true));
    _Ui->_Meet->invokeIndicatorVisibleSet(false);
    _Ui->_Meet->setContentsMargins(6, 0, 6, 0);
    _Ui->_Meet->setIconSize(QSize(22, 22));
    _Ui->_Meet->invokeIndicatorHeightSet(16);
    _Ui->_Meet->setText(QStringLiteral("一起走过"));
    _Ui->_Copyright->setText(QStringLiteral("2024 凉州刺史 - Built on %1").arg(RaptorUtil::invokeCompileTimestampCompute()));
    _Ui->_PoweredTip->setText(QStringLiteral("Powered by"));
    _Ui->_Powered->setText(QStringLiteral("Qt"));
    _TrayIcon->setIcon(QIcon(RaptorUtil::invokeIconMatch("Free", false, true)));
}

void RaptorWorld::invokeSlotInit() const
{
    connect(_DebounceTimer,
            &QTimer::timeout,
            this,
            &RaptorWorld::onDebounceTimerTimeout);

    connect(_Ui->_Adore,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onAdoreClicked);

    connect(_Ui->_Private,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onPrivateClicked);

    connect(_Ui->_Public,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onPublicClicked);

    connect(_Ui->_Space,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onSpaceClicked);

    connect(_Ui->_Transfer,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onTransferClicked);

    connect(_Ui->_Share,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onShareClicked);

    connect(_Ui->_Star,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onStarClicked);

    connect(_Ui->_Trash,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onTrashClicked);

    connect(_Ui->_Plus,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onExtraClicked);

    connect(_Ui->_Story,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onStoryClicked);

    connect(_Ui->_Setting,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onSettingClicked);

    connect(_Ui->_Powered,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onPoweredClicked);

    connect(_StartAnimationGroup,
            &QParallelAnimationGroup::finished,
            this,
            &RaptorWorld::onStartAnimationGroupFinished);

    connect(_Ui->_Avatar,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onAvatarClicked);

    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onCloseClicked);

    connect(_Ui->_Minimize,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onMinimizeClicked);

    connect(_Ui->_Maximize,
            &QPushButton::clicked,
            this,
            &RaptorWorld::onMaximizeClicked);

    connect(_MinimizeAnimation,
            &QPropertyAnimation::finished,
            this,
            &RaptorWorld::onMinimizeAnimationFinished);

    connect(_MaximizeAnimation,
            &QPropertyAnimation::valueChanged,
            this,
            &RaptorWorld::onMaximizeAnimationValueChanged);

    connect(_MaximizeAnimation,
            &QPropertyAnimation::finished,
            this,
            &RaptorWorld::onMaximizeAnimationFinished);

    connect(_CloseAnimation,
            &QPropertyAnimation::finished,
            this,
            &RaptorWorld::onCloseAnimationFinished);

    connect(_TrayAnimation,
            &QPropertyAnimation::finished,
            this,
            &RaptorWorld::onTrayAnimationFinished);

    connect(_TrayIcon,
            &QSystemTrayIcon::activated,
            this,
            &RaptorWorld::onTrayIconActivated);
}

void RaptorWorld::invokeLogoPaint() const
{
    _SvgRender->load(RaptorUtil::invokeIconMatch("Free", false, true));
    if (_SvgRender->isValid())
    {
        auto qPainter = QPainter(_Ui->_Logo);
        _SvgRender->render(&qPainter, _Ui->_Logo->rect());
    }
}

void RaptorWorld::invokeBackgroundPaint()
{
    auto qPainter = QPainter(_Ui->_Page);
    auto qTheme = property(Setting::Ui::Theme).toString();
    if (qTheme.isEmpty())
    {
        // 该属性为空则设置一次即可
        // 避免在切换主题而程序没有重启的情况下基于最新的主题进行绘制
        qTheme = RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                    Setting::Ui::Theme).toString();
        setProperty(Setting::Ui::Theme, qTheme);
    }

    if (qTheme == Setting::Ui::Auto)
    {
        if (RaptorUtil::invokeSystemDarkThemeConfirm())
        {
            qTheme = Setting::Ui::Dark;
        } else
        {
            qTheme = Setting::Ui::Light;
        }
    }

    const auto qPixmap = QPixmap(QStringLiteral("./Store/Image/%1/Raptor[LANDINGMOON].png").arg(qTheme));
    qPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    qPainter.setOpacity(0.25);
    qPainter.drawPixmap(_Ui->_Page->width() - qPixmap.width(),
                        _Ui->_Page->height() - qPixmap.height(),
                        qPixmap.width(),
                        qPixmap.height(),
                        qPixmap);
}

void RaptorWorld::invokeAvatarPaint() const
{
    if (_Avatar.isNull())
    {
        return;
    }

    auto qPainter = QPainter(_Ui->_Avatar);
    qPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    static_cast<void>(_Avatar.scaled(_Ui->_Avatar->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    auto qAvatarPath = QPainterPath();
    qAvatarPath.addEllipse(0, 0, _Ui->_Avatar->width(), _Ui->_Avatar->height());
    qPainter.setClipPath(qAvatarPath);
    qPainter.drawPixmap(0, 0, _Ui->_Avatar->width(), _Ui->_Avatar->height(), _Avatar);
}

void RaptorWorld::onAvatarClicked() const
{
    if (const auto [qOperate, item] = _User->invokeEject(
            _Ui->_Avatar->mapToGlobal(QPoint(-(_User->width() - _Ui->_Avatar->width()) / 2, 52)));
        qOperate == RaptorUser::Login)
    {
        _Login->invokeEject();
    } else if (qOperate == RaptorUser::Logout)
    {
        if (const auto qOperatf = RaptorMessageBox::invokeWarningEject(QStringLiteral("注销用户"),
                                                                       QStringLiteral("即将注销用户 %1,，是否继续?").arg(
                                                                           QString(WARNING_TEMPLATE).arg(
                                                                               item._Nickname)));
            qOperatf == RaptorMessageBox::No)
        {
            return;
        }

        auto input = RaptorInput();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(item);
        Q_EMIT itemLogouting(QVariant::fromValue<RaptorInput>(input));
    } else if (qOperate == RaptorUser::Switch)
    {
        if (const auto qOperatf = RaptorMessageBox::invokeWarningEject(QStringLiteral("切换用户"),
                                                                       QStringLiteral("即将切换到用户 %1，是否继续?").arg(
                                                                           QString(WARNING_TEMPLATE).arg(
                                                                               item._Nickname)));
            qOperatf == RaptorMessageBox::No)
        {
            return;
        }

        _Ui->_Capacity->invokeIndicatorVisibleSet(false);
        _Ui->_AlbumCapacity->invokeIndicatorVisibleSet(false);
        _Ui->_NoteCapacity->invokeIndicatorVisibleSet(false);
        _Ui->_Meet->invokeIndicatorVisibleSet(false);
        RaptorStoreSuite::invokeUserSwitchingSet(true);
        auto input = RaptorInput();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(item);
        Q_EMIT itemSwitching(QVariant::fromValue<RaptorInput>(input));
    } else if (qOperate == RaptorUser::Device)
    {
        Q_EMIT itemDevicesFetching();
        _Device->invokeEject();
    }
}

void RaptorWorld::onItemAccessTokenRefreshed(const QVariant &qVariant)
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    Q_EMIT itemsLoading();
    Q_EMIT itemSignInInfoFetching();
    const auto item = RaptorStoreSuite::invokeUserGet();
    _Ui->_Capacity->invokeIndicatorTextSet(item._Capacity._Radio);
    _Ui->_Capacity->invokeIndicatorVisibleSet(!item._Capacity._Radio.isEmpty());
    _Ui->_AlbumCapacity->invokeIndicatorTextSet(item._Capacity._Album);
    _Ui->_AlbumCapacity->invokeIndicatorVisibleSet(!item._Capacity._Album.isEmpty());
    _Ui->_NoteCapacity->invokeIndicatorTextSet(item._Capacity._Note);
    _Ui->_NoteCapacity->invokeIndicatorVisibleSet(!item._Capacity._Note.isEmpty());
    _Ui->_Meet->invokeIndicatorTextSet(item._Meet);
    _Ui->_Meet->invokeIndicatorVisibleSet(!item._Meet.isEmpty());
    _Avatar.loadFromData(item._Avatar);
    _Ui->_Avatar->setEnabled(true);
    _Ui->_Avatar->update();
    RaptorStoreSuite::invokeSpaceSet(_Ui->_Private->isChecked() ? Private : Public);
}

void RaptorWorld::onItemLogouting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item != RaptorStoreSuite::invokeUserGet())
    {
        return;
    }

    _Ui->_Capacity->invokeIndicatorVisibleSet(false);
    _Ui->_AlbumCapacity->invokeIndicatorVisibleSet(false);
    _Ui->_NoteCapacity->invokeIndicatorVisibleSet(false);
    _Ui->_Meet->invokeIndicatorVisibleSet(false);
}

void RaptorWorld::onItemLogoutd(const QVariant &qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto item = _Data.value<RaptorAuthenticationItem>();
    if (item != RaptorStoreSuite::invokeUserGet())
    {
        // 非当前用户注销，则什么也不做
        return;
    }

    // 注销的是当前用户，则选择表中第一个用户为当前用户（如果有）
    if (RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        RaptorToast::invokeSuccessEject(QStringLiteral("已切换到 %1 用户。").arg(
            QString(INFORMATION_TEMPLATE).arg(RaptorStoreSuite::invokeUserGet()._Nickname)));
        return;
    }

    _Avatar = QPixmap();
    _Ui->_Avatar->update();
    RaptorToast::invokeSuccessEject(
        QStringLiteral("%1 已退出登录!").arg(QString(SUCCESS_TEMPLATE).arg(item._Nickname)));
}

void RaptorWorld::onItemSwitched(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto item = _Data.value<RaptorAuthenticationItem>();
    RaptorToast::invokeSuccessEject(
        QStringLiteral("已切换到 %1 用户。").arg(QString(INFORMATION_TEMPLATE).arg(item._Nickname)));
    RaptorStoreSuite::invokeUserSwitchingSet(false);
    Q_EMIT itemAccessTokenRefreshing();
    Q_EMIT itemsLoading();
}

void RaptorWorld::onDebounceTimerTimeout() const
{
    RaptorStoreSuite::invokeSpaceSet(_Ui->_Private->isChecked() ? Private : Public);
    if (_Ui->_Space->isChecked())
    {
        invokeSpacePageGet()->invokeNavigate();
    } else if (_Ui->_Share->isChecked())
    {
        invokeSharePageGet()->invokeNavigate();
    } else if (_Ui->_Star->isChecked())
    {
        invokeStarPageGet()->invokeNavigate();
    } else if (_Ui->_Trash->isChecked())
    {
        invokeTrashPageGet()->invokeNavigate();
    }

    Q_EMIT itemSpaceChanging();
}

void RaptorWorld::onAdoreClicked() const
{
    QDesktopServices::openUrl(QUrl(GITHUB));
}

void RaptorWorld::onPrivateClicked(const bool &qChecked) const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    if (RaptorStoreSuite::invokeSpaceGet() == Private)
    {
        return;
    }

    _DebounceTimer->start();
}

void RaptorWorld::onPublicClicked(const bool &qChecked) const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    if (RaptorStoreSuite::invokeSpaceGet() == Public)
    {
        return;
    }

    _DebounceTimer->start();
}

void RaptorWorld::onSpaceClicked() const
{
    if (_Ui->_Page->currentWidget() == _Ui->_SpacePage)
    {
        return;
    }

    _Ui->_Page->setCurrentWidget(_Ui->_SpacePage);
}

void RaptorWorld::onTransferClicked() const
{
    if (_Ui->_Page->currentWidget() == _Ui->_TransferPage)
    {
        return;
    }

    _Ui->_Page->setCurrentWidget(_Ui->_TransferPage);
}

void RaptorWorld::onShareClicked() const
{
    if (_Ui->_Page->currentWidget() == _Ui->_SharePage)
    {
        return;
    }

    _Ui->_Page->setCurrentWidget(_Ui->_SharePage);
    _Ui->_SharePage->invokeNavigate();
}

void RaptorWorld::onStarClicked() const
{
    if (_Ui->_Page->currentWidget() == _Ui->_StarPage)
    {
        return;
    }

    _Ui->_Page->setCurrentWidget(_Ui->_StarPage);
    _Ui->_StarPage->invokeNavigate();
}

void RaptorWorld::onTrashClicked() const
{
    if (_Ui->_Page->currentWidget() == _Ui->_TrashPage)
    {
        return;
    }

    _Ui->_Page->setCurrentWidget(_Ui->_TrashPage);
    _Ui->_TrashPage->invokeNavigate();
}

void RaptorWorld::onExtraClicked() const
{
    if (_Ui->_Page->currentWidget() == _Ui->_PlusPage)
    {
        return;
    }

    _Ui->_Page->setCurrentWidget(_Ui->_PlusPage);
}

void RaptorWorld::onStoryClicked() const
{
    if (_Ui->_Page->currentWidget() == _Ui->_StoryPage)
    {
        return;
    }

    _Ui->_StoryPage->invokeAboutPageGet()->invokeNavigate();
    _Ui->_Page->setCurrentWidget(_Ui->_StoryPage);
}

void RaptorWorld::onSettingClicked() const
{
    if (_Ui->_Page->currentWidget() == _Ui->_SettingPage)
    {
        return;
    }

    _Ui->_Page->setCurrentWidget(_Ui->_SettingPage);
}

void RaptorWorld::onStartAnimationGroupFinished() const
{
    RaptorStoreSuite::invokeEngineStateSet(true);
    Q_EMIT itemAccessTokenRefreshing();
    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                           Setting::Ui::TrayIcon).toBool())
    {
        _TrayIcon->show();
    }

    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                           Setting::Ui::Notice).toBool())
    {
        Q_EMIT itemNoticeFetching();
    }
}

void RaptorWorld::onPoweredClicked() const
{
    QDesktopServices::openUrl(QUrl("https://www.qt.io"));
}

void RaptorWorld::onMinimizeClicked() const
{
    _MinimizeAnimation->setStartValue(1);
    _MinimizeAnimation->setEndValue(0);
    _MinimizeAnimation->setDuration(300);
    _MinimizeAnimation->setEasingCurve(QEasingCurve::OutBounce);
    _MinimizeAnimation->start();
}

void RaptorWorld::onMinimizeAnimationFinished()
{
    if (windowOpacity() == 0.)
    {
        setWindowState(Qt::WindowMinimized);
    } else
    {
        if (!_Maximized)
        {
            showNormal();
        }
    }
}

void RaptorWorld::onMaximizeClicked()
{
    _Maximized = !_Maximized;
    _MaximizeAnimation->setStartValue(1);
    _MaximizeAnimation->setEndValue(0);
    _MaximizeAnimation->setDuration(350);
    _MaximizeAnimation->setEasingCurve(QEasingCurve::InOutQuint);
    _MaximizeAnimation->start();
}

void RaptorWorld::onMaximizeAnimationValueChanged(const QVariant &qVariant)
{
    Q_UNUSED(qVariant)
    if (windowOpacity() == 0.)
    {
        if (_Maximized)
        {
            _Ui->_WorldLayout->setContentsMargins(0, 0, 0, 0);
            showMaximized();
        } else
        {
            _Ui->_WorldLayout->setContentsMargins(_WorldLayoutMargin, _WorldLayoutMargin, _WorldLayoutMargin,
                                                  _WorldLayoutMargin);
            showNormal();
        }
    }
}

void RaptorWorld::onMaximizeAnimationFinished() const
{
    if (windowOpacity() == 0.)
    {
        _MaximizeAnimation->setStartValue(0);
        _MaximizeAnimation->setEndValue(1);
        _MaximizeAnimation->setDuration(350);
        _MaximizeAnimation->setEasingCurve(QEasingCurve::InOutQuint);
        _MaximizeAnimation->start();
    }

    if (_Maximized)
    {
        _Ui->_Maximize->setIcon(QIcon(RaptorUtil::invokeIconMatch("Restore", false, true)));
    } else
    {
        _Ui->_Maximize->setIcon(QIcon(RaptorUtil::invokeIconMatch("Maximize", false, true)));
    }
}

void RaptorWorld::onCloseClicked()
{
    close();
}

void RaptorWorld::onCloseAnimationFinished()
{
    close();
}

void RaptorWorld::onTrayAnimationFinished()
{
    if (windowOpacity() == 0.)
    {
        hide();
    }
}

void RaptorWorld::onTrayIconActivated(const QSystemTrayIcon::ActivationReason &qReason)
{
    if (qReason != QSystemTrayIcon::Trigger)
    {
        return;
    }

    if (isVisible())
    {
        return;
    }

    _TrayAnimation->setStartValue(0);
    _TrayAnimation->setEndValue(1);
    _TrayAnimation->setDuration(350);
    _TrayAnimation->setEasingCurve(QEasingCurve::InOutExpo);
    _TrayAnimation->start();
    show();
}
