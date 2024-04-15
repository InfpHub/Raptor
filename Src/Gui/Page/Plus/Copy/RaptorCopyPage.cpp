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

RaptorCopyPage::RaptorCopyPage(QWidget *qParent) : QWidget(qParent),
                                                   _Ui(new Ui::RaptorCopyPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorCopyPage::~RaptorCopyPage()
{
    qFree(_Ui)
}

bool RaptorCopyPage::eventFilter(QObject *qObject, QEvent *qEvent)
{
    if (qObject == _Ui->_ItemSourceView)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent *>(qEvent);
                qKeyEvent->matches(QKeySequence::Refresh))
            {
                invokeRefresh();
                return true;
            }
        }
    }

    if (qObject == _Ui->_ItemSourceView->viewport())
    {
        if (qEvent->type() == QEvent::MouseButtonPress)
        {
            onItemSourceViewClicked();
        }
    }

    if (qObject == _Ui->_ItemTargetView)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent *>(qEvent);
                qKeyEvent->matches(QKeySequence::Refresh))
            {
                invokeRefresh();
                return true;
            }
        }
    }

    if (qObject == _Ui->_ItemTargetView->viewport())
    {
        if (qEvent->type() == QEvent::MouseButtonPress)
        {
            onItemTargetViewClicked();
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

RaptorCopyUser *RaptorCopyPage::invokeCopyUserUiGet() const
{
    return _CopyUser;
}

void RaptorCopyPage::invokeRefresh()
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Payload._User.first.isEmpty() || _Payload._User.second.isEmpty())
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
        _ItemSourceViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _ItemSourceViewModel->invokeItemsClear();
        _Payload._Marker.first.clear();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
        input._Parent = _Payload._Parent.first;
    } else
    {
        _ItemTargetViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _ItemTargetViewModel->invokeItemsClear();
        _Payload._Marker.second.clear();
        input._Type = _Ui->_ItemToShowAll->isChecked() ? QString() : "folder";
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.second);
        input._Parent = _Payload._Parent.second;
    }

    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
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
        _ItemSourceViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _ItemSourceViewModel->invokeItemsClear();
        _Payload._Parent.first = "root";
        _Payload._Stack.first.clear();
        _Payload._Marker.first.clear();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
    } else
    {
        _ItemTargetViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _ItemTargetViewModel->invokeItemsClear();
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
    if (_ItemQueueViewModel->rowCount() != 0)
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

    const auto qIndexList = _Ui->_ItemSourceView->selectionModel()->selectedRows();
    if (qIndexList.empty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
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

    if (const auto qIndexLisu = _Ui->_ItemTargetView->selectionModel()->selectedRows();
        qIndexLisu.length() == 1)
    {
        if (const auto item = qIndexLisu[0].data(Qt::UserRole).value<RaptorFileItem>();
            item._Type == "folder")
        {
            qName = item._Name;
            input._Id = item._Id;
        }
    }

    if (const auto qOperate = RaptorMessageBox::invokeInformationEject(QStringLiteral("拷贝文件"),
                                                                       QStringLiteral("从 %1 拷贝 %2 个文件到 %3 的 %4, 是否继续?").arg(
                                                                           QString(qInformationTemplate).arg(_Payload._User.first._NickName),
                                                                           QString(qSuccessTemplate).arg(QString::number(input._Indexes.length())),
                                                                           QString(qInformationTemplate).arg(_Payload._User.second._NickName),
                                                                           QString(qCreativeTemplate).arg(qName)));
        qOperate == RaptorMessageBox::No)
    {
        return;
    }

    for (const auto &qIndex: qIndexList)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        _ItemQueueViewModel->invokeItemAppend(item);
    }

    _Ui->_ItemQueueView->viewport()->update();
    input._Variant = QVariant::fromValue<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> >(_Payload._User);
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

    if (_ItemQueueViewModel->rowCount() == 0)
    {
        RaptorToast::invokeInformationEject(QStringLiteral("没有正在进行的拷贝任务哦~"));
        return;
    }

    if (const auto qOperate = RaptorMessageBox::invokeWarningEject(QStringLiteral("取消拷贝文件"),
                                                                   QStringLiteral("即将取消拷贝文件，是否继续?"));
        qOperate == RaptorMessageBox::No)
    {
        return;
    }

    Q_EMIT itemsCancelling();
}

