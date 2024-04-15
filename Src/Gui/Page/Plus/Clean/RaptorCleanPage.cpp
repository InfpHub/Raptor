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

#include "RaptorCleanPage.h"
#include "ui_RaptorCleanPage.h"

RaptorCleanPage::RaptorCleanPage(QWidget *qParent) : QWidget(qParent),
                                                     _Ui(new Ui::RaptorCleanPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorCleanPage::~RaptorCleanPage()
{
    qFree(_Ui)
}

bool RaptorCleanPage::eventFilter(QObject *qObject, QEvent *qEvent)
{
    if (qObject == _Ui->_ItemView)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent *>(qEvent);
                qKeyEvent->key() == Qt::Key_Delete)
            {
                invokeItemClean();
                return true;
            } else if (qKeyEvent->matches(QKeySequence::Refresh))
            {
                invokeNavigate();
                return true;
            }
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

void RaptorCleanPage::invokeNavigate()
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
    _Payload._Marker.clear();
    _ItemViewModel->invokeItemsClear();
    auto input = RaptorInput();
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorCleanPage::invokeItemClean() const
{
    if (const auto qOperate = RaptorMessageBox::invokeWarningEject(QStringLiteral("删除文件"),
                                                                   QStringLiteral("即将移动所选文件到回收站。是否继续?"));
        qOperate == RaptorMessageBox::No)
    {
        return;
    }

    auto input = RaptorInput();
    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.empty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    input._Indexes = qIndexList;
    Q_EMIT itemsCleaning(QVariant::fromValue<RaptorInput>(input));
}

void RaptorCleanPage::invokeInstanceInit()
{
    _ItemViewContextMenu = new RaptorMenu(_Ui->_ItemView);
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, this);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemViewModel = new RaptorCleanViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("所在位置") << QStringLiteral("大小");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(4);
    _ItemViewDelegate = new RaptorTableViewDelegate(this);
    _ItemViewLoading = new RaptorLoading(_Ui->_ItemView);
}

void RaptorCleanPage::invokeUiInit()
{
    _Ui->_Splitter->setStretchFactor(0, 9);
    _Ui->_Splitter->setStretchFactor(1, 1);
    _Ui->_ItemView->installEventFilter(this);
    _Ui->_ItemView->viewport()->installEventFilter(this);
    _Ui->_ItemView->setModel(_ItemViewModel);
    _Ui->_ItemView->setHorizontalHeader(_ItemViewHeader);
    _Ui->_ItemView->setItemDelegate(_ItemViewDelegate);
    _Ui->_ItemView->setContextMenuPolicy(Qt::CustomContextMenu);
    _Ui->_ItemView->horizontalHeader()->setSortIndicatorShown(true);
    _Ui->_ItemView->horizontalHeader()->setSectionsClickable(true);
    _Ui->_ItemView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ItemView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    _Ui->_ItemView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemView->verticalHeader()->setHidden(true);
    _Ui->_ItemView->setShowGrid(false);
    _Ui->_ItemView->setColumnWidth(0, 30);
    _Ui->_ItemView->setColumnWidth(2, 220);
    _Ui->_ItemView->setColumnWidth(3, 110);
    _Ui->_ItemView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemView->setSortingEnabled(true);
    _Ui->_ItemNameTip->setText(QStringLiteral("名称:"));
    _Ui->_ItemCreatedTip->setText(QStringLiteral("创建时间:"));
    _Ui->_ItemSizeTip->setText(QStringLiteral("大小:"));
    _Ui->_ItemUpdatedTip->setText(QStringLiteral("最后修改:"));
    _Ui->_ItemDirTip->setText(QStringLiteral("所在位置:"));
    _Ui->_ItemGoToDir->setText(QStringLiteral("转至所在位置"));
    _ItemViewContextMenu->invokeItemAdd("清理",
                                        RaptorUtil::invokeIconMatch("Clean", false, true),
                                        QKeySequence::Delete,
                                        std::bind(&RaptorCleanPage::invokeItemClean, this));
    _ItemViewContextMenu->invokeItemAdd("转至所在位置",
                                        RaptorUtil::invokeIconMatch("GoTo", false, true),
                                        QKeySequence(),
                                        std::bind(&RaptorCleanPage::onItemGoToDirClicked, this));
}

