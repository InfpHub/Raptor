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

#ifndef RAPTORSTORESUITE_H
#define RAPTORSTORESUITE_H

#include <QApplication>
#include <QWidget>

#include "../../Common/RaptorDeclare.h"

class RaptorStoreSuite Q_DECL_FINAL
{
public:
    static bool invokeEngineStateGet();

    static void invokeEngineStateSet(const bool& qValue);

    static bool invokeUserIsValidConfirm();

    static void invokeUserLogout();

    static void invokeUserSwitchingSet(const bool& qValue);

    static RaptorSpace invokeSpaceGet();

    static void invokeSpaceSet(const RaptorSpace& qSpace);

    static void invokeUserSet(const RaptorAuthenticationItem& item);

    static RaptorAuthenticationItem invokeUserGet();

    static QWidget* invokeWorldGet();

    static void invokeEngineSet(QObject* qEngine);

    static QObject* invokeEngineGet();

    static void invokeSpacePageSet(QWidget* qWidget);

    static QWidget* invokeSpacePageGet();

    static void invokeViewPaintableSet(const bool& qValue);

    static bool invokeViewPaintableGet();

    static void invokeAriaIsLocalHostSet(const bool& qValue);

    static bool invokeAriaIsLocalHostGet();

private:
    static inline RaptorSpace _Space = Private;
    static inline RaptorAuthenticationItem _User;
    static inline bool _Switching = false;
    static inline bool _EngineState = false;
    static inline QWidget* _World = Q_NULLPTR;
    static inline QWidget* _SpacePage = Q_NULLPTR;
    static inline QObject* _Engine = Q_NULLPTR;
    static inline bool _ViewPaintable = true;
    static inline bool _AriaLocalHost = false;
};

#endif // RAPTORSTORESUITE_H