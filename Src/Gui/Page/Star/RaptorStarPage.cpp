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

#include "RaptorStarPage.h"
#include "ui_RaptorStarPage.h"

RaptorStarPage::RaptorStarPage(QWidget* qParent) : QWidget(qParent),
                                                   _Ui(new Ui::RaptorStarPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorStarPage::~RaptorStarPage()
{
    FREE(_Ui)
}

bool RaptorStarPage::eventFilter(QObject* qObject, QEvent* qEvent)
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
                qKeyEvent->key() == Qt::Key_Delete)
            {
                onUnStarredClicked();
                return true;
            }
            else if (qKeyEvent->matches(QKeySequence::Refresh))
            {
                onRefreshClicked();
                return true;
            }
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

void RaptorStarPage::invokeNavigate()
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

void RaptorStarPage::invokeInstanceInit()
{
    _TabGroup = new QButtonGroup(this);
    _TabGroup->setExclusive(true);
    _TabGroup->addButton(_Ui->_TabAll);
    _TabGroup->addButton(_Ui->_TabFolder);
    _TabGroup->addButton(_Ui->_TabVideo);
    _TabGroup->addButton(_Ui->_TabAudio);
    _TabGroup->addButton(_Ui->_TabImage);
    _TabGroup->addButton(_Ui->_TabDocument);

    _ItemViewModel = new RaptorStarViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("大小") << QStringLiteral("最后修改");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(4);
    _ItemViewDelegate = new RaptorTableViewDelegate(this);
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));

    _Loading = new RaptorLoading(_Ui->_ItemView);
}

void RaptorStarPage::invokeUiInit()
{
    _Ui->_Title->setText(QStringLiteral("收藏"));
    _Ui->_UnStarred->setText(QStringLiteral("取消收藏"));
    _Ui->_TabPrev->setIcon(QIcon(RaptorUtil::invokeIconMatch("Left", false, true)));
    _Ui->_TabPrev->setIconSize(QSize(14, 14));
    _Ui->_TabAll->setText(QStringLiteral("所有"));
    _Ui->_TabAll->setChecked(true);
    _Ui->_TabFolder->setText(QStringLiteral("文件夹"));
    _Ui->_TabVideo->setText(QStringLiteral("视频"));
    _Ui->_TabAudio->setText(QStringLiteral("音频"));
    _Ui->_TabImage->setText(QStringLiteral("图像"));
    _Ui->_TabDocument->setText(QStringLiteral("文档"));
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
    _Ui->_ItemView->horizontalHeader()->setMinimumSectionSize(26);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemView->verticalHeader()->setHidden(true);
    _Ui->_ItemView->setShowGrid(false);
    _Ui->_ItemView->setColumnWidth(0, 30);
    _Ui->_ItemView->setColumnWidth(2, 110);
    _Ui->_ItemView->setColumnWidth(3, 80);
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemNameTip->setText(QStringLiteral("名称:"));
    _Ui->_ItemName->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemUpdatedTip->setText(QStringLiteral("最后修改:"));
    _Ui->_ItemSizeTip->setText(QStringLiteral("大小:"));
}

