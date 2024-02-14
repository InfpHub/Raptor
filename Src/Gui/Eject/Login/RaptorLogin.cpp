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

#include "RaptorLogin.h"
#include "ui_RaptorLogin.h"

RaptorLogin::RaptorLogin(QWidget* qParent) : RaptorEject(qParent),
                                             _Ui(new Ui::RaptorLogin)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorLogin::~RaptorLogin()
{
    FREE(_Ui)
}

bool RaptorLogin::eventFilter(QObject* qObject, QEvent* qEvent)
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

    if (qObject == _Ui->_QrCode)
    {
        if (qEvent->type() == QEvent::MouseButtonPress)
        {
            _QrCodeStatusTimer->stop();
            _Ui->_QrCodeStatus->clear();
            Q_EMIT itemQrCodeGenerating();
            return true;
        }
    }

    return RaptorEject::eventFilter(qObject, qEvent);
}

void RaptorLogin::invokeEject(const QVariant& qVariant)
{
    _Variant = qVariant;
    _Ui->_QrCodeStatus->clear();
    Q_EMIT itemQrCodeGenerating();
    RaptorEject::invokeEject();
}

void RaptorLogin::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _QrCodeStatusTimer = new QTimer(this);
    _QrCodeStatusTimer->setInterval(1000);
}

void RaptorLogin::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Title->setText(QStringLiteral("认证你的账号"));
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_QrCodeTitle->setText(QStringLiteral("使用阿里云盘移动端 APP 扫描此内容以立即登录。"));
    _Ui->_QrCode->installEventFilter(this);
    _Ui->_Register->setText(QStringLiteral(R"(<span>没有账号? <a href="https://www.aliyundrive.com" style="font: 12px; color: rgba(184, 112, 232, 1); text-decoration: none;">点我注册</a></span>)"));
    _Ui->_Register->setOpenExternalLinks(true);
}

void RaptorLogin::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorLogin::onCloseClicked);

    connect(_QrCodeStatusTimer,
            &QTimer::timeout,
            this,
            &RaptorLogin::onrQrCodeStatusTimerTimeout);
}

void RaptorLogin::onItemQrCodeGenerated(const QVariant& qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    _QrCode = _Data.value<RaptorQrCode>();
    auto input = RaptorInput();
    input._Url = _QrCode._Code;
    input._Variant = QVariant::fromValue<QSize>(_Ui->_QrCode->size());
    Q_EMIT itemQrCodeEncoding(QVariant::fromValue<RaptorInput>(input));
}

void RaptorLogin::onItemQrCodeEncoded(const QVariant& qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    const auto qPixmap = _Data.value<QPixmap>();
    _Ui->_QrCode->setPixmap(qPixmap);
    _QrCodeStatusTimer->start();
}

void RaptorLogin::onItemQrCodeStatusFetched(const QVariant& qVariant)
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        _Ui->_QrCodeStatus->setText(_Message);
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    switch (_Data.value<RaptorQrCodeStatus>())
    {
    case RaptorQrCodeStatus::New:
        _Ui->_QrCodeStatus->setText(QStringLiteral(INFORMATION_TEMPLATE).arg(QStringLiteral("等待扫描...")));
        break;
    case RaptorQrCodeStatus::Scanned:
        _Ui->_QrCodeStatus->setText(QStringLiteral(CREATIVE_TEMPLATE).arg(QStringLiteral("已扫描，请在移动端确认登录。")));
        break;
    case RaptorQrCodeStatus::Confirmed:
        _Ui->_QrCodeStatus->setText(QStringLiteral(SUCCESS_TEMPLATE).arg(QStringLiteral("登陆成功。")));
        break;
    case RaptorQrCodeStatus::Canceled:
        _Ui->_QrCodeStatus->setText(QStringLiteral(WARNING_TEMPLATE).arg(QStringLiteral("登录已取消。要想再次登录，请点击二维码以刷新。")));
        _QrCodeStatusTimer->stop();
        break;
    case RaptorQrCodeStatus::Expired:
        if (RaptorStoreSuite::invokeUserIsValidConfirm())
        {
            return;
        }

        _Ui->_QrCodeStatus->setText(QStringLiteral(CREATIVE_TEMPLATE).arg(QStringLiteral("二维码已过期。要想再次登录，请点击二维码以刷新。")));
        break;
    default:
        break;
    }
}

void RaptorLogin::onItemAccessTokenRefreshed(const QVariant& qVariant)
{
    Q_UNUSED(qVariant)
    _QrCodeStatusTimer->stop();
    close();
}

void RaptorLogin::onCloseClicked()
{
    close();
}

void RaptorLogin::onrQrCodeStatusTimerTimeout() const
{
    auto input = RaptorInput();
    input._Variant = QVariant::fromValue<RaptorQrCode>(_QrCode);
    Q_EMIT itemQrCodeStatusFetching(QVariant::fromValue<RaptorInput>(input));
}