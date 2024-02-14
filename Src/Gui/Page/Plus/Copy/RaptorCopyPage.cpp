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

#include "RaptorCopyPage.h"
#include "ui_RaptorCopyPage.h"

RaptorCopyPage::RaptorCopyPage(QWidget* qParent) : QWidget(qParent),
                                                   _Ui(new Ui::RaptorCopyPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorCopyPage::~RaptorCopyPage()
{
    FREE(_Ui)
}

bool RaptorCopyPage::eventFilter(QObject* qObject, QEvent* qEvent)
{
    if (qObject == _Ui->_SourceView)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->matches(QKeySequence::Refresh))
            {
                onItemRefreshClicked();
                return true;
            }
        }
    }

    if (qObject == _Ui->_SourceView->viewport())
    {
        if (qEvent->type() == QEvent::MouseButtonPress)
        {
            onSourceViewClicked();
        }
    }

    if (qObject == _Ui->_TargetView)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->matches(QKeySequence::Refresh))
            {
                onItemRefreshClicked();
                return true;
            }
        }
    }

    if (qObject == _Ui->_TargetView->viewport())
    {
        if (qEvent->type() == QEvent::MouseButtonPress)
        {
            onTargetViewClicked();
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

RaptorCopyUser* RaptorCopyPage::invokeCopyUserUiGet() const
{
    return _CopyUser;
}

void RaptorCopyPage::invokeItemSearch()
{
    if (_Payload._User.first.isEmpty() || _Payload._User.second.isEmpty())
    {
        return;
    }

    if (_Payload._Keyword.isEmpty())
    {
        onItemRootClicked();
        return;
    }

    if (!invokeSourceViewIsActiveConfirm() && !invokeTargetViewIsActiveConfirm())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何视图，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    if (invokeSourceViewIsActiveConfirm())
    {
        _SourceLoading->invokeStateSet(RaptorLoading::State::Loading);
        _SourceModel->invokeItemsClear();
        _Payload._Parent.first = "root";
        _Payload._Stack.first.clear();
        _Payload._Marker.first.clear();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
    }
    else
    {
        _TargetLoading->invokeStateSet(RaptorLoading::State::Loading);
        _TargetModel->invokeItemsClear();
        _Payload._Parent.second = "root";
        _Payload._Stack.second.clear();
        _Payload._Marker.second.clear();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.second);
    }

    input._Name = _Payload._Keyword;
    Q_EMIT itemsByConditionFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorCopyPage::invokeItemsCopy()
{
    if (_QueueModel->rowCount() != 0)
    {
        RaptorToast::invokeWarningEject(QStringLiteral("有拷贝任务正在进行!"));
        return;
    }

    if (_Payload._User.first.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择源用户，无法继续!"));
        return;
    }

    if (_Payload._User.second.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择目标用户，无法继续!"));
        return;
    }

    const auto qIndexList = _Ui->_SourceView->selectionModel()->selectedRows();
    if (qIndexList.empty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    for (auto& qIndex : qIndexList)
    {
        if (qIndex.data(Qt::UserRole).value<RaptorFileItem>()._Type == "folder")
        {
            RaptorToast::invokeWarningEject(QStringLiteral("此版本尚不支持支持拷贝文件夹!"));
            return;
        }
    }

    auto qName = QStringLiteral("根目录");
    auto input = RaptorInput();
    input._Id = "root";
    input._Indexes = qIndexList;
    if (!_Payload._Stack.second.isEmpty())
    {
        const auto item = _Payload._Stack.second.last();
        qName = item._Name;
        input._Id = item._Id;
    }

    if (const auto qIndexLisu = _Ui->_TargetView->selectionModel()->selectedRows();
        qIndexLisu.length() == 1)
    {
        if (const auto item = qIndexLisu[0].data(Qt::UserRole).value<RaptorFileItem>();
            item._Type == "folder")
        {
            qName = item._Name;
            input._Id = item._Id;
        }
    }

    if (!RaptorMessageBox::invokeInformationEject(QStringLiteral("拷贝文件"),
                                                  QStringLiteral("从 %1 拷贝 %2 个文件到 %3 的 %4, 是否继续?").arg(
                                                      QStringLiteral(INFORMATION_TEMPLATE).arg(_Payload._User.first._Nickname),
                                                      QStringLiteral(SUCCESS_TEMPLATE).arg(QString::number(input._Indexes.length())),
                                                      QStringLiteral(INFORMATION_TEMPLATE).arg(_Payload._User.second._Nickname),
                                                      QStringLiteral(CREATIVE_TEMPLATE).arg(qName))))
    {
        return;
    }

    for (auto& qIndex : qAsConst(qIndexList))
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        _QueueModel->invokeItemAppend(item);
    }

    _Ui->_ItemQueueView->viewport()->update();
    input._Variant = QVariant::fromValue<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem>>(_Payload._User);
    Q_EMIT itemsCopying(QVariant::fromValue<RaptorInput>(input));
    _Ui->_ItemUser->setEnabled(false);
    RaptorToast::invokeInformationEject(QStringLiteral("正在执行拷贝。稍后刷新即可!"));
}

void RaptorCopyPage::invokeItemCancel() const
{
    if (_Payload._User.first.isEmpty() || _Payload._User.second.isEmpty())
    {
        return;
    }

    if (_QueueModel->rowCount() == 0)
    {
        RaptorToast::invokeInformationEject(QStringLiteral("没有正在进行的拷贝任务哦~"));
        return;
    }

    if (RaptorMessageBox::invokeWarningEject(QStringLiteral("取消拷贝文件"),
                                             QStringLiteral("即将取消拷贝文件，是否继续?")))
    {
        Q_EMIT itemsCancelling();
    }
}

void RaptorCopyPage::invokeInstanceInit()
{
    _CopyUser = new RaptorCopyUser(RaptorStoreSuite::invokeWorldGet());
    _SourceHeader = new RaptorTableViewHeader(Qt::Horizontal, this);
    _SourceHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _TargetHeader = new RaptorTableViewHeader(Qt::Horizontal, this);
    _TargetHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _QueueHeader = new RaptorTableViewHeader(Qt::Horizontal, this);
    _QueueHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _SourceModel = new RaptorSpaceViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("大小");
    _SourceModel->invokeHeaderSet(qHeader);
    _SourceModel->invokeColumnCountSet(3);
    _TargetModel = new RaptorSpaceViewModel(this);
    _TargetModel->invokeHeaderSet(qHeader);
    _TargetModel->invokeColumnCountSet(3);
    _CommonViewDelegate = new RaptorTableViewDelegate(this);
    _QueueModel = new RaptorSpaceViewModel(this);
    qHeader.pop_back();
    _QueueModel->invokeHeaderSet(qHeader);
    _QueueModel->invokeColumnCountSet(2);
    _SourceLoading = new RaptorLoading(_Ui->_SourceView);
    _TargetLoading = new RaptorLoading(_Ui->_TargetView);
}

void RaptorCopyPage::invokeUiInit()
{
    _Ui->_Splitter->setStretchFactor(0, 9);
    _Ui->_Splitter->setStretchFactor(1, 1);
    _Ui->_SourceView->installEventFilter(this);
    _Ui->_SourceView->viewport()->installEventFilter(this);
    _Ui->_SourceView->setModel(_SourceModel);
    _Ui->_SourceView->setHorizontalHeader(_SourceHeader);
    _Ui->_SourceView->setItemDelegate(_CommonViewDelegate);
    _Ui->_SourceView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_SourceView->horizontalHeader()->setFixedHeight(26);
    _Ui->_SourceView->horizontalHeader()->setMinimumSectionSize(26);
    _Ui->_SourceView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_SourceView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_SourceView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_SourceView->verticalHeader()->setHidden(true);
    _Ui->_SourceView->setShowGrid(false);
    _Ui->_SourceView->setColumnWidth(0, 30);
    _Ui->_SourceView->setColumnWidth(2, 110);
    _Ui->_SourceView->setColumnWidth(3, 110);
    _Ui->_SourceView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_TargetView->installEventFilter(this);
    _Ui->_TargetView->viewport()->installEventFilter(this);
    _Ui->_TargetView->setModel(_TargetModel);
    _Ui->_TargetView->setHorizontalHeader(_TargetHeader);
    _Ui->_TargetView->setItemDelegate(_CommonViewDelegate);
    _Ui->_TargetView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_TargetView->horizontalHeader()->setFixedHeight(26);
    _Ui->_TargetView->horizontalHeader()->setMinimumSectionSize(26);
    _Ui->_TargetView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_TargetView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_TargetView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_TargetView->verticalHeader()->setHidden(true);
    _Ui->_TargetView->setShowGrid(false);
    _Ui->_TargetView->setColumnWidth(0, 30);
    _Ui->_TargetView->setColumnWidth(2, 110);
    _Ui->_TargetView->setColumnWidth(3, 110);
    _Ui->_TargetView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemUserTip->setText(QStringLiteral("用户:"));
    _Ui->_ItemUser->setText(QStringLiteral("选择"));
    _Ui->_ItemViewTip->setText(QStringLiteral("日志:"));
    _Ui->_ItemLog->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemViewPanel->setCurrentWidget(_Ui->_ItemLogPanel);
    _Ui->_ItemClearLog->setText(QStringLiteral("清空日志"));
    _Ui->_ItemShowLog->setText(QStringLiteral("显示队列视图"));
    _Ui->_ItemShowLog->setChecked(true);
    _Ui->_ItemToShowAll->setText(QStringLiteral("显示所有(仅目标盘视图)"));
    _Ui->_ItemQueueView->setModel(_QueueModel);
    _Ui->_ItemQueueView->setHorizontalHeader(_QueueHeader);
    _Ui->_ItemQueueView->setItemDelegate(_CommonViewDelegate);
    _Ui->_ItemQueueView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemQueueView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemQueueView->horizontalHeader()->setMinimumSectionSize(26);
    _Ui->_ItemQueueView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemQueueView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemQueueView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemQueueView->verticalHeader()->setHidden(true);
    _Ui->_ItemQueueView->setShowGrid(false);
    _Ui->_ItemQueueView->setColumnWidth(0, 30);
    _Ui->_ItemQueueView->setSelectionMode(QAbstractItemView::NoSelection);
    _Ui->_ItemRefresh->setText(QStringLiteral("刷新"));
    _Ui->_ItemRefresh->setEnabled(false);
    _Ui->_ItemBack->setText(QStringLiteral("后退"));
    _Ui->_ItemBack->setEnabled(false);
    _Ui->_ItemRoot->setText(QStringLiteral("根目录"));
    _Ui->_ItemRoot->setEnabled(false);
}

void RaptorCopyPage::invokeSlotInit() const
{
    connect(_Ui->_SourceView,
            &QTableView::doubleClicked,
            this,
            &RaptorCopyPage::onSourceViewDoubleClicked);

    connect(_Ui->_TargetView,
            &QTableView::doubleClicked,
            this,
            &RaptorCopyPage::onTargetViewDoubleClicked);

    connect(_Ui->_SourceView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorCopyPage::onSourceViewVerticalScrollValueChanged);

    connect(_Ui->_TargetView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorCopyPage::onTargetViewVerticalScrollValueChanged);

    connect(_Ui->_ItemUser,
            &QPushButton::clicked,
            this,
            &RaptorCopyPage::onItemUserClicked);

    connect(_Ui->_ItemClearLog,
            &QPushButton::clicked,
            this,
            &RaptorCopyPage::onItemClearLogClicked);

    connect(_Ui->_ItemShowLog,
            &QPushButton::clicked,
            this,
            &RaptorCopyPage::onItemShowLogClicked);

    connect(_Ui->_ItemRoot,
            &QPushButton::clicked,
            this,
            &RaptorCopyPage::onItemRootClicked);

    connect(_Ui->_ItemBack,
            &QPushButton::clicked,
            this,
            &RaptorCopyPage::onItemBackClicked);

    connect(_Ui->_ItemRefresh,
            &QPushButton::clicked,
            this,
            &RaptorCopyPage::onItemRefreshClicked);
}

bool RaptorCopyPage::invokeSourceViewIsActiveConfirm() const
{
    return _Ui->_SourceView->property("active").toBool();
}

bool RaptorCopyPage::invokeTargetViewIsActiveConfirm() const
{
    return _Ui->_TargetView->property("active").toBool();
}

void RaptorCopyPage::onItemAccessTokenRefreshed(const QVariant& qVariant)
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        return;
    }

    _Payload._Parent.first = "root";
    _Payload._Parent.second = "root";
    _Payload._Stack.first.clear();
    _Payload._Stack.second.clear();
    _Payload._Marker.first.clear();
    _Payload._Marker.second.clear();
    _Payload._User.first.clear();
    _Payload._User.second.clear();
}