void RaptorCopyPage::invokeInstanceInit()
{
    _CopyUser = new RaptorCopyUser(RaptorStoreSuite::invokeWorldGet());
    _ItemSourceViewHeader = new RaptorTableViewHeader(Qt::Horizontal, this);
    _ItemSourceViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemTargetViewHeader = new RaptorTableViewHeader(Qt::Horizontal, this);
    _ItemTargetViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemQueueViewHeader = new RaptorTableViewHeader(Qt::Horizontal, this);
    _ItemQueueViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemSourceViewModel = new RaptorSpaceViewModel(_Ui->_ItemSourceView);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("大小");
    _ItemSourceViewModel->invokeHeaderSet(qHeader);
    _ItemSourceViewModel->invokeColumnCountSet(3);
    _ItemTargetViewModel = new RaptorSpaceViewModel(_Ui->_ItemTargetView);
    _ItemTargetViewModel->invokeHeaderSet(qHeader);
    _ItemTargetViewModel->invokeColumnCountSet(3);
    _ItemSourceViewDelegate = new RaptorTableViewDelegate(this);
    _ItemTargetViewDelegate = new RaptorTableViewDelegate(this);
    _ItemQueueViewDelegate = new RaptorTableViewDelegate(this);
    _ItemQueueViewModel = new RaptorSpaceViewModel(_Ui->_ItemQueueView);
    qHeader.pop_back();
    _ItemQueueViewModel->invokeHeaderSet(qHeader);
    _ItemQueueViewModel->invokeColumnCountSet(2);
    _ItemSourceViewLoading = new RaptorLoading(_Ui->_ItemSourceView);
    _ItemTargetViewLoading = new RaptorLoading(_Ui->_ItemTargetView);
}

void RaptorCopyPage::invokeUiInit()
{
    _Ui->_Splitter->setStretchFactor(0, 9);
    _Ui->_Splitter->setStretchFactor(1, 1);
    _Ui->_ItemSourceView->installEventFilter(this);
    _Ui->_ItemSourceView->viewport()->installEventFilter(this);
    _Ui->_ItemSourceView->setModel(_ItemSourceViewModel);
    _Ui->_ItemSourceView->setHorizontalHeader(_ItemSourceViewHeader);
    _Ui->_ItemSourceView->setItemDelegate(_ItemSourceViewDelegate);
    _Ui->_ItemSourceView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemSourceView->horizontalHeader()->setSortIndicatorShown(true);
    _Ui->_ItemSourceView->horizontalHeader()->setSectionsClickable(true);
    _Ui->_ItemSourceView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemSourceView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ItemSourceView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_ItemSourceView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemSourceView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemSourceView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemSourceView->verticalHeader()->setHidden(true);
    _Ui->_ItemSourceView->setShowGrid(false);
    _Ui->_ItemSourceView->setColumnWidth(0, 30);
    _Ui->_ItemSourceView->setColumnWidth(2, 110);
    _Ui->_ItemSourceView->setColumnWidth(3, 110);
    _Ui->_ItemSourceView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _Ui->_ItemSourceView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemSourceView->setSortingEnabled(true);
    _Ui->_ItemTargetView->installEventFilter(this);
    _Ui->_ItemTargetView->viewport()->installEventFilter(this);
    _Ui->_ItemTargetView->setModel(_ItemTargetViewModel);
    _Ui->_ItemTargetView->setHorizontalHeader(_ItemTargetViewHeader);
    _Ui->_ItemTargetView->setItemDelegate(_ItemTargetViewDelegate);
    _Ui->_ItemTargetView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemTargetView->horizontalHeader()->setSortIndicatorShown(true);
    _Ui->_ItemTargetView->horizontalHeader()->setSectionsClickable(true);
    _Ui->_ItemTargetView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemTargetView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ItemTargetView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_ItemTargetView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemTargetView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemTargetView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemTargetView->verticalHeader()->setHidden(true);
    _Ui->_ItemTargetView->setShowGrid(false);
    _Ui->_ItemTargetView->setColumnWidth(0, 30);
    _Ui->_ItemTargetView->setColumnWidth(2, 110);
    _Ui->_ItemTargetView->setColumnWidth(3, 110);
    _Ui->_ItemTargetView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _Ui->_ItemTargetView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemTargetView->setSortingEnabled(true);
    _Ui->_ItemUserTip->setText(QStringLiteral("用户:"));
    _Ui->_ItemUser->setText(QStringLiteral("选择"));
    _Ui->_ItemViewTip->setText(QStringLiteral("日志:"));
    _Ui->_ItemLog->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemViewPanel->setCurrentWidget(_Ui->_ItemLogPanel);
    _Ui->_ItemClearLog->setText(QStringLiteral("清空日志"));
    _Ui->_ItemShowLog->setText(QStringLiteral("显示队列视图"));
    _Ui->_ItemShowLog->setChecked(true);
    _Ui->_ItemToShowAll->setText(QStringLiteral("显示所有(仅目标盘视图)"));
    _Ui->_ItemQueueView->setModel(_ItemQueueViewModel);
    _Ui->_ItemQueueView->setHorizontalHeader(_ItemQueueViewHeader);
    _Ui->_ItemQueueView->setItemDelegate(_ItemQueueViewDelegate);
    _Ui->_ItemQueueView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemQueueView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemQueueView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ItemQueueView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_ItemQueueView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemQueueView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemQueueView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemQueueView->verticalHeader()->setHidden(true);
    _Ui->_ItemQueueView->setShowGrid(false);
    _Ui->_ItemQueueView->setColumnWidth(0, 30);
    _Ui->_ItemQueueView->setSelectionMode(QAbstractItemView::NoSelection);
    _Ui->_ItemBack->setText(QStringLiteral("后退"));
    _Ui->_ItemBack->setEnabled(false);
    _Ui->_ItemRoot->setText(QStringLiteral("根目录"));
    _Ui->_ItemRoot->setEnabled(false);
}

