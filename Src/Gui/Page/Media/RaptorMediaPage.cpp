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

#include "RaptorMediaPage.h"
#include "ui_RaptorMediaPage.h"

RaptorMediaPage::RaptorMediaPage(QWidget *qParent) : QWidget(qParent),
                                                     _Ui(new Ui::RaptorMediaPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorMediaPage::~RaptorMediaPage()
{
    qFree(_Ui)
}

bool RaptorMediaPage::eventFilter(QObject *qObject, QEvent *qEvent)
{
    if (qObject == _Ui->_ItemView)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent *>(qEvent);
                qKeyEvent->matches(QKeySequence::Refresh))
            {
                onRefreshClicked();
                return true;
            }
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

void RaptorMediaPage::invokeNavigate()
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

    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
    _Ui->_ItemView->invokeBackgroundPaintableSet(false);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Type = invokeTypeFilter();
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorMediaPage::invokeInstanceInit()
{
    _TabGroup = new QButtonGroup(this);
    _TabGroup->addButton(_Ui->_TabAll);
    _TabGroup->addButton(_Ui->_TabShow);
    _TabGroup->addButton(_Ui->_TabHide);
    _TabGroup->setExclusive(true);
    _ItemViewDelegate = new RaptorTableViewDelegate(this);
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemViewModel = new RaptorMediaViewModel(_Ui->_ItemView);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("大小") << QStringLiteral("放入时间");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(4);
    _ItemViewLoading = new RaptorLoading(_Ui->_ItemView);
}

void RaptorMediaPage::invokeUiInit()
{
    _Ui->_Title->setText(QStringLiteral("放映室"));
    _Ui->_TabAll->setText(QStringLiteral("所有"));
    _Ui->_TabAll->setChecked(true);
    _Ui->_TabShow->setText(QStringLiteral("未隐藏"));
    _Ui->_TabHide->setText(QStringLiteral("已隐藏"));
    _Ui->_Refresh->setText(QStringLiteral("刷新"));
    _Ui->_TabPrev->setIcon(QIcon(RaptorUtil::invokeIconMatch("Left", false, true)));
    _Ui->_TabPrev->setIconSize(QSize(14, 14));
    _Ui->_TabNext->setIcon(QIcon(RaptorUtil::invokeIconMatch("Right", false, true)));
    _Ui->_TabNext->setIconSize(QSize(14, 14));
    _Ui->_Splitter->setStretchFactor(0, 9);
    _Ui->_Splitter->setStretchFactor(1, 1);
    _Ui->_ItemView->installEventFilter(this);
    _Ui->_ItemView->setModel(_ItemViewModel);
    _Ui->_ItemView->setHorizontalHeader(_ItemViewHeader);
    _Ui->_ItemView->setItemDelegate(_ItemViewDelegate);
    _Ui->_ItemView->setContextMenuPolicy(Qt::CustomContextMenu);
    _Ui->_ItemView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ItemView->horizontalHeader()->setDefaultSectionSize(30);
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
    _Ui->_ItemCreatedTip->setText(QStringLiteral("创建时间:"));
    _Ui->_ItemSizeTip->setText(QStringLiteral("大小:"));
}

void RaptorMediaPage::invokeSlotInit() const
{
    connect(_ItemViewLoading,
            &RaptorLoading::stateChanged,
            this,
            &RaptorMediaPage::onItemViewLoadingStateChanged);

    connect(_Ui->_Refresh,
            &QPushButton::clicked,
            this,
            &RaptorMediaPage::onRefreshClicked);

    connect(_Ui->_TabPrev,
            &QPushButton::clicked,
            this,
            &RaptorMediaPage::onTabPrevClicked);

    connect(_Ui->_TabAll,
            &QPushButton::toggled,
            this,
            &RaptorMediaPage::onTabAllToggled);

    connect(_Ui->_TabShow,
            &QPushButton::toggled,
            this,
            &RaptorMediaPage::onTabShowToggled);

    connect(_Ui->_TabHide,
            &QPushButton::toggled,
            this,
            &RaptorMediaPage::onTabHideToggled);

    connect(_Ui->_TabNext,
            &QPushButton::clicked,
            this,
            &RaptorMediaPage::onTabNextClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::indicatorClicked,
            this,
            &RaptorMediaPage::onItemViewIndicatorClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::clicked,
            this,
            &RaptorMediaPage::onItemViewClicked);

    connect(_Ui->_ItemView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &RaptorMediaPage::onItemViewSelectionChanged);

    connect(_Ui->_ItemView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorMediaPage::onItemViewVerticalScrollValueChanged);
}

QString RaptorMediaPage::invokeTypeFilter() const
{
    auto qType = "ALL";
    if (_Ui->_TabShow->isChecked())
    {
        qType = "NO_HIDDEN";
    }

    if (_Ui->_TabHide->isChecked())
    {
        qType = "ONLY_HIDDEN";
    }

    return qType;
}

void RaptorMediaPage::onItemCopyWriterHaveFound(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto items = _Data.value<QVector<RaptorCopyWriter> >();
    for (const auto &[_Page, _Content]: items)
    {
        if (_Page == metaObject()->className())
        {
            _Ui->_Description->setText(_Content);
            break;
        }
    }
}

void RaptorMediaPage::onItemAccessTokenRefreshed(const QVariant &qVariant)
{
    Q_UNUSED(qVariant)
    _Payload._Marker.clear();
}

void RaptorMediaPage::onItemLogouting(const QVariant &qVariant) const
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
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
}