void RaptorCopyPage::onItemsAccessTokenRefreshed(const QVariant& qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    _Payload._User = _Data.value<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem>>();
    _Ui->_ItemRefresh->setEnabled(true);
    _Ui->_ItemBack->setEnabled(true);
    _Ui->_ItemRoot->setEnabled(true);
}

void RaptorCopyPage::onItemLogouting(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item != RaptorStoreSuite::invokeUserGet() && item != _Payload._User.first && item != _Payload._User.second)
    {
        return;
    }

    _SourceModel->invokeItemsClear();
    _TargetModel->invokeItemsClear();
    _QueueModel->invokeItemsClear();
    _Ui->_ItemLog->clear();
    _Ui->_ItemRefresh->setEnabled(false);
    _Ui->_ItemBack->setEnabled(false);
    _Ui->_ItemRoot->setEnabled(false);
}

void RaptorCopyPage::onItemSpaceChanging()
{
    if (invokeSourceViewIsActiveConfirm())
    {
        _Payload._User.first._Space = RaptorStoreSuite::invokeSpaceGet() == Private ? _Payload._User.first._Private : _Payload._User.first._Public;
    }
    else if (invokeTargetViewIsActiveConfirm())
    {
        _Payload._User.second._Space = RaptorStoreSuite::invokeSpaceGet() == Private ? _Payload._User.second._Private : _Payload._User.second._Public;
    }
}

