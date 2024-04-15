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

#include "RaptorNetworkPage.h"
#include "ui_RaptorNetworkPage.h"

RaptorNetworkPage::RaptorNetworkPage(QWidget *qParent) : QWidget(qParent),
                                                         _Ui(new Ui::RaptorNetworkPage)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorNetworkPage::~RaptorNetworkPage()
{
    qFree(_Ui)
}

bool RaptorNetworkPage::eventFilter(QObject *qObject, QEvent *qEvent)
{
    if (qObject == _Ui->_Proxy)
    {
        if (qEvent->type() == QEvent::MouseButtonPress)
        {
            if (_Ui->_ProxyNone->isChecked())
            {
                RaptorToast::invokeWarningEject(QStringLiteral("请选择代理类型!"));
                return true;
            }
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

void RaptorNetworkPage::invokeInstanceInit()
{
    _ProxyGroup = new QButtonGroup(this);
    _ProxyGroup->addButton(_Ui->_ProxyNone);
    _ProxyGroup->addButton(_Ui->_ProxyHTTP);
    _ProxyGroup->addButton(_Ui->_ProxyHTTPS);
    _ProxyGroup->addButton(_Ui->_ProxyHTTPS2);
    _ProxyGroup->addButton(_Ui->_ProxyHTTP1_0);
    _ProxyGroup->addButton(_Ui->_ProxySOCKS4);
    _ProxyGroup->addButton(_Ui->_ProxySOCKS5);
    _ProxyGroup->setExclusive(true);

    _IPResolveGroup = new QButtonGroup(this);
    _IPResolveGroup->addButton(_Ui->_IPResolveAuto);
    _IPResolveGroup->addButton(_Ui->_IPResolveV4);
    _IPResolveGroup->addButton(_Ui->_IPResolveV6);
    _IPResolveGroup->setExclusive(true);

    _DebounceTimer = new QTimer(this);
    _DebounceTimer->setSingleShot(true);
    _DebounceTimer->setInterval(1350);
}

void RaptorNetworkPage::invokeUiInit()
{
    _Ui->_ProxyTip->setText(QStringLiteral("代理:"));
    const auto qEngine = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                            Setting::Network::ProxyEngine).toString();
    _Ui->_ProxyNone->setChecked(qEngine == Setting::Network::None);
    _Ui->_ProxyNone->setText(QStringLiteral("无"));
    _Ui->_ProxyHTTP->setChecked(qEngine == Setting::Network::HTTP);
    _Ui->_ProxyHTTP->setText(QStringLiteral("HTTP"));
    _Ui->_ProxyHTTP1_0->setChecked(qEngine == Setting::Network::HTTP1_0);
    _Ui->_ProxyHTTP1_0->setText(QStringLiteral("HTTP1_0"));
    _Ui->_ProxyHTTPS->setChecked(qEngine == Setting::Network::HTTPS);
    _Ui->_ProxyHTTPS->setText(QStringLiteral("HTTPS"));
    _Ui->_ProxyHTTPS2->setChecked(qEngine == Setting::Network::HTTPS2);
    _Ui->_ProxyHTTPS2->setText(QStringLiteral("HTTPS2"));
    _Ui->_ProxySOCKS4->setChecked(qEngine == Setting::Network::SOCKS4);
    _Ui->_ProxySOCKS4->setText(QStringLiteral("SOCKS4"));
    _Ui->_ProxySOCKS5->setChecked(qEngine == Setting::Network::SOCKS5);
    _Ui->_ProxySOCKS5->setText(QStringLiteral("SOCKS5"));
    _Ui->_ProxyHostTip->setText(QStringLiteral("主机:"));
    _Ui->_ProxyHost->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                Setting::Network::ProxyHost).toString());
    _Ui->_ProxyHost->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ProxyPortTip->setText(QStringLiteral("端口:"));
    _Ui->_ProxyPort->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                Setting::Network::ProxyPort).toString());
    _Ui->_ProxyPort->setValidator(new QRegularExpressionValidator(QRegularExpression{"[0-9]+$"}, this));
    _Ui->_ProxyPort->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ProxyUsernameTip->setText(QStringLiteral("账号:"));
    _Ui->_ProxyUsername->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                    Setting::Network::ProxyUsername).toString());
    _Ui->_ProxyUsername->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ProxyPasswordTip->setText(QStringLiteral("密码:"));
    _Ui->_ProxyPassword->setText(RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                                    Setting::Network::ProxyPassword).toString());
    _Ui->_ProxyPassword->setContextMenuPolicy(Qt::NoContextMenu);

    _Ui->_Proxy->installEventFilter(this);
    _Ui->_Proxy->setChecked(RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                               Setting::Network::Proxy).toBool());
    _Ui->_Proxy->setText(QStringLiteral("启用代理"));
    _Ui->_ProxyTest->setText(QStringLiteral("测试"));

    _Ui->_IPResolveTip->setText(QStringLiteral("IP 解析:"));
    const auto qIPResolve = RaptorSettingSuite::invokeItemFind(Setting::Section::Network,
                                                               Setting::Network::IPResolve).toString();
    _Ui->_IPResolveAuto->setChecked(qIPResolve == Setting::Network::Auto);
    _Ui->_IPResolveAuto->setText(QStringLiteral("自动"));
    _Ui->_IPResolveV4->setChecked(qIPResolve == Setting::Network::IPV4);
    _Ui->_IPResolveV4->setText(QStringLiteral("IPV4"));
    _Ui->_IPResolveV6->setChecked(qIPResolve == Setting::Network::IPV6);
    _Ui->_IPResolveV6->setText(QStringLiteral("IPV6"));
}

