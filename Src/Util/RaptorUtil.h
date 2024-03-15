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

#ifndef RAPTORUTIL_H
#define RAPTORUTIL_H

#include <QApplication>
#include <QCryptographicHash>
#include <QDate>
#include <QDir>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QProcess>
#include <QRandomGenerator>
#include <QScreen>
#include <QStandardItem>
#include <QStyleHints>
#include <QtMath>
#include <QWidget>

#include <secp256k1.h>
#include <sass.h>

#include "../Common/RaptorDeclare.h"
#include "../Suite/Setting/RaptorSettingSuite.h"
#include "../Suite/Store/RaptorStoreSuite.h"
#include "../Suite/Persistence/RaptorPersistenceSuite.h"

class RaptorUtil Q_DECL_FINAL
{
public:
    static QString invoke3rdPartyDownloaderEvoke(const QString &qUrl,
                                                 const QString &qDir,
                                                 const QString &qName);

    static QString invoke3rdPartyPlayerEvoke(const QString &qUrl, const QString &qSub);

    static QString invoke3rdPartyExcelEvoke(const QString &qUrl);

    static QString invoke3rdPartyPowerPointEvoke(const QString &qUrl);

    static QString invoke3rdPartyWordEvoke(const QString &qUrl);

    static QString invoke3rdPartyPDFEvoke(const QString &qUrl);

    static QVector<QString> invokeFontLoad();

    static QQueue<RaptorTimelineItem> invokeTimelineLoad();

    static QString invokeStyleSheetLoad(const QString &qValue);

    static QString invokeCompileTimestampCompute();

    static QSize invokePrimaryDesktopGeometryCompute();

    static void invokeItemLocate(const QString &qPath);

    static bool invokeSystemDarkThemeConfirm();

    static QString invokeIconMatch(const QString &qName,
                                   const bool &qFolder = false,
                                   const bool &qUi = false);

    static QString invoke1024SHA1Compute(const QString &qName);

    static QString invokeSHA1Compute(const QString &qName);

    static quint64 invokeChunkCompute(const quint64 &qValue);

    static QJsonArray invokeItemPartialCompute(const QString &qName);

    static QQueue<QPair<qint64, qint64> > invokeItemRangeCompute(const qint64 &qValue, const quint16& qCount);

    static QString invokeUUIDGenerate();

    static QString invokePasswordGenerate();

    static RaptorSession invokeDeviceSignature(const RaptorAuthenticationItem &item);

    static QString invokeStorageUnitConvert(const qint64 &qValue);

    static QString invokeTimeUnitConvert(const quint64 &qValue);

    static QString invokeTimestampConvert(const quint64 &qValue);

private:
    static inline QLocale _StorageUnitLocale;
};

#endif // RAPTORUTIL_H