void RaptorCopyPage::invokeSlotInit() const
{
    connect(_Ui->_ItemSourceView,
            &QTableView::doubleClicked,
            this,
            &RaptorCopyPage::onItemSourceViewDoubleClicked);

    connect(_Ui->_ItemTargetView,
            &QTableView::doubleClicked,
            this,
            &RaptorCopyPage::onItemTargetViewDoubleClicked);

    connect(_Ui->_ItemSourceView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorCopyPage::onItemSourceViewVerticalScrollValueChanged);

    connect(_Ui->_ItemTargetView->verticalScrollBar(),
            &QScrollBar::valueChanged,
            this,
            &RaptorCopyPage::onItemTargetViewVerticalScrollValueChanged);

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
}

bool RaptorCopyPage::invokeSourceViewIsActiveConfirm() const
{
    return _Ui->_ItemSourceView->property("active").toBool();
}

bool RaptorCopyPage::invokeTargetViewIsActiveConfirm() const
{
    return _Ui->_ItemTargetView->property("active").toBool();
}

void RaptorCopyPage::onItemAccessTokenRefreshed(const QVariant &qVariant)
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

void RaptorCopyPage::onItemsAccessTokenRefreshed(const QVariant &qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    _Payload._User = _Data.value<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> >();
    _Ui->_ItemBack->setEnabled(true);
    _Ui->_ItemRoot->setEnabled(true);
}

void RaptorCopyPage::onItemLogouting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item != RaptorStoreSuite::invokeUserGet() && item != _Payload._User.first && item != _Payload._User.second)
    {
        return;
    }

    _ItemSourceViewModel->invokeItemsClear();
    _ItemTargetViewModel->invokeItemsClear();
    _ItemQueueViewModel->invokeItemsClear();
    _Ui->_ItemLog->clear();
    _Ui->_ItemBack->setEnabled(false);
    _Ui->_ItemRoot->setEnabled(false);
}

void RaptorCopyPage::onItemSpaceChanging()
{
    if (invokeSourceViewIsActiveConfirm())
    {
        _Payload._User.first._Space = RaptorStoreSuite::invokeSpaceGet() == Private ? _Payload._User.first._Private : _Payload._User.first._Public;
    } else if (invokeTargetViewIsActiveConfirm())
    {
        _Payload._User.second._Space = RaptorStoreSuite::invokeSpaceGet() == Private ? _Payload._User.second._Private : _Payload._User.second._Public;
    }
}