void RaptorCopyPage::onItemsFetched(const QVariant& qVariant)
{
    _SourceLoading->invokeStateSet(RaptorLoading::State::Finished);
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        _Ui->_SourceView->invokeServerCodeSet(RaptorTableView::Forbidden);
        _Ui->_SourceView->invokeTitleSet(QStringLiteral("403 尚未登录"));
        _Ui->_SourceView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
        _Ui->_SourceView->invokeIndicatorSet(QStringLiteral("去登录"));
        _Ui->_SourceView->invokeBackgroundPaintableSet(true);
        _Ui->_SourceView->update();
        _Ui->_TargetView->invokeServerCodeSet(RaptorTableView::Forbidden);
        _Ui->_TargetView->invokeTitleSet(QStringLiteral("403 尚未登录"));
        _Ui->_TargetView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
        _Ui->_TargetView->invokeIndicatorSet(QStringLiteral("去登录"));
        _Ui->_TargetView->invokeBackgroundPaintableSet(true);
        _Ui->_TargetView->update();
        return;
    }

    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto [input, items] = _Data.value<QPair<RaptorInput, QVector<RaptorFileItem>>>();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item == _Payload._User.first)
    {
        _SourceLoading->invokeStateSet(RaptorLoading::State::Finished);
        if (_Payload._Parent.first != input._Parent && _Payload._Keyword.isEmpty())
        {
            return;
        }

        _Payload._Marker.first = input._Marker;
        _SourceModel->invokeItemsAppend(items);
    }
    else if (item == _Payload._User.second)
    {
        _TargetLoading->invokeStateSet(RaptorLoading::State::Finished);
        if (_Payload._Parent.second != input._Parent && _Payload._Keyword.isEmpty())
        {
            return;
        }

        _Payload._Marker.second = input._Marker;
        _TargetModel->invokeItemsAppend(items);
    }
}

