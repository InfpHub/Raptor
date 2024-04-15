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

#ifndef RAPTORENGINE_H
#define RAPTORENGINE_H

#include <QFontDatabase>
#include <QObject>

#include "../Gui/World/RaptorWorld.h"
#include "../Suite/Aria/RaptorAriaSuite.h"
#include "../Suite/Authentication/RaptorAuthenticationSuite.h"
#include "../Suite/Clean/RaptorCleanSuite.h"
#include "../Suite/Media/RaptorMediaSuite.h"
#include "../Suite/Copilot/RaptorCopilotSuite.h"
#include "../Suite/File/RaptorFileSuite.h"
#include "../Suite/Downloading/RaptorDownloadingSuite.h"
#include "../Suite/Downloaded/RaptorDownloadedSuite.h"
#include "../Suite/Uploading/RaptorUploadingSuite.h"
#include "../Suite/Uploaded/RaptorUploadedSuite.h"
#include "../Suite/Share/RaptorShareSuite.h"
#include "../Suite/Star/RaptorStarSuite.h"
#include "../Suite/Trash/RaptorTrashSuite.h"
#include "../Suite/Copy/RaptorCopySuite.h"
#include "../Suite/Log/RaptorLogSuite.h"
#include "../Suite/Persistence/RaptorPersistenceSuite.h"
#include "../Suite/Setting/RaptorSettingSuite.h"

class RaptorEngine Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    explicit RaptorEngine(QObject *qParent = Q_NULLPTR);

    ~RaptorEngine() Q_DECL_OVERRIDE;

    void invokeStart() const;

    Q_INVOKABLE void invokeStop() const;

private:
    void invokeInstanceInit();

    void invokeSlotInit() const;

    static void invokeFontRegister();

    static void invokeMessageCallback(QtMsgType qType,
                                      const QMessageLogContext &qContext,
                                      const QString &qMessage);

private:
    QThread *_SuiteThread = Q_NULLPTR;
    RaptorSettingSuite *_SettingSuite = Q_NULLPTR;
    RaptorPersistenceSuite *_PersistenceSuite = Q_NULLPTR;
    RaptorAriaSuite *_AriaSuite = Q_NULLPTR;
    RaptorAuthenticationSuite *_AuthenticationSuite = Q_NULLPTR;
    RaptorFileSuite *_FileSuite = Q_NULLPTR;
    RaptorDownloadingSuite *_DownloadingSuite = Q_NULLPTR;
    RaptorDownloadedSuite *_DownloadedSuite = Q_NULLPTR;
    RaptorUploadingSuite *_UploadingSuite = Q_NULLPTR;
    RaptorUploadedSuite *_UploadedSuite = Q_NULLPTR;
    RaptorShareSuite *_ShareSuite = Q_NULLPTR;
    RaptorStarSuite *_StarSuite = Q_NULLPTR;
    RaptorStoreSuite *_StoreSuite = Q_NULLPTR;
    RaptorTrashSuite *_TrashSuite = Q_NULLPTR;
    RaptorMediaSuite* _MediaSuite = Q_NULLPTR;
    RaptorCopySuite *_CopySuite = Q_NULLPTR;
    RaptorCopilotSuite *_CopilotSuite = Q_NULLPTR;
    RaptorCleanSuite *_CleanSuite = Q_NULLPTR;
    RaptorWorld *_World = Q_NULLPTR;
};

#endif // RAPTORENGINE_H