void RaptorCopyPage::onItemsFetched(const QVariant &qVariant)
{
    _ItemSourceViewLoading->invokeStateSet(RaptorLoading::State::Finished);
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        _Ui->_ItemSourceView->invokeServerCodeSet(RaptorTableView::Forbidden);
        _Ui->_ItemSourceView->invokeTitleSet(QStringLiteral("403 尚未登录"));
        _Ui->_ItemSourceView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
        _Ui->_ItemSourceView->invokeIndicatorSet(QStringLiteral("去登录"));
        _Ui->_ItemSourceView->invokeBackgroundPaintableSet(true);
        _Ui->_ItemSourceView->update();
        _Ui->_ItemTargetView->invokeServerCodeSet(RaptorTableView::Forbidden);
        _Ui->_ItemTargetView->invokeTitleSet(QStringLiteral("403 尚未登录"));
        _Ui->_ItemTargetView->invokeSummarySet(QStringLiteral("总有些门对你是关闭的"));
        _Ui->_ItemTargetView->invokeIndicatorSet(QStringLiteral("去登录"));
        _Ui->_ItemTargetView->invokeBackgroundPaintableSet(true);
        _Ui->_ItemTargetView->update();
        return;
    }

    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto [input, items] = _Data.value<QPair<RaptorInput, QVector<RaptorFileItem> > >();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item == _Payload._User.first)
    {
        _ItemSourceViewLoading->invokeStateSet(RaptorLoading::State::Finished);
        if (_Payload._Parent.first != input._Parent && _Payload._Keyword.isEmpty())
        {
            return;
        }

        _Payload._Marker.first = input._Marker;
        _ItemSourceViewModel->invokeItemsAppend(items);
    } else if (item == _Payload._User.second)
    {
        _ItemTargetViewLoading->invokeStateSet(RaptorLoading::State::Finished);
        if (_Payload._Parent.second != input._Parent && _Payload._Keyword.isEmpty())
        {
            return;
        }

        _Payload._Marker.second = input._Marker;
        _ItemTargetViewModel->invokeItemsAppend(items);
    }
}

void RaptorCopyPage::onItemCopied(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    const auto item = _Data.value<RaptorFileItem>();
    if (!_State)
    {
        _Ui->_ItemLog->append(QString(qCriticalTemplate).arg(QStringLiteral("拷贝失败: %1 -> %2").arg(_Message, item._Name)));
    } else
    {
        _Ui->_ItemLog->append(QString(qSuccessTemplate).arg(QStringLiteral("已拷贝: %1").arg(item._Name)));
    }

    for (auto i = 0; i < _ItemQueueViewModel->rowCount(); ++i)
    {
        const auto qIndex = _ItemQueueViewModel->index(i, 0);
        if (const auto iten = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
            item == iten)
        {
            _ItemQueueViewModel->removeRow(qIndex.row());
            break;
        }
    }

    _Ui->_ItemQueueView->viewport()->update();
    if (_ItemQueueViewModel->rowCount() == 0)
    {
        _Ui->_ItemUser->setEnabled(true);
    }
}

void RaptorCopyPage::onItemsCancelled(const QVariant &qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    _ItemQueueViewModel->invokeItemsClear();
    RaptorToast::invokeInformationEject(QStringLiteral("已取消拷贝。"));
}

void RaptorCopyPage::onItemErrored(const QVariant &qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        _State)
    {
        RaptorToast::invokeCriticalEject(_Message);
    }
}

void RaptorCopyPage::onSearchEditTextChanged(const QString &qText)
{
    _Payload._Keyword = qText;
}

void RaptorCopyPage::onItemSourceViewClicked() const
{
    _Ui->_ItemTargetView->setProperty("active", false);
    _Ui->_ItemSourceView->setProperty("active", true);
    _Ui->_ItemSourceView->style()->unpolish(_Ui->_ItemSourceView);
    _Ui->_ItemSourceView->style()->polish(_Ui->_ItemSourceView);
    _Ui->_ItemTargetView->style()->unpolish(_Ui->_ItemTargetView);
    _Ui->_ItemTargetView->style()->polish(_Ui->_ItemTargetView);
}

