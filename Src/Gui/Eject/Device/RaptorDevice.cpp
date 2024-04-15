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

#include "RaptorDevice.h"
#include "ui_RaptorDevice.h"

RaptorDevice::RaptorDevice(QWidget *qParent) : RaptorEject(qParent),
                                               _Ui(new Ui::RaptorDevice)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorDevice::~RaptorDevice()
{
    qFree(_Ui)
}

bool RaptorDevice::eventFilter(QObject *qObject, QEvent *qEvent)
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

    if (qObject == _Ui->_DeviceIcon)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeIconDrawing();
            return true;
        }
    }

    return RaptorEject::eventFilter(qObject, qEvent);
}

void RaptorDevice::invokeEject(const QVariant &qVariant)
{
    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Loading);
    RaptorEject::invokeEject(qVariant);
}

void RaptorDevice::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _ItemViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ItemView);
    _ItemViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ItemViewModel = new RaptorDeviceViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("设备") << QStringLiteral("城市") << QStringLiteral("登录时间");
    _ItemViewLoading = new RaptorLoading(_Ui->_ItemView);
    _ItemViewModel->invokeHeaderSet(qHeader);
    _ItemViewModel->invokeColumnCountSet(4);
    _ItemViewDelegate = new RaptorTableViewDelegate(this);
}

void RaptorDevice::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
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
    _Ui->_ItemView->setColumnWidth(2, 80);
    _Ui->_ItemView->setColumnWidth(3, 80);
    _Ui->_ItemView->setSelectionMode(QAbstractItemView::SingleSelection);
    _Ui->_ItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _Ui->_Offline->setText(QStringLiteral("下线"));
    _Ui->_Yes->setText(QStringLiteral("确定"));
    _Ui->_DeviceIcon->installEventFilter(this);
}

void RaptorDevice::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    invokeCloseCallbackSet([=]() -> void
    {
        _ItemViewLoading->invokeStateSet(RaptorLoading::State::Finished);
        _ItemViewModel->invokeItemsClear();
    });
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorDevice::onCloseClicked);

    connect(_Ui->_Offline,
            &QPushButton::clicked,
            this,
            &RaptorDevice::onOfflineClicked);

    connect(_Ui->_Yes,
            &QPushButton::clicked,
            this,
            &RaptorDevice::onYesClicked);
}

void RaptorDevice::invokeIconDrawing() const
{
    auto qPainter = QPainter(_Ui->_DeviceIcon);
    _SvgRender->load(RaptorUtil::invokeIconMatch("Device", false, true));
    if (_SvgRender->isValid())
    {
        _SvgRender->render(&qPainter, QRect(0, 0, _Ui->_DeviceIcon->width(), _Ui->_DeviceIcon->height()));
    }
}

void RaptorDevice::onItemDevicesFetched(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
    }

    _ItemViewLoading->invokeStateSet(RaptorLoading::State::Finished);
    const auto [qMax, qUsed, items] = _Data.value<std::tuple<quint8, quint8, QVector<RaptorDeviceItem> > >();
    _Ui->_Title->setText(QString(qCreativeTemplate).arg(QStringLiteral("设备 - 可用: %1，已用: %2").arg(qMax).arg(qUsed)));
    _ItemViewModel->invokeItemsAppend(items);
}

void RaptorDevice::onCloseClicked()
{
    close();
}

void RaptorDevice::onOfflineClicked() const
{
    RaptorToast::invokeInformationEject(QStringLiteral("即将在 Raptor 2024.3.0 中提供!"));
}

void RaptorDevice::onYesClicked()
{
    close();
}
