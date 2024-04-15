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

#include "RaptorEject.h"

#include "../../Suite/Store/RaptorStoreSuite.h"

RaptorEject::RaptorEject(QWidget *qParent) : QDialog(qParent)
{
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

bool RaptorEject::eventFilter(QObject *qObject, QEvent *qEvent)
{
    if (qObject == this)
    {
        if (qEvent->type() == QEvent::Close)
        {
            const auto qCloseEvent = static_cast<QCloseEvent *>(qEvent);
            if (windowOpacity() != 0.)
            {
                const auto qWidget = parentWidget();
                if (_Direction == Direction::Random)
                {
                    _Direction = static_cast<Direction>(QRandomGenerator::global()->bounded(4));
                }

                switch (_Direction)
                {
                    case Direction::T2B:
                        _StartAnimatioo->setStartValue(QPoint(
                            qWidget->x() + (qWidget->width() - width()) / 2,
                            qWidget->y() + (qWidget->height() - height()) / 2
                        ));

                        _StartAnimatioo->setEndValue(QPoint(
                            parentWidget()->x() + (qWidget->width() - width()) / 2,
                            qWidget->y() + (qWidget->height() - height()) / 2 - height() / 2
                        ));
                        break;
                    case Direction::L2R:
                        _StartAnimatioo->setStartValue(QPoint(
                            qWidget->x() + (qWidget->width() - width()) / 2,
                            qWidget->y() + (qWidget->height() - height()) / 2
                        ));

                        _StartAnimatioo->setEndValue(QPoint(
                            parentWidget()->x() + (qWidget->width() - width()) / 2 - width() / 2,
                            qWidget->y() + (qWidget->height() - height()) / 2
                        ));
                        break;
                    case Direction::B2T:
                        _StartAnimatioo->setStartValue(QPoint(
                            qWidget->x() + (qWidget->width() - width()) / 2,
                            qWidget->y() + (qWidget->height() - height()) / 2
                        ));

                        _StartAnimatioo->setEndValue(QPoint(
                            parentWidget()->x() + (qWidget->width() - width()) / 2,
                            qWidget->y() + (qWidget->height() - height()) / 2 +
                            height() / 2
                        ));
                        break;
                    case Direction::R2L:
                        _StartAnimatioo->setEndValue(QPoint(
                            parentWidget()->x() + (qWidget->width() - width()) / 2 + width() / 2,
                            qWidget->y() + (qWidget->height() - height()) / 2
                        ));

                        _StartAnimatioo->setStartValue(QPoint(
                            qWidget->x() + (qWidget->width() - width()) / 2,
                            qWidget->y() + (qWidget->height() - height()) / 2
                        ));
                        break;
                    default:
                        break;
                }

                _Direction = Direction::Random;
                const auto qEasingCurve = static_cast<QEasingCurve::Type>(QRandomGenerator::global()->bounded(40));
                _StartAnimatioo->setDuration(300);
                _StartAnimatioo->setEasingCurve(qEasingCurve);
                _StartAnimation->setStartValue(1);
                _StartAnimation->setEndValue(0);
                _StartAnimation->setDuration(300);
                _StartAnimation->setEasingCurve(qEasingCurve);
                QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(), "invokeMaskPaint", Q_ARG(bool, false));
                _StartAnimationGroup->start();
                qCloseEvent->ignore();
                return true;
            }

            qCloseEvent->accept();
            return true;
        }
    }
    return QDialog::eventFilter(qObject, qEvent);
}

int RaptorEject::exec()
{
    const auto qWidget = parentWidget();
    if (_Direction == Direction::Random)
    {
        _Direction = static_cast<Direction>(QRandomGenerator::global()->bounded(4));
    }

    switch (_Direction)
    {
        case Direction::T2B:
            _StartAnimatioo->setStartValue(QPoint(
                parentWidget()->x() + (qWidget->width() - width()) / 2,
                qWidget->y() + (qWidget->height() - height()) / 2 - height() / 2
            ));

            _StartAnimatioo->setEndValue(QPoint(
                qWidget->x() + (qWidget->width() - width()) / 2,
                qWidget->y() + (qWidget->height() - height()) / 2
            ));
            break;
        case Direction::L2R:
            _StartAnimatioo->setStartValue(QPoint(
                parentWidget()->x() + (qWidget->width() - width()) / 2 - width() / 2,
                qWidget->y() + (qWidget->height() - height()) / 2
            ));

            _StartAnimatioo->setEndValue(QPoint(
                qWidget->x() + (qWidget->width() - width()) / 2,
                qWidget->y() + (qWidget->height() - height()) / 2
            ));
            break;
        case Direction::B2T:
            _StartAnimatioo->setStartValue(QPoint(
                parentWidget()->x() + (qWidget->width() - width()) / 2,
                qWidget->y() + (qWidget->height() - height()) / 2 + height() / 2
            ));

            _StartAnimatioo->setEndValue(QPoint(
                qWidget->x() + (qWidget->width() - width()) / 2,
                qWidget->y() + (qWidget->height() - height()) / 2
            ));
            break;
        case Direction::R2L:
            _StartAnimatioo->setStartValue(QPoint(
                parentWidget()->x() + (qWidget->width() - width()) / 2 + width() / 2,
                qWidget->y() + (qWidget->height() - height()) / 2
            ));
            _StartAnimatioo->setEndValue(QPoint(
                qWidget->x() + (qWidget->width() - width()) / 2,
                qWidget->y() + (qWidget->height() - height()) / 2
            ));
            break;
        default:
            break;
    }

    _Direction = Direction::Random;
    const auto qEasingCurve = static_cast<QEasingCurve::Type>(QRandomGenerator::global()->bounded(40));
    _StartAnimatioo->setDuration(300);
    _StartAnimatioo->setEasingCurve(qEasingCurve);
    _StartAnimation->setStartValue(0);
    _StartAnimation->setEndValue(1);
    _StartAnimation->setDuration(300);
    _StartAnimation->setEasingCurve(qEasingCurve);
    QMetaObject::invokeMethod(RaptorStoreSuite::invokeWorldGet(), "invokeMaskPaint", Q_ARG(bool, true));
    _StartAnimationGroup->start();
    return QDialog::exec();
}

void RaptorEject::invokeEject(const QVariant &qVariant)
{
    exec();
}

void RaptorEject::invokeInstanceInit()
{
    _StartAnimatioo = new QPropertyAnimation(this, "pos", this);
    _StartAnimation = new QPropertyAnimation(this, "windowOpacity", this);
    _StartAnimationGroup = new QParallelAnimationGroup(this);
    _StartAnimationGroup->addAnimation(_StartAnimatioo);
    _StartAnimationGroup->addAnimation(_StartAnimation);
    _Direction = Direction::Random;
}

void RaptorEject::invokeUiInit()
{
    setWindowFlags(Qt::Dialog |
                   Qt::FramelessWindowHint |
                   Qt::NoDropShadowWindowHint);
    setWindowModality(Qt::WindowModal);
    setAttribute(Qt::WA_TranslucentBackground);
    installEventFilter(this);
}

void RaptorEject::invokeSlotInit()
{
    connect(_StartAnimationGroup,
            &QParallelAnimationGroup::finished,
            this,
            &RaptorEject::onStartAnimationGroupFinished);
}

void RaptorEject::invokeCloseCallbackSet(const std::function<void()> &qCallback)
{
    _CloseCallback = qCallback;
}

void RaptorEject::onStartAnimationGroupFinished()
{
    if (windowOpacity() == 0.)
    {
        if (_CloseCallback)
        {
            _CloseCallback();
        }

        close();
    }
}
