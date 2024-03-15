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

#include "RaptorOfficePage.h"
#include "ui_RaptorOfficePage.h"

RaptorOfficePage::RaptorOfficePage(QWidget *qParent) : QWidget(qParent),
                                                       _Ui(new Ui::RaptorOfficePage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorOfficePage::~RaptorOfficePage()
{
    FREE(_Ui)
}

void RaptorOfficePage::invokeInstanceInit()
{
    _EngineViewDelegate = new RaptorTableViewDelegate(this);
    _EngineViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_EngineView);
    _EngineViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _EngineViewModel = new RaptorSettingViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("路径");
    _EngineViewModel->invokeHeaderSet(qHeader);
    _EngineViewModel->invokeColumnCountSet(3);

    _DebounceTimer = new QTimer(this);
    _DebounceTimer->setSingleShot(true);
    _DebounceTimer->setInterval(1350);
}

void RaptorOfficePage::invokeUiInit() const
{
    _Ui->_EngineTip->setText(QStringLiteral("引擎:"));
    _Ui->_EngineView->setModel(_EngineViewModel);
    _Ui->_EngineView->setHorizontalHeader(_EngineViewHeader);
    _Ui->_EngineView->setItemDelegate(_EngineViewDelegate);
    _Ui->_EngineView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_EngineView->horizontalHeader()->setFixedHeight(26);
    _Ui->_EngineView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_EngineView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_EngineView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_EngineView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    _Ui->_EngineView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    _Ui->_EngineView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_EngineView->verticalHeader()->setHidden(true);
    _Ui->_EngineView->setShowGrid(false);
    _Ui->_EngineView->setColumnWidth(0, 30);
    _Ui->_EngineView->setColumnWidth(1, 80);
    _Ui->_EngineView->setSelectionMode(QAbstractItemView::SingleSelection);
    _Ui->_EngineView->setSelectionBehavior(QAbstractItemView::SelectRows);
    const auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Office,
                                                             Setting::Office::Engine).value<QVector<
        RaptorSettingItem> >();
    for (auto i = 0; i < qEngines.length(); ++i)
    {
        auto qEnginf = qEngines[i];
        auto [_Icon, _Name, _Path] = qEnginf;
        qEnginf._Icon = RaptorUtil::invokeIconMatch(_Name, false, true);
        _EngineViewModel->invokeItemAppend(qEnginf);
    }
}

void RaptorOfficePage::invokeSlotInit() const
{
    connect(_DebounceTimer,
            &QTimer::timeout,
            this,
            &RaptorOfficePage::onDebounceTimerTimeout);

    connect(_EngineViewModel,
            &RaptorSettingViewModel::itemEdited,
            this,
            &RaptorOfficePage::onEngineViewModelItemEdited);
}

void RaptorOfficePage::onDebounceTimerTimeout() const
{
    const auto items = _DebounceTimer->dynamicPropertyNames();
    for (auto &item: items)
    {
        const auto [qKey, qValue] = _DebounceTimer->property(item).value<QPair<QString, QVariant>>();
        RaptorSettingSuite::invokeItemSave(qKey, item, qValue);
    }
}

void RaptorOfficePage::onEngineViewModelItemEdited(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    const auto item = _Data.value<QModelIndex>().data(Qt::UserRole).value<RaptorSettingItem>();
    auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Office,
                                                       Setting::Office::Engine).value<QVector<
        RaptorSettingItem> >();
    for (auto &iten: qEngines)
    {
        if (iten._Name == item._Name)
        {
            iten._Path = item._Path;
            break;
        }
    }

    _DebounceTimer->setProperty(Setting::Office::Engine,
                                QVariant::fromValue<QPair<QString, QVariant> >(qMakePair(
                                    Setting::Section::Office,
                                    QVariant::fromValue<QVector<RaptorSettingItem> >(qEngines)))
    );

    _DebounceTimer->start();
    RaptorToast::invokeSuccessEject(
        QStringLiteral(R"(办公套件 %1 已更新!)").arg(QString(INFORMATION_TEMPLATE).arg(item._Name)));
}
