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

#include "RaptorDownloadingPage.h"
#include "ui_RaptorDownloadingPage.h"

RaptorDownloadingPage::RaptorDownloadingPage(QWidget* qParent) : QWidget(qParent),
                                                                 _Ui(new Ui::RaptorDownloadingPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorDownloadingPage::~RaptorDownloadingPage()
{
    FREE(_Ui)
}

bool RaptorDownloadingPage::eventFilter(QObject* qObject, QEvent* qEvent)
{
    if (qObject == _Ui->_ItemView)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->key() == Qt::Key_Delete || (qKeyEvent->modifiers() == Qt::ControlModifier && qKeyEvent->key() == Qt::Key_D))
            {
                if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
                    !qIndexList.empty())
                {
                    invokeItemsCancel();
                    return true;
                }
            }
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

QModelIndexList RaptorDownloadingPage::invokeSelectItemsGet() const
{
    return _Ui->_ItemView->selectionModel()->selectedRows();
}

void RaptorDownloadingPage::invokeItemsPause() const
{
    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        return;
    }

    auto input = RaptorInput();
    input._Indexes = qIndexList;
    Q_EMIT itemsPausing(QVariant::fromValue<RaptorInput>(input));
}

void RaptorDownloadingPage::invokeItemsResume() const
{
    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        return;
    }

    for (auto& qIndex : qIndexList)
    {
        auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        item._Status = QStringLiteral("队列中");
        _ItemViewModel->setData(qIndex, QVariant::fromValue<RaptorTransferItem>(item));
    }

    _Ui->_ItemView->viewport()->update();
    auto input = RaptorInput();
    input._Indexes = qIndexList;
    Q_EMIT itemsResuming(QVariant::fromValue<RaptorInput>(input));
}

void RaptorDownloadingPage::invokeItemsCancel() const
{
    auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        return;
    }

    if (const auto qOperate = RaptorMessageBox::invokeWarningEject(QStringLiteral("取消下载"),
                                              QStringLiteral("即将取消下载所选文件，是否继续?"));
                                              qOperate == RaptorMessageBox::No)
    {
        return;
    }


    std::reverse(qIndexList.begin(), qIndexList.end());
    for (auto &qIndex: qIndexList)
    {
        auto input = RaptorInput();
        const auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        input._Variant = QVariant::fromValue<RaptorTransferItem>(item);
        Q_EMIT itemCancelling(QVariant::fromValue<RaptorInput>(input));
        _ItemViewModel->removeRow(qIndex.row());
    }

    _Ui->_ItemView->viewport()->update();
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
    _Ui->_ItemEstimatedTime->clear();
    _Ui->_ItemSpeed->clear();
}

void RaptorDownloadingPage::invokeInstanceInit()
{
    _ItemViewModel = new RaptorDownloadingViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("大小") << QStringLiteral("状态");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(4);
    _ItemViewDelegate = new RaptorDownloadingViewDelegate(this);
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
}

void RaptorDownloadingPage::invokeUiInit()
{
    _Ui->_Splitter->setStretchFactor(0, 9);
    _Ui->_Splitter->setStretchFactor(1, 1);
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
    _Ui->_ItemView->setColumnWidth(2, 160);
    _Ui->_ItemView->setColumnWidth(3, 160);
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemNameTip->setText(QStringLiteral("名称:"));
    _Ui->_ItemName->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemCreatedTip->setText(QStringLiteral("创建时间:"));
    _Ui->_ItemSizeTip->setText(QStringLiteral("大小:"));
    _Ui->_ItemEstimatedTimeTip->setText(QStringLiteral("剩余时间:"));
    _Ui->_ItemSpeedTip->setText(QStringLiteral("速度:"));
    _Ui->_Item3rdPartyEvoke->setText(QStringLiteral("唤起 %1").arg(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                                                    Setting::Download::ActiveEngine).toString()));
}

void RaptorDownloadingPage::invokeSlotInit() const
{
    connect(_Ui->_ItemView,
            &QTableView::clicked,
            this,
            &RaptorDownloadingPage::onItemViewClicked);

    connect(_Ui->_ItemView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &RaptorDownloadingPage::onItemViewSelectionChanged);

    connect(_Ui->_Item3rdPartyEvoke,
            &QPushButton::clicked,
            this,
            &RaptorDownloadingPage::onItem3rdPartyEvoke);
}

void RaptorDownloadingPage::onItemsLoaded(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto items = _Data.value<QVector<RaptorTransferItem>>();
    _ItemViewModel->invokeItemsAppend(items);
}

void RaptorDownloadingPage::onItemLogouting(const QVariant& qVariant) const
{
    auto input = qVariant.value<RaptorInput>();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item != RaptorStoreSuite::invokeUserGet())
    {
        return;
    }

    auto qIndexList = QModelIndexList();
    for (auto i = 0; i < _ItemViewModel->rowCount(); ++i)
    {
        const auto qIndex = _ItemViewModel->index(i, 0);
        const auto iten = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        qIndexList << qIndex;
    }

    input._Indexes = qIndexList;
    Q_EMIT itemsPausing(QVariant::fromValue<RaptorInput>(input));
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
    _Ui->_ItemEstimatedTime->clear();
    _Ui->_ItemSpeed->clear();
}

