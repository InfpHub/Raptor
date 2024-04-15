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

#include "RaptorNotice.h"
#include "ui_RaptorNotice.h"

RaptorNotice::RaptorNotice(QWidget *qParent) : RaptorEject(qParent),
                                               _Ui(new Ui::RaptorNotice)
{
    _Ui->setupUi(this);
    invokeUiInit();
    invokeSlotInit();
}

RaptorNotice::~RaptorNotice()
{
    qFree(_Ui)
}

bool RaptorNotice::eventFilter(QObject *qObject, QEvent *qEvent)
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
            }
        }
    }

    if (qObject == _Ui->_NoticeIcon)
    {
        if (qEvent->type() == QEvent::Paint)
        {
            invokeIconDrawing();
            return true;
        }
    }

    return RaptorEject::eventFilter(qObject, qEvent);
}

void RaptorNotice::invokeEject(const QVariant &qVariant)
{
    const auto [qNewVersion, qId, qTitle, qContent] = qVariant.value<std::tuple<bool, QString, QString, QString> >();
    _Id = qId;
    if (!qNewVersion)
    {
        _Ui->_Download->setText(QStringLiteral("了解"));
        _Ui->_Skip->setVisible(false);
        _Ui->_Skip->setEnabled(false);
        disconnect(_Ui->_Download,
                   &QPushButton::clicked,
                   this,
                   &RaptorNotice::onDownloadClicked);
        connect(_Ui->_Download,
                &QPushButton::clicked,
                [=]() -> void
                {
                    close();
                });
    }

    _Ui->_Title->setText(QString(qCreativeTemplate).arg(qTitle));
    _Ui->_Content->setText(qContent);
    invokeCloseCallbackSet([=]() -> void
    {
        auto qSQLQuery = RaptorPersistenceSuite::invokeQueryGenerate();
        const auto qSQL = R"(INSERT INTO Notice("LeafId", "State") VALUES (:LeafId, :State))";
        qSQLQuery.prepare(qSQL);
        qSQLQuery.bindValue(":LeafId", _Id);
        if (_Ui->_Skip->isVisible())
        {
            qSQLQuery.bindValue(":State", -1);
        } else
        {
            qSQLQuery.bindValue(":State", 1);
        }

        qSQLQuery.exec(qSQL);
        if (qSQLQuery.lastError().isValid())
        {
            const auto qError = qSQLQuery.lastError().text();
            qCritical() << qError;
            RaptorToast::invokeCriticalEject(qError);
        }
    });
    RaptorEject::invokeEject(qVariant);
}

void RaptorNotice::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_Download->setText(QStringLiteral("下载"));
    _Ui->_Skip->setText(QStringLiteral("跳过"));
    _Ui->_NoticeIcon->installEventFilter(this);
}

void RaptorNotice::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorNotice::onCloseClicked);

    connect(_Ui->_Download,
            &QPushButton::clicked,
            this,
            &RaptorNotice::onDownloadClicked);

    connect(_Ui->_Skip,
            &QPushButton::clicked,
            this,
            &RaptorNotice::onSkipClicked);
}

void RaptorNotice::invokeIconDrawing() const
{
    auto qPainter = QPainter(_Ui->_NoticeIcon);
    _SvgRender->load(RaptorUtil::invokeIconMatch("Firework", false, true));
    if (_SvgRender->isValid())
    {
        _SvgRender->render(&qPainter, QRect(0, 0, _Ui->_NoticeIcon->width(), _Ui->_NoticeIcon->height()));
    }
}

void RaptorNotice::onCloseClicked()
{
    close();
}

void RaptorNotice::onDownloadClicked()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("%1/releases").arg(GITHUB)));
    close();
}

void RaptorNotice::onSkipClicked()
{
    close();
}
