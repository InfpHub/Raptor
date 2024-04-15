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

#include "RaptorDownload.h"
#include "ui_RaptorDownload.h"

RaptorDownload::RaptorDownload(QWidget *qParent) : RaptorEject(qParent),
                                                   _Ui(new Ui::RaptorDownload)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorDownload::~RaptorDownload()
{
    qFree(_Ui)
}

bool RaptorDownload::eventFilter(QObject *qObject, QEvent *qEvent)
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

    if (qObject == _Ui->_Icon)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeIconDrawing();
            return true;
        }
    }

    return RaptorEject::eventFilter(qObject, qEvent);
}

void RaptorDownload::invokeEject(const QVariant &qVariant)
{
    _Variant = qVariant;
    if (const auto qIndexList = _Variant.value<QModelIndexList>();
        qIndexList.length() == 1)
    {
        const auto item = _Variant.value<QModelIndexList>()[0].data(Qt::UserRole).value<RaptorFileItem>();
        const auto qTitle = _Ui->_Title->fontMetrics().elidedText(QStringLiteral("下载 %1").arg(item._Name),
                                                                  Qt::ElideMiddle,
                                                                  _Ui->_Title->width());
        _Ui->_Title->setText(qTitle);
        if (item._Type == "file")
        {
            _Ui->_Name->setText(item._Name);
        }

        _Icon = item._Icon;
    } else if (qIndexList.length() > 1)
    {
        const auto item = _Variant.value<QModelIndexList>()[0].data(Qt::UserRole).value<RaptorFileItem>();
        const auto qTitle = _Ui->_Title->fontMetrics().elidedText(QStringLiteral("下载 %1 等 %2 个文件").arg(item._Name, QString::number(qIndexList.length())),
                                                                  Qt::ElideMiddle,
                                                                  _Ui->_Title->width());
        _Ui->_Title->setText(qTitle);
        _Ui->_Name->setEnabled(false);
    }

    if (RaptorStoreSuite::invokeUserGet()._VIP)
    {
        _Ui->_ParallelSlider->setRange(1, 32);
        _Ui->_ParallelSlider->setValue(1);
        _Ui->_ParallelSlider->setEnabled(false);
        _Ui->_Parallel->setEnabled(false);
    }

    if (const auto qEngine = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Download,
                                                                         Setting::Download::PrimaryEngine).toString(); qEngine == Setting::Download::Aria)
    {
        _Ui->_ParallelSlider->setRange(1, 16);
        _Ui->_ParallelSlider->setValue(16);
        _Ui->_Dir->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                              Setting::Download::AriaRemotePath).toString());
        const auto qAriaLocalHost = RaptorStoreSuite::invokeAriaIsLocalHostGet();
        _Ui->_DirSelect->setVisible(qAriaLocalHost);
        _Ui->_DirSelect->setEnabled(qAriaLocalHost);
    } else if (qEngine == Setting::Download::Embed)
    {
        _Ui->_ParallelSlider->setRange(1, 32);
        _Ui->_ParallelSlider->setValue(10);
        if (RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                               Setting::Download::DefaultPath).toBool())
        {
            _Ui->_Dir->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                  Setting::Download::Path).toString());
        }
    }

    _Ui->_Tip->clear();
    RaptorEject::invokeEject(qVariant);
}

void RaptorDownload::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _UnitGroup = new QButtonGroup(this);
    _UnitGroup->addButton(_Ui->_MB);
    _UnitGroup->addButton(_Ui->_KB);
    _UnitGroup->setExclusive(true);
}

