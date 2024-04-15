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

#include "RaptorFont.h"
#include "ui_RaptorFont.h"

RaptorFont::RaptorFont(QWidget *qParent) : RaptorEject(qParent),
                                           _Ui(new Ui::RaptorFont)
{
    _Ui->setupUi(this);
    invokeInstanceInit();
    invokeUiInit();
    invokeSlotInit();
}

RaptorFont::~RaptorFont()
{
    qFree(_Ui)
}

bool RaptorFont::eventFilter(QObject *qObject, QEvent *qEvent)
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

    return RaptorEject::eventFilter(qObject, qEvent);
}

QString RaptorFont::invokeEject()
{
    _ItemViewModel->removeRows(0, _ItemViewModel->rowCount());
    _ItemViewModel->setStringList(QFontDatabase::families(QFontDatabase::SimplifiedChinese));
    RaptorEject::invokeEject();
    return _FontName;
}

void RaptorFont::invokeInstanceInit()
{
    RaptorEject::invokeInstanceInit();
    _ItemViewModel = new QStringListModel(this);
}

void RaptorFont::invokeUiInit()
{
    RaptorEject::invokeUiInit();
    installEventFilter(this);
    _Ui->_Title->setText(QStringLiteral("选择字体"));
    _Ui->_Close->setIcon(QIcon(RaptorUtil::invokeIconMatch("Close", false, true)));
    _Ui->_Close->setIconSize(QSize(10, 10));
    _Ui->_ItemView->setModel(_ItemViewModel);
    _Ui->_ItemView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _Ui->_ItemView->setContextMenuPolicy(Qt::NoContextMenu);
    _Ui->_ItemView->setSelectionMode(QAbstractItemView::SingleSelection);
    _Ui->_OK->setText(QStringLiteral("确定"));
}

void RaptorFont::invokeSlotInit()
{
    RaptorEject::invokeSlotInit();
    connect(_Ui->_Close,
            &QPushButton::clicked,
            this,
            &RaptorFont::onCloseClicked);

    connect(_Ui->_ItemView,
            &QListView::clicked,
            this,
            &RaptorFont::onItemViewClicked);

    connect(_Ui->_ItemView,
            &QListView::doubleClicked,
            this,
            &RaptorFont::onItemViewDoubleClicked);

    connect(_Ui->_OK,
            &QPushButton::clicked,
            this,
            &RaptorFont::onOKClicked);
}

void RaptorFont::onCloseClicked()
{
    _FontName.clear();
    close();
}

void RaptorFont::onItemViewClicked(const QModelIndex &qIndex)
{
    if (!qIndex.isValid())
    {
        return;
    }

    _FontName = qIndex.data().value<QString>();
}

void RaptorFont::onItemViewDoubleClicked(const QModelIndex &qIndex)
{
    if (!qIndex.isValid())
    {
        return;
    }

    _FontName = qIndex.data().value<QString>();
    close();
}

void RaptorFont::onOKClicked()
{
    if (_FontName.isEmpty())
    {
        RaptorToast::invokeCriticalEject(QStringLiteral("尚未选择任何字体!"));
        return;
    }

    onCloseClicked();
}
