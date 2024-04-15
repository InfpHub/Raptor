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

#include "RaptorTransferPage.h"
#include "ui_RaptorTransferPage.h"

RaptorTransferPage::RaptorTransferPage(QWidget* qParent) : QWidget(qParent),
                                                           _Ui(new Ui::RaptorTransferPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorTransferPage::~RaptorTransferPage()
{
    qFree(_Ui)
}

RaptorDownloadingPage* RaptorTransferPage::invokeDownloadingPageGet() const
{
    return _Ui->_DownloadingPage;
}

RaptorDownloadedPage* RaptorTransferPage::invokeDownloadedPageGet() const
{
    return _Ui->_DownloadedPage;
}

RaptorUploadingPage* RaptorTransferPage::invokeUploadingPageGet() const
{
    return _Ui->_UploadingPage;
}

RaptorUploadedPage* RaptorTransferPage::invokeUploadedPageGet() const
{
    return _Ui->_UploadedPage;
}

void RaptorTransferPage::onItemCopyWriterHaveFound(const QVariant& qVariant) const
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

void RaptorTransferPage::invokeInstanceInit()
{
    _TabGroup = new QButtonGroup(this);
    _TabGroup->setExclusive(true);
    _TabGroup->addButton(_Ui->_TabDownloading);
    _TabGroup->addButton(_Ui->_TabDownloaded);
    _TabGroup->addButton(_Ui->_TabUploading);
    _TabGroup->addButton(_Ui->_TabUploaded);
}

void RaptorTransferPage::invokeUiInit() const
{
    _Ui->_Title->setText(QStringLiteral("传输"));
    _Ui->_TabPrev->setIcon(QIcon(RaptorUtil::invokeIconMatch("Left", false, true)));
    _Ui->_TabPrev->setIconSize(QSize(14, 14));
    _Ui->_TabDownloading->setText(QStringLiteral("下载中"));
    _Ui->_TabDownloaded->setText(QStringLiteral("已下载"));
    _Ui->_TabUploading->setText(QStringLiteral("上传中"));
    _Ui->_TabUploaded->setText(QStringLiteral("已上传"));
    _Ui->_TabNext->setIcon(QIcon(RaptorUtil::invokeIconMatch("Right", false, true)));
    _Ui->_TabNext->setIconSize(QSize(14, 14));
    _Ui->_Body->setCurrentWidget(_Ui->_DownloadingPage);
    _Ui->_Resume->setText(QStringLiteral("继续"));
    _Ui->_Pause->setText(QStringLiteral("暂停"));
    _Ui->_Locate->setVisible(false);
    _Ui->_Locate->setEnabled(false);
    _Ui->_Locate->setText(QStringLiteral("定位"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
    _Ui->_Cancel->setVisible(true);
    _Ui->_Cancel->setEnabled(true);
    _Ui->_Delete->setText(QStringLiteral("删除"));
    _Ui->_Delete->setVisible(false);
    _Ui->_Delete->setEnabled(false);
    _Ui->_Clear->setText(QStringLiteral("清空"));
    _Ui->_Clear->setVisible(false);
    _Ui->_Clear->setEnabled(false);
}

void RaptorTransferPage::invokeSlotInit() const
{
    connect(_Ui->_TabPrev,
            &QPushButton::clicked,
            this,
            &RaptorTransferPage::onTabPrevClicked);

    connect(_Ui->_TabDownloading,
            &QPushButton::toggled,
            this,
            &RaptorTransferPage::onTabDownloadingToggled);

    connect(_Ui->_TabDownloaded,
            &QPushButton::toggled,
            this,
            &RaptorTransferPage::onTabDownloadedToggled);

    connect(_Ui->_TabUploading,
            &QPushButton::toggled,
            this,
            &RaptorTransferPage::onTabUploadingToggled);

    connect(_Ui->_TabUploaded,
            &QPushButton::toggled,
            this,
            &RaptorTransferPage::onTabUploadedToggled);

    connect(_Ui->_TabNext,
            &QPushButton::clicked,
            this,
            &RaptorTransferPage::onTabNextClicked);

    connect(_Ui->_Resume,
            &QPushButton::clicked,
            this,
            &RaptorTransferPage::onResumeClicked);

    connect(_Ui->_Pause,
            &QPushButton::clicked,
            this,
            &RaptorTransferPage::onPauseClicked);

    connect(_Ui->_Locate,
            &QPushButton::clicked,
            this,
            &RaptorTransferPage::onLocateClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorTransferPage::onCancelClicked);

    connect(_Ui->_Clear,
            &QPushButton::clicked,
            this,
            &RaptorTransferPage::onClearClicked);

    connect(_Ui->_Delete,
            &QPushButton::clicked,
            this,
            &RaptorTransferPage::onDeleteClicked);

    connect(_Ui->_Body,
            &QStackedWidget::currentChanged,
            this,
            &RaptorTransferPage::onBodyChanged);
}

void RaptorTransferPage::invokeItemsLocate(const QModelIndexList& qIndexList)
{
    if (qIndexList.isEmpty())
    {
        RaptorToast::invokeInformationEject(QStringLiteral("尚未选择任何文件，无法继续!"));
        return;
    }

    if (RaptorSettingSuite::invokeImmutableItemFind(Setting::Section::Download,
        Setting::Download::PrimaryEngine).toString() == Setting::Download::Aria)
    {
        if (!RaptorStoreSuite::invokeAriaIsLocalHostGet())
        {
            RaptorToast::invokeWarningEject(QStringLiteral("无法定位下载到远程计算机的文件。"));
            return;
        }
    }

    if (qIndexList.length() == 1)
    {
        const auto item = qIndexList[0].data(Qt::UserRole).value<RaptorTransferItem>();
        auto qPath = QString();
        if (item._Path.length() == 3)
        {
            // I://01.MP4 -> I:/01.MP4
            qPath = QStringLiteral("%1%2").arg(item._Path, item._Name);
        }
        else
        {
            qPath = QStringLiteral("%1/%2").arg(item._Path, item._Name);
        }

        if (const auto qFileInfo = QFileInfo(qPath);
            qFileInfo.exists())
        {
            RaptorUtil::invokeItemLocate(qPath);
        }
        else
        {
            RaptorToast::invokeWarningEject(QStringLiteral("无法定位 %1 可能已被移动或删除。").arg(item._Name));
        }

        return;
    }

    if (qIndexList.length() > 1)
    {
        if (const auto qOperate = RaptorMessageBox::invokeInformationEject(QStringLiteral("定位多个文件"),
                                                      QStringLiteral("即将定位多个文件在文件资源管理器中。是否继续?"));
                                                      qOperate == RaptorMessageBox::No)
        {
            return;
        }

        for (auto& qIndex : qIndexList)
        {
            const auto item = qIndex.data(Qt::UserRole).value<RaptorTransferItem>();
            auto qPath = QString();
            if (item._Path.length() == 3)
            {
                // I://01.MP4 -> I:/01.MP4
                qPath = QStringLiteral("%1%2").arg(item._Path, item._Name);
            }
            else
            {
                qPath = QStringLiteral("%1/%2").arg(item._Path, item._Name);
            }

            if (const auto qFileInfo = QFileInfo(qPath); qFileInfo.exists())
            {
                RaptorUtil::invokeItemLocate(qPath);
            }
            else
            {
                RaptorToast::invokeWarningEject(QStringLiteral("无法定位 %1 可能已被移动或删除。"));
            }
        }
    }
}

void RaptorTransferPage::onTabPrevClicked() const
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
            if (i == 0)
            {
                qPushButtonList[qPushButtonList.length() - 1]->setChecked(true);
                _Ui->_Body->setCurrentIndex(qPushButtonList.length() - 1);
            }
            else
            {
                qPushButtonList[i - 1]->setChecked(true);
                _Ui->_Body->setCurrentIndex(i - 1);
            }

            break;
        }
    }
}

