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

#include "RaptorOtherPage.h"
#include "ui_RaptorOtherPage.h"

RaptorOtherPage::RaptorOtherPage(QWidget* qParent) : QWidget(qParent),
                                                     _Ui(new Ui::RaptorOtherPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorOtherPage::~RaptorOtherPage()
{
    qFree(_Ui)
}

void RaptorOtherPage::invokeInstanceInit()
{
    _ExpireGroup = new QButtonGroup(this);
    _ExpireGroup->addButton(_Ui->_Week);
    _ExpireGroup->addButton(_Ui->_Month);
    _ExpireGroup->addButton(_Ui->_Year);
    _ExpireGroup->addButton(_Ui->_Never);
    _ExpireGroup->setExclusive(true);

    _PasswordGroup = new QButtonGroup(this);
    _PasswordGroup->addButton(_Ui->_Random);
    _PasswordGroup->addButton(_Ui->_None);
    _PasswordGroup->addButton(_Ui->_Custom);
    _PasswordGroup->setExclusive(true);
    _DebounceTimer = new QTimer(this);
    _DebounceTimer->setSingleShot(true);
    _DebounceTimer->setInterval(1350);
}

void RaptorOtherPage::invokeUiInit() const
{
    _Ui->_NewFolderTemplateTip->setText(QStringLiteral("新建文件夹模板:"));
    _Ui->_NewFolderTemplate->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                                        Setting::Other::NewFolderTemplate).toString());
    _Ui->_NewFolderTemplate->setContextMenuPolicy(Qt::NoContextMenu);

    _Ui->_ExpireTip->setText(QStringLiteral("过期时间:"));
    const auto qExpire = RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                            Setting::Other::ShareExpire).toString();
    _Ui->_Week->setChecked(qExpire == Setting::Other::Week);
    _Ui->_Week->setText(QStringLiteral("7 天"));
    _Ui->_Month->setChecked(qExpire == Setting::Other::Month);
    _Ui->_Month->setText(QStringLiteral("1 个月"));
    _Ui->_Year->setChecked(qExpire == Setting::Other::Year);
    _Ui->_Year->setText(QStringLiteral("1 年"));
    _Ui->_Never->setChecked(qExpire == Setting::Other::Never);
    _Ui->_Never->setText(QStringLiteral("永不过期"));

    _Ui->_PasswordTip->setText(QStringLiteral("密码:"));
    const auto qPassword = RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                              Setting::Other::Password).toString();
    _Ui->_Random->setChecked(qPassword == Setting::Other::Random);
    _Ui->_Random->setText(QStringLiteral("随机"));
    _Ui->_None->setChecked(qPassword == Setting::Other::None);
    _Ui->_None->setText(QStringLiteral("无"));
    _Ui->_Custom->setChecked(qPassword == Setting::Other::Custom);
    _Ui->_Custom->setText(QStringLiteral("自定义"));
    _Ui->_Password->setEnabled(qPassword == Setting::Other::Custom);
    _Ui->_Password->setText(qPassword == Setting::Other::Custom
                                ? RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                                     Setting::Other::CustomPassword).
                                toString()
                                : "");
    _Ui->_Password->setContextMenuPolicy(Qt::NoContextMenu);

    _Ui->_LinkTemplateTip->setText(QStringLiteral("分享链接模板:"));
    _Ui->_LinkTemplate->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Other,
                                                                   Setting::Other::LinkTemplate).toString());
    _Ui->_LinkTemplate->setContextMenuPolicy(Qt::NoContextMenu);
}

void RaptorOtherPage::invokeSlotInit() const
{
    connect(_DebounceTimer,
            &QTimer::timeout,
            this,
            &RaptorOtherPage::onDebounceTimerTimeout);

    connect(_Ui->_NewFolderTemplate,
            &QLineEdit::textChanged,
            this,
            &RaptorOtherPage::onNewFolderTemplateTextChanged);

    connect(_Ui->_Week,
            &QPushButton::clicked,
            this,
            &RaptorOtherPage::onWeekClicked);

    connect(_Ui->_Month,
            &QPushButton::clicked,
            this,
            &RaptorOtherPage::onMonthClicked);

    connect(_Ui->_Year,
            &QPushButton::clicked,
            this,
            &RaptorOtherPage::onYearClicked);

    connect(_Ui->_Never,
            &QPushButton::clicked,
            this,
            &RaptorOtherPage::onNeverClicked);

    connect(_Ui->_Random,
            &QPushButton::clicked,
            this,
            &RaptorOtherPage::onRandomClicked);

    connect(_Ui->_None,
            &QPushButton::clicked,
            this,
            &RaptorOtherPage::onNoneClicked);

    connect(_Ui->_Custom,
            &QPushButton::clicked,
            this,
            &RaptorOtherPage::onCustomClicked);

    connect(_Ui->_Password,
            &QLineEdit::textChanged,
            this,
            &RaptorOtherPage::onPasswordTextChanged);

    connect(_Ui->_LinkTemplate,
            &QLineEdit::textChanged,
            this,
            &RaptorOtherPage::onLinkTemplateTextChanged);
}

void RaptorOtherPage::onDebounceTimerTimeout() const
{
    const auto items = _DebounceTimer->dynamicPropertyNames();
    for (auto& item : items)
    {
        const auto [qKey, qValue] = _DebounceTimer->property(item).value<QPair<QString, QVariant>>();
        RaptorSettingSuite::invokeItemSave(qKey, item, qValue);
    }
}

void RaptorOtherPage::onNewFolderTemplateTextChanged(const QString& qValue) const
{
    _DebounceTimer->setProperty(Setting::Other::NewFolderTemplate,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(qValue))));
    _DebounceTimer->start();
}

void RaptorOtherPage::onWeekClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Other::ShareExpire,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(Setting::Other::Week))));
    _DebounceTimer->start();
}

void RaptorOtherPage::onMonthClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Other::ShareExpire,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(Setting::Other::Month))));
    _DebounceTimer->start();
}

void RaptorOtherPage::onYearClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Other::ShareExpire,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(Setting::Other::Year))));
    _DebounceTimer->start();
}

void RaptorOtherPage::onNeverClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Other::ShareExpire,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(Setting::Other::Never))));
    _DebounceTimer->start();
}

void RaptorOtherPage::onRandomClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _Ui->_Password->setEnabled(false);
    _DebounceTimer->setProperty(Setting::Other::Password,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(Setting::Other::Random))));
    _DebounceTimer->start();
}

void RaptorOtherPage::onNoneClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _Ui->_Password->setEnabled(false);
    _DebounceTimer->setProperty(Setting::Other::Password,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(Setting::Other::None))));
    _DebounceTimer->start();
}

void RaptorOtherPage::onCustomClicked(const bool& qChecked) const
{
    if (!qChecked)
    {
        _Ui->_Password->setEnabled(false);
        return;
    }

    _Ui->_Password->setEnabled(true);
    _DebounceTimer->setProperty(Setting::Other::Password,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(Setting::Other::Custom))));
}

void RaptorOtherPage::onPasswordTextChanged(const QString& qValue) const
{
    _DebounceTimer->setProperty(Setting::Other::CustomPassword,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(qValue))));
    _DebounceTimer->start();
}

void RaptorOtherPage::onLinkTemplateTextChanged(const QString& qValue) const
{
    _DebounceTimer->setProperty(Setting::Other::LinkTemplate,
                                QVariant::fromValue<QPair<QString, QVariant>>(qMakePair(Setting::Section::Other, QVariant::fromValue<QString>(qValue))));
    _DebounceTimer->start();
}