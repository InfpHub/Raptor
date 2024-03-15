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

#include "RaptorShare.h"
#include "ui_RaptorShare.h"

RaptorShare::RaptorShare(QWidget *qParent) : RaptorEject(qParent),
                                             _Ui(new Ui::RaptorShare)
{
    _Ui->setupUi(this);
    invokeUiInit();
    invokeSlotInit();
}

RaptorShare::~RaptorShare()
{
    FREE(_Ui)
}

bool RaptorShare::eventFilter(QObject *qObject, QEvent *qEvent)
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

    if (qObject == _Ui->_AttributeIcon)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeIconDrawing(_Ui->_AttributeIcon);
            return true;
        }
    }

    if (qObject == _Ui->_TipIcon)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeIconDrawing(_Ui->_TipIcon, true);
            return true;
        }
    }

    if (qObject == _Ui->_LinkIcon)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeIconDrawing(_Ui->_LinkIcon);
            return true;
        }
    }

    return RaptorEject::eventFilter(qObject, qEvent);
}

void RaptorShare::invokeEject(const QVariant &qVariant)
{
    _Variant = qVariant;
    if (const auto qIndexList = _Variant.value<QModelIndexList>();
        qIndexList.length() == 1)
    {
        const auto item = qIndexList[0].data(Qt::UserRole).value<RaptorFileItem>();
        const auto qTitle = _Ui->_Title->fontMetrics().elidedText(QStringLiteral("分享 %1").arg(item._Name),
                                                                  Qt::ElideMiddle,
                                                                  _Ui->_Title->width());
        _Ui->_Title->setText(qTitle);
        _Ui->_Name->setText(item._Name);
        _Ui->_Description->setText(QStringLiteral("%1 的分享").arg(RaptorStoreSuite::invokeUserGet()._Nickname));
    } else if (qIndexList.length() > 1)
    {
        const auto item = qIndexList[0].data(Qt::UserRole).value<RaptorFileItem>();
        const auto qTitle = _Ui->_Title->fontMetrics().elidedText(QStringLiteral("分享 %1 等 %2 个文件").arg(item._Name, QString::number(qIndexList.length())),
                                                                  Qt::ElideMiddle,
                                                                  _Ui->_Title->width());
        _Ui->_Title->setText(qTitle);
    }

    if (const auto qRule = RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                              Setting::Other::Password).toString();
        qRule == Setting::Other::None)
    {
        _Ui->_Password->clear();
    } else if (qRule == Setting::Other::Random)
    {
        _Ui->_Password->setText(RaptorUtil::invokePasswordGenerate());
    } else if (qRule == Setting::Other::Custom)
    {
        _Ui->_Password->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                                   Setting::Other::CustomPassword).toString());
    }

    if (const auto qExpired = RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                                 Setting::Other::ShareExpire).toString();
        qExpired == Setting::Other::Week)
    {
        _Ui->_Expired->setMinimumDateTime(QDateTime::currentDateTime().addDays(7));
    } else if (qExpired == Setting::Other::Month)
    {
        _Ui->_Expired->setMinimumDateTime(QDateTime::currentDateTime().addMonths(1));
    } else if (qExpired == Setting::Other::Year)
    {
        _Ui->_Expired->setMinimumDateTime(QDateTime::currentDateTime().addYears(1));
    } else if (qExpired == Setting::Other::Never)
    {
        _Ui->_Expired->setEnabled(false);
        _Ui->_NeverExpire->setChecked(true);
    }

    _Ui->_Link->clear();
    _Ui->_Body->setCurrentWidget(_Ui->_AttributeWrapper);
    RaptorEject::invokeEject(qVariant);
}

