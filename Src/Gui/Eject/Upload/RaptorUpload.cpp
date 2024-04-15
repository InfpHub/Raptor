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

#include "RaptorUpload.h"
#include "ui_RaptorUpload.h"

RaptorUpload::RaptorUpload(QWidget *qParent) : RaptorEject(qParent),
                                               _Ui(new Ui::RaptorUpload)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorUpload::~RaptorUpload()
{
    qFree(_Ui)
}

bool RaptorUpload::eventFilter(QObject *qObject, QEvent *qEvent)
{
    if (qObject == this)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent *>(qEvent);
                qKeyEvent->key() == Qt::Key_Escape)
            {
                onCloseClicked();
                return true;
            }
        }
    }

    return RaptorEject::eventFilter(qObject, qEvent);
}

void RaptorUpload::invokeEject(const QVariant &qVariant)
{
    _Variant = qVariant;
    _Ui->_Title->setText(
        QStringLiteral("上传文件到 %1").arg(
            QString(qCreativeTemplate).arg(_Variant.value<QPair<QString, QString> >().second)));
    RaptorEject::invokeEject();
}

void RaptorUpload::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemViewModel = new RaptorUploadViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(2);
    _ItemViewDelegate = new RaptorTableViewDelegate(this);
}

void RaptorUpload::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_Splitter->setStretchFactor(0, 9);
    _Ui->_Splitter->setStretchFactor(1, 1);
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
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemNameTip->setText(QStringLiteral("名称:"));
    _Ui->_ItemName->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemPathTip->setText(QStringLiteral("路径:"));
    _Ui->_ItemPath->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemSizeTip->setText(QStringLiteral("大小:"));
    _Ui->_ItemLocate->setText(QStringLiteral("定位"));
    _Ui->_ItemDelete->setText(QStringLiteral("删除"));
    _Ui->_Add->setText(QStringLiteral("添加"));
    _Ui->_AddDir->setText(QStringLiteral("添加文件夹"));
    _Ui->_Delete->setText(QStringLiteral("删除"));
    _Ui->_Clear->setText(QStringLiteral("清空"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
    _Ui->_OK->setText(QStringLiteral("确定"));
}

void RaptorUpload::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    invokeCloseCallbackSet([=]() -> void
    {
        onClearClicked();
    });
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorUpload::onCloseClicked);

    connect(_Ui->_ItemView,
            &QTableView::clicked,
            this,
            &RaptorUpload::onItemViewClicked);

    connect(_Ui->_ItemView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &RaptorUpload::onItemViewSelectionChanged);

    connect(_Ui->_ItemLocate,
            &QPushButton::clicked,
            this,
            &RaptorUpload::onItemLocateClicked);

    connect(_Ui->_ItemDelete,
            &QPushButton::clicked,
            this,
            &RaptorUpload::onItemDeleteClicked);

    connect(_Ui->_Add,
            &QPushButton::clicked,
            this,
            &RaptorUpload::onAddClicked);

    connect(_Ui->_AddDir,
            &QPushButton::clicked,
            this,
            &RaptorUpload::onAddDirClicked);

    connect(_Ui->_Delete,
            &QPushButton::clicked,
            this,
            &RaptorUpload::onDeleteClicked);

    connect(_Ui->_Clear,
            &QPushButton::clicked,
            this,
            &RaptorUpload::onClearClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorUpload::onCancelClicked);

    connect(_Ui->_OK,
            &QPushButton::clicked,
            this, &RaptorUpload::onOKClicked);
}

void RaptorUpload::onCloseClicked()
{
    close();
}

void RaptorUpload::onItemViewClicked(const QModelIndex &qIndex) const
{
    if (!qIndex.isValid())
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
    _Ui->_ItemName->setText(item._Name);
    _Ui->_ItemPath->setText(item._Path);
    _Ui->_ItemSize->setText(item._Size);
}

void RaptorUpload::onItemViewSelectionChanged(const QItemSelection &qSelected,
                                              const QItemSelection &qDeselected) const
{
    Q_UNUSED(qDeselected)
    if (const auto qIndexList = qSelected.indexes();
        qIndexList.length() == 1)
    {
        if (const auto item = qIndexList[0].data(Qt::UserRole).value<RaptorTransferItem>();
            item._Type == "file")
        {
            _Ui->_ItemLocate->setEnabled(true);
        } else
        {
            _Ui->_ItemLocate->setEnabled(false);
        }

        return;
    }

    _Ui->_ItemName->clear();
    _Ui->_ItemPath->clear();
    _Ui->_ItemSize->clear();
}

