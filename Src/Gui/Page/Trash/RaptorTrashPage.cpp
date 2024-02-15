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

#include "RaptorTrashPage.h"
#include "ui_RaptorTrashPage.h"

RaptorTrashPage::RaptorTrashPage(QWidget* qParent) : QWidget(qParent),
                                                     _Ui(new Ui::RaptorTrashPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorTrashPage::~RaptorTrashPage()
{
    FREE(_Ui)
}

bool RaptorTrashPage::eventFilter(QObject* qObject, QEvent* qEvent)
{
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

void RaptorTrashPage::invokeNavigate()
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
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(RaptorInput()));
}

void RaptorTrashPage::invokeInstanceInit()
{
    _ItemViewDelegate = new RaptorTableViewDelegate(this);
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemViewModel = new RaptorTrashViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("大小") << QStringLiteral("放入时间");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(4);
    _Loading = new RaptorLoading(_Ui->_ItemView);
}

void RaptorTrashPage::invokeUiInit()
{
    _Ui->_Title->setText(QStringLiteral("回收站"));
    _Ui->_Restore->setText(QStringLiteral("还原"));
    _Ui->_Delete->setText(QStringLiteral("删除"));
    _Ui->_Clear->setText(QStringLiteral("清空"));
    _Ui->_TabAll->setText(QStringLiteral("所有"));
    _Ui->_Refresh->setText(QStringLiteral("刷新"));
    _Ui->_TabPrev->setIcon(QIcon(RaptorUtil::invokeIconMatch("Left", false, true)));
    _Ui->_TabPrev->setIconSize(QSize(14, 14));
    _Ui->_TabAll->setChecked(true);
    _Ui->_TabNext->setIcon(QIcon(RaptorUtil::invokeIconMatch("Right", false, true)));
    _Ui->_TabNext->setIconSize(QSize(14, 14));
    _Ui->_Splitter->setStretchFactor(0, 9);
    _Ui->_Splitter->setStretchFactor(1, 1);
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
    _Ui->_ItemTrashedTip->setText(QStringLiteral("放入时间:"));
    _Ui->_ItemSizeTip->setText(QStringLiteral("大小:"));
}

void RaptorTrashPage::invokeSlotInit() const
{
    connect(_Loading,
            &RaptorLoading::stateChanged,
            this,
            &RaptorTrashPage::onLoadingStateChanged);

    connect(_Ui->_Restore,
            &QPushButton::clicked,
            this,
            &RaptorTrashPage::onRestoreClicked);

    connect(_Ui->_Delete,
            &QPushButton::clicked,
            this,
            &RaptorTrashPage::onDeleteClicked);

    connect(_Ui->_Clear,
            &QPushButton::clicked,
            this,
            &RaptorTrashPage::onClearClicked);

    connect(_Ui->_Refresh,
            &QPushButton::clicked,
            this,
            &RaptorTrashPage::onRefreshClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::indicatorClicked,
            this,
            &RaptorTrashPage::onItemViewIndicatorClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::clicked,
            this,
            &RaptorTrashPage::onItemViewClicked);

    connect(_Ui->_ItemView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &RaptorTrashPage::onItemViewSelectionChanged);

    connect(_Ui->_ItemView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorTrashPage::onItemViewVerticalScrollValueChanged);
}

void RaptorTrashPage::onItemCopyWriterHaveFound(const QVariant& qVariant) const
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

void RaptorTrashPage::onItemAccessTokenRefreshed(const QVariant& qVariant)
{
    Q_UNUSED(qVariant)
    _Payload._Marker.clear();
}

void RaptorTrashPage::onItemLogouting(const QVariant& qVariant) const
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
    _Ui->_ItemTrashed->clear();
    _Ui->_ItemSize->clear();
}

void RaptorTrashPage::onItemSpaceChanging()
{
    _Payload._Marker.clear();
}

void RaptorTrashPage::onItemSwitching(const QVariant& qVariant) const
{
    Q_UNUSED(qVariant)
    _ItemViewModel->invokeItemsClear();
    _Ui->_ItemName->clear();
    _Ui->_ItemTrashed->clear();
    _Ui->_ItemSize->clear();
}

void RaptorTrashPage::onItemsFetched(const QVariant& qVariant)
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

    const auto [items, qMarker] = _Data.value<QPair<QVector<RaptorTrashItem>, QString>>();
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

