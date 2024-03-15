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

#ifndef RAPTORMESSAGEBOX_H
#define RAPTORMESSAGEBOX_H

#include <QKeyEvent>
#include <QStyle>

#include "../../../Common/RaptorDeclare.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Suite/Store/RaptorStoreSuite.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorMessageBox;
}

QT_END_NAMESPACE

class RaptorMessageBox Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    enum Operate
    {
        Nothing,
        Yes,
        No
    };

    Q_ENUM(Operate)

    explicit RaptorMessageBox(QWidget *qParent = Q_NULLPTR);

    ~RaptorMessageBox() Q_DECL_OVERRIDE;

    bool eventFilter(QObject *qObject, QEvent *qEvent) Q_DECL_OVERRIDE;

    static Operate invokeInformationEject(const QString &qTitle,
                                       const QString &qContent,
                                       const QString &qYesText = QStringLiteral("确定"),
                                       const QString &qNoText = QStringLiteral("取消"));

    static Operate invokeSuccessEject(const QString &qTitle,
                                   const QString &qYesText,
                                   const QString &qOKText = QStringLiteral("确定"),
                                   const QString &qNoText = QStringLiteral("取消"));

    static Operate invokeWarningEject(const QString &qTitle,
                                   const QString &qContent,
                                   const QString &qYesText = QStringLiteral("确定"),
                                   const QString &qNoText = QStringLiteral("取消"));

    static Operate invokeCriticalEject(const QString &qTitle,
                                    const QString &qContent,
                                    const QString &qYesText = QStringLiteral("确定"),
                                    const QString &qNoText = QStringLiteral("取消"));

private:
    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

    void invokeTitleSet(const QString &qTitle) const;

    void invokeContentSet(const QString &qContent) const;

    static Operate invokeEject(const QString &qTitle,
                            const QString &qContent,
                            const QString &qYesText,
                            const QString &qNoText,
                            const QString &qLevel);

private Q_SLOTS:
    Q_SLOT void onCloseClicked();

    Q_SLOT void onYesClicked();

    Q_SLOT void onNoClicked();

private:
    Ui::RaptorMessageBox *_Ui = Q_NULLPTR;
    Operate _Operate;
};

#endif // RAPTORMESSAGEBOX_H
