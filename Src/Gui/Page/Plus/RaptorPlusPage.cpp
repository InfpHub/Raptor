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

#include "RaptorPlusPage.h"
#include "ui_RaptorPlusPage.h"

RaptorPlusPage::RaptorPlusPage(QWidget *qParent) : QWidget(qParent),
                                                   _Ui(new Ui::RaptorPlusPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorPlusPage::~RaptorPlusPage()
{
    FREE(_Ui)
}

bool RaptorPlusPage::eventFilter(QObject *qObject, QEvent *qEvent)
{
    if (qObject == _Ui->_SearchEdit)
    {
        if (qEvent->type() == QEvent::KeyPress)
        {
            if (const auto qKeyEvent = static_cast<QKeyEvent *>(qEvent);
                qKeyEvent->key() == Qt::Key_Enter || qKeyEvent->key() == Qt::Key_Return)
            {
                onSearchClicked();
                return true;
            }
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

RaptorCopyPage *RaptorPlusPage::invokeCopyPageGet() const
{
    return _Ui->_CopyPage;
}

RaptorCleanPage *RaptorPlusPage::invokeCleanPageGet() const
{
    return _Ui->_CleanPage;
}

void RaptorPlusPage::invokeInstanceInit()
{
    _TabGroup = new QButtonGroup(this);
    _TabGroup->setExclusive(true);
    _TabGroup->addButton(_Ui->_TabCopy);
    _TabGroup->addButton(_Ui->_TabClean);
}

void RaptorPlusPage::invokeUiInit()
{
    _Ui->_Title->setText(QStringLiteral("Plus"));
    _Ui->_TabPrev->setIcon(QIcon(RaptorUtil::invokeIconMatch("Left", false, true)));
    _Ui->_TabPrev->setIconSize(QSize(14, 14));
    _Ui->_TabCopy->setChecked(true);
    _Ui->_TabCopy->setText(QStringLiteral("跨网盘复制"));
    _Ui->_TabClean->setText(QStringLiteral("大文件清理"));
    _Ui->_TabNext->setIcon(QIcon(RaptorUtil::invokeIconMatch("Right", false, true)));
    _Ui->_TabNext->setIconSize(QSize(14, 14));
    _Ui->_SearchEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_Search->setIcon(QIcon(RaptorUtil::invokeIconMatch("Search", false, true)));
    _Ui->_Search->setIconSize(QSize(16, 16));
    _Ui->_SearchEdit->installEventFilter(this);
    _Ui->_Execute->setText(QStringLiteral("执行"));
    _Ui->_Refresh->setText(QStringLiteral("刷新"));
    _Ui->_Cancel->setText(QStringLiteral("取消"));
    _Ui->_Body->setCurrentWidget(_Ui->_CopyPage);
}

void RaptorPlusPage::invokeSlotInit() const
{
    connect(_Ui->_TabPrev,
            &QPushButton::clicked,
            this,
            &RaptorPlusPage::onTabPrevClicked);

    connect(_Ui->_TabCopy,
            &QPushButton::toggled,
            this,
            &RaptorPlusPage::onTabCopyToggled);

    connect(_Ui->_TabClean,
            &QPushButton::toggled,
            this,
            &RaptorPlusPage::onTabCleanToggled);

    connect(_Ui->_TabNext,
            &QPushButton::clicked,
            this,
            &RaptorPlusPage::onTabNextClicked);

    connect(_Ui->_SearchEdit,
            &QLineEdit::textChanged,
            invokeCopyPageGet(),
            &RaptorCopyPage::onSearchEditTextChanged);

    connect(_Ui->_Search,
            &QPushButton::clicked,
            this,
            &RaptorPlusPage::onSearchClicked);

    connect(_Ui->_Execute,
            &QPushButton::clicked,
            this,
            &RaptorPlusPage::onExecuteClicked);

    connect(_Ui->_Refresh,
            &QPushButton::clicked,
            this,
            &RaptorPlusPage::onRefreshClicked);

    connect(_Ui->_Cancel,
            &QPushButton::clicked,
            this,
            &RaptorPlusPage::onCancelClicked);

    connect(_Ui->_Body,
            &QStackedWidget::currentChanged,
            this,
            &RaptorPlusPage::onBodyChanged);
}

void RaptorPlusPage::onItemCopyWriterHaveFound(const QVariant &qVariant) const
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

void RaptorPlusPage::onItemLogouting(const QVariant &qVariant) const
{
    const auto input = qVariant.value<RaptorInput>();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item != RaptorStoreSuite::invokeUserGet())
    {
        return;
    }

    _Ui->_SearchEdit->clear();
}

void RaptorPlusPage::onTabPrevClicked() const
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

void RaptorPlusPage::onTabCopyToggled() const
{
    if (_Ui->_Body->currentWidget() == _Ui->_CopyPage)
    {
        return;
    }

    _Ui->_Body->setCurrentWidget(_Ui->_CopyPage);
}

void RaptorPlusPage::onTabCleanToggled() const
{
    if (_Ui->_Body->currentWidget() == _Ui->_CleanPage)
    {
        return;
    }

    _Ui->_CleanPage->invokeNavigate();
    _Ui->_Body->setCurrentWidget(_Ui->_CleanPage);
}

void RaptorPlusPage::onTabNextClicked() const
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

void RaptorPlusPage::onSearchClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_CopyPage)
    {
        _Ui->_CopyPage->invokeItemSearch();
    }
}

void RaptorPlusPage::onExecuteClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_CopyPage)
    {
        _Ui->_CopyPage->invokeItemsCopy();
    } else if (_Ui->_Body->currentWidget() == _Ui->_CleanPage)
    {
        _Ui->_CleanPage->invokeItemClean();
    }
}

void RaptorPlusPage::onRefreshClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_CopyPage)
    {
        _Ui->_CopyPage->invokeRefresh();
    } else if (_Ui->_Body->currentWidget() == _Ui->_CleanPage)
    {
        _Ui->_CleanPage->invokeNavigate();
    }
}

void RaptorPlusPage::onCancelClicked() const
{
    if (!RaptorStoreSuite::invokeUserIsValidConfirm())
    {
        return;
    }

    if (_Ui->_Body->currentWidget() == _Ui->_CopyPage)
    {
        _Ui->_CopyPage->invokeItemCancel();
    }
}

void RaptorPlusPage::onBodyChanged(const int &qIndex) const
{
    if (const auto qWidget = _Ui->_Body->widget(qIndex);
        qWidget == _Ui->_CopyPage)
    {
        _Ui->_SearchEdit->setEnabled(true);
        _Ui->_SearchEdit->setVisible(true);
        _Ui->_Search->setEnabled(true);
        _Ui->_Search->setVisible(true);
        _Ui->_Cancel->setEnabled(true);
        _Ui->_Cancel->setVisible(true);
    } else
    {
        _Ui->_SearchEdit->setVisible(false);
        _Ui->_SearchEdit->setEnabled(false);
        _Ui->_Search->setVisible(false);
        _Ui->_Search->setEnabled(false);
        _Ui->_Cancel->setVisible(false);
        _Ui->_Cancel->setEnabled(false);
    }
}
