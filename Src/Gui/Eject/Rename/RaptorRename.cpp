/**
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

#include "RaptorRename.h"
#include "ui_RaptorRename.h"

RaptorRename::RaptorRename(QWidget *qParent) : RaptorEject(qParent),
                                               _Ui(new Ui::RaptorRename)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorRename::~RaptorRename()
{
    FREE(_Ui)
}

bool RaptorRename::eventFilter(QObject *qObject, QEvent *qEvent)
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

void RaptorRename::invokeEject(const QVariant &qVariant)
{
    _SourceViewModel->invokeItemsClear();
    _TargetViewModel->invokeItemsClear();
    const auto [qIndexList, qParent] = qVariant.value<QPair<QModelIndexList, QString> >();
    _Payload._Items.clear();
    _Payload._Parent = qParent;
    for (auto &qIndex: qIndexList)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        _SourceViewModel->invokeItemAppend(item);
        _TargetViewModel->invokeItemAppend(item);
    }

    _Ui->_Title->setText(QString(CREATIVE_TEMPLATE).arg(QStringLiteral("重命名 %1 个文件").arg(qIndexList.length())));
    RaptorEject::invokeEject(qVariant);
}

void RaptorRename::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _SourceViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_SourceView);
    _SourceViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _SourceViewModel = new RaptorSpaceViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("源名称");
    _SourceViewModel->invokeHeaderSet(qHeader);
    _SourceViewModel->invokeColumnCountSet(2);
    _TargetViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_TargetView);
    _TargetViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _TargetViewModel = new RaptorSpaceViewModel(this);
    qHeader.clear();
    qHeader << QStringLiteral("新名称");
    _TargetViewModel->invokeHeaderSet(qHeader);
    _TargetViewModel->invokeColumnCountSet(2);
    _SourceViewDelegate = new RaptorTableViewDelegate(this);
    _TargetViewDelegate = new RaptorTableViewDelegate(this);
}

void RaptorRename::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_SourceView->setModel(_SourceViewModel);
    _Ui->_SourceView->setHorizontalHeader(_SourceViewHeader);
    _Ui->_SourceView->setItemDelegate(_SourceViewDelegate);
    _Ui->_SourceView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_SourceView->horizontalHeader()->setFixedHeight(26);
    _Ui->_SourceView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_SourceView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_SourceView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_SourceView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_SourceView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_SourceView->verticalHeader()->setHidden(true);
    _Ui->_SourceView->setShowGrid(false);
    _Ui->_SourceView->setColumnWidth(0, 30);
    _Ui->_SourceView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _Ui->_SourceView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_TargetView->setModel(_TargetViewModel);
    _Ui->_TargetView->setHorizontalHeader(_TargetViewHeader);
    _Ui->_TargetView->setItemDelegate(_TargetViewDelegate);
    _Ui->_TargetView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_TargetView->horizontalHeader()->setFixedHeight(26);
    _Ui->_TargetView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_TargetView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_TargetView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_TargetView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_TargetView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_TargetView->verticalHeader()->setHidden(true);
    _Ui->_TargetView->setShowGrid(false);
    _Ui->_TargetView->setColumnWidth(0, 30);
    _Ui->_TargetView->setEditTriggers(QAbstractItemView::DoubleClicked);
    _Ui->_TargetView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_OK->setText(QStringLiteral("确定"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
}

void RaptorRename::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorRename::onCloseClicked);

    connect(_TargetViewModel,
            &RaptorSpaceViewModel::itemEdited,
            this,
            &RaptorRename::onTargetViewModelItemEdited);

    connect(_Ui->_OK,
            &QPushButton::clicked,
            this,
            &RaptorRename::onOkClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorRename::onCancelClicked);
}

void RaptorRename::onTargetViewModelItemEdited(const QVariant &qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto item = _Data.value<QModelIndex>().data(Qt::UserRole).value<RaptorFileItem>();
    if (_Payload._Items.isEmpty())
    {
        _Payload._Items << item;
        return;
    }

    // 尝试进行替换
    auto qHadFound = false;
    for (auto i = 0; i < _Payload._Items.length(); ++i)
    {
        if (const auto iten = _Payload._Items[i];
            item == iten)
        {
            _Payload._Items.replace(i, item);
            qHadFound = true;
            break;
        }
    }

    if (qHadFound)
    {
        return;
    }

    // 不为空但是也没有
    _Payload._Items <<  item;
}

void RaptorRename::onCloseClicked()
{
    close();
}

void RaptorRename::onOkClicked()
{
    if (!_Payload._Items.isEmpty())
    {
        auto input = RaptorInput();
        input._Parent = _Payload._Parent;
        input._Variant = QVariant::fromValue<QVector<RaptorFileItem> >(_Payload._Items);
        Q_EMIT itemsRenaming(QVariant::fromValue<RaptorInput>(input));
    }

    onCloseClicked();
}

void RaptorRename::onCancelClicked()
{
    onCloseClicked();
}
