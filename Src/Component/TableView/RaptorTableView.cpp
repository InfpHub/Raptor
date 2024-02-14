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

#include "RaptorTableView.h"

RaptorTableView::RaptorTableView(QWidget* qParent) : QTableView(qParent)
{
    invokeInstanceInit();
    invokeSlotInit();
}

void RaptorTableView::invokeServerCodeSet(const Code& code)
{
    _Code = code;
}

void RaptorTableView::invokeBackgroundPaintableSet(const bool& qValue)
{
    _Value = qValue;
    update();
}

void RaptorTableView::invokeTitleSet(const QString& qText)
{
    _Title = qText;
}

void RaptorTableView::invokeSummarySet(const QString& qText)
{
    _Summary = qText;
}

void RaptorTableView::invokeIndicatorSet(const QString& qText) const
{
    _Indicator->setText(qText);
    _Indicator->adjustSize();
}

void RaptorTableView::paintEvent(QPaintEvent* qPaintEvent)
{
    QTableView::paintEvent(qPaintEvent);
    invokeBackgroundPaint();
}

void RaptorTableView::invokeInstanceInit()
{
    _Indicator = new QPushButton(this);
    _Indicator->setObjectName("QIndicator");
    _Indicator->setFixedHeight(32);
    _Indicator->setVisible(false);
    _Indicator->setEnabled(false);
}

void RaptorTableView::invokeSlotInit() const
{
    connect(_Indicator,
            &QPushButton::clicked,
            this,
            &RaptorTableView::onIndicatorClicked);
}

void RaptorTableView::invokeBackgroundPaint()
{
    if (!_Value)
    {
        _Indicator->setVisible(false);
        _Indicator->setEnabled(false);
        return;
    }

    auto qColor = QColor();
    auto qTheme = property(Setting::Ui::Theme).toString();
    if (qTheme.isEmpty())
    {
        // 该属性为空则设置一次即可
        // 避免在切换主题而程序没有重启的情况下基于最新的主题进行绘制
        qTheme = RaptorSettingSuite::invokeItemFind(Setting::Section::Ui,
                                                    Setting::Ui::Theme).toString();
        setProperty(Setting::Ui::Theme, qTheme);
    }

    if (qTheme == Setting::Ui::System)
    {
        if (RaptorUtil::invokeSystemDarkThemeConfirm())
        {
            qColor.setRed(255);
            qColor.setGreen(255);
            qColor.setBlue(255);
            qColor.setAlpha(209);
        }
        else
        {
            qColor.setRed(31);
            qColor.setGreen(34);
            qColor.setBlue(37);
            qColor.setAlpha(255);
        }
    }
    else if (qTheme == Setting::Ui::Light)
    {
        qColor.setRed(31);
        qColor.setGreen(34);
        qColor.setBlue(37);
        qColor.setAlpha(255);
    }
    else if (qTheme == Setting::Ui::Dark)
    {
        qColor.setRed(255);
        qColor.setGreen(255);
        qColor.setBlue(255);
        qColor.setAlpha(209);
    }

    auto qPainter = QPainter(viewport());
    qPainter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    const auto qRect = QRect(viewport()->width() / 2 - 40 - horizontalHeader()->offset(), (viewport()->height() - 260) / 2, 80, 80);
    _SvgRender->load(QStringLiteral("./Store/Image/%1[%2].svg").arg(APPLICATION_NAME).arg(_Code));
    if (_SvgRender->isValid())
    {
        _SvgRender->render(&qPainter, qRect);
    }

    auto qFont = qApp->font();
    qFont.setPixelSize(32);
    qPainter.setFont(qFont);
    qPainter.setPen(qColor);
    auto qFontMetrics = QFontMetrics(qFont);
    auto qWidth = qFontMetrics.horizontalAdvance(_Title);
    const auto qRecu = QRect(viewport()->width() / 2 - qWidth / 2 - horizontalHeader()->offset(), qRect.y() + qRect.height() + 16, qWidth, 50);
    qPainter.drawText(qRecu, Qt::AlignHCenter | Qt::AlignVCenter, _Title);

    qFont.setPixelSize(14);
    qPainter.setFont(qFont);
    qFontMetrics = QFontMetrics(qFont);
    qWidth = qFontMetrics.horizontalAdvance(_Summary);
    const auto qRecv = QRect(viewport()->width() / 2 - qWidth / 2 - horizontalHeader()->offset(), qRecu.y() + qRecu.height() + 4, qWidth, 22);
    qPainter.drawText(qRecv, Qt::AlignHCenter | Qt::AlignVCenter, _Summary);

    _Indicator->move(viewport()->width() / 2 - _Indicator->width() / 2 - horizontalHeader()->offset(), qRecv.y() + qRecv.height() + 48);
    _Indicator->setVisible(true);
    _Indicator->setEnabled(true);
}

void RaptorTableView::onIndicatorClicked()
{
    Q_EMIT indicatorClicked(_Code);
}