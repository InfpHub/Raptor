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

#include "RaptorToast.h"
#include "ui_RaptorToast.h"

RaptorToast::RaptorToast(QWidget* qParent) : QWidget(qParent),
                                             _Ui(new Ui::RaptorToast)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorToast::~RaptorToast()
{
    qFree(_Ui)
    _Instance = Q_NULLPTR;
}

bool RaptorToast::eventFilter(QObject* qObject, QEvent* qEvent)
{
    if (qObject == _Ui->_Icon)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeIconDrawing();
            return true;
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

void RaptorToast::show()
{
    move((RaptorStoreSuite::invokeWorldGet()->width() - width()) / 2, height() + 10);
    QWidget::show();
}

void RaptorToast::invokeInformationEject(const QString& qContent,
                                         const int& qInterval)
{
    invokeEject(qContent, "Information", qInterval);
}

void RaptorToast::invokeSuccessEject(const QString& qContent,
                                     const int& qInterval)
{
    invokeEject(qContent, "Success", qInterval);
}

void RaptorToast::invokeWarningEject(const QString& qContent,
                                     const int& qInterval)
{
    invokeEject(qContent, "Warning", qInterval);
}

void RaptorToast::invokeCriticalEject(const QString& qContent,
                                      const int& qInterval)
{
    invokeEject(qContent, "Critical", qInterval);
}

void RaptorToast::invokeEject(const QString& qContent,
                              const QString& qLevel,
                              const int& qInterval)
{
    auto qInstance = static_cast<RaptorToast*>(Q_NULLPTR);
    if (_Instance)
    {
        qInstance = _Instance;
    }
    else
    {
        qInstance = new RaptorToast(RaptorStoreSuite::invokeWorldGet());
        _Instance = qInstance;
    }

    qInstance->_Ui->_Icon->setProperty("Level", qLevel);
    qInstance->_Ui->_Icon->style()->unpolish(qInstance->_Ui->_Icon);
    qInstance->_Ui->_Icon->style()->polish(qInstance->_Ui->_Icon);
    qInstance->invokeContentSet(qContent);
    qInstance->setFixedWidth(qInstance->_Ui->_StageLayout->contentsMargins().left() +
        qInstance->_Ui->_StageLayout->contentsMargins().left() +
        qInstance->_Ui->_Icon->width() +
        qInstance->_Ui->_StageLayout->spacing() +
        qInstance->_Ui->_Content->sizeHint().width() +
        qInstance->_Ui->_StageLayout->contentsMargins().right() +
        qInstance->_Ui->_StageLayout->contentsMargins().right());
    qInstance->_CloseTimer->setInterval(qInterval);
    qInstance->show();
    qInstance->_CloseTimer->start();
}

void RaptorToast::invokeInstanceInit()
{
    _CloseTimer = new QTimer(this);
}

void RaptorToast::invokeUiInit()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    _Ui->_Icon->installEventFilter(this);
}

void RaptorToast::invokeSlotInit() const
{
    connect(_CloseTimer,
            &QTimer::timeout,
            this,
            &RaptorToast::onCloseTimerTimeout);
}

void RaptorToast::invokeContentSet(const QString& qContent) const
{
    _Ui->_Content->setText(qContent);
}

void RaptorToast::invokeIconDrawing() const
{
    auto qPainter = QPainter(_Ui->_Icon);
    _SvgRender->load(RaptorUtil::invokeIconMatch(_Ui->_Icon->property("Level").toString(), false, true));
    if (_SvgRender->isValid())
    {
        _SvgRender->render(&qPainter, QRect(0, 0, _Ui->_Icon->width(), _Ui->_Icon->height()));
    }
}

void RaptorToast::onCloseTimerTimeout()
{
    close();
}
