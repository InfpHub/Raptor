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

#include "RaptorEngine.h"

RaptorEngine::RaptorEngine(QObject *qParent) : QObject(qParent)
{
    invokeInstanceInit();
    invokeSlotInit();
    invokeFontRegister();
    RaptorStoreSuite::invokeEngineSet(this);
}

RaptorEngine::~RaptorEngine()
{
    _SuiteThread->quit();
    _SuiteThread->wait();
    qFree(_World)
    qInfo() << QStringLiteral("Raptor 引擎已停止。");
}

void RaptorEngine::invokeStart() const
{
    _World->invokeRender();
    qInfo() << QStringLiteral("Raptor 引擎已启动。");
}

void RaptorEngine::invokeStop() const
{
    QMetaObject::invokeMethod(_AuthenticationSuite,
                              "invokeStop",
                              Qt::AutoConnection);

    if (const auto qEngine = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Download,
                                                                         Setting::Download::PrimaryEngine).toString();
        qEngine == Setting::Download::Aria)
    {
        QMetaObject::invokeMethod(_AriaSuite,
                                  "invokeStop",
                                  Qt::AutoConnection);
    } else if (qEngine == Setting::Download::Embed)
    {
        QMetaObject::invokeMethod(_DownloadingSuite,
                                  "invokeStop",
                                  Qt::AutoConnection);
    }

    QMetaObject::invokeMethod(_UploadingSuite,
                              "invokeStop",
                              Qt::AutoConnection);

    QMetaObject::invokeMethod(_CopilotSuite,
                              "invokeStop",
                              Qt::AutoConnection);

    RaptorSettingSuite::invokeStop();
}

void RaptorEngine::invokeInstanceInit()
{
    qInstallMessageHandler(invokeMessageCallback);
    _SuiteThread = new QThread(this);
    _SettingSuite = RaptorSettingSuite::invokeSingletonGet();
    _SettingSuite->moveToThread(_SuiteThread);
    _PersistenceSuite = RaptorPersistenceSuite::invokeSingletonGet();
    _PersistenceSuite->moveToThread(_SuiteThread);
    _AriaSuite = RaptorAriaSuite::invokeSingletonGet();
    _AriaSuite->moveToThread(_SuiteThread);
    _DownloadingSuite = RaptorDownloadingSuite::invokeSingletonGet();
    _DownloadingSuite->moveToThread(_SuiteThread);
    _AuthenticationSuite = RaptorAuthenticationSuite::invokeSingletonGet();
    _AuthenticationSuite->moveToThread(_SuiteThread);
    _FileSuite = RaptorFileSuite::invokeSingletonGet();
    _FileSuite->moveToThread(_SuiteThread);
    _DownloadedSuite = RaptorDownloadedSuite::invokeSingletonGet();
    _DownloadedSuite->moveToThread(_SuiteThread);
    _UploadingSuite = RaptorUploadingSuite::invokeSingletonGet();
    _UploadingSuite->moveToThread(_SuiteThread);
    _UploadedSuite = RaptorUploadedSuite::invokeSingletonGet();
    _UploadedSuite->moveToThread(_SuiteThread);
    _ShareSuite = RaptorShareSuite::invokeSingletonGet();
    _ShareSuite->moveToThread(_SuiteThread);
    _StarSuite = RaptorStarSuite::invokeSingletonGet();
    _StarSuite->moveToThread(_SuiteThread);
    _TrashSuite = RaptorTrashSuite::invokeSingletonGet();
    _TrashSuite->moveToThread(_SuiteThread);
    _MediaSuite = RaptorMediaSuite::invokeSingletonGet();
    _MediaSuite->moveToThread(_SuiteThread);
    _CopySuite = RaptorCopySuite::invokeSingletonGet();
    _CopySuite->moveToThread(_SuiteThread);
    _CopilotSuite = RaptorCopilotSuite::invokeSingletonGet();
    _CopilotSuite->moveToThread(_SuiteThread);
    _CleanSuite = RaptorCleanSuite::invokeSingletonGet();
    _CleanSuite->moveToThread(_SuiteThread);
    _SuiteThread->start();
    _World = new RaptorWorld();
    if (RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Download,
                                                    Setting::Download::PrimaryEngine).toString() == Setting::Download::Aria)
    {
        QMetaObject::invokeMethod(_CopilotSuite, "invokeAriaHostParse");
    }
}

