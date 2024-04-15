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

#ifndef RAPTORDECLARE_H
#define RAPTORDECLARE_H

#include <QJsonDocument>
#include <QModelIndexList>
#include <QQueue>

#include "RaptorDefine.h"

struct RaptorSession
{
    QString _Device;
    QString _Signature;
    QString _PrivateKey;
    QString _PublicKey;

    RaptorSession() Q_DECL_EQ_DEFAULT;

    RaptorSession(QString _Device,
                  QString _Signature,
                  QString _PrivateKey,
                  QString _PublicKey) : _Device(std::move(_Device)),
                                        _Signature(std::move(_Signature)),
                                        _PrivateKey(std::move(_PrivateKey)),
                                        _PublicKey(std::move(_PublicKey))
    {
    }
};

Q_DECLARE_METATYPE(RaptorSession)

struct RaptorQrCode
{
    quint64 _Timestamp;
    QString _Code;
    QString _Cookie;
};

Q_DECLARE_METATYPE(RaptorQrCode)

enum struct RaptorQrCodeStatus
{
    New,
    Scanned,
    Confirmed,
    Canceled,
    Expired
};

Q_DECLARE_METATYPE(RaptorQrCodeStatus)

struct RaptorCapacity
{
    QString _Total;
    QString _Used;
    QString _Radio;
    QString _Album;
    QString _Note;
};

Q_DECLARE_METATYPE(RaptorCapacity)

struct RaptorSignIn
{
    QString _LeafId;
    QString _Goods;

    [[nodiscard]]
    bool invokeItemSignInConfirm() const
    {
        return !_LeafId.isEmpty();
    }
};

Q_DECLARE_METATYPE(RaptorSignIn)

enum RaptorSpace
{
    Private,
    Public
};

Q_DECLARE_METATYPE(RaptorSpace)

struct RaptorDeviceItem
{
    QString _Name;
    QString _City;
    QString _Logined;
};

Q_DECLARE_METATYPE(RaptorDeviceItem)

struct RaptorAuthenticationItem
{
    QString _LeafId;
    QString _Space;
    QString _Private;
    QString _Public;
    QString _Description;
    QString _NickName;
    QByteArray _Avatar;
    QString _Meet;
    QString _AccessToken;
    QString _RefreshToken;
    RaptorSession _Session;
    RaptorCapacity _Capacity;
    RaptorSignIn _SignIn;
    bool _Active;
    bool _VIP;

    bool operator==(const RaptorAuthenticationItem &item) const
    {
        return _LeafId == item._LeafId;
    }

    bool operator!=(const RaptorAuthenticationItem &item) const
    {
        return _LeafId != item._LeafId;
    }

    [[nodiscard]]
    bool isEmpty() const
    {
        return _LeafId.isEmpty();
    }

    void clear()
    {
        _LeafId.clear();
        _AccessToken.clear();
    }
};

Q_DECLARE_METATYPE(RaptorAuthenticationItem)

struct RaptorInput
{
    QString _Id;
    QString _Parent;
    QString _Marker;
    QString _Type;
    QString _Category;
    QString _Name;
    QString _Description;
    quint16 _Preview = 0;
    quint16 _Save = 0;
    quint16 _Download = 0;
    quint16 _Parallel = 1;
    quint64 _Speed = 0;
    QString _Space;
    QString _Link;
    QString _Password;
    QString _Expire;
    QString _Token;
    QString _Path;
    QString _Icon;
    QString _Expired;
    QString _Url;
    QString _Dir;
    QModelIndex _Index;
    QModelIndexList _Indexes;
    bool _State;
    QVariant _Variant;
};

Q_DECLARE_METATYPE(RaptorInput)

struct RaptorFileItem
{
    QString _Id;
    QString _Icon;
    QString _Parent;
    QString _Name;
    QString _Dir;
    QString _Space;
    QString _Domain;
    QString _Extension;
    QString _Size;
    quint64 _Byte;
    QString _Type;
    QString _SHA1;
    QString _Created;
    QString _Updated;
    QString _Url;
    QString _Mime;

