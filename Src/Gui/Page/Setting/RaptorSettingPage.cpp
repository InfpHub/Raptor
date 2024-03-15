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

#include "RaptorSettingPage.h"
#include "ui_RaptorSettingPage.h"

RaptorSettingPage::RaptorSettingPage(QWidget *qParent) : QWidget(qParent),
                                                         _Ui(new Ui::RaptorSettingPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorSettingPage::~RaptorSettingPage()
{
    FREE(_Ui)
}

RaptorNetworkPage *RaptorSettingPage::invokeNetworkPageGet() const
{
    return _Ui->_NetworkPage;
}

void RaptorSettingPage::invokeInstanceInit()
{
    _TabGroup = new QButtonGroup(this);
    _TabGroup->addButton(_Ui->_TabUi);
    _TabGroup->addButton(_Ui->_TabUpload);
    _TabGroup->addButton(_Ui->_TabDownload);
    _TabGroup->addButton(_Ui->_TabVideo);
#ifdef Q_OS_WIN
    _TabGroup->addButton(_Ui->_TabOffice);
#endif
    _TabGroup->addButton(_Ui->_TabNetwork);
    _TabGroup->addButton(_Ui->_TabOther);
    _TabGroup->setExclusive(true);
}

void RaptorSettingPage::invokeUiInit() const
{
    _Ui->_Title->setText(QStringLiteral("设置"));
    _Ui->_TabPrev->setIcon(QIcon(RaptorUtil::invokeIconMatch("Left", false, true)));
    _Ui->_TabPrev->setIconSize(QSize(14, 14));
    _Ui->_TabUi->setText(QStringLiteral("外观"));
    _Ui->_TabUi->setChecked(true);
    _Ui->_TabDownload->setText(QStringLiteral("下载"));
    _Ui->_TabUpload->setText(QStringLiteral("上传"));
    _Ui->_TabOffice->setText(QStringLiteral("办公"));
#ifndef Q_OS_WIN
    _Ui->_TabOffice->setVisible(false);
    _Ui->_TabOffice->setEnabled(false);
#endif
    _Ui->_TabVideo->setText(QStringLiteral("视频"));
    _Ui->_TabNetwork->setText(QStringLiteral("网络"));
    _Ui->_TabOther->setText(QStringLiteral("其他"));
    _Ui->_TabNext->setIcon(QIcon(RaptorUtil::invokeIconMatch("Right", false, true)));
    _Ui->_TabNext->setIconSize(QSize(14, 14));
    _Ui->_Body->setCurrentWidget(_Ui->_UiPage);
}

void RaptorSettingPage::invokeSlotInit() const
{
    connect(_Ui->_TabPrev,
            &QPushButton::clicked,
            this,
            &RaptorSettingPage::onTabPrevClicked);

    connect(_Ui->_TabUi,
            &QPushButton::toggled,
            this,
            &RaptorSettingPage::onTabUiToggled);

    connect(_Ui->_TabDownload,
            &QPushButton::toggled,
            this,
            &RaptorSettingPage::onTabDownloadToggled);

    connect(_Ui->_TabUpload,
            &QPushButton::toggled,
            this,
            &RaptorSettingPage::onTabUploadToggled);

    connect(_Ui->_TabVideo,
            &QPushButton::toggled,
            this,
            &RaptorSettingPage::onTabVideoToggled);

    connect(_Ui->_TabOffice,
            &QPushButton::toggled,
            this,
            &RaptorSettingPage::onTabOfficeToggled);

    connect(_Ui->_TabNetwork,
            &QPushButton::toggled,
            this,
            &RaptorSettingPage::onTabNetworkToggled);

    connect(_Ui->_TabOther,
            &QPushButton::toggled,
            this,
            &RaptorSettingPage::onTabOtherToggled);

    connect(_Ui->_TabNext,
            &QPushButton::clicked,
            this,
            &RaptorSettingPage::onTabNextClicked);
}

void RaptorSettingPage::onItemCopyWriterHaveFound(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto items = _Data.value<QVector<RaptorCopyWriter> >();
    for (auto &[_Page, _Content]: items)
    {
        if (_Page == metaObject()->className())
        {
            _Ui->_Description->setText(_Content);
            break;
        }
    }
}

void RaptorSettingPage::onTabPrevClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton *>();
    qPushButtonList.pop_front();
    qPushButtonList.pop_back();
    for (auto i = 0; i < qPushButtonList.length(); ++i)
    {
        if (qPushButtonList[i]->isChecked())
        {
            if (i == 0)
            {
                qPushButtonList[qPushButtonList.length() - 1]->setChecked(true);
            } else
            {
                qPushButtonList[i - 1]->setChecked(true);
            }

            break;
        }
    }
}

void RaptorSettingPage::onTabUiToggled(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_UiPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_UiPage);
}

void RaptorSettingPage::onTabDownloadToggled(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_DownloadPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_DownloadPage);
}

void RaptorSettingPage::onTabUploadToggled(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_UploadPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_UploadPage);
}

void RaptorSettingPage::onTabVideoToggled(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_VideoPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_VideoPage);
}

void RaptorSettingPage::onTabOfficeToggled(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_OfficePage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_OfficePage);
}

void RaptorSettingPage::onTabNetworkToggled(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_NetworkPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_NetworkPage);
}

void RaptorSettingPage::onTabOtherToggled(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_OtherPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_OtherPage);
}

void RaptorSettingPage::onTabNextClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton *>();
    qPushButtonList.pop_front();
    qPushButtonList.pop_back();
    if (qPushButtonList.length() < 2)
    {
        return;
    }

    for (auto i = 0; i < qPushButtonList.length(); ++i)
    {
        if (qPushButtonList[i]->isChecked())
        {
            if (i == qPushButtonList.length() - 1)
            {
                qPushButtonList[0]->setChecked(true);
            } else
            {
                qPushButtonList[i + 1]->setChecked(true);
            }

            break;
        }
    }
}
