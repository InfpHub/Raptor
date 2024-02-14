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
#include <QPainter>
#include <QPixmap>
#include <QProcess>
#include <QRandomGenerator>
#include <QScreen>
#include <QStandardItem>
#include <QtMath>
#include <QWidget>

#if defined(STANDALONE)
#include <dsa.h>
#include <osrng.h>
#include <hex.h>
#include <eccrypto.h>
#include <oids.h>
#elif defined(INTEGRATED)
#include <secp256k1.h>
#endif

#include <sass.h>

#include "../Common/RaptorDeclare.h"
#include "../Suite/Blur/RaptorBlurSuite.h"
#include "../Suite/Setting/RaptorSettingSuite.h"
#include "../Suite/Store/RaptorStoreSuite.h"
#include "../Suite/Persistence/RaptorPersistenceSuite.h"

class RaptorUtil Q_DECL_FINAL
{
public:
    static QString invoke3rdPartyDownloaderEvoke(const QString& qUrl,
                                                 const QString& qDir,
                                                 const QString& qName);

    static QString invoke3rdPartyPlayerEvoke(const QString& qUrl);

    static QVector<QString> invokeFontLoad();

    static QQueue<RaptorTimelineItem> invokeTimelineLoad();

    static QString invokeStyleSheetLoad(const QString& qValue);

    static QString invokeCompileTimestampCompute();

    static QSize invokePrimaryDesktopGeometryCompute();

    static QPixmap invokeShadowGenerate(const QSize& qSize,
                                        const quint16& qMargin,
                                        const quint16& qRadius,
                                        const QColor& qColor);

    static void invokeItemLocate(const QString& qPath);

    static bool invokeSystemDarkThemeConfirm();

    static QString invokeIconMatch(const QString& qName,
                                   const bool& qFolder = false,
                                   const bool& qUi = false);

    static QString invoke1024SHA1Compute(const QString& qName);

    static QString invokeSHA1Compute(const QString& qName);

    static quint64 invokeChunkCompute(const quint64& qValue);

    static QJsonArray invokeItemPartialCompute(const QString& qName);

    static QString invokeUUIDGenerate();

    static QString invokePasswordGenerate();

    static RaptorSession invokeDeviceSignature(const RaptorAuthenticationItem& item);

    static QString invokeStorageUnitConvert(const qint64& qValue);

    static QString invokeTimeUnitConvert(const quint64& qValue);

    static QString invokeTimestampConvert(const quint64& qValue);

private:
    static inline QLocale _StorageUnitLocale;
};

#endif // RAPTORUTIL_H
