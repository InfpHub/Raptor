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

#include <QSharedMemory>

#include "Engine/RaptorEngine.h"

int main(int argc, char* argv[])
{
    const auto _App = QApplication(argc, argv);
    auto _Memory = QSharedMemory(APPLICATION_NAME);
    if (_Memory.attach())
    {
        return 0;
    }

    if (_Memory.create(sizeof(APPLICATION_NAME)))
    {
        const auto _Engine = RaptorEngine();
        _Engine.invokeStart();
        return QApplication::exec();
    }

    return 0;
}