void RaptorCopyPage::onItemSourceViewDoubleClicked(const QModelIndex &qIndex)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        item._Type == "folder")
    {
        _ItemSourceViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _Payload._Parent.first = item._Id;
        _Payload._Stack.first.push(item);
        _Payload._Marker.first.clear();
        _ItemSourceViewModel->invokeItemsClear();
        auto input = RaptorInput();
        input._Parent = item._Id;
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
        Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorCopyPage::onItemTargetViewClicked() const
{
    _Ui->_ItemSourceView->setProperty("active", false);
    _Ui->_ItemTargetView->setProperty("active", true);
    _Ui->_ItemSourceView->style()->polish(_Ui->_ItemSourceView);
    _Ui->_ItemTargetView->style()->polish(_Ui->_ItemTargetView);
    _Ui->_ItemSourceView->update();
    _Ui->_ItemTargetView->update();
}

void RaptorCopyPage::onItemTargetViewDoubleClicked(const QModelIndex &qIndex)
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        item._Type == "folder")
    {
        _ItemTargetViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _Payload._Parent.second = item._Id;
        _Payload._Stack.second.push(item);
        _Payload._Marker.second.clear();
        _ItemTargetViewModel->invokeItemsClear();
        auto input = RaptorInput();
        input._Parent = item._Id;
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.second);
        Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
    }
}

void RaptorCopyPage::onItemSourceViewVerticalScrollValueChanged(const int &qValue) const
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

    if (qValue != _Ui->_ItemSourceView->verticalScrollBar()->maximum())
    {
        return;
    }

    _ItemSourceViewLoading->invokeStateSet(RaptorLoading::State::Loading);
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

void RaptorCopyPage::onItemTargetViewVerticalScrollValueChanged(const int &qValue) const
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

    if (qValue != _Ui->_ItemTargetView->verticalScrollBar()->maximum())
    {
        return;
    }

    _ItemTargetViewLoading->invokeStateSet(RaptorLoading::State::Loading);
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
    const auto qUser = _CopyUser->invokeEject();
    if (_Payload._User.first.isEmpty() || _Payload._User.second.isEmpty())
    {
        return;
    }

    if (qUser.first == _Payload._User.first && qUser.second == _Payload._User.second)
    {
        return;
    }

    _Payload._User = _CopyUser->invokeEject();
    Q_EMIT itemsAccessTokenRefreshing(QVariant::fromValue<QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> >(_Payload._User));
}

void RaptorCopyPage::onItemClearLogClicked() const
{
    _Ui->_ItemLog->clear();
}

void RaptorCopyPage::onItemShowLogClicked(const bool &qChecked) const
{
    if (qChecked)
    {
        _Ui->_ItemViewTip->setText(QStringLiteral("日志:"));
        _Ui->_ItemShowLog->setText(QStringLiteral("显示队列视图"));
        _Ui->_ItemViewPanel->setCurrentWidget(_Ui->_ItemLogPanel);
    } else
    {
        _Ui->_ItemViewTip->setText(QStringLiteral("队列:"));
        _Ui->_ItemShowLog->setText(QStringLiteral("显示日志视图"));
        _Ui->_ItemViewPanel->setCurrentWidget(_Ui->_ItemQueuePanel);
    }
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
            _ItemSourceViewLoading->invokeStateSet(RaptorLoading::State::Loading);
            const auto item = qStack.pop();
            _ItemSourceViewModel->invokeItemsClear();
            _Payload._Parent.first = item._Parent;
            _Payload._Marker.first.clear();
            input._Id = item._Id;
            input._Parent = item._Parent;
            input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
        }
    } else
    {
        if (auto qStack = _Payload._Stack.second;
            !qStack.isEmpty())
        {
            _ItemTargetViewLoading->invokeStateSet(RaptorLoading::State::Loading);
            const auto item = qStack.pop();
            _ItemTargetViewModel->invokeItemsClear();
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

        _ItemSourceViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _ItemSourceViewModel->invokeItemsClear();
        _Payload._Parent.first = "root";
        _Payload._Stack.first.clear();
        _Payload._Marker.first.clear();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.first);
        input._Parent = _Payload._Parent.first;
    } else
    {
        if (_Payload._Parent.second == "root")
        {
            return;
        }

        _ItemTargetViewLoading->invokeStateSet(RaptorLoading::State::Loading);
        _ItemTargetViewModel->invokeItemsClear();
        _Payload._Parent.second = "root";
        _Payload._Stack.second.clear();
        _Payload._Marker.second.clear();
        input._Variant = QVariant::fromValue<RaptorAuthenticationItem>(_Payload._User.second);
        input._Parent = _Payload._Parent.second;
        input._Type = _Ui->_ItemToShowAll->isChecked() ? QString() : "folder";
    }

    Q_EMIT itemsByParentIdFetching(QVariant::fromValue<RaptorInput>(input));
}