void RaptorCopyPage::onItemCopied(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    const auto item = _Data.value<RaptorFileItem>();
    if (!_State)
    {
        _Ui->_ItemLog->append(QStringLiteral(CRITICAL_TEMPLATE).arg(QStringLiteral("拷贝失败: %1 -> %2").arg(_Message, item._Name)));
    }
    else
    {
        _Ui->_ItemLog->append(QStringLiteral(SUCCESS_TEMPLATE).arg(QStringLiteral("已拷贝: %1").arg(item._Name)));
    }

    for (auto i = 0; i < _QueueModel->rowCount(); ++i)
    {
        const auto qIndex = _QueueModel->index(i, 0);
        if (const auto iten = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
            item == iten)
        {
            _QueueModel->removeRow(qIndex.row());
            break;
        }
    }

    _Ui->_ItemQueueView->viewport()->update();
    if (_QueueModel->rowCount() == 0)
    {
        _Ui->_ItemUser->setEnabled(true);
    }
}

void RaptorCopyPage::onItemsCancelled(const QVariant& qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    _QueueModel->invokeItemsClear();
    RaptorToast::invokeInformationEject(QStringLiteral("已取消拷贝。"));
}

void RaptorCopyPage::onSearchEditTextChanged(const QString& qText)
{
    _Payload._Keyword = qText;
}