void RaptorMediaPage::onItemSwitching(const QVariant &qVariant) const
{
    Q_UNUSED(qVariant)
    _ItemViewModel->invokeItemsClear();
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
}

void RaptorMediaPage::onItemsFetched(const QVariant &qVariant)
{
    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Finished);
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

    const auto [items, qMarker] = _Data.value<QPair<QVector<RaptorMediaItem>, QString> >();
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

void RaptorMediaPage::onItemsHided(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    auto [qState, qIndexList] = _Data.value<QPair<bool, QModelIndexList> >();
    std::reverse(qIndexList.begin(), qIndexList.end());
    for (auto &qIndex: qIndexList)
    {
        _ItemViewModel->removeRow(qIndex.row());
    }

    _Ui->_ItemView->viewport()->update();
    if (_ItemViewModel->rowCount() == 0)
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::NotFound);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("404 资源不存在"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("生活总归带点荒谬"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("找点乐子吧"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        _Ui->_ItemName->clear();
        _Ui->_ItemCreated->clear();
        _Ui->_ItemSize->clear();
    }

    RaptorToast::invokeInformationEject(qState ? QStringLiteral("所选文件已被隐藏。") : QStringLiteral("所选文件已被显示。"));
}

void RaptorMediaPage::onItemViewLoadingStateChanged(const RaptorLoading::State &state) const
{
    _Ui->_Refresh->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
}


void RaptorMediaPage::onRefreshClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(RaptorInput()));
}

void RaptorMediaPage::onTabPrevClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton *>();
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
            } else
            {
                qPushButtonList[i - 1]->setChecked(true);
            }

            break;
        }
    }
}

void RaptorMediaPage::onTabAllToggled(const bool &qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Type = invokeTypeFilter();
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorMediaPage::onTabShowToggled(const bool &qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Type = invokeTypeFilter();
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorMediaPage::onTabHideToggled(const bool &qChecked)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!qChecked)
    {
        return;
    }

    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    auto input = RaptorInput();
    input._Type = invokeTypeFilter();
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorMediaPage::onTabNextClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton *>();
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
            } else
            {
                qPushButtonList[i + 1]->setChecked(true);
            }

            break;
        }
    }
}

void RaptorMediaPage::onItemViewIndicatorClicked(const RaptorTableView::Code &qCode) const
{
    if (qCode == RaptorTableView::Forbidden)
    {
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeLoginEject",
                                  Qt::AutoConnection);
    }
}

void RaptorMediaPage::onItemViewClicked(const QModelIndex &qIndex) const
{
    if (!qIndex.isValid())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorMediaItem>();
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemCreated->setText(item._Created);
    _Ui->_ItemSize->setText(item._Size);
}

void RaptorMediaPage::onItemViewSelectionChanged(const QItemSelection &qSelected,
                                                 const QItemSelection &qDeselected) const
{
    Q_UNUSED(qDeselected)
    if (qSelected.length() == 1)
    {
        const auto item = qSelected.indexes()[0].data(Qt::UserRole).value<RaptorMediaItem>();
        _Ui->_ItemName->setText(item._Name);
        _Ui->_ItemCreated->setText(item._Created);
        _Ui->_ItemSize->setText(item._Size);
        return;
    }

    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
}

void RaptorMediaPage::onItemViewVerticalScrollValueChanged(const int &qValue) const
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

    if (qValue == _Ui->_ItemView->verticalScrollBar()->maximum())
    {
        _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        auto input = RaptorInput();
        input._Marker = qMarker;
        Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
    }
}
