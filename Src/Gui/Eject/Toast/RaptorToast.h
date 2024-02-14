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

#ifndef RAPTORTOAST_H
#define RAPTORTOAST_H

#include <QCloseEvent>
#include <QMetaEnum>
#include <QStyle>
#include <QSvgRenderer>
#include <QTimer>

#include "../../../Suite/Store/RaptorStoreSuite.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorToast;
}

QT_END_NAMESPACE

class RaptorToast Q_DECL_FINAL : public QWidget
{
    Q_OBJECT

public:
    explicit RaptorToast(QWidget* qParent = Q_NULLPTR);

    ~RaptorToast() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void show();

    static void invokeInformationEject(const QString& qContent,
                                       const int& qInterval = 2000);

    static void invokeSuccessEject(const QString& qContent,
                                   const int& qInterval = 2000);

    static void invokeWarningEject(const QString& qContent,
                                   const int& qInterval = 2000);

    static void invokeCriticalEject(const QString& qContent,
                                    const int& qInterval = 2000);

private:
    static void invokeEject(const QString& qContent,
                            const QString& qLevel,
                            const int& qInterval);

    void invokeInstanceInit();

    void invokeUiInit();

    void invokeSlotInit() const;

    void invokeContentSet(const QString& qContent) const;

    void invokeIconDrawing() const;

private Q_SLOTS:
    Q_SLOT void onCloseTimerTimeout();

private:
    Ui::RaptorToast* _Ui = Q_NULLPTR;
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());
    QTimer* _CloseTimer = Q_NULLPTR;
    static inline RaptorToast* _Instance = Q_NULLPTR;
};

#endif // RAPTORTOAST_H
