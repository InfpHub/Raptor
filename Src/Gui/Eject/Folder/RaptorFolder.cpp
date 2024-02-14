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

#include "RaptorFolder.h"
#include "ui_RaptorFolder.h"

RaptorFolder::RaptorFolder(QWidget* qParent) : RaptorEject(qParent),
                                               _Ui(new Ui::RaptorFolder)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorFolder::~RaptorFolder()
{
    FREE(_Ui)
}

bool RaptorFolder::eventFilter(QObject* qObject, QEvent* qEvent)
{
    if (qObject == this)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent*>(qEvent);
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

void RaptorFolder::invokeEject(const QVariant& qVariant)
{
    _Variant = qVariant;
    _Ui->_Title->setText(QStringLiteral("在 %1 下新建文件夹").arg(QStringLiteral(CREATIVE_TEMPLATE).arg(_Variant.value<QPair<QString, QString>>().second)));
    const auto qNewFolderTemplate = RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                                       Setting::Other::NewFolderTemplate).toString();
    _Ui->_Name->setText(QDateTime::currentDateTime().toString(qNewFolderTemplate));
    RaptorEject::invokeEject(qVariant);
}

void RaptorFolder::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _RuleGroup = new QButtonGroup(this);
    _RuleGroup->setExclusive(true);
    _RuleGroup->addButton(_Ui->_Refuse);
    _RuleGroup->addButton(_Ui->_AutoRename);
    _RuleGroup->addButton(_Ui->_Ignore);
}

void RaptorFolder::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_NameTip->setText(QStringLiteral("名称:"));
    _Ui->_RuleTip->setText(QStringLiteral("规则:"));
    _Ui->_Refuse->setText(QStringLiteral("如果存在则不创建"));
    _Ui->_AutoRename->setText(QStringLiteral("自动重命名"));
    _Ui->_AutoRename->setChecked(true);
    _Ui->_Ignore->setText(QStringLiteral("忽略"));
    _Ui->_Icon->installEventFilter(this);
    _Ui->_Tip->setText(QStringLiteral(INFORMATION_TEMPLATE).arg(QStringLiteral("温馨提示: 至少 1 个字符。 至多 1024 字符。不可包含 <>:*?\\/\'\" 等字符。")));
    _Ui->_Create->setText(QStringLiteral("创建"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
}

void RaptorFolder::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorFolder::onCloseClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorFolder::onCancelClicked);

    connect(_Ui->_Create,
            &QPushButton::clicked,
            this,
            &RaptorFolder::onCreateClicked);
}

void RaptorFolder::invokeIconDrawing() const
{
    auto qPainter = QPainter(_Ui->_Icon);
    _SvgRender->load(RaptorUtil::invokeIconMatch("Folder", true));
    if (_SvgRender->isValid())
    {
        _SvgRender->render(&qPainter, QRect(0, 0, _Ui->_Icon->width(), _Ui->_Icon->height()));
    }
}

QString RaptorFolder::invokeRuleFilter() const
{
    if (_Ui->_Refuse->isChecked())
    {
        return "refuse";
    }

    if (_Ui->_AutoRename->isChecked())
    {
        return "auto_rename";
    }

    return "ignore";
}

void RaptorFolder::onCloseClicked()
{
    close();
}

void RaptorFolder::onCancelClicked()
{
    onCloseClicked();
}

void RaptorFolder::onCreateClicked()
{
    const auto qName = _Ui->_Name->toPlainText();
    if (qName.length() == 0)
    {
        RaptorToast::invokeWarningEject(QStringLiteral("至少 1 个字符!"));
        return;
    }

    if (qName.length() > 1024)
    {
        RaptorToast::invokeWarningEject(QStringLiteral("至多 1024 字符!"));
        return;
    }

    if (qName.endsWith('/', Qt::CaseInsensitive))
    {
        RaptorToast::invokeWarningEject(QStringLiteral("不能以 / 结尾!"));
        return;
    }

    auto input = RaptorInput();
    input._Parent = _Variant.value<QPair<QString, QString>>().first;
    input._Name = qName;
    input._Type = invokeRuleFilter();
    Q_EMIT itemCreating(QVariant::fromValue<RaptorInput>(input));
    close();
}