void RaptorStarPage::invokeSlotInit() const
{
    connect(_Loading,
            &RaptorLoading::stateChanged,
            this,
            &RaptorStarPage::onLoadingStateChanged);

    connect(_Ui->_UnStarred,
            &QPushButton::clicked,
            this,
            &RaptorStarPage::onUnStarredClicked);

    connect(_Ui->_TabPrev,
            &QPushButton::clicked,
            this,
            &RaptorStarPage::onTabPrevClicked);

    connect(_Ui->_TabAll,
            &QPushButton::toggled,
            this,
            &RaptorStarPage::onTabAllToggled);

    connect(_Ui->_TabFolder,
            &QPushButton::toggled,
            this,
            &RaptorStarPage::onTabFolderToggled);

    connect(_Ui->_TabAudio,
            &QPushButton::toggled,
            this,
            &RaptorStarPage::onTabAudioToggled);

    connect(_Ui->_TabVideo,
            &QPushButton::toggled,
            this,
            &RaptorStarPage::onTabVideoToggled);

    connect(_Ui->_TabImage,
            &QPushButton::toggled,
            this,
            &RaptorStarPage::onTabImageToggled);

    connect(_Ui->_TabDocument,
            &QPushButton::toggled,
            this,
            &RaptorStarPage::onTabDocumentToggled);

    connect(_Ui->_TabNext,
            &QPushButton::clicked,
            this,
            &RaptorStarPage::onTabNextClicked);

    connect(_Ui->_Search,
            &QPushButton::clicked,
            this,
            &RaptorStarPage::onSearchClicked);

    connect(_Ui->_Refresh,
            &QPushButton::clicked,
            this,
            &RaptorStarPage::onRefreshClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::indicatorClicked,
            this,
            &RaptorStarPage::onItemViewIndicatorClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::clicked,
            this,
            &RaptorStarPage::onItemViewClicked);

    connect(_Ui->_ItemView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &RaptorStarPage::onItemViewSelectionChanged);

    connect(_Ui->_ItemView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorStarPage::onItemViewVerticalScrollValueChanged);
}

QPair<QString, QString> RaptorStarPage::invokeTypeWithCategoryFilter() const
{
    auto qType = QString();
    auto qCategory = QString();
    if (_Ui->_TabFolder->isChecked())
    {
        qType = "folder";
    }
    else
    {
        if (_Ui->_TabAudio->isChecked())
        {
            qCategory = "audio";
        }
        else if (_Ui->_TabVideo->isChecked())
        {
            qCategory = "video";
        }
        else if (_Ui->_TabImage->isChecked())
        {
            qCategory = "image";
        }
        else if (_Ui->_TabDocument->isChecked())
        {
            qCategory = "doc";
        }
    }

    return qMakePair(qType, qCategory);
}

void RaptorStarPage::onItemCopyWriterHaveFound(const QVariant& qVariant) const
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

void RaptorStarPage::onItemAccessTokenRefreshed(const QVariant& qVariant)
{
    Q_UNUSED(qVariant)
    _Payload._Marker.clear();
}

void RaptorStarPage::onItemLogouting(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto item = input._Variant.value<RaptorAuthenticationItem>();
    if (item != RaptorStoreSuite::invokeUserGet())
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
    _Ui->_ItemUpdated->clear();
    _Ui->_ItemSize->clear();
}

void RaptorStarPage::onItemSpaceChanging()
{
    _Payload._Marker.clear();
}

void RaptorStarPage::onItemSwitching(const QVariant& qVariant) const
{
    Q_UNUSED(qVariant)
    _ItemViewModel->invokeItemsClear();
    _Ui->_SearchEdit->clear();
    _Ui->_ItemName->clear();
    _Ui->_ItemUpdated->clear();
    _Ui->_ItemSize->clear();
}

void RaptorStarPage::onItemsFetched(const QVariant& qVariant)
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

    const auto [items, qMarker] = _Data.value<QPair<QVector<RaptorStarItem>, QString>>();
    if (items.isEmpty())
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::NotFound);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("404 资源不存在"));
        _Ui->_ItemView->invokeSummarySet("一切尽在不言中");
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("找点乐子吧"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        return;
    }

    _Ui->_ItemView->invokeBackgroundPaintableSet(false);
    _Payload._Marker = qMarker;
    _ItemViewModel->invokeItemsAppend(items);
}

