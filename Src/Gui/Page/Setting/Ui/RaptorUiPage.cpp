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

#include "RaptorUiPage.h"
#include "ui_RaptorUiPage.h"

RaptorUiPage::RaptorUiPage(QWidget *qParent) : QWidget(qParent),
                                               _Ui(new Ui::RaptorUiPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorUiPage::~RaptorUiPage()
{
    qFree(_Ui)
}

void RaptorUiPage::invokeInstanceInit()
{
    _ThemeGroup = new QButtonGroup(this);
    _ThemeGroup->addButton(_Ui->_ThemeAuto);
    _ThemeGroup->addButton(_Ui->_ThemeLight);
    _ThemeGroup->addButton(_Ui->_ThemeDark);
    _ThemeGroup->setExclusive(true);
    _Font = new RaptorFont(RaptorStoreSuite::invokeWorldGet());
    _DebounceTimer = new QTimer(this);
    _DebounceTimer->setSingleShot(true);
    _DebounceTimer->setInterval(1350);
}

void RaptorUiPage::invokeUiInit() const
{
    _Ui->_ThemeTip->setText(QStringLiteral("主题:"));
    const auto qTheme = RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                           Setting::Ui::Theme).toString();
    _Ui->_ThemeAuto->setChecked(qTheme == Setting::Ui::Auto);
    _Ui->_ThemeAuto->setText(QStringLiteral("自动"));
    _Ui->_ThemeLight->setChecked(qTheme == Setting::Ui::Light);
    _Ui->_ThemeLight->setText(QStringLiteral("亮色"));
    _Ui->_ThemeDark->setChecked(qTheme == Setting::Ui::Dark);
    _Ui->_ThemeDark->setText(QStringLiteral("暗色"));
    _Ui->_FontTip->setText(QStringLiteral("字体:"));
    _Ui->_Font->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                           Setting::Ui::Font).toString());

    // TODO: 目前没有服务器，暂时禁用
    _Ui->_NoticeTip->setVisible(false);
    _Ui->_NoticeTip->setEnabled(false);
    _Ui->_Notice->setVisible(false);
    _Ui->_Notice->setEnabled(false);
    // _Ui->_NoticeTip->setText(QStringLiteral("通知:"));
    // _Ui->_Notice->setText(QStringLiteral("允许来自 %1 仓库的新版本和公告通知").arg(APPLICATION_NAME));
    // _Ui->_Notice->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
    //                                                               Setting::Ui::Notice).toBool());
    _Ui->_AutoSignTip->setText(QStringLiteral("签到:"));
    _Ui->_AutoSign->setText(QStringLiteral("自动签到"));
    _Ui->_AutoSign->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                                  Setting::Ui::Sign).toBool());
    _Ui->_ContextMenuTip->setText(QStringLiteral("视图菜单:"));
    _Ui->_ContextMenu->setText(QStringLiteral("启用视图菜单"));
    _Ui->_ContextMenu->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                                     Setting::Ui::ContextMenu).toBool());
    _Ui->_TrayIconTip->setText(QStringLiteral("托盘图标:"));
    _Ui->_TrayIcon->setText(QStringLiteral("启用托盘图标"));
    _Ui->_TrayIcon->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                                  Setting::Ui::TrayIcon).toBool());
    _Ui->_MinimizeToTrayTip->setText(QStringLiteral("最小化到托盘:"));
    _Ui->_MinimizeToTray->setText(QStringLiteral("关闭时最小化到托盘而不是退出"));
    _Ui->_MinimizeToTray->setEnabled(_Ui->_TrayIcon->isChecked());
    _Ui->_MinimizeToTray->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                                        Setting::Ui::MinimizeToTray).toBool());
}

void RaptorUiPage::invokeSlotInit() const
{
    connect(_DebounceTimer,
            &QTimer::timeout,
            this,
            &RaptorUiPage::onDebounceTimerTimeout);

    connect(_Ui->_ThemeAuto,
            &QPushButton::clicked,
            this,
            &RaptorUiPage::onThemeAutoClicked);

    connect(_Ui->_ThemeLight,
            &QPushButton::clicked,
            this,
            &RaptorUiPage::onThemeLightClicked);

    connect(_Ui->_ThemeDark,
            &QPushButton::clicked,
            this,
            &RaptorUiPage::onThemeDarkClicked);

    connect(_Ui->_Font,
            &QPushButton::clicked,
            this,
            &RaptorUiPage::onFontClicked);

    connect(_Ui->_Notice,
            &QCheckBox::stateChanged,
            this,
            &RaptorUiPage::onNoticeStateChanged);

    connect(_Ui->_AutoSign,
            &QCheckBox::stateChanged,
            this,
            &RaptorUiPage::onAutoSignStateChanged);

    connect(_Ui->_ContextMenu,
            &QCheckBox::stateChanged,
            this,
            &RaptorUiPage::onContextMenuStateChanged);

    connect(_Ui->_TrayIcon,
            &QCheckBox::stateChanged,
            this,
            &RaptorUiPage::onTrayIconStateChanged);

    connect(_Ui->_MinimizeToTray,
            &QCheckBox::stateChanged,
            this,
            &RaptorUiPage::onMinimizeToTrayStateChanged);
}

void RaptorUiPage::onDebounceTimerTimeout() const
{
    const auto items = _DebounceTimer->dynamicPropertyNames();
    for (auto &item: items)
    {
        const auto [qKey, qValue] = _DebounceTimer->property(item).value<QPair<QString, QVariant> >();
        RaptorSettingSuite::invokeItemSave(qKey, item, qValue);
    }
}

void RaptorUiPage::onThemeAutoClicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Ui::Theme,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Ui,
                                              QVariant::fromValue<QString>(Setting::Ui::Auto))));
    _DebounceTimer->start();
}

void RaptorUiPage::onThemeLightClicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Ui::Theme,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Ui, QVariant::fromValue<QString>(Setting::Ui::Light))));
    _DebounceTimer->start();
}

void RaptorUiPage::onThemeDarkClicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Ui::Theme,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Ui, QVariant::fromValue<QString>(Setting::Ui::Dark))));
    _DebounceTimer->start();
}

void RaptorUiPage::onFontClicked() const
{
    const auto qFontName = _Font->invokeEject();
    if (qFontName.isEmpty())
    {
        return;
    }

    _Ui->_Font->setText(qFontName);
    _DebounceTimer->setProperty(Setting::Ui::Font,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Ui, QVariant::fromValue<QString>(qFontName))));
    _DebounceTimer->start();
}

void RaptorUiPage::onNoticeStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Ui::Notice,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Ui::Notice,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorUiPage::onAutoSignStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Ui::Sign,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Ui::Sign,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorUiPage::onContextMenuStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Ui::ContextMenu,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Ui::ContextMenu,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorUiPage::onTrayIconStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _Ui->_MinimizeToTray->setEnabled(true);
        _DebounceTimer->setProperty(Setting::Ui::TrayIcon,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _Ui->_MinimizeToTray->setEnabled(false);
        _DebounceTimer->setProperty(Setting::Ui::TrayIcon,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorUiPage::onMinimizeToTrayStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Ui::MinimizeToTray,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Ui::MinimizeToTray,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Ui, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}