    bool operator==(const RaptorFileItem &item) const
    {
        return _Id == item._Id;
    }
};

Q_DECLARE_METATYPE(RaptorFileItem)

struct RaptorMediaItem : RaptorFileItem
{
    QString _CompilationId;
    QString _Thumbnail;
    QString _Description;
    quint16 _Number;
    qreal _Duration;
};

Q_DECLARE_METATYPE(RaptorMediaItem)

struct RaptorPartial
{
    QString _LeafId;
    quint16 _Number;
    QString _Url;
    qint64 _Offset = 0;
    qint64 _End = 0;
    qint64 _Transferred = 0;
    QString _Name;
    QString _Path;
    qint64 _Speed;
    qint64 _ETR;

    bool operator==(const RaptorPartial &item) const
    {
        return _LeafId == item._LeafId;
    }

    bool operator!=(const RaptorPartial &item) const
    {
        return _LeafId == item._LeafId;
    }
};

Q_DECLARE_METATYPE(RaptorPartial)

struct RaptorTransferItem
{
    QString _LeafId;
    QString _Id;
    QString _Parent;
    QString _Icon;
    QString _WorkerId;
    QString _Space;
    QString _Name;
    QString _Path;
    QString _1024Hash;
    QString _SHA1;
    QString _Created;
    QVariant _Speed;
    QString _Size;
    QString _Type;
    QString _Status;
    quint64 _Byte = 0;
    qint64 _Transferred = 0;
    quint16 _Parallel;
    QString _Url;
    quint64 _Limit = 0;
    QString _ETR;
    bool _Rapid;
    QQueue<RaptorPartial> _Partials;

    bool operator==(const RaptorTransferItem &item) const
    {
        return _LeafId == item._LeafId;
    }

    [[nodiscard]]
    bool isEmpty() const
    {
        return _LeafId.isEmpty();
    }
};

Q_DECLARE_METATYPE(RaptorTransferItem)

struct RaptorShareItem
{
    QString _Id;
    QString _Icon;
    QString _Name;
    QString _Description;
    QString _Size;
    QString _Created;
    QString _Updated;
    QString _Expired;
    quint16 _Save;
    quint16 _Preview;
    quint16 _Download;
    QString _Url;
    QString _Password;
};

Q_DECLARE_METATYPE(RaptorShareItem)

struct RaptorStarItem
{
    QString _Id;
    QString _Icon;
    QString _Name;
    QString _Size;
    QString _Created;
    QString _Updated;
    quint64 _Byte = 0;
};

Q_DECLARE_METATYPE(RaptorStarItem)

struct RaptorTrashItem
{
    QString _Id;
    QString _Icon;
    QString _Parent;
    QString _Name;
    QString _Space;
    QString _Domain;
    QString _Extension;
    QString _Size;
    QString _Type;
    QString _SHA1;
    QString _Created;
    QString _Updated;
    QString _Trashed;
    quint64 _Byte = 0;
    QString _Url;
};

Q_DECLARE_METATYPE(RaptorTrashItem)

struct RaptorTimelineItem
{
    bool _Active;
    QString _Date;
    QString _Summary;
    bool _Initial;
};

Q_DECLARE_METATYPE(RaptorTimelineItem)

struct RaptorSettingItem
{
    QString _Icon;
    QString _Name;
    QString _Path;

    bool operator==(const RaptorSettingItem &item) const
    {
        return _Name == item._Name;
    }
};

Q_DECLARE_METATYPE(RaptorSettingItem)

struct RaptorCopyWriter
{
    QString _Page;
    QString _Content;
};

Q_DECLARE_METATYPE(RaptorCopyWriter)

struct RaptorHttpPayload
{
    QString _Url;
    QMultiMap<QString, QVariant> _Header;
    QMultiMap<QString, QVariant> _Query;
    QMultiMap<QString, QVariant> _Form;
    QJsonDocument _Body;
    QByteArray _Content;
};

struct RaptorOutput
{
    bool _State;
    QString _Message;
    QVariant _Data;
};

Q_DECLARE_METATYPE(RaptorOutput)

#endif // RAPTORDECLARE_H
