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

#include "RaptorCopyUser.h"
#include "ui_RaptorCopyUser.h"

RaptorCopyUser::RaptorCopyUser(QWidget* qParent) : RaptorEject(qParent),
                                                   _Ui(new Ui::RaptorCopyUser)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorCopyUser::~RaptorCopyUser()
{
    qFree(_Ui)
}

bool RaptorCopyUser::eventFilter(QObject* qObject, QEvent* qEvent)
{
    if (qObject == this)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->key() == Qt::Key_Escape)
            {
                onCloseClicked();
                return true;
            }
        }
    }

    return RaptorEject::eventFilter(qObject, qEvent);
}

QPair<RaptorAuthenticationItem, RaptorAuthenticationItem> RaptorCopyUser::invokeEject()
{
    _ItemSourceViewModel->invokeItemsClear();
    _ItemTargetViewModel->invokeItemsClear();
    RaptorEject::invokeEject();
    return _User;
}

void RaptorCopyUser::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _ItemSourceViewHeader = new RaptorUserViewHeader(Qt::Horizontal, _Ui->_ItemSourceView);
    _ItemSourceViewModel = new RaptorUserViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("源用户");
    _ItemSourceViewModel->invokeHeaderSet(qHeader);
    _ItemSourceViewModel->invokeColumnCountSet(2);
    _ItemTargetViewHeader = new RaptorUserViewHeader(Qt::Horizontal, _Ui->_ItemTargetView);
    _ItemTargetViewModel = new RaptorUserViewModel(this);
    qHeader.clear();
    qHeader << QStringLiteral("目标用户");
    _ItemTargetViewModel->invokeHeaderSet(qHeader);
    _ItemTargetViewModel->invokeColumnCountSet(2);
    _ItemSourceViewDelegate = new RaptorUserViewDelegate(this);
    _ItemTargetViewDelegate = new RaptorUserViewDelegate(this);
}

void RaptorCopyUser::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Title->setText(QStringLiteral("选择用户"));
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
    _Ui->_ItemSourceView->setSelectionMode(QAbstractItemView::SingleSelection);
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
    _Ui->_ItemTargetView->setSelectionMode(QAbstractItemView::SingleSelection);
    _Ui->_ItemTargetView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_OK->setText(QStringLiteral("确定"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
}

void RaptorCopyUser::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorCopyUser::onCloseClicked);

    connect(_Ui->_ItemSourceView,
            &QTableView::clicked,
            this,
            &RaptorCopyUser::onSourceViewClicked);

    connect(_Ui->_ItemTargetView,
            &QTableView::clicked,
            this,
            &RaptorCopyUser::onTargetViewClicked);

    connect(_Ui->_OK,
            &QPushButton::clicked,
            this,
            &RaptorCopyUser::onOkClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorCopyUser::onCancelClicked);
}

void RaptorCopyUser::onItemsLoaded(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    const auto items = _Data.value<QVector<RaptorAuthenticationItem>>();
    _ItemSourceViewModel->invokeItemsAppend(items);
    _ItemTargetViewModel->invokeItemsAppend(items);
}

void RaptorCopyUser::onCloseClicked()
{
    close();
}

void RaptorCopyUser::onSourceViewClicked(const QModelIndex& qIndex)
{
    _User.first = qIndex.data(Qt::UserRole).value<RaptorAuthenticationItem>();
}

void RaptorCopyUser::onTargetViewClicked(const QModelIndex& qIndex)
{
    _User.second = qIndex.data(Qt::UserRole).value<RaptorAuthenticationItem>();
}

void RaptorCopyUser::onOkClicked()
{
    if (_Ui->_ItemSourceView->selectionModel()->selectedRows().isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择源用户!"));
        return;
    }

    if (_Ui->_ItemTargetView->selectionModel()->selectedRows().isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择目标用户!"));
        return;
    }

    if (_User.first == _User.second)
    {
        RaptorToast::invokeWarningEject(QStringLiteral("源用户不能与目标用户相同!"));
        return;
    }

    onCloseClicked();
}

void RaptorCopyUser::onCancelClicked()
{
    onCloseClicked();
}