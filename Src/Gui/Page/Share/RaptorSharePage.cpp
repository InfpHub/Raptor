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

#include "RaptorSharePage.h"
#include "ui_RaptorSharePage.h"

RaptorSharePage::RaptorSharePage(QWidget* qParent) : QWidget(qParent),
                                                     _Ui(new Ui::RaptorSharePage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorSharePage::~RaptorSharePage()
{
    FREE(_Ui)
}

bool RaptorSharePage::eventFilter(QObject* qObject, QEvent* qEvent)
{
    if (qObject == _Ui->_SearchEdit)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->key() == Qt::Key_Enter || qKeyEvent->key() == Qt::Key_Return)
            {
                onSearchClicked();
                return true;
            }
        }
    }

    if (qObject == _Ui->_ItemView)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->matches(QKeySequence::Refresh))
            {
                onRefreshClicked();
                return true;
            }
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

void RaptorSharePage::invokeNavigate()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::Forbidden);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("403 尚未登录"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("去登录"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _Ui->_ItemView->invokeBackgroundPaintableSet(false);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    const auto [qType, qCatagory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCatagory;
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::invokeInstanceInit()
{
    _TabGroup = new QButtonGroup(this);
    _TabGroup->setExclusive(true);
    _TabGroup->addButton(_Ui->_TabAll);
    _TabGroup->addButton(_Ui->_TabSharing);
    _TabGroup->addButton(_Ui->_TabCancelled);
    _TabGroup->addButton(_Ui->_TabExpired);
    _TabGroup->addButton(_Ui->_TabNeverExpire);
    _TabGroup->addButton(_Ui->_TabRapid);
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemViewModel = new RaptorShareViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("过期时间");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(3);
    _ItemViewDelegate = new RaptorTableViewDelegate(this);

    _Loading = new RaptorLoading(_Ui->_ItemView);
}

void RaptorSharePage::invokeUiInit()
{
    _Ui->_Title->setText(QStringLiteral("分享"));
    _Ui->_Export->setText(QStringLiteral("导出"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
    _Ui->_TabAll->setText(QStringLiteral("所有"));
    _Ui->_TabAll->setChecked(true);
    _Ui->_TabPrev->setIcon(QIcon(RaptorUtil::invokeIconMatch("Left", false, true)));
    _Ui->_TabPrev->setIconSize(QSize(14, 14));
    _Ui->_TabCancelled->setText(QStringLiteral("已取消"));
    _Ui->_TabSharing->setText(QStringLiteral("分享中"));
    _Ui->_TabExpired->setText(QStringLiteral("已过期"));
    _Ui->_TabNeverExpire->setText(QStringLiteral("永不过期"));
    _Ui->_TabRapid->setText(QStringLiteral("快传"));
    _Ui->_TabNext->setIcon(QIcon(RaptorUtil::invokeIconMatch("Right", false, true)));
    _Ui->_TabNext->setIconSize(QSize(14, 14));
    _Ui->_Splitter->setStretchFactor(0, 9);
    _Ui->_Splitter->setStretchFactor(1, 1);
    _Ui->_SearchEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_SearchEdit->installEventFilter(this);
    _Ui->_Search->setIcon(QIcon(RaptorUtil::invokeIconMatch("Search", false, true)));
    _Ui->_Search->setIconSize(QSize(16, 16));
    _Ui->_Refresh->setText(QStringLiteral("刷新"));
    _Ui->_ItemView->installEventFilter(this);
    _Ui->_ItemView->setModel(_ItemViewModel);
    _Ui->_ItemView->setHorizontalHeader(_ItemViewHeader);
    _Ui->_ItemView->setItemDelegate(_ItemViewDelegate);
    _Ui->_ItemView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ItemView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemView->verticalHeader()->setHidden(true);
    _Ui->_ItemView->setShowGrid(false);
    _Ui->_ItemView->setColumnWidth(0, 30);
    _Ui->_ItemView->setColumnWidth(2, 80);
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemNameTip->setText(QStringLiteral("名称:"));
    _Ui->_ItemName->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemCreatedTip->setText(QStringLiteral("创建时间:"));
    _Ui->_ItemExpiredTip->setText(QStringLiteral("过期时间:"));
    _Ui->_ItemPreviewTip->setText(QStringLiteral("预览:"));
    _Ui->_ItemSaveTip->setText(QStringLiteral("转存:"));
    _Ui->_ItemDownloadTip->setText(QStringLiteral("下载:"));
    _Ui->_ItemPasswordTip->setText(QStringLiteral("密码:"));
    _Ui->_ItemPasswordCopy->setText(QStringLiteral("复制"));
    _Ui->_ItemUrlTip->setText(QStringLiteral("链接:"));
    _Ui->_ItemUrl->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemCopy->setText(QStringLiteral("复制"));
}

void RaptorSharePage::invokeSlotInit() const
{
    connect(_Loading,
            &RaptorLoading::stateChanged,
            this,
            &RaptorSharePage::onLoadingStateChanged);

    connect(_Ui->_Export,
            &QPushButton::clicked,
            this,
            &RaptorSharePage::onExportClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorSharePage::onCancelClicked);

    connect(_Ui->_TabPrev,
            &QPushButton::clicked,
            this,
            &RaptorSharePage::onTabPrevClicked);

    connect(_Ui->_TabAll,
            &QPushButton::toggled,
            this,
            &RaptorSharePage::onTabAllToggled);

    connect(_Ui->_TabCancelled,
            &QPushButton::toggled,
            this,
            &RaptorSharePage::onTabCanceledToggled);

    connect(_Ui->_TabSharing,
            &QPushButton::toggled,
            this,
            &RaptorSharePage::onTabSharingToggled);

    connect(_Ui->_TabExpired,
            &QPushButton::toggled,
            this,
            &RaptorSharePage::onTabExpiredToggled);

    connect(_Ui->_TabNeverExpire,
            &QPushButton::toggled,
            this,
            &RaptorSharePage::onTabNeverExpireToggled);

    connect(_Ui->_TabRapid,
            &QPushButton::toggled,
            this,
            &RaptorSharePage::onTabRapidToggled);

    connect(_Ui->_TabNext,
            &QPushButton::clicked,
            this,
            &RaptorSharePage::onTabNextClicked);

    connect(_Ui->_Search,
            &QPushButton::clicked,
            this,
            &RaptorSharePage::onSearchClicked);

    connect(_Ui->_Refresh,
            &QPushButton::clicked,
            this,
            &RaptorSharePage::onRefreshClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::indicatorClicked,
            this,
            &RaptorSharePage::onItemViewIndicatorClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::clicked,
            this,
            &RaptorSharePage::onItemViewClicked);

    connect(_Ui->_ItemView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &RaptorSharePage::onItemViewSelectionChanged);

    connect(_Ui->_ItemView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorSharePage::onItemViewVerticalScrollValueChanged);

    connect(_Ui->_ItemPasswordCopy,
            &QPushButton::clicked,
            this,
            &RaptorSharePage::onItemPasswordCopyClicked);

    connect(_Ui->_ItemCopy,
            &QPushButton::clicked,
            this,
            &RaptorSharePage::onItemCopyClicked);
}

QPair<QString, QString> RaptorSharePage::invokeTypeWithCategoryFilter() const
{
    auto qType = QString();
    auto qExpired = QString();
    if (_Ui->_TabCancelled->isChecked())
    {
        qType = "disabled";
    }
    else if (_Ui->_TabSharing->isChecked())
    {
        qType = "enabled";
    }
    else
    {
        if (_Ui->_TabNeverExpire->isChecked())
        {
            qExpired = "1970-01-01T00:00:00";
        }
    }

    return qMakePair(qType, qExpired);
}

void RaptorSharePage::onItemCopyWriterHaveFound(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto items = _Data.value<QVector<RaptorCopyWriter>>();
    for (auto& [_Page, _Content] : items)
    {
        if (_Page == metaObject()->className())
        {
            _Ui->_Description->setText(_Content);
            break;
        }
    }
}

void RaptorSharePage::onItemAccessTokenRefreshed(const QVariant& qVariant)
{
    Q_UNUSED(qVariant)
    _Payload._Marker.clear();
}

void RaptorSharePage::onItemLogouting(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item != RaptorStoreSuite::invokeUserGet())
    {
        return;
    }

    _ItemViewModel->invokeItemsClear();
    _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::Forbidden);
    _Ui->_ItemView->invokeTitleSet(QStringLiteral("403 尚未登录"));
    _Ui->_ItemView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
    _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("去登录"));
    _Ui->_ItemView->invokeBackgroundPaintableSet(true);
    _Ui->_SearchEdit->clear();
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemExpired->clear();
    _Ui->_ItemPreview->clear();
    _Ui->_ItemSave->clear();
    _Ui->_ItemDownload->clear();
    _Ui->_ItemPassword->clear();
    _Ui->_ItemUrl->clear();
}

void RaptorSharePage::onItemSpaceChanging()
{
    _Payload._Marker.clear();
}

void RaptorSharePage::onItemSwitching(const QVariant& qVariant) const
{
    Q_UNUSED(qVariant)
    _ItemViewModel->invokeItemsClear();
    _Ui->_SearchEdit->clear();
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemExpired->clear();
    _Ui->_ItemPreview->clear();
    _Ui->_ItemSave->clear();
    _Ui->_ItemDownload->clear();
    _Ui->_ItemPassword->clear();
    _Ui->_ItemUrl->clear();
}

void RaptorSharePage::onItemsFetched(const QVariant& qVariant)
{
    _Loading->invokeStateSet(RaptorLoading::State::Finished);
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::Forbidden);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("403 尚未登录"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("去登录"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        return;
    }

    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::InternalError);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("500 服务器错误"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("服务器出错可能说明该雇更多程序员了"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("散财消灾"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto [items, qMarker] = _Data.value<QPair<QVector<RaptorShareItem>, QString>>();
    if (items.isEmpty())
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::NotFound);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("404 资源不存在"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("有些事 错过了就不再"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("找点乐子吧"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        return;
    }

    _Ui->_ItemView->invokeBackgroundPaintableSet(false);
    _Payload._Marker = qMarker;
    _ItemViewModel->invokeItemsAppend(items);
}

void RaptorSharePage::onItemsExported(const QVariant& qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto qShare = _Data.value<QString>();
    if (const auto qOperate = RaptorMessageBox::invokeInformationEject(QStringLiteral("导出分享"),
                                                 QStringLiteral("所选分享是否需要保存至文件?如果不，则将拷贝至剪切板。"));
                                                 qOperate == RaptorMessageBox::Yes)
    {
        if (const auto qName = QFileDialog::getSaveFileName(this,
                                                            QStringLiteral("保存分享"),
                                                            QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
                                                            "文本文件 (*.txt)");
            !qName.isEmpty())
        {
            auto qFile = QFile(qName);
            if (!qFile.open(QIODevice::WriteOnly))
            {
                RaptorToast::invokeCriticalEject(QStringLiteral("无法写入文件: %1").arg(QString(CRITICAL_TEMPLATE).arg(qName)));
                return;
            }

            qFile.write(qShare.toUtf8());
            qFile.close();
            RaptorToast::invokeSuccessEject(QStringLiteral("所选分享已保存至 %1").arg(QString(CREATIVE_TEMPLATE).arg(qName)));
            return;
        }
    }

    qApp->clipboard()->setText(qShare);
    RaptorToast::invokeSuccessEject(QStringLiteral("所选分享已拷贝到剪切板。"));
}

void RaptorSharePage::onItemsCancelled(const QVariant& qVariant)
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    RaptorToast::invokeInformationEject(QStringLiteral("所选分享已被取消。"));
    onRefreshClicked();
}

void RaptorSharePage::onLoadingStateChanged(const RaptorLoading::State& state) const
{
    _Ui->_TabPrev->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabAll->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabCancelled->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabSharing->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabExpired->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabNeverExpire->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabRapid->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabNext->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_Export->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_Cancel->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_Refresh->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemPasswordCopy->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemCopy->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemExpired->clear();
    _Ui->_ItemPreview->clear();
    _Ui->_ItemSave->clear();
    _Ui->_ItemDownload->clear();
    _Ui->_ItemPassword->clear();
    _Ui->_ItemUrl->clear();
}

void RaptorSharePage::onExportClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    input._Indexes = qIndexList;
    Q_EMIT itemsExporting(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::onCancelClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Ui->_TabRapid->isChecked())
    {
        RaptorToast::invokeInformationEject(QStringLiteral("快传无法被取消!"));
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    input._Indexes = qIndexList;
    Q_EMIT itemsCancelling(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::onTabPrevClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton*>();
    qPushButtonList.pop_front();
    qPushButtonList.pop_back();
    if (qPushButtonList.length() < 2)
    {
        return;
    }

    for (auto i = 0; i < qPushButtonList.length(); ++i)
    {
        if (qPushButtonList[i]->isChecked())
        {
            if (i == 0)
            {
                qPushButtonList[qPushButtonList.length() - 1]->setChecked(true);
            }
            else
            {
                qPushButtonList[i - 1]->setChecked(true);
            }

            break;
        }
    }
}

void RaptorSharePage::onTabAllToggled(const bool& qChecked)
{
    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(RaptorInput()));
}

void RaptorSharePage::onTabCanceledToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::onTabSharingToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::onTabExpiredToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::onTabNeverExpireToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::onTabRapidToggled(const bool& qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    Q_EMIT itemsRapidFetching();
}

void RaptorSharePage::onTabNextClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton*>();
    qPushButtonList.pop_front();
    qPushButtonList.pop_back();
    if (qPushButtonList.length() < 2)
    {
        return;
    }

    for (auto i = 0; i < qPushButtonList.length(); ++i)
    {
        if (qPushButtonList[i]->isChecked())
        {
            if (i == qPushButtonList.length() - 1)
            {
                qPushButtonList[0]->setChecked(true);
            }
            else
            {
                qPushButtonList[i + 1]->setChecked(true);
            }

            break;
        }
    }
}

void RaptorSharePage::onSearchClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    const auto qKeyword = _Ui->_SearchEdit->text();
    auto input = RaptorInput();
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    if (!qKeyword.isEmpty())
    {
        input._Name = qKeyword;
    }

    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::onRefreshClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::onItemViewIndicatorClicked(const RaptorTableView::Code& qCode) const
{
    if (qCode == RaptorTableView::Forbidden)
    {
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeLoginEject",
                                  Qt::AutoConnection);
    }
}

void RaptorSharePage::onItemViewClicked(const QModelIndex& qIndex) const
{
    if (!qIndex.isValid())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorShareItem>();
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemCreated->setText(item._Created);
    _Ui->_ItemExpired->setText(item._Expired);
    _Ui->_ItemPreview->setText(QString::number(item._Preview));
    _Ui->_ItemSave->setText(QString::number(item._Save));
    _Ui->_ItemDownload->setText(QString::number(item._Download));
    _Ui->_ItemPassword->setText(item._Password);
    _Ui->_ItemUrl->setText(item._Url);
}

void RaptorSharePage::onItemViewSelectionChanged(const QItemSelection& qSelected,
                                                 const QItemSelection& qDeselected) const
{
    Q_UNUSED(qDeselected)
    if (qSelected.length() == 1)
    {
        const auto item = qSelected.indexes()[0].data(Qt::UserRole).value<RaptorShareItem>();
        _Ui->_ItemName->setText(item._Name);
        _Ui->_ItemCreated->setText(item._Created);
        _Ui->_ItemExpired->setText(item._Expired);
        _Ui->_ItemPreview->setText(QString::number(item._Preview));
        _Ui->_ItemSave->setText(QString::number(item._Save));
        _Ui->_ItemDownload->setText(QString::number(item._Download));
        _Ui->_ItemPassword->setText(item._Password);
        _Ui->_ItemUrl->setText(item._Url);
        return;
    }

    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemExpired->clear();
    _Ui->_ItemPreview->clear();
    _Ui->_ItemSave->clear();
    _Ui->_ItemDownload->clear();
    _Ui->_ItemPassword->clear();
    _Ui->_ItemUrl->clear();
}

void RaptorSharePage::onItemViewVerticalScrollValueChanged(const int& qValue) const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto qMarker = _Payload._Marker;
    if (qMarker.isEmpty())
    {
        return;
    }

    if (qValue != _Ui->_ItemView->verticalScrollBar()->maximum())
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    auto input = RaptorInput();
    if (const auto qKeyword = _Ui->_SearchEdit->text();
        !qKeyword.isEmpty())
    {
        input._Name = qKeyword;
    }

    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    input._Marker = qMarker;
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorSharePage::onItemPasswordCopyClicked() const
{
    if (_Ui->_ItemPassword->text().isEmpty())
    {
        return;
    }

    qApp->clipboard()->setText(_Ui->_ItemPassword->text());
    RaptorToast::invokeSuccessEject(QStringLiteral("提取码已拷贝至剪切板!"));
}

void RaptorSharePage::onItemCopyClicked() const
{
    if (_Ui->_ItemPassword->text().isEmpty())
    {
        qApp->clipboard()->setText(QStringLiteral("链接: %1").arg(_Ui->_ItemUrl->toPlainText()));
        RaptorToast::invokeSuccessEject(QStringLiteral("链接已拷贝至剪切板!"));
        return;
    }

    qApp->clipboard()->setText(QStringLiteral("链接: %1 提取码: %2").arg(_Ui->_ItemUrl->toPlainText(),
                                                                    _Ui->_ItemPassword->text()));
    RaptorToast::invokeSuccessEject(QStringLiteral("链接及提取码已拷贝至剪切板!"));
}