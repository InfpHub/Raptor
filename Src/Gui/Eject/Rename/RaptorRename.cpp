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
    qFree(_Ui)
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
    _ItemSourceViewModel->invokeItemsClear();
    _ItemTargetViewModel->invokeItemsClear();
    const auto [qIndexList, qParent] = qVariant.value<QPair<QModelIndexList, QString> >();
    _Payload._Items.clear();
    _Payload._Parent = qParent;
    for (auto &qIndex: qIndexList)
    {
        const auto item = qIndex.data(Qt::UserRole).value<RaptorFileItem>();
        _ItemSourceViewModel->invokeItemAppend(item);
        _ItemTargetViewModel->invokeItemAppend(item);
    }

    _Ui->_Title->setText(QString(qCreativeTemplate).arg(QStringLiteral("重命名 %1 个文件").arg(qIndexList.length())));
    RaptorEject::invokeEject(qVariant);
}

void RaptorRename::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _ItemSourceViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemSourceView);
    _ItemSourceViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemSourceViewModel = new RaptorSpaceViewModel(_Ui->_ItemSourceView);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("源名称");
    _ItemSourceViewModel->invokeHeaderSet(qHeader);
    _ItemSourceViewModel->invokeColumnCountSet(2);
    _ItemTargetViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemTargetView);
    _ItemTargetViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemTargetViewModel = new RaptorSpaceViewModel(_Ui->_ItemTargetView);
    qHeader.clear();
    qHeader << QStringLiteral("新名称");
    _ItemTargetViewModel->invokeHeaderSet(qHeader);
    _ItemTargetViewModel->invokeColumnCountSet(2);
    _ItemSourceViewDelegate = new RaptorTableViewDelegate(this);
    _ItemTargetViewDelegate = new RaptorTableViewDelegate(this);
}

void RaptorRename::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_ItemSourceView->setModel(_ItemSourceViewModel);
    _Ui->_ItemSourceView->setHorizontalHeader(_ItemSourceViewHeader);
    _Ui->_ItemSourceView->setItemDelegate(_ItemSourceViewDelegate);
    _Ui->_ItemSourceView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemSourceView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemSourceView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ItemSourceView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_ItemSourceView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemSourceView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemSourceView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemSourceView->verticalHeader()->setHidden(true);
    _Ui->_ItemSourceView->setShowGrid(false);
    _Ui->_ItemSourceView->setColumnWidth(0, 30);
    _Ui->_ItemSourceView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _Ui->_ItemSourceView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_ItemTargetView->setModel(_ItemTargetViewModel);
    _Ui->_ItemTargetView->setHorizontalHeader(_ItemTargetViewHeader);
    _Ui->_ItemTargetView->setItemDelegate(_ItemTargetViewDelegate);
    _Ui->_ItemTargetView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemTargetView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemTargetView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ItemTargetView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_ItemTargetView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemTargetView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemTargetView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemTargetView->verticalHeader()->setHidden(true);
    _Ui->_ItemTargetView->setShowGrid(false);
    _Ui->_ItemTargetView->setColumnWidth(0, 30);
    _Ui->_ItemTargetView->setEditTriggers(QAbstractItemView::DoubleClicked);
    _Ui->_ItemTargetView->setSelectionBehavior(QAbstractItemView::SelectRows);
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

    connect(_ItemTargetViewModel,
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
