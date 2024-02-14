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

#include "RaptorStoreSuite.h"

bool RaptorStoreSuite::invokeEngineStateGet()
{
    return _EngineState;
}

void RaptorStoreSuite::invokeEngineStateSet(const bool& qValue)
{
    _EngineState = qValue;
}

bool RaptorStoreSuite::invokeUserIsValidConfirm()
{
    return !_User.isEmpty() && !_Switching;
}

void RaptorStoreSuite::invokeUserLogout()
{
    _User.clear();
}

void RaptorStoreSuite::invokeUserSwitchingSet(const bool& qValue)
{
    _Switching = qValue;
}

RaptorSpace RaptorStoreSuite::invokeSpaceGet()
{
    return _Space;
}

void RaptorStoreSuite::invokeSpaceSet(const RaptorSpace& qSpace)
{
    _Space = qSpace;
    _User._Space = _Space == Private ? _User._Private : _User._Public;
}

void RaptorStoreSuite::invokeUserSet(const RaptorAuthenticationItem& item)
{
    _User = item;
}

RaptorAuthenticationItem RaptorStoreSuite::invokeUserGet()
{
    return _User;
}

QWidget* RaptorStoreSuite::invokeWorldGet()
{
    if (_World)
    {
        return _World;
    }

    for (const auto& item : qApp->topLevelWidgets())
    {
        if (item->objectName() == "RaptorWorld")
        {
            _World = item;
            return item;
        }
    }

    return _World;
}

void RaptorStoreSuite::invokeEngineSet(QObject* qEngine)
{
    _Engine = qEngine;
}

QObject* RaptorStoreSuite::invokeEngineGet()
{
    return _Engine;
}

void RaptorStoreSuite::invokeViewPaintableSet(const bool& qValue)
{
    _ViewPaintable = qValue;
}

bool RaptorStoreSuite::invokeViewPaintableGet()
{
    return _ViewPaintable;
}