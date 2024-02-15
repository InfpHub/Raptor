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

#include "RaptorUser.h"
#include "ui_RaptorUser.h"

RaptorUser::RaptorUser(QWidget* qParent) : QDialog(qParent),
                                           _Ui(new Ui::RaptorUser)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorUser::~RaptorUser()
{
    FREE(_Ui)
}

bool RaptorUser::eventFilter(QObject* qObject, QEvent* qEvent)
{
    if (qObject == this)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
                qKeyEvent->key() == Qt::Key_Escape)
            {
                _StartAnimation->setStartValue(1);
                _StartAnimation->setEndValue(0);
                _StartAnimation->setDuration(200);
                _StartAnimation->start();
                return true;
            }
        }

        if (qEvent->type() == QEvent::Close)
        {
            if (const auto qCloseEvent = static_cast<QCloseEvent*>(qEvent);
                windowOpacity() == 0.)
            {
                qCloseEvent->accept();
            }
            else
            {
                _StartAnimation->setStartValue(1);
                _StartAnimation->setEndValue(0);
                _StartAnimation->setDuration(200);
                _StartAnimation->start();
                qCloseEvent->ignore();
            }

            return true;
        }
    }
    return QDialog::eventFilter(qObject, qEvent);
}

int RaptorUser::exec()
{
    _StartAnimation->setStartValue(0);
    _StartAnimation->setEndValue(1);
    _StartAnimation->setDuration(200);
    _StartAnimation->setEasingCurve(QEasingCurve::InOutExpo);
    _StartMoveAnimation->setStartValue(QPoint(_Point.x() - width() / 2, _Point.y()));
    _StartMoveAnimation->setEndValue(_Point);
    _StartMoveAnimation->setDuration(200);
    _StartMoveAnimation->setEasingCurve(QEasingCurve::InOutExpo);
    _StartAnimationGroup->start();
    return QDialog::exec();
}

QPair<RaptorUser::Operate, RaptorAuthenticationItem> RaptorUser::invokeEject(const QPoint& qPoint)
{
    _ItemViewModel->invokeItemsClear();
    Q_EMIT itemsLoading();
    _Operate = Nothing;
    _Point = qPoint;
    exec();
    return qMakePair(_Operate, _User);
}

void RaptorUser::invokeInstanceInit()
{
    _StartAnimationGroup = new QParallelAnimationGroup(this);
    _StartAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    _StartMoveAnimation = new QPropertyAnimation(this, "pos", this);
    _StartAnimationGroup->addAnimation(_StartAnimation);
    _StartAnimationGroup->addAnimation(_StartMoveAnimation);
    _ItemViewHeader = new RaptorUserViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewDelegate = new RaptorUserViewDelegate(this);
    _ItemViewModel = new RaptorUserViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("双击以切换用户");
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(3);
}

