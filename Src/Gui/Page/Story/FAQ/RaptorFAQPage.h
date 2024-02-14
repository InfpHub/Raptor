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

#ifndef RAPTORFAQPAGE_H
#define RAPTORFAQPAGE_H

#include <QFile>
#include <QWidget>

#include "../../../../Common/RaptorDeclare.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorFAQPage;
}

QT_END_NAMESPACE

class RaptorFAQPage Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorFAQPage(QWidget* qParent = Q_NULLPTR);

    ~RaptorFAQPage() Q_DECL_OVERRIDE;

private:
    void invokeUiInit() const;

private:
    Ui::RaptorFAQPage* _Ui = Q_NULLPTR;
};

#endif // RAPTORFAQPAGE_H