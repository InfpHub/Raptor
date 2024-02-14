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

#include "RaptorFAQPage.h"
#include "ui_RaptorFAQPage.h"

RaptorFAQPage::RaptorFAQPage(QWidget* qParent) : QWidget(qParent),
                                                 _Ui(new Ui::RaptorFAQPage)
{
    _Ui->setupUi(this);
    invokeUiInit();
}

RaptorFAQPage::~RaptorFAQPage()
{
    FREE(_Ui)
}

void RaptorFAQPage::invokeUiInit() const
{
    if (auto qFile = QFile("./Store/Story/FAQ.html");
        qFile.open(QIODevice::ReadOnly))
    {
        _Ui->_FAQ->setText(qFile.readAll());
        qFile.close();
    }

    _Ui->_FAQ->setOpenExternalLinks(true);
}