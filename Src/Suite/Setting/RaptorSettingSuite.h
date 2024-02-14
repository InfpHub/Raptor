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

#ifndef RAPTORSETTINGSUITE_H
#define RAPTORSETTINGSUITE_H

#include <QApplication>
#include <QDataStream>
#include <QDir>
#include <QObject>
#include <QSettings>
#include <QStandardPaths>

#include "../../Common/RaptorDeclare.h"

QT_BEGIN_NAMESPACE

namespace Setting
{
    namespace Section
    {
        Q_DECL_CONSTEXPR auto Ui = "Ui";
        Q_DECL_CONSTEXPR auto Download = "Download";
        Q_DECL_CONSTEXPR auto Upload = "Upload";
        Q_DECL_CONSTEXPR auto Play = "Play";
        Q_DECL_CONSTEXPR auto Network = "Network";
        Q_DECL_CONSTEXPR auto Other = "Other";
    }

    namespace Ui
    {
        Q_DECL_CONSTEXPR auto Theme = "Theme";
        Q_DECL_CONSTEXPR auto System = "System";
        Q_DECL_CONSTEXPR auto Light = "Light";
        Q_DECL_CONSTEXPR auto Dark = "Dark";
        Q_DECL_CONSTEXPR auto Font = "Font";
        Q_DECL_CONSTEXPR auto Sign = "Sign";
    }

    namespace Download
    {
        Q_DECL_CONSTEXPR auto Engine = "Engine";
        Q_DECL_CONSTEXPR auto ActiveEngine = "ActiveEngine";
        Q_DECL_CONSTEXPR auto IDM = "IDM";
        Q_DECL_CONSTEXPR auto FDM = "FDM";
        Q_DECL_CONSTEXPR auto Path = "Path";
        Q_DECL_CONSTEXPR auto DefaultPath = "DefaultPath";
        Q_DECL_CONSTEXPR auto FullPath = "FullPath";
        Q_DECL_CONSTEXPR auto SkipViolation = "SkipViolation";
        Q_DECL_CONSTEXPR auto AutoRename = "AutoRename";
        Q_DECL_CONSTEXPR auto IDMExecutable = "IDMExecutable";
        Q_DECL_CONSTEXPR auto FDMExecutable = "FDMExecutable";
        Q_DECL_CONSTEXPR auto Concurrent = "Concurrent";
    }

    namespace Upload
    {
        Q_DECL_CONSTEXPR auto Filter = "Filter";
        Q_DECL_CONSTEXPR auto SkipNoRapid = "SkipNoRapid";
        Q_DECL_CONSTEXPR auto Concurrent = "Concurrent";
    }

    namespace Play
    {
        Q_DECL_CONSTEXPR auto Engine = "Engine";
        Q_DECL_CONSTEXPR auto ActiveEngine = "ActiveEngine";
        Q_DECL_CONSTEXPR auto Quality = "Quality";
        Q_DECL_CONSTEXPR auto SD = "SD";
        Q_DECL_CONSTEXPR auto HD = "HD";
        Q_DECL_CONSTEXPR auto FHD = "FHD";
        Q_DECL_CONSTEXPR auto UHD = "UHD";
        Q_DECL_CONSTEXPR auto Origin = "Origin";
        Q_DECL_CONSTEXPR auto MPV = "MPV";
        Q_DECL_CONSTEXPR auto VLC = "VLC";
        Q_DECL_CONSTEXPR auto PotPlayer = "PotPlayer";
        Q_DECL_CONSTEXPR auto Stream = "Stream";
        Q_DECL_CONSTEXPR auto Quick = "Quick";
        Q_DECL_CONSTEXPR auto Live = "Live";
        Q_DECL_CONSTEXPR auto UseOrigin = "UseOrigin";
    }

    namespace Network
    {
        Q_DECL_CONSTEXPR auto Proxy = "Proxy";
        Q_DECL_CONSTEXPR auto None = "None";
        Q_DECL_CONSTEXPR auto HTTP = "HTTP";
        Q_DECL_CONSTEXPR auto HTTPS = "HTTPS";
        Q_DECL_CONSTEXPR auto HTTPS2 = "HTTPS2";
        Q_DECL_CONSTEXPR auto HTTP1_0 = "HTTP1_0";
        Q_DECL_CONSTEXPR auto SOCKS4 = "SOCKS4";
        Q_DECL_CONSTEXPR auto SOCKS5 = "SOCKS5";
        Q_DECL_CONSTEXPR auto ProxyEngine = "ProxyEngine";
        Q_DECL_CONSTEXPR auto ProxyHost = "ProxyHost";
        Q_DECL_CONSTEXPR auto ProxyPort = "ProxyPort";
        Q_DECL_CONSTEXPR auto ProxyUserName = "ProxyUserName";
        Q_DECL_CONSTEXPR auto ProxyPassword = "ProxyPassword";
    }

    namespace Other
    {
        Q_DECL_CONSTEXPR auto NewFolderTemplate = "NewFolderTemplate";
        Q_DECL_CONSTEXPR auto ShareExpire = "ShareExpire";
        Q_DECL_CONSTEXPR auto Week = "Week";
        Q_DECL_CONSTEXPR auto Month = "Month";
        Q_DECL_CONSTEXPR auto Year = "Year";
        Q_DECL_CONSTEXPR auto Never = "Never";
        Q_DECL_CONSTEXPR auto Random = "Random";
        Q_DECL_CONSTEXPR auto None = "None";
        Q_DECL_CONSTEXPR auto Custom = "Custom";
        Q_DECL_CONSTEXPR auto Password = "Password";
        Q_DECL_CONSTEXPR auto CustomPassword = "CustomPassword";
        Q_DECL_CONSTEXPR auto LinkTemplate = "LinkTemplate";
    }
}

QT_END_NAMESPACE

class RaptorSettingSuite Q_DECL_FINAL : public QObject
{
    Q_OBJECT

public:
    explicit RaptorSettingSuite();

    friend QDataStream& operator<<(QDataStream& qStream,
                                   const RaptorSettingItem& item);

    friend QDataStream& operator>>(QDataStream& qStream,
                                   RaptorSettingItem& item);

    static RaptorSettingSuite* invokeSingletonGet();

    static QVariant invokeItemFind(const QString& qSection,
                                   const QString& qKey);

    static void invokeItemSave(const QString& qSection,
                               const QString& qKey,
                               const QVariant& qVariant);

    QSettings* invokeINIInstanceGet() const;

    Q_INVOKABLE void invokeStop() const;

private:
    void invokeInstanceInit();

    void invokeLogicInit() const;

private:
    QSettings* _Settings = Q_NULLPTR;
    static inline QMap<QString, QVariantMap> _Config;
};

#endif // RAPTORSETTINGSUITE_H