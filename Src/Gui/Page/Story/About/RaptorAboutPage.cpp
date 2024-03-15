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

#include "RaptorAboutPage.h"
#include "ui_RaptorAboutPage.h"

RaptorAboutPage::RaptorAboutPage(QWidget* qParent) : QWidget(qParent),
                                                     _Ui(new Ui::RaptorAboutPage),
                                                     _AvatarAlpha(155),
                                                     _AvatarRadius(50)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorAboutPage::~RaptorAboutPage()
{
    FREE(_Ui)
}

bool RaptorAboutPage::eventFilter(QObject* qObject, QEvent* qEvent)
{
    if (qObject == _Ui->_Avatar)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeAvatarPaint();
            return true;
        }
    }

    return QWidget::eventFilter(qObject, qEvent);
}

void RaptorAboutPage::invokeNavigate() const
{
    if (_AvatarTimer->isActive())
    {
        return;
    }

    _AvatarTimer->start();
}


void RaptorAboutPage::invokeInstanceInit()
{
    _TimelineModel = new QStandardItemModel(this);
    _TimelineDelegate = new RaptorTimelineDelegate(this);
    _AvatarAnimation = new QPropertyAnimation(this, "", this);
    _AvatarTimer = new QTimer(this);
    _AvatarTimer->setInterval(3000);
}

void RaptorAboutPage::invokeUiInit()
{
    _Ui->_ApplicationName->setText(APPLICATION_NAME);
    _Ui->_ApplicationVersion->setText(QStringLiteral("%1.%2.%3").arg(QString::number(MAJOR_VERSION), QString::number(MINOR_VERSION), QString::number(PATCH_VERSION)));
    _Ui->_MetaSignature->setText(QStringLiteral("尽最大努力，将最好的献给最需要的你。"));
    _Ui->_Avatar->installEventFilter(this);

    _Ui->_Manifesto->setText(QStringLiteral("在通往无比强大的路上，%1 才刚刚起步。需要你的支持与鼓励，请给 %2 更多的时间。").arg(APPLICATION_NAME, APPLICATION_NAME));
    _Ui->_DeclarationTip->setText(QStringLiteral("软件声明"));
    _Ui->_Declaration->setText(QStringLiteral("<p>%1 是自由软件，依据 <a href=\"https://www.gnu.org/licenses\" style=\"text-decoration: underline; color: rgba(0, 164, 255, 255);\">GNU General Public License V3.0</a> 授权发布。</p> <p>你可以再分发之和/或依照由自由软件基金会发布的 GNU 通用公共许可证修改之，无论是版本 3 许可证，还是（按你的决定）任何以后版都可以。</p><p>发布 %2 是希望它能有用，但是并无保障；甚至连可销售和符合某个特定的目的都不保证。请参看 GNU 通用公共许可证，了解详情。</p><p>你应该随程序获得一份 GNU 通用公共许可证的复本。如果没有，请看 <a href=\"https://www.gnu.org/licenses\" style=\"text-decoration: underline; color: rgba(0, 164, 255, 255);\">在线许可证</a>。</p>").arg(APPLICATION_NAME, APPLICATION_NAME));
    _Ui->_Declaration->setOpenExternalLinks(true);
    _Ui->_ThanksTip->setText(QStringLiteral("感谢这些了不起的人"));
    _Ui->_ThanksQt->invokeContributorSet(qMakePair(RaptorUtil::invokeIconMatch("Qt", false, true), QStringLiteral("https://www.qt.io")));
    _Ui->_ThanksCurl->invokeContributorSet(qMakePair(RaptorUtil::invokeIconMatch("Curl", false, true), QStringLiteral("https://curl.se")));
    _Ui->_ThanksSass->invokeContributorSet(qMakePair(RaptorUtil::invokeIconMatch("Sass", false, true), QStringLiteral("https://sass-lang.com")));
    _Ui->_ThanksSECP256K1->invokeContributorSet(qMakePair(RaptorUtil::invokeIconMatch("SECP256K1", false, true), QStringLiteral("https://bitcoincore.org")));
    _Ui->_ThanksUV->invokeContributorSet(qMakePair(RaptorUtil::invokeIconMatch("UV", false, true), QStringLiteral("https://libuv.org")));
    _Ui->_ThanksYaml->invokeContributorSet(qMakePair(RaptorUtil::invokeIconMatch("Yaml", false, true), QStringLiteral("https://github.com/jbeder/yaml-cpp")));
    _Ui->_ThanksZInt->invokeContributorSet(qMakePair(RaptorUtil::invokeIconMatch("ZInt", false, true), QStringLiteral("https://www.zint.org.uk")));

    _Ui->_TimelineTip->setText(QStringLiteral("发行历史"));
    const auto items = RaptorUtil::invokeTimelineLoad();
    _Ui->_Timeline->setFixedHeight(items.length() * RaptorTimelineDelegate::_ItemHeight);
    for (const auto& item : items)
    {
        const auto iten = new QStandardItem();
        iten->setData(QVariant::fromValue<RaptorTimelineItem>(item), Qt::UserRole);
        _TimelineModel->appendRow(iten);
    }

    _Ui->_Timeline->setModel(_TimelineModel);
    _Ui->_Timeline->setItemDelegate(_TimelineDelegate);
}

