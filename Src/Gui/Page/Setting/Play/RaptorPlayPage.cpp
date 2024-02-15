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

#include "RaptorPlayPage.h"
#include "ui_RaptorPlayPage.h"

RaptorPlayPage::RaptorPlayPage(QWidget* qParent) : QWidget(qParent),
                                                   _Ui(new Ui::RaptorPlayPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorPlayPage::~RaptorPlayPage()
{
    FREE(_Ui)
}

void RaptorPlayPage::invokeInstanceInit()
{
    _EngineViewDelegate = new RaptorTableViewDelegate(this);
    _EngineViewHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_EngineView);
    _EngineViewHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _EngineViewModel = new RaptorSettingViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称") << QStringLiteral("路径");
    _EngineViewModel->invokeHeaderSet(qHeader);
    _EngineViewModel->invokeColumnCountSet(3);

    _QualityGroup = new QButtonGroup(this);
    _QualityGroup->addButton(_Ui->_QualitySD);
    _QualityGroup->addButton(_Ui->_QualityHD);
    _QualityGroup->addButton(_Ui->_QualityFHD);
    _QualityGroup->addButton(_Ui->_QualityUHD);
    _QualityGroup->addButton(_Ui->_QualityOrigin);
    _QualityGroup->setExclusive(true);

    _StreamGroup = new QButtonGroup(this);
    _StreamGroup->addButton(_Ui->_StreamQuick);
    _StreamGroup->addButton(_Ui->_StreamLive);

    _DebounceTimer = new QTimer(this);
    _DebounceTimer->setSingleShot(true);
    _DebounceTimer->setInterval(1350);
}

void RaptorPlayPage::invokeUiInit() const
{
    _Ui->_EngineTip->setText(QStringLiteral("引擎:"));
    _Ui->_EngineView->setModel(_EngineViewModel);
    _Ui->_EngineView->setHorizontalHeader(_EngineViewHeader);
    _Ui->_EngineView->setItemDelegate(_EngineViewDelegate);
    _Ui->_EngineView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_EngineView->horizontalHeader()->setFixedHeight(26);
    _Ui->_EngineView->horizontalHeader()->setMinimumSectionSize(26);
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
    const auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Play,
                                                             Setting::Play::Engine).value<QVector<RaptorSettingItem>>();
    const auto qEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Play,
                                                            Setting::Play::ActiveEngine).toString();
    for (auto i = 0; i < qEngines.length(); ++i)
    {
        auto qEnginf = qEngines[i];
        auto [_Icon, _Name, _Path] = qEnginf;
        qEnginf._Icon = RaptorUtil::invokeIconMatch(_Name, false, true);
        _EngineViewModel->invokeItemAppend(qEnginf);
        if (qEngine == _Name)
        {
            _Ui->_EngineView->setCurrentIndex(_Ui->_EngineView->model()->index(i, 0));
        }
    }

    _Ui->_QualityTip->setText(QStringLiteral("画质"));
    const auto qQuality = RaptorSettingSuite::invokeItemFind(Setting::Section::Play,
                                                             Setting::Play::Quality).toString();
    _Ui->_QualitySD->setChecked(qQuality == Setting::Play::SD);
    _Ui->_QualitySD->setText(QStringLiteral("SD"));
    _Ui->_QualityHD->setChecked(qQuality == Setting::Play::HD);
    _Ui->_QualityHD->setText(QStringLiteral("HD"));
    _Ui->_QualityFHD->setChecked(qQuality == Setting::Play::FHD);
    _Ui->_QualityFHD->setText(QStringLiteral("FHD"));
    _Ui->_QualityUHD->setChecked(qQuality == Setting::Play::UHD);
    _Ui->_QualityUHD->setText(QStringLiteral("UHD"));
    _Ui->_QualityOrigin->setChecked(qQuality == Setting::Play::Origin);
    _Ui->_QualityOrigin->setText(QStringLiteral("原画"));

    _Ui->_StreamTip->setText(QStringLiteral("流"));
    const auto qStream = RaptorSettingSuite::invokeItemFind(Setting::Section::Play,
                                                            Setting::Play::Stream).toString();
    _Ui->_StreamQuick->setChecked(qStream == Setting::Play::Quick);
    _Ui->_StreamQuick->setText(QStringLiteral("快速"));
    _Ui->_StreamLive->setChecked(qStream == Setting::Play::Live);
    _Ui->_StreamLive->setText(QStringLiteral("实时"));
    _Ui->_UseOriginTip->setText(QStringLiteral("使用直链:"));
    _Ui->_UseOrigin->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Play,
                                                                   Setting::Play::UseOrigin).toBool());
    _Ui->_UseOrigin->setText(QStringLiteral("当指定画质或流类型不可用时尝试使用直链播放"));
}

