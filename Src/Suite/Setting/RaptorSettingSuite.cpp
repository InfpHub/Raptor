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

#include "RaptorSettingSuite.h"

Q_GLOBAL_STATIC(RaptorSettingSuite, _SettingSuite);

RaptorSettingSuite::RaptorSettingSuite()
{
    invokeInstanceInit();
    invokeLogicInit();
}

QDataStream& operator<<(QDataStream& qStream, const RaptorSettingItem& item)
{
    qStream << item._Icon << item._Name << item._Path;
    return qStream;
}

QDataStream& operator>>(QDataStream& qStream, RaptorSettingItem& item)
{
    qStream >> item._Icon >> item._Name >> item._Path;
    return qStream;
}

RaptorSettingSuite* RaptorSettingSuite::invokeSingletonGet()
{
    return _SettingSuite();
}

QVariant RaptorSettingSuite::invokeItemFind(const QString& qSection,
                                            const QString& qKey)
{
    const auto qValue = _Config[qSection][qKey];
    if (qValue.isNull())
    {
        const auto qSettings = invokeSingletonGet()->invokeINIInstanceGet();
        qSettings->beginGroup(qSection);
        const auto qValuf = qSettings->value(qKey);
        qSettings->endGroup();
        invokeItemSave(qSection, qKey, qValuf);
        return qValuf;
    }

    return qValue;
}

void RaptorSettingSuite::invokeItemSave(const QString& qSection,
                                        const QString& qKey,
                                        const QVariant& qVariant)
{
    _Config[qSection][qKey] = qVariant;
}

QSettings* RaptorSettingSuite::invokeINIInstanceGet() const
{
    return _Settings;
}

void RaptorSettingSuite::invokeStop() const
{
    auto qIteraotr = _Config.begin();
    while (qIteraotr != _Config.end())
    {
        _Settings->beginGroup(qIteraotr.key());
        const auto value = qIteraotr.value();
        auto qIteraots = value.begin();
        while (qIteraots != value.end())
        {
            _Settings->setValue(qIteraots.key(), qIteraots.value());
            ++qIteraots;
        }

        _Settings->endGroup();
        ++qIteraotr;
    }
}

void RaptorSettingSuite::invokeInstanceInit()
{
    _Settings = new QSettings(QStringLiteral("%1.ini").arg(APPLICATION_NAME), QSettings::IniFormat, this);
    _Settings->setAtomicSyncRequired(true);
}

void RaptorSettingSuite::invokeLogicInit() const
{
    _Settings->setIniCodec("UTF-8");
    qRegisterMetaTypeStreamOperators<QVector<QString>>("QVector<QString>");
    qRegisterMetaTypeStreamOperators<QVector<RaptorSettingItem>>("QVector<RaptorSettingItem>");
    const auto items = _Settings->childGroups();
    for (auto& item : items)
    {
        _Settings->beginGroup(item);
        const auto itens = _Settings->allKeys();
        for (auto& iten : itens)
        {
            const auto qValue = _Settings->value(iten);
            _Config[item][iten] = qValue;
        }

        _Settings->endGroup();
    }
}