void RaptorTransferPage::onTabDownloadingToggled(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_DownloadingPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_DownloadingPage);
}

void RaptorTransferPage::onTabDownloadedToggled(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_DownloadedPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_DownloadedPage);
}

void RaptorTransferPage::onTabUploadingToggled(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_UploadingPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_UploadingPage);
}

void RaptorTransferPage::onTabUploadedToggled(const bool& qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_UploadedPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_UploadedPage);
}

void RaptorTransferPage::onTabNextClicked() const
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
                _Ui->_Body->setCurrentIndex(0);
            }
            else
            {
                qPushButtonList[i + 1]->setChecked(true);
                _Ui->_Body->setCurrentIndex(i + 1);
            }

            break;
        }
    }
}

void RaptorTransferPage::onResumeClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_DownloadingPage)
    {
        _Ui->_DownloadingPage->invokeItemsResume();
    }
    else if (_Ui->_Body->currentWidget() == _Ui->_UploadingPage)
    {
        _Ui->_UploadingPage->invokeItemsResume();
    }
}

void RaptorTransferPage::onPauseClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_DownloadingPage)
    {
        _Ui->_DownloadingPage->invokeItemsPause();
    }
    else if (_Ui->_Body->currentWidget() == _Ui->_UploadingPage)
    {
        _Ui->_UploadingPage->invokeItemsPause();
    }
}