void RaptorShare::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_NameTip->setText(QStringLiteral("名称:"));
    _Ui->_Name->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_DescriptionTip->setText(QStringLiteral("描述:"));
    _Ui->_Description->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_LimitTip->setText(QStringLiteral("限制:"));
    _Ui->_PreviewTip->setText(QStringLiteral("预览:"));
    _Ui->_Preview->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_Preview->setValidator(new QRegularExpressionValidator(QRegularExpression{"[0-9]+$"}, this));
    _Ui->_SaveTip->setText(QStringLiteral("转存:"));
    _Ui->_Save->setValidator(_Ui->_Preview->validator());
    _Ui->_Save->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_DownloadTip->setText(QStringLiteral("下载:"));
    _Ui->_Download->setValidator(_Ui->_Preview->validator());
    _Ui->_Download->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_PasswordTip->setText(QStringLiteral("提取码:"));
    _Ui->_Password->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_Generate->setText(QStringLiteral("生成"));
    _Ui->_ExpiredTip->setText(QStringLiteral("过期:"));
    _Ui->_Expired->setButtonSymbols(QAbstractSpinBox::NoButtons);
    _Ui->_Expired->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_NeverExpire->setText(QStringLiteral("永不过期"));
    _Ui->_AttributeIcon->installEventFilter(this);
    _Ui->_Create->setText(QStringLiteral("创建"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
    _Ui->_TipIcon->installEventFilter(this);
    _Ui->_Descriptioo->setText(QStringLiteral("已生成分享链接和提取码。您可以通过邮件等方式与朋友分享。"));
    _Ui->_LinkTip->setText(QStringLiteral("链接:"));
    _Ui->_Passwore->setText(QStringLiteral("提取码:"));
    _Ui->_LinkIcon->installEventFilter(this);
    _Ui->_Copy->setText(QStringLiteral("复制"));
}

void RaptorShare::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorShare::onCloseClicked);

    connect(_Ui->_Generate,
            &QPushButton::clicked,
            this,
            &RaptorShare::onGenerateClicked);

    connect(_Ui->_NeverExpire,
            &QCheckBox::stateChanged,
            this,
            &RaptorShare::onNeverExpireStateChanged);

    connect(_Ui->_Create,
            &QPushButton::clicked,
            this,
            &RaptorShare::onCreateClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorShare::onCancelClicked);

    connect(_Ui->_Copy,
            &QPushButton::clicked,
            this,
            &RaptorShare::onCopyClicked);
}

void RaptorShare::invokeIconDrawing(QWidget *qWidget, const bool &qTip) const
{
    auto qPainter = QPainter(qWidget);
    if (qTip)
    {
        _SvgRender->load(RaptorUtil::invokeIconMatch("Success", false, true));
    } else
    {
        _SvgRender->load(RaptorUtil::invokeIconMatch("Share", false, true));
    }

    if (_SvgRender->isValid())
    {
        _SvgRender->render(&qPainter, QRect(0, 0, qWidget->width(), qWidget->height()));
    }
}

void RaptorShare::onItemCreated(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto input = _Data.value<RaptorInput>();
    _Ui->_Link->setText(input._Url);
    _Ui->_Passwore->setText(input._Password);
    _Ui->_Body->setCurrentWidget(_Ui->_LinkWrapper);
}

void RaptorShare::onCloseClicked()
{
    close();
}

void RaptorShare::onGenerateClicked() const
{
    _Ui->_Password->setText(RaptorUtil::invokePasswordGenerate());
}

void RaptorShare::onNeverExpireStateChanged(const int &qChecked) const
{
    if (qChecked == Qt::Checked)
    {
        _Ui->_Expired->setEnabled(false);
    } else if (qChecked == Qt::Unchecked)
    {
        _Ui->_Expired->setEnabled(true);
    }
}

void RaptorShare::onCreateClicked()
{
    auto input = RaptorInput();
    input._Name = _Ui->_Name->text();
    input._Description = _Ui->_Description->toPlainText();
    input._Preview = _Ui->_Preview->text().toUInt();
    input._Save = _Ui->_Save->text().toUInt();
    input._Download = _Ui->_Download->text().toUInt();
    input._Password = _Ui->_Password->text();
    input._Expire = _Ui->_NeverExpire->isChecked() ? "" : _Ui->_Expired->text();
    input._Indexes = _Variant.value<QModelIndexList>();
    Q_EMIT itemCreating(QVariant::fromValue<RaptorInput>(input));
}

void RaptorShare::onCancelClicked()
{
    onCloseClicked();
}

void RaptorShare::onCopyClicked()
{
    qApp->clipboard()->setText(QString(RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                                          Setting::Other::LinkTemplate).toString()).arg(_Ui->_Link->toPlainText(),
                                                                                                                        _Ui->_Passwore->text()));
    close();
    RaptorToast::invokeSuccessEject(QStringLiteral("分享链接及提取码已经拷贝至剪切板!"));
}
