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
    FREE(_Ui)
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
    _SourceViewModel->invokeItemsClear();
    _TargetViewModel->invokeItemsClear();
    RaptorEject::invokeEject();
    return _User;
}

void RaptorCopyUser::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _SourceViewHeader = new RaptorUserViewHeader(Qt::Horizontal, _Ui->_SourceView);
    _SourceViewModel = new RaptorUserViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("源用户");
    _SourceViewModel->invokeHeaderSet(qHeader);
    _SourceViewModel->invokeColumnCountSet(2);
    _TargetViewHeader = new RaptorUserViewHeader(Qt::Horizontal, _Ui->_TargetView);
    _TargetViewModel = new RaptorUserViewModel(this);
    qHeader.clear();
    qHeader << QStringLiteral("目标用户");
    _TargetViewModel->invokeHeaderSet(qHeader);
    _TargetViewModel->invokeColumnCountSet(2);
    _SourceViewDelegate = new RaptorUserViewDelegate(this);
    _TargetViewDelegate = new RaptorUserViewDelegate(this);
}

void RaptorCopyUser::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Title->setText(QStringLiteral("选择用户"));
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
    _Ui->_SourceView->setSelectionMode(QAbstractItemView::SingleSelection);
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
    _Ui->_TargetView->setSelectionMode(QAbstractItemView::SingleSelection);
    _Ui->_TargetView->setSelectionBehavior(QAbstractItemView::SelectRows);
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

    connect(_Ui->_SourceView,
            &QTableView::clicked,
            this,
            &RaptorCopyUser::onSourceViewClicked);

    connect(_Ui->_TargetView,
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
    _SourceViewModel->invokeItemsAppend(items);
    _TargetViewModel->invokeItemsAppend(items);
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
    if (_Ui->_SourceView->selectionModel()->selectedRows().isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择源用户!"));
        return;
    }

    if (_Ui->_TargetView->selectionModel()->selectedRows().isEmpty())
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