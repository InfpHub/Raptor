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

#include "RaptorDownloadPage.h"
#include "ui_RaptorDownloadPage.h"

RaptorDownloadPage::RaptorDownloadPage(QWidget *qParent) : QWidget(qParent),
                                                           _Ui(new Ui::RaptorDownloadPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorDownloadPage::~RaptorDownloadPage()
{
    FREE(_Ui)
}

void RaptorDownloadPage::invokeInstanceInit()
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

void RaptorDownloadPage::invokeUiInit() const
{
    _Ui->_EngineTip->setText(QStringLiteral("第三方引擎:"));
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
    _Ui->_EngineView->setEditTriggers(QAbstractItemView::DoubleClicked);
    _Ui->_EngineView->setSelectionMode(QAbstractItemView::SingleSelection);
    _Ui->_EngineView->setSelectionBehavior(QAbstractItemView::SelectRows);
    const auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                             Setting::Download::Engine).value<QVector<
        RaptorSettingItem> >();
    const auto qEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                            Setting::Download::ActiveEngine).toString();
    for (auto i = 0; i < qEngines.length(); ++i)
    {
        const auto item = qEngines[i];
        auto [_Icon, _Name, _Path] = qEngines[i];
        _EngineViewModel->invokeItemAppend(item);
        if (qEngine == _Name)
        {
            _Ui->_EngineView->setCurrentIndex(_Ui->_EngineView->model()->index(i, 0));
        }
    }

    _Ui->_PathTip->setText(QStringLiteral("路径:"));
    _Ui->_AutoRename->setText(QStringLiteral("自动重命名 (否则将删除已存在的文件)"));
    _Ui->_Path->setText(
        RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                           Setting::Download::Path).toString());
    _Ui->_Path->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_PathSelect->setText(QStringLiteral("选择"));
    _Ui->_DefaultPath->setText(QStringLiteral("默认路径"));
    _Ui->_DefaultPath->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                     Setting::Download::DefaultPath).toBool());
    _Ui->_FullPath->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                  Setting::Download::FullPath).toBool());
    _Ui->_FullPath->setText(QStringLiteral("按照网盘路径保存"));
    _Ui->_SkipViolation->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                       Setting::Download::SkipViolation).toBool());
    _Ui->_SkipViolation->setText(QStringLiteral("跳过违规文件"));
    _Ui->_AutoRename->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                    Setting::Download::AutoRename).toBool());
    _Ui->_ConcurrentTip->setText(QStringLiteral("并发:"));
    _Ui->_ConcurrentSlider->setValue(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                        Setting::Download::Concurrent).toInt());
    _Ui->_Concurrent->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                 Setting::Download::Concurrent).toString());
}

void RaptorDownloadPage::invokeSlotInit() const
{
    connect(_DebounceTimer,
            &QTimer::timeout,
            this,
            &RaptorDownloadPage::onDebounceTimerTimeout);

    connect(_EngineViewModel,
            &RaptorSettingViewModel::itemEdited,
            this,
            &RaptorDownloadPage::onEngineViewModelItemEdited);

    connect(_Ui->_EngineView,
            &QTableView::doubleClicked,
            this,
            &RaptorDownloadPage::onEngineViewDoubleClicked);

    connect(_Ui->_PathSelect,
            &QPushButton::clicked,
            this,
            &RaptorDownloadPage::onPathSelectClicked);

    connect(_Ui->_DefaultPath,
            &QCheckBox::stateChanged,
            this,
            &RaptorDownloadPage::onDefaultPathStateChanged);

    connect(_Ui->_FullPath,
            &QCheckBox::stateChanged,
            this,
            &RaptorDownloadPage::onFullPathStateChanged);

    connect(_Ui->_SkipViolation,
            &QCheckBox::stateChanged,
            this,
            &RaptorDownloadPage::onSkipViolationStateChanged);

    connect(_Ui->_AutoRename,
            &QCheckBox::stateChanged,
            this,
            &RaptorDownloadPage::onAutoRenameStateChanged);

    connect(_Ui->_ConcurrentSlider,
            &QSlider::valueChanged,
            this,
            &RaptorDownloadPage::onConcurrentSliderValueChanged);
}

void RaptorDownloadPage::onDebounceTimerTimeout() const
{
    const auto items = _DebounceTimer->dynamicPropertyNames();
    for (auto &item: items)
    {
        const auto [qKey, qValue] = _DebounceTimer->property(item).value<QPair<QString, QVariant> >();
        RaptorSettingSuite::invokeItemSave(qKey, item, qValue);
    }
}

void RaptorDownloadPage::onEngineViewModelItemEdited(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    const auto item = _Data.value<QModelIndex>().data(Qt::UserRole).value<RaptorSettingItem>();
    auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                       Setting::Video::Engine).value<QVector<RaptorSettingItem> >();
    for (auto &iten: qEngines)
    {
        if (iten._Name == item._Name)
        {
            iten._Path = item._Path;
            break;
        }
    }

    _DebounceTimer->setProperty(Setting::Download::Engine, QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Download,
                                              QVariant::fromValue<QVector<RaptorSettingItem> >(qEngines))));
    _DebounceTimer->start();
    RaptorToast::invokeSuccessEject(
        QStringLiteral("引擎 %1 已更新!").arg(QString(CREATIVE_TEMPLATE).arg(item._Name)));
}

void RaptorDownloadPage::onEngineViewDoubleClicked(const QModelIndex &qIndex) const
{
    if (qIndex.column() == 2)
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorSettingItem>();
    if (const auto qOperate = RaptorMessageBox::invokeInformationEject(QStringLiteral("切换第三方下载引擎"),
                                                                       QStringLiteral(R"(即将切换第三方下载引擎到 %1，是否继续?)").arg(
                                                                           QString(INFORMATION_TEMPLATE).arg(
                                                                               item._Name)));
        qOperate == RaptorMessageBox::No)
    {
        return;
    }

    _Ui->_EngineView->setCurrentIndex(qIndex);
    _DebounceTimer->setProperty(Setting::Download::ActiveEngine,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Download, QVariant::fromValue<QString>(item._Name))));
    _DebounceTimer->start();
}

void RaptorDownloadPage::onPathSelectClicked() const
{
    const auto qPath = QFileDialog::getExistingDirectory(RaptorStoreSuite::invokeWorldGet(), QStringLiteral("选择默认下载路径"),
                                                         qApp->applicationDirPath());
    if (qPath.isEmpty())
    {
        return;
    }

    _Ui->_Path->setText(qPath);
    _DebounceTimer->setProperty(Setting::Download::Path,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Download, QVariant::fromValue<QString>(qPath))));
    _DebounceTimer->start();
}

void RaptorDownloadPage::onDefaultPathStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Download::DefaultPath,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Download::DefaultPath,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorDownloadPage::onFullPathStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Download::FullPath,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Download::FullPath,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorDownloadPage::onSkipViolationStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Download::SkipViolation,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Download::SkipViolation,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorDownloadPage::onAutoRenameStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Download::AutoRename,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Download::AutoRename,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorDownloadPage::onConcurrentSliderValueChanged(const int &qValue) const
{
    _Ui->_Concurrent->setText(QString::number(qValue));
    _DebounceTimer->setProperty(Setting::Download::Concurrent,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Download, QVariant::fromValue<int>(qValue))));

    _DebounceTimer->start();
}