void RaptorTransferPage::onLocateClicked() const
{
    if (_Ui->_Body->currentWidget() == _Ui->_DownloadedPage)
    {
        invokeItemsLocate(_Ui->_DownloadedPage->invokeSelectItemsGet());
    }
    else if (_Ui->_Body->currentWidget() == _Ui->_DownloadingPage)
    {
        invokeItemsLocate(_Ui->_DownloadingPage->invokeSelectItemsGet());
    }
    else if (_Ui->_Body->currentWidget() == _Ui->_UploadingPage)
    {
        invokeItemsLocate(_Ui->_UploadingPage->invokeSelectItemsGet());
    }
    else if (_Ui->_Body->currentWidget() == _Ui->_UploadedPage)
    {
        invokeItemsLocate(_Ui->_UploadedPage->invokeSelectItemsGet());
    }
}

void RaptorTransferPage::onCancelClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_DownloadingPage)
    {
        _Ui->_DownloadingPage->invokeItemsCancel();
    }
    else if (_Ui->_Body->currentWidget() == _Ui->_UploadingPage)
    {
        _Ui->_UploadingPage->invokeItemsCancel();
    }
}

void RaptorTransferPage::onClearClicked() const
{
    if (_Ui->_Body->currentWidget() == _Ui->_DownloadedPage)
    {
        _Ui->_DownloadedPage->invokeItemsClear();
    }
    else if (_Ui->_Body->currentWidget() == _Ui->_UploadedPage)
    {
        _Ui->_UploadedPage->invokeItemsClear();
    }
}

void RaptorTransferPage::onDeleteClicked() const
{
    if (_Ui->_Body->currentWidget() == _Ui->_DownloadedPage)
    {
        _Ui->_DownloadedPage->invokeItemsDelete();
    }
    else if (_Ui->_Body->currentWidget() == _Ui->_UploadedPage)
    {
        _Ui->_UploadedPage->invokeItemsDelete();
    }
}

void RaptorTransferPage::onBodyChanged(const int& qIndex) const
{
    if (const auto qWidget = _Ui->_Body->widget(qIndex);
        qWidget == _Ui->_DownloadingPage || qWidget == _Ui->_UploadingPage)
    {
        _Ui->_Pause->setVisible(true);
        _Ui->_Pause->setEnabled(true);
        _Ui->_Resume->setVisible(true);
        _Ui->_Resume->setEnabled(true);
        _Ui->_Cancel->setVisible(true);
        _Ui->_Cancel->setEnabled(true);
        _Ui->_Locate->setVisible(false);
        _Ui->_Locate->setEnabled(false);
        _Ui->_Clear->setVisible(false);
        _Ui->_Clear->setEnabled(false);
        _Ui->_Delete->setVisible(false);
        _Ui->_Delete->setEnabled(false);
    }
    else
    {
        _Ui->_Pause->setVisible(false);
        _Ui->_Pause->setEnabled(false);
        _Ui->_Resume->setVisible(false);
        _Ui->_Resume->setEnabled(false);
        _Ui->_Cancel->setVisible(false);
        _Ui->_Cancel->setEnabled(false);
        _Ui->_Locate->setVisible(true);
        _Ui->_Locate->setEnabled(true);
        _Ui->_Clear->setVisible(true);
        _Ui->_Clear->setEnabled(true);
        _Ui->_Delete->setVisible(true);
        _Ui->_Delete->setEnabled(true);
    }
}