void RaptorUpload::onAddClicked()
{
    const auto items = QFileDialog::getOpenFileNames(this, QStringLiteral("选择文件"),
                                                     QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
                                                     "所有文件 (*.*)");
    if (items.isEmpty())
    {
        return;
    }

    for (auto &item: items)
    {
        auto iten = RaptorTransferItem();
        const auto qFileInfo = QFileInfo(item);
        iten._Type = "file";
        iten._Name = qFileInfo.fileName();
        iten._Icon = RaptorUtil::invokeIconMatch(iten._Name);
        iten._Path = qFileInfo.path();
        iten._Size = RaptorUtil::invokeStorageUnitConvert(qFileInfo.size());
        _ItemViewModel->invokeItemAppend(iten);
    }

    RaptorToast::invokeInformationEject(QStringLiteral("已添加 %1 个文件").arg(items.length()));
}

void RaptorUpload::onAddDirClicked()
{
    const auto qPath = QFileDialog::getExistingDirectory(this,
                                                         QStringLiteral("选择文件夹"),
                                                         QStandardPaths::writableLocation(
                                                             QStandardPaths::DownloadLocation),
                                                         QFileDialog::Option::ShowDirsOnly |
                                                         QFileDialog::Option::DontResolveSymlinks);
    if (qPath.isEmpty())
    {
        return;
    }

    auto item = RaptorTransferItem();
    item._Type = "folder";
    item._Name = QDir(qPath).dirName();
    item._Icon = RaptorUtil::invokeIconMatch(QString{}, true);
    item._Path = qPath;
    _ItemViewModel->invokeItemAppend(item);
    RaptorToast::invokeInformationEject(QStringLiteral("已添加 1 个文件夹"));
}

void RaptorUpload::onDeleteClicked() const
{
    if (auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        !qIndexList.empty())
    {
        std::reverse(qIndexList.begin(), qIndexList.end());
        for (auto &qIndex: qIndexList)
        {
            _ItemViewModel->removeRow(qIndex.row());
        }
    }

    if (_ItemViewModel->rowCount() == 0)
    {
        _Ui->_ItemName->clear();
        _Ui->_ItemPath->clear();
        _Ui->_ItemSize->clear();
    }
}

void RaptorUpload::onClearClicked() const
{
    _ItemViewModel->invokeItemsClear();
    _Ui->_ItemName->clear();
    _Ui->_ItemPath->clear();
    _Ui->_ItemSize->clear();
}

void RaptorUpload::onItemLocateClicked() const
{
    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        qIndexList.length() == 1)
    {
        const auto item = qIndexList[0].data(Qt::UserRole).value<RaptorTransferItem>();
        auto qFileName = QStringLiteral("%1/%2").arg(item._Path, item._Name);
        if (item._Path.length() == 3)
        {
            // I://01.MP4 -> I:/01.MP4
            qFileName = QStringLiteral("%1%2").arg(item._Path, item._Name);
        }

        RaptorUtil::invokeItemLocate(qFileName);
    }
}

void RaptorUpload::onItemDeleteClicked() const
{
    if (const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
        qIndexList.length() == 1)
    {
        const auto qIndex = qIndexList[0];
        _ItemViewModel->removeRow(qIndex.row());
        _Ui->_ItemView->viewport()->update();
    }

    if (_ItemViewModel->rowCount() == 0)
    {
        _Ui->_ItemName->clear();
        _Ui->_ItemPath->clear();
        _Ui->_ItemSize->clear();
    }
}

void RaptorUpload::onCancelClicked()
{
    onCloseClicked();
}

void RaptorUpload::onOKClicked()
{
    if (_ItemViewModel->rowCount() > 0)
    {
        auto input = RaptorInput();
        input._Parent = _Variant.value<QPair<QString, QString> >().first;
        // 此处传递 Space 防止在递归创建过程中切换空间类型
        input._Space = RaptorStoreSuite::invokeUserGet()._Space;
        input._Variant = QVariant::fromValue<QVector<RaptorTransferItem> >(_ItemViewModel->invokeItemsEject());
        Q_EMIT itemsUploading(QVariant::fromValue<RaptorInput>(input));
    }

    close();
}
