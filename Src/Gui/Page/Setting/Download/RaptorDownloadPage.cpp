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
    qFree(_Ui)
}

void RaptorDownloadPage::invokeInstanceInit()
{
    _PrimaryEngeineGroup = new QButtonGroup(this);
    _PrimaryEngeineGroup->addButton(_Ui->_Embed);
    _PrimaryEngeineGroup->addButton(_Ui->_Aria);
    _PrimaryEngeineGroup->setExclusive(true);
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

void RaptorDownloadPage::invokeUiInit()
{
    _Ui->_PrimaryEngineTip->setText(QStringLiteral("主引擎:"));
    _Ui->_Embed->setText(QStringLiteral("内置"));
    const auto qEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                            Setting::Download::PrimaryEngine).toString();
    _Ui->_Embed->setChecked(qEngine == Setting::Download::Embed);
    _Ui->_Aria->setText(QStringLiteral("Aria"));
    _Ui->_Aria->setChecked(qEngine == Setting::Download::Aria);
    _Ui->_AriaTip->setText(QStringLiteral("Aria:"));
    _Ui->_AriaHostTip->setText(QStringLiteral("主机:"));
    _Ui->_AriaHost->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                               Setting::Download::AriaHost).toString());
    _Ui->_AriaHost->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_AriaPortTip->setText(QStringLiteral("端口:"));
    _Ui->_AriaPort->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                               Setting::Download::AriaPort).toString());
    _Ui->_AriaPort->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$"), this));
    _Ui->_AriaPort->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_AriaTokenTip->setText(QStringLiteral("令牌:"));
    _Ui->_AriaToken->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                Setting::Download::AriaToken).toString());
    _Ui->_AriaToken->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_AriaRemotePathTip->setText(QStringLiteral("路径:"));
    _Ui->_AriaRemotePath->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                     Setting::Download::AriaRemotePath).toString());
    _Ui->_AriaRemotePath->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_AriaSSL->setText(QStringLiteral("启用 SSL"));
    _Ui->_AriaSSL->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                 Setting::Download::AriaSSL).toBool());
    _Ui->_AriaTest->setText(QStringLiteral("测试"));
    _Ui->_ThirdPartyEngineTip->setText(QStringLiteral("第三方引擎:"));
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
    _Ui->_ThirdPartyEngineView->setEditTriggers(QAbstractItemView::DoubleClicked);
    _Ui->_ThirdPartyEngineView->setSelectionMode(QAbstractItemView::SingleSelection);
    _Ui->_ThirdPartyEngineView->setSelectionBehavior(QAbstractItemView::SelectRows);
    const auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                             Setting::Download::ThirdPartyEngine).value<QVector<RaptorSettingItem> >();
    const auto qEnginf = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                            Setting::Download::ThirdPartyActiveEngine).toString();
    for (auto i = 0; i < qEngines.length(); ++i)
    {
        const auto item = qEngines[i];
        auto [_Icon, _Name, _Path] = qEngines[i];
        _ThirdPartyEngineViewModel->invokeItemAppend(item);
        if (qEnginf == _Name)
        {
            _Ui->_ThirdPartyEngineView->setCurrentIndex(_Ui->_ThirdPartyEngineView->model()->index(i, 0));
        }
    }

    _Ui->_PathTip->setText(QStringLiteral("路径:"));
    _Ui->_AutoRename->setText(QStringLiteral("自动重命名 (否则将删除已存在的文件)"));
    _Ui->_Path->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
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
    _Ui->_ConcurrentTip->setText(QStringLiteral("同时下载:"));
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

    connect(_Ui->_Embed,
            &QCheckBox::stateChanged,
            this,
            &RaptorDownloadPage::onEmbedStateChanged);

    connect(_Ui->_Aria,
            &QCheckBox::stateChanged,
            this,
            &RaptorDownloadPage::onAriaStateChanged);

    connect(_Ui->_AriaHost,
            &QLineEdit::textChanged,
            this,
            &RaptorDownloadPage::onAriaHostTextChanged);

    connect(_Ui->_AriaPort,
            &QLineEdit::textChanged,
            this,
            &RaptorDownloadPage::onAriaPortTextChanged);

    connect(_Ui->_AriaToken,
            &QLineEdit::textChanged,
            this,
            &RaptorDownloadPage::onAriaTokenTextChanged);

    connect(_Ui->_AriaRemotePath,
            &QLineEdit::textChanged,
            this,
            &RaptorDownloadPage::onAriaRemotePathTextChanged);

    connect(_Ui->_AriaSSL,
            &QCheckBox::stateChanged,
            this,
            &RaptorDownloadPage::onAriaSSLStateChanged);

    connect(_Ui->_AriaTest,
            &QPushButton::clicked,
            this,
            &RaptorDownloadPage::onAriaTestClicked);

    connect(_ThirdPartyEngineViewModel,
            &RaptorSettingViewModel::itemEdited,
            this,
            &RaptorDownloadPage::onThirdPartyEngineViewModelItemEdited);

    connect(_Ui->_ThirdPartyEngineView,
            &QTableView::doubleClicked,
            this,
            &RaptorDownloadPage::onThirdPartyEngineViewDoubleClicked);

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