void RaptorCopyPage::onSourceViewClicked() const
{
    _Ui->_TargetView->setProperty("active", false);
    _Ui->_SourceView->setProperty("active", true);
    _Ui->_SourceView->style()->unpolish(_Ui->_SourceView);
    _Ui->_SourceView->style()->polish(_Ui->_SourceView);
    _Ui->_TargetView->style()->unpolish(_Ui->_TargetView);
    _Ui->_TargetView->style()->polish(_Ui->_TargetView);
}

void RaptorCopyPage::onSourceViewDoubleClicked(const QModelIndex& qIndex)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        item._Type == "folder")
    {
        _SourceLoading->invokeStateSet(RaptorLoading::State::Loading);
        _Payload._Parent.first = item._Id;
        _Payload._Stack.first.push(item);
        _Payload._Marker.first.clear();
        _SourceModel->invokeItemsClear();
        auto input = RaptorInput();
        input._Parent = item._Id;
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
        Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorCopyPage::onTargetViewClicked() const
{
    _Ui->_SourceView->setProperty("active", false);
    _Ui->_TargetView->setProperty("active", true);
    _Ui->_SourceView->style()->polish(_Ui->_SourceView);
    _Ui->_TargetView->style()->polish(_Ui->_TargetView);
    _Ui->_SourceView->update();
    _Ui->_TargetView->update();
}

void RaptorCopyPage::onTargetViewDoubleClicked(const QModelIndex& qIndex)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        item._Type == "folder")
    {
        _TargetLoading->invokeStateSet(RaptorLoading::State::Loading);
        _Payload._Parent.second = item._Id;
        _Payload._Stack.second.push(item);
        _Payload._Marker.second.clear();
        _TargetModel->invokeItemsClear();
        auto input = RaptorInput();
        input._Parent = item._Id;
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.second);
        Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorCopyPage::onSourceViewVerticalScrollValueChanged(const int& qValue) const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!_Payload._User.first.isEmpty() || !_Payload._User.second.isEmpty())
    {
        return;
    }

    const auto qMarker = _Payload._Marker.first;
    if (qMarker.isEmpty())
    {
        return;
    }

    if (qValue != _Ui->_SourceView->verticalScrollBar()->maximum())
    {
        return;
    }

    _SourceLoading->invokeStateSet(RaptorLoading::State::Loading);
    auto input = RaptorInput();
    if (!_Payload._Keyword.isEmpty())
    {
        input._Marker = qMarker;
        input._Name = _Payload._Keyword;
        Q_EMIT itemsByConditionFetching(QVariant::fromValue<RaptorInput>(input));
        return;
    }

    input._Parent = _Payload._Parent.first;
    input._Marker = qMarker;
    input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorCopyPage::onTargetViewVerticalScrollValueChanged(const int& qValue) const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!_Payload._User.first.isEmpty() || !_Payload._User.second.isEmpty())
    {
        return;
    }

    const auto qMarker = _Payload._Marker.second;
    if (qMarker.isEmpty())
    {
        return;
    }

    if (qValue != _Ui->_TargetView->verticalScrollBar()->maximum())
    {
        return;
    }

    _TargetLoading->invokeStateSet(RaptorLoading::State::Loading);
    auto input = RaptorInput();
    if (!_Payload._Keyword.isEmpty())
    {
        input._Marker = qMarker;
        input._Name = _Payload._Keyword;
        Q_EMIT itemsByConditionFetching(QVariant::fromValue<RaptorInput>(input));
        return;
    }

    input._Parent = _Payload._Parent.second;
    input._Marker = qMarker;
    input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.second);
    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorCopyPage::onItemUserClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    Q_EMIT itemsLoading();
    _Payload._User = _CopyUser->invokeEject();
    if (_Payload._User.first.isEmpty() || _Payload._User.second.isEmpty())
    {
        return;
    }

    if (_Payload._User.first == _Payload._User.second)
    {
        return;
    }

    Q_EMIT itemsAccessTokenRefreshing(QVariant::fromValue<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem>>(_Payload._User));
}

