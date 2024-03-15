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

#include "RaptorUploadedPage.h"
#include "ui_RaptorUploadedPage.h"

RaptorUploadedPage::RaptorUploadedPage(QWidget* qParent) : QWidget(qParent),
                                                           _Ui(new Ui::RaptorUploadedPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorUploadedPage::~RaptorUploadedPage()
{
    FREE(_Ui)
}

bool RaptorUploadedPage::eventFilter(QObject* qObject, QEvent* qEvent)
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
                    invokeItemsDelete();
                    return true;
                }
            }
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

QModelIndexList RaptorUploadedPage::invokeSelectItemsGet() const
{
    return _Ui->_ItemView->selectionModel()->selectedRows();
}

void RaptorUploadedPage::invokeItemsClear() const
{
    if (const auto qOperate = RaptorMessageBox::invokeCriticalEject(QStringLiteral("清空记录"),
                                               QStringLiteral("即将清空上传记录，是否继续?"));
                                               qOperate == RaptorMessageBox::No)
    {
        return;
    }

    _ItemViewModel->invokeItemsClear();
    Q_EMIT itemsClearing();
}

void RaptorUploadedPage::invokeItemsDelete() const
{
    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        return;
    }

    if (const auto qOperate = RaptorMessageBox::invokeWarningEject(QStringLiteral("删除记录"),
                                              QStringLiteral("即将删除所选上传记录，是否继续?"));
                                              qOperate == RaptorMessageBox::No)
    {
        return;
    }

    auto items = QVector<RaptorTransferItem>();
    for (auto& qIndex : qIndexList)
    {
        items << qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
        _ItemViewModel->removeRow(qIndex.row());
    }

    _Ui->_ItemView->viewport()->update();
    auto input = RaptorInput();
    input._Variant = QVariant::fromValue<QVector<RaptorTransferItem>>(items);
    Q_EMIT itemsDeleting(QVariant::fromValue<RaptorInput>(input));
}

void RaptorUploadedPage::invokeInstanceInit()
{
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemViewModel = new RaptorUploadedViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("大小") << QStringLiteral("创建时间");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(4);
    _ItemViewDelegate = new RaptorTableViewDelegate(this);
}

void RaptorUploadedPage::invokeUiInit()
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
    _Ui->_ItemView->setColumnWidth(2, 140);
    _Ui->_ItemView->setColumnWidth(3, 80);
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemNameTip->setText(QStringLiteral("名称:"));
    _Ui->_ItemName->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemCreatedTip->setText(QStringLiteral("创建时间:"));
    _Ui->_ItemSizeTip->setText(QStringLiteral("大小:"));
}

void RaptorUploadedPage::invokeSlotInit() const
{
    connect(_Ui->_ItemView,
            &QTableView::clicked,
            this,
            &RaptorUploadedPage::onItemViewClicked);

    connect(_Ui->_ItemView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &RaptorUploadedPage::onItemViewSelectionChanged);
}

void RaptorUploadedPage::onItemsLoaded(const QVariant& qVariant) const
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

void RaptorUploadedPage::onItemsDeleted(const QVariant& qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("已删除所选上传记录。"));
}

void RaptorUploadedPage::onItemsCleared(const QVariant& qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("已清空上传记录。"));
}

void RaptorUploadedPage::onItemLogouting(const QVariant& qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    const auto item = input._Variant.value<RaptorAuthenticationItem>();
    if (item != RaptorStoreSuite::invokeUserGet())
    {
        return;
    }

    _ItemViewModel->invokeItemsClear();
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
}

void RaptorUploadedPage::onItemSwitching(const QVariant& qVariant) const
{
    Q_UNUSED(qVariant)
    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
}

void RaptorUploadedPage::onItemCompleted(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto item = _Data.value<RaptorTransferItem>();
    _ItemViewModel->invokeItemAppend(item);
}

void RaptorUploadedPage::onItemViewClicked(const QModelIndex& qIndex) const
{
    if (!qIndex.isValid())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
    _Ui->_ItemCreated->setText(item._Created);
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemSize->setText(item._Size);
}

void RaptorUploadedPage::onItemViewSelectionChanged(const QItemSelection& qSelected,
                                                    const QItemSelection& qDeselected) const
{
    Q_UNUSED(qDeselected)
    if (qSelected.length() == 1)
    {
        const auto item = qSelected.indexes()[0].data(Qt::UserRole).value<RaptorTransferItem>();
        _Ui->_ItemName->setText(item._Name);
        _Ui->_ItemCreated->setText(item._Created);
        _Ui->_ItemSize->setText(item._Size);
        return;
    }

    _Ui->_ItemName->clear();
    _Ui->_ItemCreated->clear();
    _Ui->_ItemSize->clear();
}