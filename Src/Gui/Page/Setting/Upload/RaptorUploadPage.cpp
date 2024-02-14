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

#include "RaptorUploadPage.h"
#include "ui_RaptorUploadPage.h"

RaptorUploadPage::RaptorUploadPage(QWidget* qParent) : QWidget(qParent),
                                                       _Ui(new Ui::RaptorUploadPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorUploadPage::~RaptorUploadPage()
{
    FREE(_Ui)
}

void RaptorUploadPage::invokeInstanceInit()
{
    _FilterDelegate = new RaptorTableViewDelegate(this);
    _FilterHeader = new RaptorTableViewHeader(Qt::Horizontal, _Ui->_FilterView);
    _FilterHeader->invokeIconSet(RaptorUtil::invokeIconMatch("Legend", false, true));
    _FilterModel = new RaptorSettingViewModel(this);
    auto qHeader = QVector<QString>();
    qHeader << QStringLiteral("名称");
    _FilterModel->invokeHeaderSet(qHeader);
    _FilterModel->invokeColumnCountSet(2);
    _DebounceTimer = new QTimer(this);
    _DebounceTimer->setSingleShot(true);
    _DebounceTimer->setInterval(1350);
}

void RaptorUploadPage::invokeUiInit() const
{
    _Ui->_FilterTip->setText(QStringLiteral("过滤:"));
    _Ui->_FilterView->setModel(_FilterModel);
    _Ui->_FilterView->setHorizontalHeader(_FilterHeader);
    _Ui->_FilterView->setItemDelegate(_FilterDelegate);
    _Ui->_FilterView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_FilterView->horizontalHeader()->setFixedHeight(26);
    _Ui->_FilterView->horizontalHeader()->setMinimumSectionSize(26);
    _Ui->_FilterView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    _Ui->_FilterView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _Ui->_FilterView->verticalHeader()->setDefaultSectionSize(26);
    _Ui->_FilterView->verticalHeader()->setHidden(true);
    _Ui->_FilterView->setShowGrid(false);
    _Ui->_FilterView->setColumnWidth(0, 30);
    _Ui->_FilterView->setSelectionBehavior(QAbstractItemView::SelectRows);
    const auto qEngines = RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                             Setting::Upload::Filter).value<QVector<QString>>();
    for (auto& qEngine : qEngines)
    {
        auto item = RaptorSettingItem();
        item._Name = qEngine;
        _FilterModel->invokeItemAppend(item);
    }

    _Ui->_FilterNameTip->setText(QStringLiteral("名称:"));
    _Ui->_FilterName->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_FilterAdd->setText(QStringLiteral("添加"));
    _Ui->_FilterRemove->setText(QStringLiteral("移除"));
    _Ui->_FilterClear->setText(QStringLiteral("清空"));
    _Ui->_FilterMessageTip->setText(QStringLiteral("消息:"));
    _Ui->_SkipRapidTip->setText(QStringLiteral("跳过秒传"));
    _Ui->_SkipNoRapid->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                                     Setting::Upload::SkipNoRapid).toBool());
    _Ui->_SkipNoRapid->setText(QStringLiteral("跳过无法秒传的文件"));
    _Ui->_ConcurrentTip->setText(QStringLiteral("并发"));
    _Ui->_ConcurrentSlider->setValue(RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                                        Setting::Upload::Concurrent).toInt());
    _Ui->_Concurrent->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                                 Setting::Upload::Concurrent).toString());
}

void RaptorUploadPage::invokeSlotInit() const
{
    connect(_Ui->_FilterAdd,
            &QPushButton::clicked,
            this,
            &RaptorUploadPage::onFilterAddClicked);

    connect(_Ui->_FilterRemove,
            &QPushButton::clicked,
            this,
            &RaptorUploadPage::onFilterRemoveClicked);

    connect(_Ui->_FilterClear,
            &QPushButton::clicked,
            this,
            &RaptorUploadPage::onFilterClearClicked);

    connect(_Ui->_SkipNoRapid,
            &QCheckBox::stateChanged,
            this,
            &RaptorUploadPage::onSkipNoRapidStateChanged);

    connect(_Ui->_ConcurrentSlider,
            &QSlider::valueChanged,
            this,
            &RaptorUploadPage::onConcurrentSliderValueChanged);
}


void RaptorUploadPage::onFilterAddClicked() const
{
    const auto qText = _Ui->_FilterName->text();
    if (qText.isEmpty())
    {
        return;
    }
    for (auto i = 0; i < _FilterModel->rowCount(); ++i)
    {
        if (const auto iten = _FilterModel->index(i, 0).data(Qt::UserRole).value<RaptorSettingItem>();
            qText == iten._Name)
        {
            RaptorToast::invokeWarningEject(QStringLiteral("过滤规则 %1 已存在!").arg(qText));
            return;
        }
    }

    auto item = RaptorSettingItem();
    item._Name = qText;
    _FilterModel->invokeItemAppend(item);
    auto items = RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                    Setting::Upload::Filter).value<QVector<QString>>();
    items << qText;
    _DebounceTimer->setProperty(Setting::Upload::Filter,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Upload, QVariant::fromValue<QVector<QString>>(items))));
    _DebounceTimer->start();
}

void RaptorUploadPage::onFilterRemoveClicked() const
{
    const auto qIndexList = _Ui->_FilterView->selectionModel()->selectedRows();
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择任何过滤规则，无法继续!"));
        return;
    }

    const auto qIndex = qIndexList[0];
    const auto item = qIndex.data(Qt::UserRole).value<RaptorSettingItem>();
    if (!RaptorMessageBox::invokeWarningEject(QStringLiteral("移除过滤规则"),
                                              QStringLiteral(R"("即将移除过滤规则 <span style="color: #00A4FF;">%1</span>，是否继续?")").arg(item._Name)))
    {
        return;
    }
    _FilterModel->removeRow(qIndex.row());
    _Ui->_FilterView->viewport()->update();
    auto items = RaptorSettingSuite::invokeItemFind(Setting::Section::Upload,
                                                    Setting::Upload::Filter).value<QVector<QString>>();
    items.removeOne(item._Name);
    _DebounceTimer->setProperty(Setting::Upload::Filter,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Upload, QVariant::fromValue<QVector<QString>>(items))));
    _DebounceTimer->start();
}

void RaptorUploadPage::onFilterClearClicked() const
{
    if (!RaptorMessageBox::invokeWarningEject(QStringLiteral("清空过滤规则"),
                                              QStringLiteral("即将清空所有上传过滤规则，是否继续?")))
    {
        return;
    }

    _FilterModel->invokeItemsClear();
    _Ui->_FilterView->viewport()->update();
    _DebounceTimer->setProperty(Setting::Upload::Filter,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Upload, QVariant::fromValue<QVector<QString>>(QVector<QString>{}))));
    _DebounceTimer->start();
}

void RaptorUploadPage::onSkipNoRapidStateChanged(const int& qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Upload::SkipNoRapid,
                                    QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Upload, QVariant::fromValue<bool>(true))));
    }
    else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Upload::SkipNoRapid,
                                    QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Upload, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorUploadPage::onConcurrentSliderValueChanged(const int& qValue) const
{
    _Ui->_Concurrent->setText(QString::number(qValue));
    _DebounceTimer->setProperty(Setting::Upload::Concurrent,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Upload, QVariant::fromValue<int>(qValue))));

    _DebounceTimer->start();
}