void RaptorAboutPage::invokeSlotInit() const
{
    connect(_Ui->_ThanksQt,
            &QPushButton::clicked,
            this,
            &RaptorAboutPage::onThanksQtClicked);

    connect(_Ui->_ThanksCurl,
            &QPushButton::clicked,
            this,
            &RaptorAboutPage::onThanksCurlClicked);

    connect(_Ui->_ThanksSass,
            &QPushButton::clicked,
            this,
            &RaptorAboutPage::onThanksSassClicked);

    connect(_Ui->_ThanksSECP256K1,
            &QPushButton::clicked,
            this,
            &RaptorAboutPage::onThanksSECP256K1Clicked);

    connect(_Ui->_ThanksYaml,
            &QPushButton::clicked,
            this,
            &RaptorAboutPage::onThanksYamlClicked);

    connect(_Ui->_ThanksUV,
            &QPushButton::clicked,
            this,
            &RaptorAboutPage::onThanksUVClicked);

    connect(_Ui->_ThanksZInt,
            &QPushButton::clicked,
            this,
            &RaptorAboutPage::onThanksZIntClicked);

    connect(_AvatarAnimation,
            &QPropertyAnimation::valueChanged,
            this,
            &RaptorAboutPage::onAvatarAnimationValueChanged);

    connect(_AvatarTimer,
            &QTimer::timeout,
            this,
            &RaptorAboutPage::onAvatarTimerTimeout);
}

void RaptorAboutPage::invokeAvatarPaint() const
{
    auto qPainter = QPainter{_Ui->_Avatar};
    if (_Avatar.isNull())
    {
        _SvgRender->load(RaptorUtil::invokeIconMatch("Free", false, true));
        if (_SvgRender->isValid())
        {
            _SvgRender->render(&qPainter, _Ui->_Avatar->rect());
        }

        return;
    }

    qPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    qPainter.setPen(Qt::NoPen);
    qPainter.setBrush(QColor(136, 136, 136, _AvatarAlpha));
    qPainter.drawEllipse(_Ui->_Avatar->rect().center(), _AvatarRadius, _AvatarRadius);
    qPainter.setBrush(QColor(136, 136, 136, 125));
    qPainter.drawEllipse(25, 25, _Ui->_Avatar->width() - 50, _Ui->_Avatar->height() - 50);
    static_cast<void>(_Avatar.scaled(_Ui->_Avatar->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    auto qPainterPath = QPainterPath();
    qPainterPath.addEllipse(30, 30, _Ui->_Avatar->width() - 60, _Ui->_Avatar->height() - 60);
    qPainter.setClipPath(qPainterPath);
    qPainter.drawPixmap(30, 30, _Ui->_Avatar->width() - 60, _Ui->_Avatar->height() - 60, _Avatar);
}

void RaptorAboutPage::onItemAccessTokenRefreshed(const QVariant& qVariant)
{
    if (const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>(); !_State)
    {
        return;
    }

    const auto item = RaptorStoreSuite::invokeUserGet();
    _Avatar.loadFromData(item._Avatar);
    if (!item._Description.isEmpty())
    {
        _Ui->_Manifesto->setText(item._Description);
    }
}

void RaptorAboutPage::onItemSignInInfoFetched(const QVariant &qVariant) const
{
    const auto [_State, _Message, _Data] = qVariant.value<RaptorOutput>();
    if (!_State)
    {
        return;
    }

    const auto qBlessing = _Data.value<QString>();
    _Ui->_MetaSignature->setText(qBlessing);
}

void RaptorAboutPage::onItemLogouting(const QVariant& qVariant)
{
    const auto input = qVariant.value<RaptorInput>();
    if (const auto item = input._Variant.value<RaptorAuthenticationItem>();
        item != RaptorStoreSuite::invokeUserGet())
    {
        return;
    }

    _Avatar = QPixmap();
    _Ui->_Manifesto->setText(QStringLiteral("在通往无比强大的路上，%1 才刚刚起步。需要你的支持与鼓励，请给 %2 更多的时间。").arg(APPLICATION_NAME, APPLICATION_NAME));
}

void RaptorAboutPage::onThanksQtClicked() const
{
    QDesktopServices::openUrl(QUrl(_Ui->_ThanksQt->invokeContributorGet().second));
}

void RaptorAboutPage::onThanksCurlClicked() const
{
    QDesktopServices::openUrl(QUrl(_Ui->_ThanksCurl->invokeContributorGet().second));
}

void RaptorAboutPage::onThanksUVClicked() const
{
    QDesktopServices::openUrl(QUrl(_Ui->_ThanksUV->invokeContributorGet().second));
}

void RaptorAboutPage::onThanksYamlClicked() const
{
    QDesktopServices::openUrl(QUrl(_Ui->_ThanksYaml->invokeContributorGet().second));
}

void RaptorAboutPage::onThanksSassClicked() const
{
    QDesktopServices::openUrl(QUrl(_Ui->_ThanksSass->invokeContributorGet().second));
}

void RaptorAboutPage::onThanksSECP256K1Clicked() const
{
    QDesktopServices::openUrl(QUrl(_Ui->_ThanksSECP256K1->invokeContributorGet().second));
}

void RaptorAboutPage::onThanksZIntClicked() const
{
    QDesktopServices::openUrl(QUrl(_Ui->_ThanksZInt->invokeContributorGet().second));
}

void RaptorAboutPage::onAvatarAnimationValueChanged(const QVariant& qVariant)
{
    Q_UNUSED(qVariant)
    if (_AvatarRadius > width() / 2)
    {
        _AvatarAnimation->stop();
    }
    else
    {
        _AvatarRadius += 1;
        _AvatarAlpha -= 6;
        _Ui->_Avatar->update();
    }
}

void RaptorAboutPage::onAvatarTimerTimeout()
{
    _AvatarRadius = 50;
    _AvatarAlpha = 155;
    _AvatarAnimation->setStartValue(1);
    _AvatarAnimation->setEndValue(30);
    _AvatarAnimation->setDuration(1500);
    _AvatarAnimation->setEasingCurve(QEasingCurve::BezierSpline);
    _AvatarAnimation->start();
}