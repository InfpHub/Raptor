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

#include "RaptorStoryPage.h"
#include "ui_RaptorStoryPage.h"

RaptorStoryPage::RaptorStoryPage(QWidget* qParent) : QWidget(qParent),
                                                     _Ui(new Ui::RaptorStoryPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorStoryPage::~RaptorStoryPage()
{
    FREE(_Ui)
}

RaptorAboutPage* RaptorStoryPage::invokeAboutPageGet() const
{
    return _Ui->_AboutPage;
}

void RaptorStoryPage::invokeInstanceInit()
{
    _TabGroup = new QButtonGroup(this);
    _TabGroup->setExclusive(true);
    _TabGroup->addButton(_Ui->_TabAbout);
    _TabGroup->addButton(_Ui->_TabAboutQt);
    _TabGroup->addButton(_Ui->_TabFAQ);
}

void RaptorStoryPage::invokeUiInit() const
{
    _Ui->_Title->setText(QStringLiteral("故事"));
    _Ui->_TabPrev->setIcon(QIcon(RaptorUtil::invokeIconMatch("Left", false, true)));
    _Ui->_TabPrev->setIconSize(QSize(14, 14));
    _Ui->_TabAbout->setText(QStringLiteral("关于 %1").arg(APPLICATION_NAME));
    _Ui->_TabAboutQt->setText(QStringLiteral("关于 Qt"));
    _Ui->_TabFAQ->setText(QStringLiteral("FAQ"));
    _Ui->_TabNext->setIcon(QIcon(RaptorUtil::invokeIconMatch("Right", false, true)));
    _Ui->_TabNext->setIconSize(QSize(14, 14));
    _Ui->_GitHub->setText(QStringLiteral("GitHub"));
    _Ui->_Body->setCurrentWidget(_Ui->_AboutPage);
}

void RaptorStoryPage::invokeSlotInit() const
{
    connect(_Ui->_TabPrev,
            &QPushButton::clicked,
            this,
            &RaptorStoryPage::onTabPrevClicked);

    connect(_Ui->_TabAbout,
            &QPushButton::toggled,
            this,
            &RaptorStoryPage::onTabAboutToggled);

    connect(_Ui->_TabAboutQt,
            &QPushButton::toggled,
            this,
            &RaptorStoryPage::onTabAboutQtToggled);

    connect(_Ui->_TabFAQ,
            &QPushButton::toggled,
            this,
            &RaptorStoryPage::onTabFAQToggled);

    connect(_Ui->_TabNext,
            &QPushButton::clicked,
            this,
            &RaptorStoryPage::onTabNextClicked);

    connect(_Ui->_GitHub,
            &QPushButton::clicked,
            this,
            &RaptorStoryPage::onGitHubClicked);
}

void RaptorStoryPage::onItemCopyWriterHaveFound(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto items = _Data.value<QVector<RaptorCopyWriter>>();
    for (auto& [_Page, _Content] : items)
    {
        if (_Page == metaObject()->className())
        {
            _Ui->_Description->setText(_Content);
            break;
        }
    }
}

void RaptorStoryPage::onTabPrevClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton*>();
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
            if (i == 1)
            {
                qPushButtonList[qPushButtonList.length() - 1]->setChecked(true);
            }
            else
            {
                qPushButtonList[i - 1]->setChecked(true);
            }

            break;
        }
    }
}

void RaptorStoryPage::onTabAboutToggled() const
{
    if (_Ui->_Body->currentWidget() == _Ui->_AboutPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_AboutPage);
}

void RaptorStoryPage::onTabAboutQtToggled() const
{
    if (_Ui->_Body->currentWidget() == _Ui->_AboutQtPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_AboutQtPage);
}

void RaptorStoryPage::onTabFAQToggled() const
{
    if (_Ui->_Body->currentWidget() == _Ui->_FAQPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_FAQPage);
}

void RaptorStoryPage::onTabNextClicked() const
{
    auto qPushButtonList = _Ui->_TabPanel->findChildren<QPushButton*>();
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
            }
            else
            {
                qPushButtonList[i + 1]->setChecked(true);
            }

            break;
        }
    }
}

void RaptorStoryPage::onGitHubClicked() const
{
    QDesktopServices::openUrl(QUrl(GITHUB));
}