void RaptorTrashPage::onItemsRestored(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    auto qIndexList = _Data.value<QModelIndexList>();
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
        _Ui->_ItemTrashed->clear();
        _Ui->_ItemSize->clear();
    }

    RaptorToast::invokeInformationEject(QStringLiteral("所选文件已被还原。"));
}

void RaptorTrashPage::onItemsDeleted(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    auto qIndexList = _Data.value<QModelIndexList>();
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
        _Ui->_ItemTrashed->clear();
        _Ui->_ItemSize->clear();
    }

    RaptorToast::invokeInformationEject(QStringLiteral("所选文件已被永久删除。"));
}

void RaptorTrashPage::onItemsCleared(const QVariant& qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>(); !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    _ItemViewModel->invokeItemsClear();
    if (_ItemViewModel->rowCount() == 0)
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::NotFound);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("404 资源不存在"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("生活总归带点荒谬"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("找点乐子吧"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
        _Ui->_ItemName->clear();
        _Ui->_ItemTrashed->clear();
        _Ui->_ItemSize->clear();
    }

    RaptorToast::invokeInformationEject(QStringLiteral("回收站已清空。"));
}

void RaptorTrashPage::onLoadingStateChanged(const RaptorLoading::State& state) const
{
    _Ui->_Refresh->setEnabled(state == RaptorLoading::State::Finished);
    _Ui->_ItemName->clear();
    _Ui->_ItemTrashed->clear();
    _Ui->_ItemSize->clear();
}

void RaptorTrashPage::onRestoreClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        !qIndexList.isEmpty())
    {
        auto input = RaptorInput();
        input._Indexes = qIndexList;
        Q_EMIT itemsRestoring(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorTrashPage::onDeleteClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_ItemViewModel->rowCount() == 0)
    {
        return;
    }

    if (!RaptorMessageBox::invokeCriticalEject(QStringLiteral("永久删除"),
                                               QStringLiteral("即将永久删除所选文件。是否继续?")))
    {
        return;
    }

    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        !qIndexList.isEmpty())
    {
        auto input = RaptorInput();
        input._Indexes = qIndexList;
        Q_EMIT itemsDeleting(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorTrashPage::onClearClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_ItemViewModel->rowCount() == 0)
    {
        return;
    }

    if (RaptorMessageBox::invokeCriticalEject(QStringLiteral("清空回收站"),
                                              QStringLiteral("即将永久删除所有文件。是否继续?")))
    {
        Q_EMIT itemsClearing();
    }
}

void RaptorTrashPage::onRefreshClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    _Loading->invokeStateSet(RaptorLoading::State::Loading);
    _ItemViewModel->invokeItemsClear();
    _Payload._Marker.clear();
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(RaptorInput()));
}

void RaptorTrashPage::onItemViewIndicatorClicked(const RaptorTableView::Code& qCode) const
{
    if (qCode == RaptorTableView::Forbidden)
    {
        QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                                  "invokeLoginEject",
                                  Qt::AutoConnection);
    }
}

void RaptorTrashPage::onItemViewClicked(const QModelIndex& qIndex) const
{
    const auto item = qIndex.data(Qt::UserRole).value<RaptorTrashItem>();
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemTrashed->setText(item._Trashed);
    _Ui->_ItemSize->setText(item._Size);
}

void RaptorTrashPage::onItemViewSelectionChanged(const QItemSelection& qSelected,
                                                 const QItemSelection& qDeselected) const
{
    Q_UNUSED(qDeselected)
    if (qSelected.length() == 1)
    {
        const auto item = qSelected.indexes()[0].data(Qt::UserRole).value<RaptorTrashItem>();
        _Ui->_ItemName->setText(item._Name);
        _Ui->_ItemTrashed->setText(item._Trashed);
        _Ui->_ItemSize->setText(item._Size);
        return;
    }

    _Ui->_ItemName->clear();
    _Ui->_ItemTrashed->clear();
    _Ui->_ItemSize->clear();
}

void RaptorTrashPage::onItemViewVerticalScrollValueChanged(const int& qValue) const
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
        _Loading->invokeStateSet(RaptorLoading::State::Loading);
        auto input = RaptorInput();
        input._Marker = qMarker;
        Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
    }
}