void RaptorCopyPage::onItemClearLogClicked() const
{
    _Ui->_ItemLog->clear();
}

void RaptorCopyPage::onItemShowLogClicked(const bool& qChecked) const
{
    if (qChecked)
    {
        _Ui->_ItemViewTip->setText(QStringLiteral("日志:"));
        _Ui->_ItemShowLog->setText(QStringLiteral("显示队列视图"));
        _Ui->_ItemViewPanel->setCurrentWidget(_Ui->_ItemLogPanel);
    }
    else
    {
        _Ui->_ItemViewTip->setText(QStringLiteral("队列:"));
        _Ui->_ItemShowLog->setText(QStringLiteral("显示日志视图"));
        _Ui->_ItemViewPanel->setCurrentWidget(_Ui->_ItemQueuePanel);
    }
}

void RaptorCopyPage::onItemRefreshClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!invokeSourceViewIsActiveConfirm() && !invokeTargetViewIsActiveConfirm())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何视图，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    if (invokeSourceViewIsActiveConfirm())
    {
        _SourceLoading->invokeStateSet(RaptorLoading::State::Loading);
        _SourceModel->invokeItemsClear();
        _Payload._Marker.first.clear();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
        input._Parent = _Payload._Parent.first;
    }
    else
    {
        _TargetLoading->invokeStateSet(RaptorLoading::State::Loading);
        _TargetModel->invokeItemsClear();
        _Payload._Marker.second.clear();
        input._Type = _Ui->_ItemToShowAll->isChecked() ? QString() : "folder";
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.second);
        input._Parent = _Payload._Parent.second;
    }

    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorCopyPage::onItemBackClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!invokeSourceViewIsActiveConfirm() && !invokeTargetViewIsActiveConfirm())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何视图，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    if (invokeSourceViewIsActiveConfirm())
    {
        if (auto qStack = _Payload._Stack.first;
            !qStack.isEmpty())
        {
            _SourceLoading->invokeStateSet(RaptorLoading::State::Loading);
            const auto item = qStack.pop();
            _SourceModel->invokeItemsClear();
            _Payload._Parent.first = item._Parent;
            _Payload._Marker.first.clear();
            input._Id = item._Id;
            input._Parent = item._Parent;
            input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
        }
    }
    else
    {
        if (auto qStack = _Payload._Stack.second;
            !qStack.isEmpty())
        {
            _TargetLoading->invokeStateSet(RaptorLoading::State::Loading);
            const auto item = qStack.pop();
            _TargetModel->invokeItemsClear();
            _Payload._Parent.second = item._Parent;
            _Payload._Marker.second.clear();
            input._Id = item._Id;
            input._Parent = item._Parent;
            input._Type = _Ui->_ItemToShowAll->isChecked() ? QString() : "folder";
            input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.second);
        }
    }

    Q_EMIT itemsByIdFetching(QVariant::fromValue<RaptorInput>(input));
}

void RaptorCopyPage::onItemRootClicked()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (!invokeSourceViewIsActiveConfirm() && !invokeTargetViewIsActiveConfirm())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何视图，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    if (invokeSourceViewIsActiveConfirm())
    {
        if (_Payload._Parent.first == "root")
        {
            return;
        }

        _SourceLoading->invokeStateSet(RaptorLoading::State::Loading);
        _SourceModel->invokeItemsClear();
        _Payload._Parent.first = "root";
        _Payload._Stack.first.clear();
        _Payload._Marker.first.clear();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
        input._Parent = _Payload._Parent.first;
    }
    else
    {
        if (_Payload._Parent.second == "root")
        {
            return;
        }

        _TargetLoading->invokeStateSet(RaptorLoading::State::Loading);
        _TargetModel->invokeItemsClear();
        _Payload._Parent.second = "root";
        _Payload._Stack.second.clear();
        _Payload._Marker.second.clear();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.second);
        input._Parent = _Payload._Parent.second;
        input._Type = _Ui->_ItemToShowAll->isChecked() ? QString() : "folder";
    }

    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}