void RaptorDownloadPage::onItemAriaConnectTested(const QVariant &qVariant) const
{
    _Ui->_AriaTest->setEnabled(true);
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("目标主机可连接!"));
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

void RaptorDownloadPage::onEmbedStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Download::PrimaryEngine,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<QString>(Setting::Download::Embed))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Download::PrimaryEngine,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<QString>(Setting::Download::Aria))));
    }

    _DebounceTimer->start();
}

void RaptorDownloadPage::onAriaStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Download::PrimaryEngine,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<QString>(Setting::Download::Aria))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Download::PrimaryEngine,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<QString>(Setting::Download::Embed))));
    }

    _DebounceTimer->start();
}

void RaptorDownloadPage::onAriaHostTextChanged(const QString &qValue) const
{
    _DebounceTimer->setProperty(Setting::Download::AriaHost,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Download, QVariant::fromValue<QString>(qValue))));


    _DebounceTimer->start();
}

void RaptorDownloadPage::onAriaPortTextChanged(const QString &qValue) const
{
    _DebounceTimer->setProperty(Setting::Download::AriaPort,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Download, QVariant::fromValue<quint32>(qValue.toUInt()))));


    _DebounceTimer->start();
}

void RaptorDownloadPage::onAriaTokenTextChanged(const QString &qValue) const
{
    _DebounceTimer->setProperty(Setting::Download::AriaToken,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Download, QVariant::fromValue<QString>(qValue))));


    _DebounceTimer->start();
}

void RaptorDownloadPage::onAriaRemotePathTextChanged(const QString &qValue) const
{
    _DebounceTimer->setProperty(Setting::Download::AriaRemotePath,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Download, QVariant::fromValue<QString>(qValue))));


    _DebounceTimer->start();
}

void RaptorDownloadPage::onAriaSSLStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Download::AriaSSL,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Download::AriaSSL,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Download, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorDownloadPage::onAriaTestClicked() const
{
    const auto qHost = _Ui->_AriaHost->text();
    if (qHost.isEmpty())
    {
        RaptorToast::invokeWarningEject("主机地址不能为空!");
        return;
    }


    const auto qPort = _Ui->_AriaPort->text();
    if (qPort.isEmpty())
    {
        RaptorToast::invokeWarningEject("端口不能为空!");
        return;
    }

    auto input = RaptorInput();
    input._State = _Ui->_AriaSSL->isChecked();
    input._Variant = QVariant::fromValue<QPair<QString, QString> >(qMakePair(qHost, qPort));
    _Ui->_AriaTest->setEnabled(false);
    Q_EMIT itemAriaConnectTesting(QVariant::fromValue<RaptorInput>(input));
}

void RaptorDownloadPage::onThirdPartyEngineViewModelItemEdited(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeWarningEject(_Message);
        return;
    }

    const auto item = _Data.value<QModelIndex>().data(Qt::UserRole).value<RaptorSettingItem>();
    auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                       Setting::Video::ThirdPartyEngine).value<QVector<RaptorSettingItem> >();
    for (auto &iten: qEngines)
    {
        if (iten._Name == item._Name)
        {
            iten._Path = item._Path;
            break;
        }
    }

    _DebounceTimer->setProperty(Setting::Download::ThirdPartyEngine, QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Download,
                                              QVariant::fromValue<QVector<RaptorSettingItem> >(qEngines))));
    _DebounceTimer->start();
    RaptorToast::invokeSuccessEject(
        QStringLiteral("引擎 %1 已更新!").arg(QString(qCreativeTemplate).arg(item._Name)));
}

void RaptorDownloadPage::onThirdPartyEngineViewDoubleClicked(const QModelIndex &qIndex) const
{
    if (qIndex.column() == 2)
    {
        return;
    }

    const auto item = qIndex.data(Qt::UserRole).value<RaptorSettingItem>();
    if (const auto qOperate = RaptorMessageBox::invokeInformationEject(QStringLiteral("切换第三方下载引擎"),
                                                                       QStringLiteral(R"(即将切换第三方下载引擎到 %1，是否继续?)").arg(
                                                                           QString(qInformationTemplate).arg(
                                                                               item._Name)));
        qOperate == RaptorMessageBox::No)
    {
        return;
    }

    _Ui->_ThirdPartyEngineView->setCurrentIndex(qIndex);
    _DebounceTimer->setProperty(Setting::Download::ThirdPartyActiveEngine,
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