void RaptorDownloadingPage::onItemSwitching(const QVariant& qVariant) const
{
    Q_UNUSED(qVariant)
    auto input = RaptorInput();
    auto qIndexList = QModelIndexList();
    for (auto i = 0; i < _ItemViewModel->rowCount(); ++i)
    {
        const auto qIndex = _ItemViewModel->index(i, 0);
        const auto iten = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        qIndexList << qIndex;
    }

    input._Indexes = qIndexList;
    Q_EMIT itemsPausing(QVariant::fromValue<RaptorInput>(input));
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
    _Ui->_ItemEstimatedTime->clear();
    _Ui->_ItemSpeed->clear();
}

void RaptorDownloadingPage::onItemsQueuing(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    _ItemViewModel->invokeItemsAppend(_Data.value<QVector<RaptorTransferItem>>());
}

void RaptorDownloadingPage::onItemsStatusChanged(const QVariant& qVariant) const
{
    const auto [_State, rMessgae, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(rMessgae);
        return;
    }

    if (!RaptorStoreSuite::invokeViewPaintableGet())
    {
        return;
    }

    const auto items = _Data.value<QQueue<RaptorTransferItem>>();
    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        qIndexList.length() == 1)
    {
        const auto iten = qIndexList[0].data(Qt::UserRole).value<RaptorTransferItem>();
        for (auto& item : items)
        {
            if (item == iten)
            {
                _Ui->_ItemEstimatedTime->setText(item._ETR);
                _Ui->_ItemSpeed->setText(QStringLiteral("%1/S").arg(item._Speed.toString()));
            }
        }
    }

    for (auto i = 0; i < _ItemViewModel->rowCount(); ++i)
    {
        const auto qIndex = _ItemViewModel->index(i, 0);
        auto iten = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        for (auto& item : items)
        {
            if (item == iten)
            {
                _ItemViewModel->setData(qIndex, QVariant::fromValue<RaptorTransferItem>(item));
            }
        }
    }

    _Ui->_ItemView->viewport()->update();
}

void RaptorDownloadingPage::onItemPaused(const QVariant& qVariant) const
{
    const auto [_State, rMessgae, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(rMessgae);
        return;
    }

    const auto item = _Data.value<RaptorTransferItem>();
    for (auto i = 0; i < _ItemViewModel->rowCount(); ++i)
    {
        const auto qIndex = _ItemViewModel->index(i, 0);
        if (auto iten = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
            item == iten)
        {
            _ItemViewModel->setData(qIndex, QVariant::fromValue<RaptorTransferItem>(item));
            break;
        }
    }

    _Ui->_ItemView->viewport()->update();
}

void RaptorDownloadingPage::onItemCompleted(const QVariant& qVariant) const
{
    const auto [_State, rMessgae, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(rMessgae);
        return;
    }

    const auto item = _Data.value<RaptorTransferItem>();
    for (auto i = 0; i < _ItemViewModel->rowCount(); ++i)
    {
        const auto qIndex = _ItemViewModel->index(i, 0);
        if (const auto iten = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
            item == iten)
        {
            _ItemViewModel->removeRow(qIndex.row());
            break;
        }
    }

    _Ui->_ItemView->viewport()->update();
    if (_ItemViewModel->invokeItemsEject().isEmpty())
    {
        _Ui->_ItemName->clear();
        _Ui->_ItemCreated->clear();
        _Ui->_ItemSize->clear();
        _Ui->_ItemEstimatedTime->clear();
        _Ui->_ItemSpeed->clear();
    }
}

void RaptorDownloadingPage::onItemErrored(const QVariant& qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        _State)
    {
        RaptorToast::invokeCriticalEject(_Message);
    }
}

void RaptorDownloadingPage::onItemViewClicked(const QModelIndex& qIndex) const
{
    if (!qIndex.isValid())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemCreated->setText(item._Created);
    _Ui->_ItemSize->setText(item._Size);
}

void RaptorDownloadingPage::onItemViewSelectionChanged(const QItemSelection& qSelected,
                                                       const QItemSelection& qDeselected) const
{
    Q_UNUSED(qDeselected)
    if (!qSelected.isEmpty())
    {
        return;
    }

    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
    _Ui->_ItemEstimatedTime->clear();
    _Ui->_ItemSpeed->clear();
}

void RaptorDownloadingPage::onItem3rdPartyEvoke() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (const auto qEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                Setting::Download::ActiveEngine).toString();
        qEngine.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未设置活跃的第三方下载引擎。无法继续!"));
        return;
    }

    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.length() != 1)
    {
        return;
    }

    const auto qIndex = qIndexList[0];
    const auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
    if (item._Url.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("请先刷新地址，然后再次 %1 进行下载。").arg(_Ui->_Item3rdPartyEvoke->text()));
        return;
    }

    if (const auto qError = RaptorUtil::invoke3rdPartyDownloaderEvoke(item._Url, item._Path, item._Name);
        !qError.isEmpty())
    {
        RaptorToast::invokeWarningEject(qError);
        return;
    }

    auto input = RaptorInput();
    input._Variant = QVariant::fromValue<RaptorTransferItem>(item);
    Q_EMIT itemCancelling(QVariant::fromValue<RaptorInput>(input));
    _ItemViewModel->removeRow(qIndex.row());
    _Ui->_ItemView->viewport()->update();
    if (_ItemViewModel->invokeItemsEject().isEmpty())
    {
        _Ui->_ItemName->clear();
        _Ui->_ItemCreated->clear();
        _Ui->_ItemSize->clear();
        _Ui->_ItemEstimatedTime->clear();
        _Ui->_ItemSpeed->clear();
    }
}
