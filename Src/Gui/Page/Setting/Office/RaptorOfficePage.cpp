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
    qFree(_Ui)
}

void RaptorOfficePage::invokeInstanceInit()
{
    _ThirdPartyEngineViewDelegate = new RaptorTableViewDelegate(this);
    _ThirdPartyEngineViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_ThirdPartyEngineView);
    _ThirdPartyEngineViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _ThirdPartyEngineViewModel = new RaptorSettingViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("路径");
    _ThirdPartyEngineViewModel->invokeHeaderSet(qHeader);
    _ThirdPartyEngineViewModel->invokeColumnCountSet(3);

    _DebounceTimer = new QTimer(this);
    _DebounceTimer->setSingleShot(true);
    _DebounceTimer->setInterval(1350);
}

void RaptorOfficePage::invokeUiInit() const
{
    _Ui->_ThirdPartyEngineTip->setText(QStringLiteral("引擎:"));
    _Ui->_ThirdPartyEngineView->setModel(_ThirdPartyEngineViewModel);
    _Ui->_ThirdPartyEngineView->setHorizontalHeader(_ThirdPartyEngineViewHeader);
    _Ui->_ThirdPartyEngineView->setItemDelegate(_ThirdPartyEngineViewDelegate);
    _Ui->_ThirdPartyEngineView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ThirdPartyEngineView->horizontalHeader()->setFixedHeight(26);
    _Ui->_ThirdPartyEngineView->horizontalHeader()->setMinimumSectionSize(30);
    _Ui->_ThirdPartyEngineView->horizontalHeader()->setDefaultSectionSize(30);
    _Ui->_ThirdPartyEngineView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_ThirdPartyEngineView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    _Ui->_ThirdPartyEngineView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    _Ui->_ThirdPartyEngineView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_ThirdPartyEngineView->verticalHeader()->setHidden(true);
    _Ui->_ThirdPartyEngineView->setShowGrid(false);
    _Ui->_ThirdPartyEngineView->setColumnWidth(0, 30);
    _Ui->_ThirdPartyEngineView->setColumnWidth(1, 80);
    _Ui->_ThirdPartyEngineView->setSelectionMode(QAbstractItemView::SingleSelection);
    _Ui->_ThirdPartyEngineView->setSelectionBehavior(QAbstractItemView::SelectRows);
    const auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Office,
                                                             Setting::Office::ThirdPartyEngine).value<QVector<
        RaptorSettingItem> >();
    for (auto i = 0; i < qEngines.length(); ++i)
    {
        auto qEnginf = qEngines[i];
        auto [_Icon, _Name, _Path] = qEnginf;
        qEnginf._Icon = RaptorUtil::invokeIconMatch(_Name, false, true);
        _ThirdPartyEngineViewModel->invokeItemAppend(qEnginf);
    }
}

void RaptorOfficePage::invokeSlotInit() const
{
    connect(_DebounceTimer,
            &QTimer::timeout,
            this,
            &RaptorOfficePage::onDebounceTimerTimeout);

    connect(_ThirdPartyEngineViewModel,
            &RaptorSettingViewModel::itemEdited,
            this,
            &RaptorOfficePage::onThirdPartyEngineViewModelItemEdited);
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

void RaptorOfficePage::onThirdPartyEngineViewModelItemEdited(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    const auto item = _Data.value<QModelIndex>().data(Qt::UserRole).value<RaptorSettingItem>();
    auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Office,
                                                       Setting::Office::ThirdPartyEngine).value<QVector<
        RaptorSettingItem> >();
    for (auto &iten: qEngines)
    {
        if (iten._Name == item._Name)
        {
            iten._Path = item._Path;
            break;
        }
    }

    _DebounceTimer->setProperty(Setting::Office::ThirdPartyEngine,
                                QVariant::fromValue<QPair<QString, QVariant> >(qMakePair(
                                    Setting::Section::Office,
                                    QVariant::fromValue<QVector<RaptorSettingItem> >(qEngines)))
    );

    _DebounceTimer->start();
    RaptorToast::invokeSuccessEject(
        QStringLiteral(R"(办公套件 %1 已更新!)").arg(QString(qInformationTemplate).arg(item._Name)));
}
