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

#include "RaptorMessageBox.h"
#include "ui_RaptorMessageBox.h"

RaptorMessageBox::RaptorMessageBox(QWidget *qParent) : RaptorEject(qParent),
                                                       _Ui(new Ui::RaptorMessageBox),
                                                       _Operate(Nothing)
{
    _Ui->setupUi(this);
    invokeUiInit();
    invokeSlotInit();
}

RaptorMessageBox::~RaptorMessageBox()
{
    qFree(_Ui)
}

bool RaptorMessageBox::eventFilter(QObject *qObject, QEvent *qEvent)
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
            } else if (qKeyEvent->key() == Qt::Key_Return)
            {
                onYesClicked();
                return true;
            }
        }
    }

    return RaptorEject::eventFilter(qObject, qEvent);
}

RaptorMessageBox::Operate RaptorMessageBox::invokeInformationEject(const QString &qTitle,
                                                                   const QString &qContent,
                                                                   const QString &qYesText,
                                                                   const QString &qNoText)
{
    return invokeEject(qTitle, qContent, qYesText, qNoText, "Information");
}

RaptorMessageBox::Operate RaptorMessageBox::invokeSuccessEject(const QString &qTitle,
                                                               const QString &qContent,
                                                               const QString &qYesText,
                                                               const QString &qNoText)
{
    return invokeEject(qTitle, qContent, qYesText, qNoText, "Success");
}

RaptorMessageBox::Operate RaptorMessageBox::invokeWarningEject(const QString &qTitle,
                                                               const QString &qContent,
                                                               const QString &qYesText,
                                                               const QString &qNoText)
{
    return invokeEject(qTitle, qContent, qYesText, qNoText, "Warning");
}

RaptorMessageBox::Operate RaptorMessageBox::invokeCriticalEject(const QString &qTitle,
                                                                const QString &qContent,
                                                                const QString &qYesText,
                                                                const QString &qNoText)
{
    return invokeEject(qTitle, qContent, qYesText, qNoText, "Critical");
}

void RaptorMessageBox::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_Yes->setText(QStringLiteral("确定"));
    _Ui->_No->setText(QStringLiteral("取消"));
}

void RaptorMessageBox::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorMessageBox::onCloseClicked);

    connect(_Ui->_Yes,
            &QPushButton::clicked,
            this,
            &RaptorMessageBox::onYesClicked);

    connect(_Ui->_No,
            &QPushButton::clicked,
            this,
            &RaptorMessageBox::onNoClicked);
}

RaptorMessageBox::Operate RaptorMessageBox::invokeEject(const QString &qTitle,
                                                        const QString &qContent,
                                                        const QString &qYesText,
                                                        const QString &qNoText,
                                                        const QString &qLevel)
{
    auto qInstance = RaptorMessageBox(RaptorStoreSuite::invokeWorldGet());
    qInstance.invokeTitleSet(qTitle);
    qInstance.invokeContentSet(qContent);
    qInstance._Ui->_Yes->setText(qYesText);
    qInstance._Ui->_Yes->setProperty("Level", qLevel);
    qInstance._Ui->_Yes->style()->unpolish(qInstance._Ui->_Yes);
    qInstance._Ui->_Yes->style()->polish(qInstance._Ui->_Yes);
    qInstance._Ui->_No->setText(qNoText);
    qInstance.exec();
    return qInstance._Operate;
}

void RaptorMessageBox::invokeTitleSet(const QString &qTitle) const
{
    _Ui->_Title->setText(qTitle);
}

void RaptorMessageBox::invokeContentSet(const QString &qContent) const
{
    _Ui->_Content->setText(qContent);
}

void RaptorMessageBox::onCloseClicked()
{
    _Operate = Nothing;
    close();
}

void RaptorMessageBox::onYesClicked()
{
    _Operate = Yes;
    close();
}

void RaptorMessageBox::onNoClicked()
{
    _Operate = No;
    close();
}