void RaptorNetworkPage::invokeSlotInit() const
{
    connect(_DebounceTimer,
            &QTimer::timeout,
            this,
            &RaptorNetworkPage::onDebounceTimerTimeout);

    connect(_Ui->_ProxyNone,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onProxyNoneClicked);

    connect(_Ui->_ProxyHTTP,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onProxyHTTPClicked);

    connect(_Ui->_ProxyHTTP1_0,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onProxyHTTP1_0Clicked);

    connect(_Ui->_ProxyHTTPS,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onProxyHTTPSClicked);

    connect(_Ui->_ProxyHTTPS2,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onProxyHTTPS2Clicked);

    connect(_Ui->_ProxySOCKS4,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onProxySOCKS4Clicked);

    connect(_Ui->_ProxySOCKS5,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onProxySOCKS5Clicked);

    connect(_Ui->_ProxyHost,
            &QLineEdit::textChanged,
            this,
            &RaptorNetworkPage::onProxyHostTextChanged);

    connect(_Ui->_ProxyPort,
            &QLineEdit::textChanged,
            this,
            &RaptorNetworkPage::onProxyPortTextChanged);

    connect(_Ui->_ProxyUsername,
            &QLineEdit::textChanged,
            this,
            &RaptorNetworkPage::onProxyUsernameTextChanged);

    connect(_Ui->_ProxyPassword,
            &QLineEdit::textChanged,
            this,
            &RaptorNetworkPage::onProxyPasswordTextChanged);

    connect(_Ui->_Proxy,
            &QCheckBox::stateChanged,
            this,
            &RaptorNetworkPage::onProxyStateChanged);

    connect(_Ui->_ProxyTest,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onProxyTestClicked);

    connect(_Ui->_IPResolveAuto,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onIPResolveAutoClicked);

    connect(_Ui->_IPResolveV4,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onIPResolveV4Clicked);

    connect(_Ui->_IPResolveV6,
            &QPushButton::clicked,
            this,
            &RaptorNetworkPage::onIPResolveV6Clicked);
}

void RaptorNetworkPage::onItemProxyConnectTested(const QVariant &qVariant) const
{
    _Ui->_ProxyTest->setEnabled(true);
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
        !_State)
    {
        RaptorToast::invokeCriticalEject(_Message);
        return;
    }

    RaptorToast::invokeSuccessEject(QStringLiteral("目标主机可连接!"));
}

void RaptorNetworkPage::onDebounceTimerTimeout() const
{
    const auto items = _DebounceTimer->dynamicPropertyNames();
    for (auto &item: items)
    {
        const auto [qKey, qValue] = _DebounceTimer->property(item).value<QPair<QString, QVariant> >();
        RaptorSettingSuite::invokeItemSave(qKey, item, qValue);
    }
}