void RaptorUser::invokeUiInit()
{
    setWindowFlags(Qt::FramelessWindowHint |
        Qt::Popup |
        Qt::MSWindowsFixedSizeDialogHint |
        Qt::NoDropShadowWindowHint);
    setWindowModality(Qt::NonModal);
    setAttribute(Qt::WA_TranslucentBackground);
    installEventFilter(this);
    _Ui->_NickName->setText(QStringLiteral("Hi，别来无恙~"));
    _Ui->_Refresh->setText(QStringLiteral("刷新"));
    _Ui->_Sign->setText(QStringLiteral("签到"));
    _Ui->_Logout->setText(QStringLiteral("注销"));
    _Ui->_Login->setText(QStringLiteral("登录新用户"));
    _Ui->_ItemView->setModel(_ItemViewModel);
    _Ui->_ItemView->setHorizontalHeader(_ItemViewHeader);
    _Ui->_ItemView->setItemDelegate(_ItemViewDelegate);
    _Ui->_ItemView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ItemView->horizontalHeader()->setMinimumSectionSize(26);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ItemView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_ItemView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ItemView->verticalHeader()->setHidden(true);
    _Ui->_ItemView->setShowGrid(false);
    _Ui->_ItemView->setColumnWidth(0, 30);
    _Ui->_ItemView->setColumnWidth(2, 40);
    _Ui->_ItemView->setSelectionMode(QAbstractItemView::SingleSelection);
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void RaptorUser::invokeSlotInit() const
{
    connect(_Ui->_Refresh,
            &QPushButton::clicked,
            this,
            &RaptorUser::onRefreshClicked);

    connect(_Ui->_Sign,
            &QPushButton::clicked,
            this,
            &RaptorUser::onSignInClicked);

    connect(_Ui->_Logout,
            &QPushButton::clicked,
            this,
            &RaptorUser::onLogoutClicked);

    connect(_Ui->_Login,
            &QPushButton::clicked,
            this,
            &RaptorUser::onLoginClicked);

    connect(_StartAnimation,
            &QPropertyAnimation::finished,
            this,
            &RaptorUser::onStartAnimationFinished);

    connect(_Ui->_ItemView,
            &QTableView::doubleClicked,
            this,
            &RaptorUser::onItemViewDoubleClicked);
}

void RaptorUser::onItemsLoaded(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    const auto items = _Data.value<QVector<RaptorAuthenticationItem>>();
    _ItemViewModel->invokeItemsAppend(items);
}

void RaptorUser::onItemLogoutd(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    if (const auto item = _Data.value<RaptorAuthenticationItem>();
        item == RaptorStoreSuite::invokeUserGet())
    {
        for (auto i = 0; i < _ItemViewModel->rowCount(); ++i)
        {
            const auto qIndex = _ItemViewModel->index(i, 0);
            if (const auto iten = qIndex.data(Qt::UserRole).value<RaptorAuthenticationItem>();
                item == iten)
            {
                _ItemViewModel->removeRow(qIndex.row());
                break;
            }
        }

        _Ui->_ItemView->viewport()->update();
        if (_User == RaptorStoreSuite::invokeUserGet())
        {
            _Ui->_NickName->setText(QStringLiteral("Hi，别来无恙~"));
            _Ui->_Capacity->clear();
        }

        RaptorStoreSuite::invokeUserLogout();
    }
}

void RaptorUser::onItemAccessTokenRefreshed(const QVariant& qVariant) const
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>(); !_State)
    {
        return;
    }

    const auto item = RaptorStoreSuite::invokeUserGet();
    _Ui->_NickName->setText(QStringLiteral("Hi, %1").arg(item._Nickname));
    _Ui->_Capacity->setText(QStringLiteral("%1 / %2").arg(item._Capacity._Used,
                                                          item._Capacity._Total));
    if (RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                           Setting::Ui::Sign).toBool())
    {
        Q_EMIT itemSignInConfirming();
    }
}

void RaptorUser::onItemSwitching(const QVariant& qVariant) const
{
    Q_UNUSED(qVariant)
    _ItemViewModel->invokeItemsClear();
}

void RaptorUser::onItemSignInConfirmed(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    if (_Data.value<bool>())
    {
        return;
    }

    Q_EMIT itemSigningIn();
}

void RaptorUser::onItemSignedIn(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    RaptorToast::invokeSuccessEject(_Message);
}

void RaptorUser::onItemCapacityRefreshed(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    const auto [qTotal, qUsed, qRadio, rAlbum, qNote] = _Data.value<RaptorCapacity>();
    _Ui->_Capacity->setText(QStringLiteral("%1 / %2").arg(qUsed, qTotal));
    RaptorToast::invokeSuccessEject(QStringLiteral("容量信息已刷新!"));
}

void RaptorUser::onRefreshClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    Q_EMIT itemCapacityRefreshing();
}

void RaptorUser::onSignInClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    Q_EMIT itemSigningIn();
}

void RaptorUser::onLogoutClicked()
{
    const auto qIndexList = _Ui->_ItemView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择目标用户，无法继续!"));
        return;
    }

    const auto qIndex = qIndexList[0];
    _Operate = Logout;
    _User = qIndex.data(Qt::UserRole).value<RaptorAuthenticationItem>();
    close();
}

void RaptorUser::onLoginClicked()
{
    _Operate = Login;
    close();
}

void RaptorUser::onStartAnimationFinished()
{
    if (windowOpacity() == 0.)
    {
        close();
    }
}

void RaptorUser::onItemViewDoubleClicked(const QModelIndex& qIndex)
{
    if (const auto item = qIndex.data(Qt::UserRole).value<RaptorAuthenticationItem>();
        item._Active)
    {
        return;
    }

    _Operate = Switch;
    _User = qIndex.data(Qt::UserRole).value<RaptorAuthenticationItem>();
    close();
}