void RaptorEngine::invokeSlotInit() const
{
    connect(_World->invokeUserUiGet(),
            &RaptorUser::itemsLoading,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemsLoading);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemsLoaded,
            _World->invokeUserUiGet(),
            &RaptorUser::onItemsLoaded);

    // 签到
    connect(_World->invokeUserUiGet(),
            &RaptorUser::itemSigningIn,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemSigningIn);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemSignedIn,
            _World->invokeUserUiGet(),
            &RaptorUser::onItemSignedIn);

    // 刷新容量
    connect(_World->invokeUserUiGet(),
            &RaptorUser::itemCapacityRefreshing,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemCapacityRefreshing);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemCapacityRefreshed,
            _World->invokeUserUiGet(),
            &RaptorUser::onItemCapacityRefreshed);

    // 注销
    connect(_World,
            &RaptorWorld::itemLogouting,
            _World,
            &RaptorWorld::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeDownloadingPageGet(),
            &RaptorDownloadingPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeDownloadedPageGet(),
            &RaptorDownloadedPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeUploadingPageGet(),
            &RaptorUploadingPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeUploadedPageGet(),
            &RaptorUploadedPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeSharePageGet(),
            &RaptorSharePage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeStarPageGet(),
            &RaptorStarPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeTrashPageGet(),
            &RaptorTrashPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeMediaPageGet(),
            &RaptorMediaPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokePlusPageGet(),
            &RaptorPlusPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeCopyPageGet(),
            &RaptorCopyPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _World->invokeAboutPageGet(),
            &RaptorAboutPage::onItemLogouting);

    connect(_World,
            &RaptorWorld::itemLogouting,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemLogouting);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemLogouted,
            _World->invokeUserUiGet(),
            &RaptorUser::onItemLogoutd);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemLogouted,
            _World,
            &RaptorWorld::onItemLogoutd);

    // 切换用户
    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeDownloadingPageGet(),
            &RaptorDownloadingPage::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeDownloadedPageGet(),
            &RaptorDownloadedPage::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeUploadingPageGet(),
            &RaptorUploadingPage::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeUploadedPageGet(),
            &RaptorUploadedPage::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeSharePageGet(),
            &RaptorSharePage::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeStarPageGet(),
            &RaptorStarPage::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeTrashPageGet(),
            &RaptorTrashPage::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeMediaPageGet(),
            &RaptorMediaPage::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _World->invokeUserUiGet(),
            &RaptorUser::onItemSwitching);

    connect(_World,
            &RaptorWorld::itemSwitching,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemSwitching);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemSwitched,
            _World,
            &RaptorWorld::onItemSwitched);

    // 登录设备
    connect(_World,
            &RaptorWorld::itemDevicesFetching,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemDevicesFetching);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemDevicesFetched,
            _World->invokeDeviceUiGet(),
            &RaptorDevice::onItemDevicesFetched);

    // 生成登录二维码
    connect(_World->invokeLoginUiGet(),
            &RaptorLogin::itemQrCodeGenerating,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemQrCodeGenerating);

    // 登录二维码已生成
    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemQrCodeGenerated,
            _World->invokeLoginUiGet(),
            &RaptorLogin::onItemQrCodeGenerated);

    connect(_World->invokeLoginUiGet(),
            &RaptorLogin::itemQrCodeEncoding,
            _CopilotSuite,
            &RaptorCopilotSuite::onItemQrCodeEncoding);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemQrCodeEncoded,
            _World->invokeLoginUiGet(),
            &RaptorLogin::onItemQrCodeEncoded);

    // 拉取登录二维码状态
    connect(_World->invokeLoginUiGet(),
            &RaptorLogin::itemQrCodeStatusFetching,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemQrCodeStatusFetching);

    // 登录二维码状态已拉取
    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemQrCodeStatusFetched,
            _World->invokeLoginUiGet(),
            &RaptorLogin::onItemQrCodeStatusFetched);

    // 二维码登录成功
    connect(_World,
            &RaptorWorld::itemAccessTokenRefreshing,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemAccessTokenRefreshing);

    // 令牌已刷新
    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World,
            &RaptorWorld::onItemAccessTokenRefreshed);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemAccessTokenRefreshed);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World->invokeSharePageGet(),
            &RaptorSharePage::onItemAccessTokenRefreshed);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World->invokeStarPageGet(),
            &RaptorStarPage::onItemAccessTokenRefreshed);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World->invokeTrashPageGet(),
            &RaptorTrashPage::onItemAccessTokenRefreshed);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World->invokeMediaPageGet(),
            &RaptorMediaPage::onItemAccessTokenRefreshed);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World->invokeCopyPageGet(),
            &RaptorCopyPage::onItemAccessTokenRefreshed);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World->invokeLoginUiGet(),
            &RaptorLogin::onItemAccessTokenRefreshed);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World->invokeUserUiGet(),
            &RaptorUser::onItemAccessTokenRefreshed);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemAccessTokenRefreshed,
            _World->invokeAboutPageGet(),
            &RaptorAboutPage::onItemAccessTokenRefreshed);

    connect(_World,
            &RaptorWorld::itemNoticeFetching,
            _CopilotSuite,
            &RaptorCopilotSuite::onItemNoticeFetching);

    connect(_World,
            &RaptorWorld::itemSignInInfoFetching,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemSignInInfoFetching);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemSignInInfoFetched,
            _World->invokeAboutPageGet(),
            &RaptorAboutPage::onItemSignInInfoFetched);

    connect(_World,
            &RaptorWorld::itemCopyWriteFinding,
            _CopilotSuite,
            &RaptorCopilotSuite::onItemCopyWriterFinding);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemCopyWriterHaveFound,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemCopyWriterHaveFound);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemCopyWriterHaveFound,
            _World->invokeTransferPageGet(),
            &RaptorTransferPage::onItemCopyWriterHaveFound);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemCopyWriterHaveFound,
            _World->invokeSharePageGet(),
            &RaptorSharePage::onItemCopyWriterHaveFound);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemCopyWriterHaveFound,
            _World->invokeStarPageGet(),
            &RaptorStarPage::onItemCopyWriterHaveFound);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemCopyWriterHaveFound,
            _World->invokeTrashPageGet(),
            &RaptorTrashPage::onItemCopyWriterHaveFound);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemCopyWriterHaveFound,
            _World->invokeMediaPageGet(),
            &RaptorMediaPage::onItemCopyWriterHaveFound);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemCopyWriterHaveFound,
            _World->invokePlusPageGet(),
            &RaptorPlusPage::onItemCopyWriterHaveFound);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemCopyWriterHaveFound,
            _World->invokeStoryPageGet(),
            &RaptorStoryPage::onItemCopyWriterHaveFound);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemCopyWriterHaveFound,
            _World->invokeSettingPageGet(),
            &RaptorSettingPage::onItemCopyWriterHaveFound);

    connect(_World,
            &RaptorWorld::itemsLoading,
            _DownloadedSuite,
            &RaptorDownloadedSuite::onItemsLoading);

    connect(_World,
            &RaptorWorld::itemsLoading,
            _UploadingSuite,
            &RaptorUploadingSuite::onItemsLoading);

    connect(_World,
            &RaptorWorld::itemsLoading,
            _UploadedSuite,
            &RaptorUploadedSuite::onItemsLoading);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsByParentIdFetching,
            _FileSuite,
            &RaptorFileSuite::onItemsByParentIdFetching);

    connect(_FileSuite,
            &RaptorFileSuite::itemsFetched,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsFetched);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsByIdFetching,
            _FileSuite,
            &RaptorFileSuite::onItemsByIdFetching);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsByConditionFetching,
            _FileSuite,
            &RaptorFileSuite::onItemsByConditionFetching);

    connect(_FileSuite,
            &RaptorFileSuite::itemCreated,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemCreated);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsRenaming,
            _FileSuite,
            &RaptorFileSuite::onItemsRenaming);

    connect(_World->invokeRenameUiGet(),
            &RaptorRename::itemsRenaming,
            _FileSuite,
            &RaptorFileSuite::onItemsRenaming);

    connect(_FileSuite, &RaptorFileSuite::itemsRenamed,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsRenamed);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsStarring,
            _StarSuite,
            &RaptorStarSuite::onItemsStarring);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsRapidCreating,
            _ShareSuite,
            &RaptorShareSuite::onItemsRapidCreating);

    connect(_ShareSuite,
            &RaptorShareSuite::itemsRapidCreated,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsRapidCreated);

    connect(_ShareSuite,
            &RaptorShareSuite::itemsRapidImported,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsRapidImported);

    connect(_StarSuite,
            &RaptorStarSuite::itemsStarred,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsStarred);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemUrlFetching,
            _FileSuite,
            &RaptorFileSuite::onItemUrlFetching);

    connect(_FileSuite,
            &RaptorFileSuite::itemUrlFetched,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemUrlFetched);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemVideoPreviewPlayInfoFetching,
            _FileSuite,
            &RaptorFileSuite::onItemVideoPreviewPlayInfoFetching);

    connect(_FileSuite,
            &RaptorFileSuite::itemVideoPreviewPlayInfoFetched,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemVideoPreviewPlayInfoFetched);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsTrashing,
            _TrashSuite,
            &RaptorTrashSuite::onItemsTrashing);

    connect(_TrashSuite,
            &RaptorTrashSuite::itemsTrashed,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsTrashed);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsCopying,
            _FileSuite,
            &RaptorFileSuite::onItemsCopying);

    connect(_FileSuite,
            &RaptorFileSuite::itemsCopied,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsCopied);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsMoving,
            _FileSuite,
            &RaptorFileSuite::onItemsMoving);

    connect(_FileSuite,
            &RaptorFileSuite::itemsMoved,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsMoved);

    connect(_World->invokeSpacePageGet(),
            &RaptorSpacePage::itemsDeleting,
            _FileSuite,
            &RaptorFileSuite::onItemsDeleting);

    connect(_FileSuite,
            &RaptorFileSuite::itemsDeleted,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsDeleted);

    if (const auto qEngine = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Download,
                                                                         Setting::Download::PrimaryEngine).toString();
        qEngine == Setting::Download::Aria)
    {
        connect(_World->invokeDownloadUiGet(),
                &RaptorDownload::itemsDownloading,
                _AriaSuite,
                &RaptorAriaSuite::onItemsDownloading);

        connect(_AriaSuite,
                &RaptorAriaSuite::itemsQueuing,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemsQueuing);

        connect(_AriaSuite,
                &RaptorAriaSuite::itemsStatusChanged,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemsStatusChanged);

        // 暂停下载
        connect(_World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::itemsPausing,
                _AriaSuite,
                &RaptorAriaSuite::onItemsPausing);

        connect(_AriaSuite,
                &RaptorAriaSuite::itemPaused,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemPaused);

        // 继续下载
        connect(_World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::itemsResuming,
                _AriaSuite,
                &RaptorAriaSuite::onItemsResuming);

        connect(_AriaSuite,
                &RaptorAriaSuite::itemCompleted,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemCompleted);

        connect(_AriaSuite,
                &RaptorAriaSuite::itemCompleted,
                _World->invokeDownloadedPageGet(),
                &RaptorDownloadedPage::onItemCompleted);

        connect(_AriaSuite,
                &RaptorAriaSuite::itemCompleted,
                _DownloadedSuite,
                &RaptorDownloadedSuite::onItemCompleted);

        connect(_AriaSuite,
                &RaptorAriaSuite::itemErrored,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemErrored);

        connect(_World,
                &RaptorWorld::itemsLoading,
                _AriaSuite,
                &RaptorAriaSuite::onItemsLoading);

        connect(_AriaSuite,
                &RaptorAriaSuite::itemsLoaded,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemsLoaded);

        connect(_World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::itemCancelling,
                _AriaSuite,
                &RaptorAriaSuite::onItemCancelling);
    } else if (qEngine == Setting::Download::Embed)
    {
        connect(_World->invokeDownloadUiGet(),
                &RaptorDownload::itemsDownloading,
                _DownloadingSuite,
                &RaptorDownloadingSuite::onItemsDownloading);

        connect(_DownloadingSuite,
                &RaptorDownloadingSuite::itemsQueuing,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemsQueuing);

        connect(_DownloadingSuite,
                &RaptorDownloadingSuite::itemsStatusChanged,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemsStatusChanged);

        // 暂停下载
        connect(_World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::itemsPausing,
                _DownloadingSuite,
                &RaptorDownloadingSuite::onItemsPausing);

        connect(_DownloadingSuite,
                &RaptorDownloadingSuite::itemPaused,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemPaused);

        // 继续下载
        connect(_World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::itemsResuming,
                _DownloadingSuite,
                &RaptorDownloadingSuite::onItemsResuming);

        connect(_DownloadingSuite,
                &RaptorDownloadingSuite::itemCompleted,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemCompleted);

        connect(_DownloadingSuite,
                &RaptorDownloadingSuite::itemCompleted,
                _World->invokeDownloadedPageGet(),
                &RaptorDownloadedPage::onItemCompleted);

        connect(_DownloadingSuite,
                &RaptorDownloadingSuite::itemCompleted,
                _DownloadedSuite,
                &RaptorDownloadedSuite::onItemCompleted);

        connect(_DownloadingSuite,
                &RaptorDownloadingSuite::itemErrored,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemErrored);

        connect(_World,
                &RaptorWorld::itemsLoading,
                _DownloadingSuite,
                &RaptorDownloadingSuite::onItemsLoading);

        connect(_DownloadingSuite,
                &RaptorDownloadingSuite::itemsLoaded,
                _World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::onItemsLoaded);

        connect(_World->invokeDownloadingPageGet(),
                &RaptorDownloadingPage::itemCancelling,
                _DownloadingSuite,
                &RaptorDownloadingSuite::onItemCancelling);
    }

    connect(_DownloadedSuite,
            &RaptorDownloadedSuite::itemsLoaded,
            _World->invokeDownloadedPageGet(),
            &RaptorDownloadedPage::onItemsLoaded);

    // 删除下载记录
    connect(_World->invokeDownloadedPageGet(),
            &RaptorDownloadedPage::itemsDeleting,
            _DownloadedSuite,
            &RaptorDownloadedSuite::onItemsDeleting);

    connect(_DownloadedSuite,
            &RaptorDownloadedSuite::itemsDeleted,
            _World->invokeDownloadedPageGet(),
            &RaptorDownloadedPage::onItemsDeleted);

    // 清空下载记录
    connect(_World->invokeDownloadedPageGet(),
            &RaptorDownloadedPage::itemsClearing,
            _DownloadedSuite,
            &RaptorDownloadedSuite::onItemsClearing);

    connect(_DownloadedSuite,
            &RaptorDownloadedSuite::itemsCleared,
            _World->invokeDownloadedPageGet(),
            &RaptorDownloadedPage::onItemsCleared);

    connect(_UploadingSuite,
            &RaptorUploadingSuite::itemsLoaded,
            _World->invokeUploadingPageGet(),
            &RaptorUploadingPage::onItemsLoaded);

    connect(_UploadingSuite,
            &RaptorUploadingSuite::itemsQueuing,
            _World->invokeUploadingPageGet(),
            &RaptorUploadingPage::onItemsQueuing);

    connect(_UploadingSuite,
            &RaptorUploadingSuite::itemsStatusChanged,
            _World->invokeUploadingPageGet(),
            &RaptorUploadingPage::onItemsStatusChanged);

    connect(_World->invokeUploadUiGet(),
            &RaptorUpload::itemsUploading,
            _UploadingSuite,
            &RaptorUploadingSuite::onItemsUploading);

    // 暂停上传
    connect(_World->invokeUploadingPageGet(),
            &RaptorUploadingPage::itemsPausing,
            _UploadingSuite,
            &RaptorUploadingSuite::onItemsPausing);

    // 已暂停上传
    connect(_UploadingSuite,
            &RaptorUploadingSuite::itemPaused,
            _World->invokeUploadingPageGet(),
            &RaptorUploadingPage::onItemPaused);

    // 继续上传
    connect(_World->invokeUploadingPageGet(),
            &RaptorUploadingPage::itemsResuming,
            _UploadingSuite,
            &RaptorUploadingSuite::onItemsResuming);

    // 取消上传
    connect(_World->invokeUploadingPageGet(),
            &RaptorUploadingPage::itemCancelling,
            _UploadingSuite,
            &RaptorUploadingSuite::onItemCancelling);

    connect(_UploadedSuite,
            &RaptorUploadedSuite::itemsLoaded,
            _World->invokeUploadedPageGet(),
            &RaptorUploadedPage::onItemsLoaded);

    // 清空上传记录
    connect(_World->invokeUploadedPageGet(),
            &RaptorUploadedPage::itemsClearing,
            _UploadedSuite,
            &RaptorUploadedSuite::onItemsClearing);

    connect(_UploadedSuite,
            &RaptorUploadedSuite::itemsCleared,
            _World->invokeDownloadedPageGet(),
            &RaptorDownloadedPage::onItemsCleared);

    // 删除上传记录
    connect(_World->invokeUploadedPageGet(),
            &RaptorUploadedPage::itemsDeleting,
            _UploadedSuite,
            &RaptorUploadedSuite::onItemsDeleting);

    connect(_UploadedSuite,
            &RaptorUploadedSuite::itemsDeleted,
            _World->invokeDownloadedPageGet(),
            &RaptorDownloadedPage::onItemsDeleted);

    // 上传完成
    connect(_UploadingSuite,
            &RaptorUploadingSuite::itemCompleted,
            _World->invokeUploadingPageGet(),
            &RaptorUploadingPage::onItemCompleted);

    // 上传完成
    connect(_UploadingSuite,
            &RaptorUploadingSuite::itemCompleted,
            _World->invokeUploadedPageGet(),
            &RaptorUploadedPage::onItemCompleted);

    // 上传完成
    connect(_UploadingSuite,
            &RaptorUploadingSuite::itemCompleted,
            _UploadedSuite,
            &RaptorUploadedSuite::onItemCompleted);

    // 上传失败
    connect(_UploadingSuite,
            &RaptorUploadingSuite::itemErrored,
            _World->invokeUploadingPageGet(),
            &RaptorUploadingPage::onItemErrored);

    connect(_World->invokeSharePageGet(),
            &RaptorSharePage::itemsFetching,
            _ShareSuite,
            &RaptorShareSuite::onItemsFetching);

    connect(_World->invokeSharePageGet(),
            &RaptorSharePage::itemsRapidFetching,
            _ShareSuite,
            &RaptorShareSuite::onItemsRapidFetching);

    connect(_ShareSuite,
            &RaptorShareSuite::itemsFetched,
            _World->invokeSharePageGet(),
            &RaptorSharePage::onItemsFetched);

    connect(_World->invokeSharePageGet(),
            &RaptorSharePage::itemsExporting,
            _ShareSuite,
            &RaptorShareSuite::onItemsExporting);

    connect(_ShareSuite,
            &RaptorShareSuite::itemsExported,
            _World->invokeSharePageGet(),
            &RaptorSharePage::onItemsExported);

    connect(_World->invokeSharePageGet(),
            &RaptorSharePage::itemsCancelling,
            _ShareSuite,
            &RaptorShareSuite::onItemsCancelling);

    connect(_ShareSuite,
            &RaptorShareSuite::itemsCancelled,
            _World->invokeSharePageGet(),
            &RaptorSharePage::onItemsCancelled);

    connect(_World->invokeShareUiGet(),
            &RaptorShare::itemCreating,
            _ShareSuite,
            &RaptorShareSuite::onItemCreating);

    connect(_ShareSuite,
            &RaptorShareSuite::itemCreated,
            _World->invokeShareUiGet(),
            &RaptorShare::onItemCreated);

    connect(_World->invokeStarPageGet(),
            &RaptorStarPage::itemsFetching,
            _StarSuite,
            &RaptorStarSuite::onItemsFetching);

    connect(_StarSuite,
            &RaptorStarSuite::itemsFetched,
            _World->invokeStarPageGet(),
            &RaptorStarPage::onItemsFetched);

    connect(_World->invokeStarPageGet(),
            &RaptorStarPage::itemsByConditionFetching,
            _StarSuite,
            &RaptorStarSuite::onItemsByConditionFetching);

    connect(_World->invokeStarPageGet(),
            &RaptorStarPage::itemsUnStarring,
            _StarSuite,
            &RaptorStarSuite::onItemsStarring);

    connect(_StarSuite, &RaptorStarSuite::itemsStarred,
            _World->invokeStarPageGet(),
            &RaptorStarPage::onItemsUnStarred);

    connect(_World->invokeTrashPageGet(),
            &RaptorTrashPage::itemsFetching,
            _TrashSuite,
            &RaptorTrashSuite::onItemsFetching);

    connect(_TrashSuite,
            &RaptorTrashSuite::itemsFetched,
            _World->invokeTrashPageGet(),
            &RaptorTrashPage::onItemsFetched);

    connect(_World->invokeTrashPageGet(),
            &RaptorTrashPage::itemsRecovering,
            _TrashSuite,
            &RaptorTrashSuite::onItemsRecovering);

    connect(_TrashSuite,
            &RaptorTrashSuite::itemsRecovered,
            _World->invokeTrashPageGet(),
            &RaptorTrashPage::onItemsRecovered);

    connect(_World->invokeTrashPageGet(),
            &RaptorTrashPage::itemsDeleting,
            _TrashSuite,
            &RaptorTrashSuite::onItemsDeleting);

    connect(_TrashSuite,
            &RaptorTrashSuite::itemsDeleted,
            _World->invokeTrashPageGet(),
            &RaptorTrashPage::onItemsDeleted);

    connect(_World->invokeTrashPageGet(),
            &RaptorTrashPage::itemsClearing,
            _TrashSuite,
            &RaptorTrashSuite::onItemsClearing);

    connect(_TrashSuite,
            &RaptorTrashSuite::itemsCleared,
            _World->invokeTrashPageGet(),
            &RaptorTrashPage::onItemsCleared);

    connect(_World->invokeMediaPageGet(),
            &RaptorMediaPage::itemsFetching,
            _MediaSuite,
            &RaptorMediaSuite::onItemsFetching);
    connect(_MediaSuite,
            &RaptorMediaSuite::itemsFetched,
            _World->invokeMediaPageGet(),
            &RaptorMediaPage::onItemsFetched);

    // 创建文件夹
    connect(_World->invokeFolderUiGet(),
            &RaptorFolder::itemCreating,
            _FileSuite,
            &RaptorFileSuite::onItemCreating);

    // 解析和导入分享
    connect(_World->invokeImportUiGet(),
            &RaptorImport::itemParsing,
            _ShareSuite,
            &RaptorShareSuite::onItemParsing);

    // 导入快传
    connect(_World->invokeImportUiGet(),
            &RaptorImport::itemRapidImporting,
            _ShareSuite,
            &RaptorShareSuite::onItemRapidImporting);

    // 链接已解析
    connect(_ShareSuite,
            &RaptorShareSuite::itemParsed,
            _World->invokeImportUiGet(),
            &RaptorImport::onItemParsed);

    // 分享视图的子项
    connect(_World->invokeImportUiGet(),
            &RaptorImport::itemsByParentIdFetching,
            _ShareSuite,
            &RaptorShareSuite::onItemsByParentIdFetching);

    // 执行导入分享
    connect(_World->invokeImportUiGet(),
            &RaptorImport::itemImporting,
            _ShareSuite,
            &RaptorShareSuite::onItemImporting);

    connect(_ShareSuite,
            &RaptorShareSuite::itemsImported,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemsImported);

    connect(_World,
            &RaptorWorld::itemSpaceChanging,
            _World->invokeSpacePageGet(),
            &RaptorSpacePage::onItemSpaceChanging);

    connect(_World,
            &RaptorWorld::itemSpaceChanging,
            _World->invokeSharePageGet(),
            &RaptorSharePage::onItemSpaceChanging);

    connect(_World,
            &RaptorWorld::itemSpaceChanging,
            _World->invokeStarPageGet(),
            &RaptorStarPage::onItemSpaceChanging);

    connect(_World,
            &RaptorWorld::itemSpaceChanging,
            _World->invokeTrashPageGet(),
            &RaptorTrashPage::onItemSpaceChanging);

    connect(_World,
            &RaptorWorld::itemSpaceChanging,
            _World->invokeCopyPageGet(),
            &RaptorCopyPage::onItemSpaceChanging);

    connect(_World->invokeCopyPageGet(),
            &RaptorCopyPage::itemsLoading,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemsLoading);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemsLoaded,
            _World->invokeCopyUserUiGet(),
            &RaptorCopyUser::onItemsLoaded);

    connect(_World->invokeCopyPageGet(),
            &RaptorCopyPage::itemsAccessTokenRefreshing,
            _AuthenticationSuite,
            &RaptorAuthenticationSuite::onItemsAccessTokenRefreshing);

    connect(_AuthenticationSuite,
            &RaptorAuthenticationSuite::itemsAccessTokenRefreshed,
            _World->invokeCopyPageGet(),
            &RaptorCopyPage::onItemsAccessTokenRefreshed);

    connect(_World->invokeCopyPageGet(),
            &RaptorCopyPage::itemsByParentIdFetching,
            _CopySuite,
            &RaptorCopySuite::onItemsByParentIdFetching);

    connect(_World->invokeCopyPageGet(),
            &RaptorCopyPage::itemsByIdFetching,
            _CopySuite,
            &RaptorCopySuite::onItemsByIdFetching);

    connect(_World->invokeCopyPageGet(),
            &RaptorCopyPage::itemsByConditionFetching,
            _CopySuite,
            &RaptorCopySuite::onItemsByConditionFetching);

    connect(_CopySuite,
            &RaptorCopySuite::itemsFetched,
            _World->invokeCopyPageGet(),
            &RaptorCopyPage::onItemsFetched);

    connect(_World->invokeCopyPageGet(),
            &RaptorCopyPage::itemsCopying,
            _CopySuite,
            &RaptorCopySuite::onItemsCopying);

    connect(_CopySuite,
            &RaptorCopySuite::itemCopied,
            _World->invokeCopyPageGet(),
            &RaptorCopyPage::onItemCopied);

    connect(_World->invokeCopyPageGet(),
            &RaptorCopyPage::itemsCancelling,
            _CopySuite,
            &RaptorCopySuite::onItemsCancelling);

    connect(_CopySuite,
            &RaptorCopySuite::itemsCancelled,
            _World->invokeCopyPageGet(),
            &RaptorCopyPage::onItemsCancelled);

    connect(_CopySuite,
            &RaptorCopySuite::itemErrored,
            _World->invokeCopyPageGet(),
            &RaptorCopyPage::onItemErrored);

    connect(_World->invokeCleanPageGet(),
            &RaptorCleanPage::itemsFetching,
            _CleanSuite,
            &RaptorCleanSuite::onItemsFetching);

    connect(_CleanSuite,
            &RaptorCleanSuite::itemsFetched,
            _World->invokeCleanPageGet(),
            &RaptorCleanPage::onItemsFetched);

    connect(_World->invokeCleanPageGet(),
            &RaptorCleanPage::itemsCleaning,
            _CleanSuite,
            &RaptorCleanSuite::onItemsCleaning);

    connect(_CleanSuite,
            &RaptorCleanSuite::itemsCleaned,
            _World->invokeCleanPageGet(),
            &RaptorCleanPage::onItemsCleaned);

    connect(_World->invokeDownloadPageGet(),
            &RaptorDownloadPage::itemAriaConnectTesting,
            _CopilotSuite,
            &RaptorCopilotSuite::onItemAriaConnectTesting);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemAriaConnectTested,
            _World->invokeDownloadPageGet(),
            &RaptorDownloadPage::onItemAriaConnectTested);

    connect(_World->invokeNetworkPageGet(),
            &RaptorNetworkPage::itemProxyConnectTesting,
            _CopilotSuite,
            &RaptorCopilotSuite::onItemProxyConnectTesting);

    connect(_CopilotSuite,
            &RaptorCopilotSuite::itemProxyConnectTested,
            _World->invokeNetworkPageGet(),
            &RaptorNetworkPage::onItemProxyConnectTested);
}

void RaptorEngine::invokeFontRegister()
{
    const auto items = RaptorUtil::invokeFontLoad();
    for (auto &item: items)
    {
        if (const auto qId = QFontDatabase::addApplicationFont(item);
            qId == -1)
        {
            qWarning() << QStringLiteral("字体注册失败: %1").arg(item);
        }
    }

    qInfo() << QStringLiteral("全局字体已注册。");
}

void RaptorEngine::invokeMessageCallback(QtMsgType qType,
                                         const QMessageLogContext &qContext,
                                         const QString &qMessage)
{
    RaptorLogSuite::invokeSingletonGet()->onItemCallback(qType, qContext, qMessage);
}
