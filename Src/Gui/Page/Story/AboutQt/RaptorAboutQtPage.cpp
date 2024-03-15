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

#include "RaptorAboutQtPage.h"
#include "ui_RaptorAboutQtPage.h"

RaptorAboutQtPage::RaptorAboutQtPage(QWidget *qParent) : QWidget(qParent),
                                                         _Ui(new Ui::RaptorAboutQtPage)
{
    _Ui->setupUi(this);
    invokeUiInit();
}

RaptorAboutQtPage::~RaptorAboutQtPage()
{
    FREE(_Ui)
}

void RaptorAboutQtPage::invokeUiInit() const
{
    if (!_Ui->_Qt->text().isEmpty())
    {
        return;
    }

    if (auto qFile = QFile("./Store/Story/AboutQt.html");
        qFile.open(QIODevice::ReadOnly))
    {
        _Ui->_Qt->setText(QString::fromUtf8(qFile.readAll()).arg(qVersion()));
        qFile.close();
    }

    _Ui->_Qt->setOpenExternalLinks(true);
}