void RaptorPlayPage::invokeSlotInit() const
{
    connect(_DebounceTimer,
            &QTimer::timeout,
            this,
            &RaptorPlayPage::onDebounceTimerTimeout);

    connect(_EngineViewModel,
            &RaptorSettingViewModel::itemEdited,
            this,
            &RaptorPlayPage::onEngineViewModelItemEdited);

    connect(_Ui->_QualitySD,
            &QPushButton::clicked,
            this,
            &RaptorPlayPage::onQualitySDClicked);

    connect(_Ui->_QualityHD,
            &QPushButton::clicked,
            this,
            &RaptorPlayPage::onQualityHDClicked);

    connect(_Ui->_QualityUHD,
            &QPushButton::clicked,
            this,
            &RaptorPlayPage::onQualityUHDClicked);

    connect(_Ui->_QualityFHD,
            &QPushButton::clicked,
            this,
            &RaptorPlayPage::onQualityFHDClicked);

    connect(_Ui->_QualityOrigin,
            &QPushButton::clicked,
            this,
            &RaptorPlayPage::onQualityOriginClicked);

    connect(_Ui->_EngineView,
            &QTableView::doubleClicked,
            this,
            &RaptorPlayPage::onEngineViewDoubleClicked);

    connect(_Ui->_StreamQuick,
            &QPushButton::clicked,
            this,
            &RaptorPlayPage::onStreamQuickClicked);

    connect(_Ui->_StreamLive,
            &QPushButton::clicked,
            this,
            &RaptorPlayPage::onStreamLiveClicked);

    connect(_Ui->_UseOrigin,
            &QCheckBox::stateChanged,
            this,
            &RaptorPlayPage::onUseOriginStateChanged);
}

void RaptorPlayPage::onDebounceTimerTimeout() const
{
    const auto items = _DebounceTimer->dynamicPropertyNames();
    for (auto& item : items)
    {
        const auto qPair = _DebounceTimer->property(item).value<QPair<QString, QVariant>>();
        RaptorSettingSuite::invokeItemSave(qPair.first, item, qPair.second);
    }
}

void RaptorPlayPage::onEngineViewModelItemEdited(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    const auto item = _Data.value<QModelIndex>().data(Qt::UserRole).value<RaptorSettingItem>();
    auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Play,
                                                       Setting::Play::Engine).value<QVector<RaptorSettingItem>>();
    for (auto& iten : qEngines)
    {
        if (iten._Name == item._Name)
        {
            iten._Path = item._Path;
            break;
        }
    }

    _DebounceTimer->setProperty(Setting::Play::Engine,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<QVector<RaptorSettingItem>>(qEngines)))
    );
    _DebounceTimer->start();
    RaptorToast::invokeSuccessEject(QStringLiteral(R"(播放引擎 %1 已更新!)").arg(QStringLiteral(INFORMATION_TEMPLATE).arg(item._Name)));
}

void RaptorPlayPage::onEngineViewDoubleClicked(const QModelIndex& qIndex) const
{
    if (qIndex.column() == 2)
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorSettingItem>();
    if (!RaptorMessageBox::invokeInformationEject(QStringLiteral("切换播放引擎"),
                                                  QStringLiteral(R"(即将切换播放引擎到 %1，是否继续？)").arg(QStringLiteral(INFORMATION_TEMPLATE).arg(item._Name))))
    {
        return;
    }

    _Ui->_EngineView->setCurrentIndex(qIndex);
    _DebounceTimer->setProperty(Setting::Play::ActiveEngine,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<QString>(item._Name))));
    _DebounceTimer->start();
}

void RaptorPlayPage::onQualitySDClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Play::Quality,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<QString>(Setting::Play::HD))));
    _DebounceTimer->start();
}

void RaptorPlayPage::onQualityHDClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Play::Quality,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<QString>(Setting::Play::SD))));
    _DebounceTimer->start();
}

void RaptorPlayPage::onQualityFHDClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Play::Quality,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<QString>(Setting::Play::FHD))));
    _DebounceTimer->start();
}

void RaptorPlayPage::onQualityUHDClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Play::Quality,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<QString>(Setting::Play::UHD))));
    _DebounceTimer->start();
}

void RaptorPlayPage::onQualityOriginClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Play::Quality,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<QString>(Setting::Play::Origin))));
    _DebounceTimer->start();
}

void RaptorPlayPage::onStreamQuickClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Play::Stream,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<QString>(Setting::Play::Quick))));
    _DebounceTimer->start();
}

void RaptorPlayPage::onStreamLiveClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Play::Stream,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<QString>(Setting::Play::Live))));
    _DebounceTimer->start();
}

void RaptorPlayPage::onUseOriginStateChanged(const int& qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Play::UseOrigin,
                                    QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<bool>(true))));
    }
    else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Play::UseOrigin,
                                    QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Play, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}