void RaptorDownload::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_NameTip->setText(QStringLiteral("名称:"));
    _Ui->_Name->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_SpeedTip->setText(QStringLiteral("限速:"));
    _Ui->_Speed->setEnabled(!_Ui->_SpeedNoLimit->isChecked());
    _Ui->_Speed->setButtonSymbols(QAbstractSpinBox::NoButtons);
    _Ui->_MB->setText(QStringLiteral("kB/S"));
    _Ui->_KB->setText(QStringLiteral("MB/S"));
    _Ui->_KB->setChecked(true);
    _Ui->_SpeedNoLimit->setText(QStringLiteral("不限速"));
    _Ui->_ParallelTip->setText(QStringLiteral("并行:"));
    _Ui->_ParallelSlider->setRange(1, 32);
    _Ui->_Parallel->setText(QString::number(_Ui->_ParallelSlider->value()));
    _Ui->_DirTip->setText(QStringLiteral("目录"));
    _Ui->_Dir->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    _Ui->_Dir->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_DirSelect->setText(QStringLiteral("选择"));
    _Ui->_Icon->installEventFilter(this);
    if (const auto qEngine = RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Download,
                                                                         Setting::Download::PrimaryEngine).toString();
        qEngine == Setting::Download::Embed)
    {
        _Ui->_3rdPartyEvoke->setText(QStringLiteral("唤起 %1").arg(RaptorSettingSuite::invokeItemFind(Setting::Section::Download,
                                                                                                    Setting::Download::ThirdPartyActiveEngine).toString()));
    } else if (qEngine == Setting::Download::Aria)
    {
        _Ui->_3rdPartyEvoke->setVisible(false);
        _Ui->_3rdPartyEvoke->setEnabled(false);
    }

    _Ui->_Download->setText(QStringLiteral("下载"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
}

void RaptorDownload::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorDownload::onCloseClicked);

    connect(_Ui->_SpeedNoLimit,
            &QCheckBox::stateChanged,
            this,
            &RaptorDownload::onSpeedNoLimitStateChanged);

    connect(_Ui->_ParallelSlider,
            &QSlider::valueChanged,
            this,
            &RaptorDownload::onParallelSliderValueChanged);

    connect(_Ui->_DirSelect,
            &QPushButton::clicked,
            this,
            &RaptorDownload::onDirSelectClicked);

    connect(_Ui->_3rdPartyEvoke,
            &QPushButton::clicked,
            this,
            &RaptorDownload::on3rdPartyEvokeClicked);

    connect(_Ui->_Download,
            &QPushButton::clicked,
            this,
            &RaptorDownload::onDownloadClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorDownload::onCancelClicked);
}

void RaptorDownload::invokeIconDrawing() const
{
    if (_Icon.isNull())
    {
        return;
    }

    auto qPainter = QPainter(_Ui->_Icon);
    _SvgRender->load(_Icon);
    if (_SvgRender->isValid())
    {
        _SvgRender->render(&qPainter, QRect(0, 0, _Ui->_Icon->width(), _Ui->_Icon->height()));
    }
}

void RaptorDownload::onCloseClicked()
{
    close();
}

void RaptorDownload::onSpeedNoLimitStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _Ui->_Speed->setEnabled(false);
    } else if (qState == Qt::Unchecked)
    {
        _Ui->_Speed->setEnabled(true);
    }
}

void RaptorDownload::onParallelSliderValueChanged(const int &qValue) const
{
    if (qValue > 10)
    {
        _Ui->_Tip->setText(QString(qCriticalTemplate).arg(QStringLiteral("温馨提示：%1 并发可能会触发风控检测处罚!").arg(qValue)));
    } else
    {
        _Ui->_Tip->clear();
    }

    _Ui->_Parallel->setText(QString::number(qValue));
}

void RaptorDownload::onDirSelectClicked()
{
    if (const auto directory = QFileDialog::getExistingDirectory(this,
                                                                 QStringLiteral("选择保存目录"),
                                                                 QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
        !directory.isNull()
    )
    {
        _Ui->_Dir->setText(directory);
    }
}

void RaptorDownload::on3rdPartyEvokeClicked()
{
    const auto indexes = _Variant.value<QModelIndexList>();
    for (auto &index: indexes)
    {
        const auto item = index.data(Qt::UserRole).value<RaptorFileItem>();
        if (item._Type == "folder")
        {
            continue;
        }

        if (const auto qError = RaptorUtil::invoke3rdPartyDownloaderEvoke(item._Url, _Ui->_Dir->text(), item._Name);
            !qError.isEmpty())
        {
            RaptorToast::invokeWarningEject(qError);
        }
    }

    close();
}

void RaptorDownload::onDownloadClicked()
{
    if (_Ui->_Dir->text().isEmpty())
    {
        RaptorToast::invokeWarningEject(QStringLiteral("尚未选择保存位置，无法继续!"));
        return;
    }

    auto input = RaptorInput();
    input._Dir = _Ui->_Dir->text();
    input._Indexes = _Variant.value<QModelIndexList>();
    input._Speed = _Ui->_SpeedNoLimit->isChecked() ? (_Ui->_KB->isChecked() ? _Ui->_Speed->value() : _Ui->_Speed->value() / 1024) : 0;
    input._Parallel = _Ui->_ParallelSlider->value();
    Q_EMIT itemsDownloading(QVariant::fromValue<RaptorInput>(input));
    onCloseClicked();
}

void RaptorDownload::onCancelClicked()
{
    onCloseClicked();
}