void RaptorCleanPage::invokeSlotInit() const
{
    connect(_Ui->_ItemView,
            &QTableView::clicked,
            this,
            &RaptorCleanPage::onItemViewClicked);

    connect(_Ui->_ItemView,
            &RaptorTableView::customContextMenuRequested,
            this,
            &RaptorCleanPage::onItemViewCustomContextMenuRequested);

    connect(_Ui->_ItemView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorCleanPage::onItemViewVerticalScrollValueChanged);

    connect(_Ui->_ItemGoToDir,
            &QPushButton::clicked,
            this,
            &RaptorCleanPage::onItemGoToDirClicked);
}

void RaptorCleanPage::onItemAccessTokenRefreshed(const QVariant &qVariant)
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        return;
    }

    _Payload._Marker.clear();
}

void RaptorCleanPage::onItemLogouting(const QVariant &qVariant) const
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
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
    _Ui->_ItemUpdated->clear();
    _Ui->_ItemDir->clear();
}

void RaptorCleanPage::onItemSpaceChanging()
{
    _Payload._Marker.clear();
}

void RaptorCleanPage::onItemsFetched(const QVariant &qVariant)
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

    const auto [items, qMarker] = _Data.value<QPair<QVector<RaptorFileItem>, QString> >();
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

void RaptorCleanPage::onItemsCleaned(const QVariant &qVariant) const
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

    _Ui->_ItemView->selectionModel()->clearSelection();
    _Ui->_ItemView->viewport()->update();
    if (_ItemViewModel->rowCount() == 0)
    {
        _Ui->_ItemView->invokeServerCodeSet(RaptorTableView::NotFound);
        _Ui->_ItemView->invokeTitleSet(QStringLiteral("404 资源不存在"));
        _Ui->_ItemView->invokeSummarySet(QStringLiteral("生活总归带点荒谬"));
        _Ui->_ItemView->invokeIndicatorSet(QStringLiteral("找点乐子吧"));
        _Ui->_ItemView->invokeBackgroundPaintableSet(true);
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("所选文件已被放入回收站。"));
}

void RaptorCleanPage::onItemViewClicked(const QModelIndex &qIndex) const
{
    if (!qIndex.isValid())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemCreated->setText(item._Created);
    _Ui->_ItemSize->setText(item._Size);
    _Ui->_ItemUpdated->setText(item._Updated);
    _Ui->_ItemDir->setText(item._Dir);
}

void RaptorCleanPage::onItemViewCustomContextMenuRequested(const QPoint &qPoint) const
{
    if (!_Ui->_ItemView->indexAt(qPoint).isValid())
    {
        return;
    }

    if (!RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                            Setting::Ui::ContextMenu).toBool())
    {
        return;
    }

    _ItemViewContextMenu->move(_Ui->_ItemView->mapToGlobal(QPoint(qPoint.x(),
                                                                  qPoint.y() + _Ui->_ItemView->verticalHeader()->
                                                                  defaultSectionSize())));
    _ItemViewContextMenu->invokeEject();
}

void RaptorCleanPage::onItemViewVerticalScrollValueChanged(const int &qValue) const
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

    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
    auto input = RaptorInput();
    input._Marker = qMarker;
    Q_EMIT itemsFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorCleanPage::onItemGoToDirClicked() const
{
    auto input = RaptorInput();
    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.empty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(),
                              "invokeGoToSpacePage");
    const auto item = qIndexList[qIndexList.length() - 1].data(Qt::UserRole).value<RaptorFileItem>();
    QMetaObject::invokeMethod(RaptorStoreSuite::invokeSpacePageGet(),
                              "invokeGoToDir",
                              Q_ARG(QString, item._Parent));
}