void RaptorNetworkPage::onProxyNoneClicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _Ui->_Proxy->setText(QStringLiteral("启用代理"));
    _DebounceTimer->setProperty(Setting::Network::ProxyEngine,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Network,
                                              QVariant::fromValue<QString>(Setting::Network::None))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyHTTPClicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _Ui->_Proxy->setText(QStringLiteral("启用 HTTP 代理"));
    _DebounceTimer->setProperty(Setting::Network::ProxyEngine,
                                QVariant::fromValue<QPair<QString, QVariant> >(qMakePair(
                                    Setting::Section::Network, QVariant::fromValue<QString>(Setting::Network::HTTP))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyHTTP1_0Clicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _Ui->_Proxy->setText(QStringLiteral("启用 HTTP1_0 代理"));
    _DebounceTimer->setProperty(Setting::Network::ProxyEngine,
                                QVariant::fromValue<QPair<QString, QVariant> >(qMakePair(
                                    Setting::Section::Network,
                                    QVariant::fromValue<QString>(Setting::Network::HTTP1_0))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyHTTPSClicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _Ui->_Proxy->setText(QStringLiteral("启用 HTTPS 代理"));
    _DebounceTimer->setProperty(Setting::Network::ProxyEngine,
                                QVariant::fromValue<QPair<QString, QVariant> >(qMakePair(
                                    Setting::Section::Network, QVariant::fromValue<QString>(Setting::Network::HTTPS))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyHTTPS2Clicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _Ui->_Proxy->setText(QStringLiteral("启用 HTTPS2 代理"));
    _DebounceTimer->setProperty(Setting::Network::ProxyEngine,
                                QVariant::fromValue<QPair<QString, QVariant> >(qMakePair(
                                    Setting::Section::Network,
                                    QVariant::fromValue<QString>(Setting::Network::HTTPS2))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxySOCKS4Clicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _Ui->_Proxy->setText(QStringLiteral("启用 SOCKS4 代理"));
    _DebounceTimer->setProperty(Setting::Network::ProxyEngine,
                                QVariant::fromValue<QPair<QString, QVariant> >(qMakePair(
                                    Setting::Section::Network,
                                    QVariant::fromValue<QString>(Setting::Network::SOCKS4))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxySOCKS5Clicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _Ui->_Proxy->setText(QStringLiteral("启用 SOCKS5 代理"));
    _DebounceTimer->setProperty(Setting::Network::ProxyEngine,
                                QVariant::fromValue<QPair<QString, QVariant> >(qMakePair(
                                    Setting::Section::Network,
                                    QVariant::fromValue<QString>(Setting::Network::SOCKS5))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyHostTextChanged(const QString &qValue) const
{
    _DebounceTimer->setProperty(Setting::Network::ProxyHost,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Network, QVariant::fromValue<QString>(qValue))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyPortTextChanged(const QString &qValue) const
{
    _DebounceTimer->setProperty(Setting::Network::ProxyPort,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Network,
                                              QVariant::fromValue<quint32>(qValue.toUInt()))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyUsernameTextChanged(const QString &qValue) const
{
    _DebounceTimer->setProperty(Setting::Network::ProxyUsername,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Network, QVariant::fromValue<QString>(qValue))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyPasswordTextChanged(const QString &qValue) const
{
    _DebounceTimer->setProperty(Setting::Network::ProxyPassword,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Network, QVariant::fromValue<QString>(qValue))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyStateChanged(const int &qState) const
{
    if (qState == Qt::Checked)
    {
        _DebounceTimer->setProperty(Setting::Network::Proxy,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Network, QVariant::fromValue<bool>(true))));
    } else if (qState == Qt::Unchecked)
    {
        _DebounceTimer->setProperty(Setting::Network::Proxy,
                                    QVariant::fromValue<QPair<QString, QVariant> >(
                                        qMakePair(Setting::Section::Network, QVariant::fromValue<bool>(false))));
    }

    _DebounceTimer->start();
}

void RaptorNetworkPage::onProxyTestClicked() const
{
    if (_Ui->_ProxyNone->isChecked())
    {
        RaptorToast::invokeWarningEject("尚未选择代理类型，无法继续!");
        return;
    }

    const auto qHost = _Ui->_ProxyHost->text();
    if (qHost.isEmpty())
    {
        RaptorToast::invokeWarningEject("主机地址不能为空!");
        return;
    }

    const auto qPort = _Ui->_ProxyPort->text();
    auto input = RaptorInput();
    if (_Ui->_ProxyHTTP->isChecked())
    {
        input._Type = Setting::Network::HTTP;
    } else if (_Ui->_ProxyHTTP1_0->isChecked())
    {
        input._Type = Setting::Network::HTTP1_0;
    } else if (_Ui->_ProxyHTTPS->isChecked())
    {
        input._Type = Setting::Network::HTTPS;
    } else if (_Ui->_ProxyHTTPS2->isChecked())
    {
        input._Type = Setting::Network::HTTPS2;
    } else if (_Ui->_ProxySOCKS4->isChecked())
    {
        input._Type = Setting::Network::SOCKS4;
    } else if (_Ui->_ProxySOCKS5->isChecked())
    {
        input._Type = Setting::Network::SOCKS5;
    }

    input._Name = _Ui->_ProxyUsername->text();
    input._Password = _Ui->_ProxyPassword->text();
    input._Variant = QVariant::fromValue<QPair<QString, QString> >(qMakePair(qHost, qPort));
    _Ui->_ProxyTest->setEnabled(false);
    Q_EMIT itemProxyConnectTesting(QVariant::fromValue<RaptorInput>(input));
}

void RaptorNetworkPage::onIPResolveAutoClicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }


    _DebounceTimer->setProperty(Setting::Network::IPResolve,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Network,
                                              QVariant::fromValue<QString>(Setting::Network::Auto))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onIPResolveV4Clicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Network::IPResolve,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Network,
                                              QVariant::fromValue<QString>(Setting::Network::IPV4))));
    _DebounceTimer->start();
}

void RaptorNetworkPage::onIPResolveV6Clicked(const bool &qChecked) const
{
    if (!qChecked)
    {
        return;
    }

    _DebounceTimer->setProperty(Setting::Network::IPResolve,
                                QVariant::fromValue<QPair<QString, QVariant> >(
                                    qMakePair(Setting::Section::Network,
                                              QVariant::fromValue<QString>(Setting::Network::IPV6))));
    _DebounceTimer->start();
}