void RaptorStarPage::onItemsUnStarred(const QVariant& qVariant) const
{
    auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    if (auto [qIndexList, qState] = _Data.value<QPair<QModelIndexList, bool>>();
        !qState)
    {
        std::reverse(qIndexList.begin(), qIndexList.end());
        for (auto &qIndex: qIndexList)
        {
            _ItemViewModel->removeRow(qIndex.row());
        }

        _Ui->_ItemView->selectionModel()->clearSelection();
        _Ui->_ItemView->viewport()->update();
        RaptorToast::invokeInformationEject(QStringLiteral("The selected item has been unstarred!"));
    }

    if (_ItemViewModel->rowCount() == 0)
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::NotFound);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("404 资源不存在"));
        _Ui->_ItemView->invokeSummarySet("一切尽在不言中");
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("找点乐子吧"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
    }
}

void RaptorStarPage::onLoadingStateChanged(const RaptorLoading::State& state) const
{
    _Ui->_Refresh->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabPrev->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabAll->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabAudio->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabVideo->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabImage->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabDocument->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_TabNext->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemName->clear();
    _Ui->_ItemUpdated->clear();
    _Ui->_ItemSize->clear();
}

void RaptorStarPage::onUnStarredClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        return;
    }

    if (!RaptorMessageBox::invokeWarningEject(QStringLiteral("取消收藏"),
                                              QStringLiteral("即将取消收藏所选文件，是否继续?")))
    {
        return;
    }

    auto input = RaptorInput();
    input._State = false;
    input._Indexes = qIndexList;
    Q_EMIT itemsUnStarring(QVariant::fromValue<RaptorInput>(input));
}

void RaptorStarPage::onTabPrevClicked() const
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
            if (i == 1)
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

void RaptorStarPage::onTabAllToggled(const bool& qChecked)
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
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(RaptorInput()));
}

void RaptorStarPage::onTabFolderToggled(const bool& qChecked)
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

void RaptorStarPage::onTabAudioToggled(const bool& qChecked)
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

void RaptorStarPage::onTabVideoToggled(const bool& qChecked)
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

void RaptorStarPage::onTabImageToggled(const bool& qChecked)
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

void RaptorStarPage::onTabDocumentToggled(const bool& qChecked)
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

void RaptorStarPage::onTabNextClicked() const
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

void RaptorStarPage::onSearchClicked()
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
    if (qKeyword.isEmpty())
    {
        onRefreshClicked();
    }
    else
    {
        input._Name = qKeyword;
        const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
        input._Type = qType;
        input._Category = qCategory;
        Q_EMIT itemsByConditionFetching(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorStarPage::onRefreshClicked()
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

void RaptorStarPage::onItemViewIndicatorClicked(const RaptorTableView::Code& qCode) const
{
    if (qCode == RaptorTableView::Forbidden)
    {
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeLoginEject",
                                  Qt::AutoConnection);
    }
}

void RaptorStarPage::onItemViewClicked(const QModelIndex& qIndex) const
{
    const auto item = qIndex.data(Qt::UserRole).value<RaptorStarItem>();
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemUpdated->setText(item._Updated);
    _Ui->_ItemSize->setText(item._Size);
}

void RaptorStarPage::onItemViewSelectionChanged(const QItemSelection& qSelected,
                                                const QItemSelection& qDeselected) const
{
    Q_UNUSED(qDeselected)
    if (qSelected.length() == 1)
    {
        const auto item = qSelected.indexes()[0].data(Qt::UserRole).value<RaptorStarItem>();
        _Ui->_ItemName->setText(item._Name);
        _Ui->_ItemUpdated->setText(item._Updated);
        _Ui->_ItemSize->setText(item._Size);
        return;
    }

    _Ui->_ItemName->clear();
    _Ui->_ItemUpdated->clear();
    _Ui->_ItemSize->clear();
}

void RaptorStarPage::onItemViewVerticalScrollValueChanged(const int& qValue) const
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
        const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
        input._Type = qType;
        input._Category = qCategory;
        input._Marker = qMarker;
        input._Name = qKeyword;
        Q_EMIT itemsByConditionFetching(QVariant::fromValue<RaptorInput>(input));
        return;
    }

    const auto [qType, qCategory] = invokeTypeWithCategoryFilter();
    input._Type = qType;
    input._Category = qCategory;
    input._Marker = qMarker;
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}