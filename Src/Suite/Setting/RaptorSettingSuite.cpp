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

RaptorSettingSuite *RaptorSettingSuite::invokeSingletonGet()
{
    return _SettingSuite();
}

QVariant RaptorSettingSuite::invokeItemFind(const QString &qSection,
                                            const QString &qKey)
{
    return _Config[qSection][qKey];
}

void RaptorSettingSuite::invokeItemSave(const QString &qSection,
                                        const QString &qKey,
                                        const QVariant &qVariant)
{
    _Config[qSection][qKey] = qVariant;
}

void RaptorSettingSuite::invokeStop()
{
    auto qEmitter = YAML::Emitter();
    qEmitter << YAML::BeginMap;
    auto qIterator = _Config.begin();
    while (qIterator != _Config.end())
    {
        const auto qKey = qIterator.key();
        const auto qValue = qIterator.value();
        qEmitter << YAML::Key << qKey.toStdString();
        qEmitter << YAML::Value << YAML::BeginMap;
        auto qIteratos = qValue.begin();
        while (qIteratos != qValue.end())
        {
            const auto qSubKey = qIteratos.key();
            const auto qSubValue = qIteratos.value();
            qEmitter << YAML::Key << qSubKey.toStdString();
            if (qKey == "Upload" && qSubKey == "Filter")
            {
                const auto items = qSubValue.value<QVector<QString> >();
                qEmitter << YAML::Value << YAML::BeginSeq;
                for (const QString &str: items)
                {
                    qEmitter << str.toStdString();
                }

                qEmitter << YAML::EndSeq;
            } else if ((qKey == "Download" || qKey == "Video" || qKey == "Office") && qSubKey == "Engine")
            {
                const auto items = qSubValue.value<QVector<RaptorSettingItem> >();
                qEmitter << YAML::Value << YAML::BeginSeq;
                for (auto &item: items)
                {
                    qEmitter << YAML::BeginMap;
                    qEmitter << YAML::Key << "Name" << YAML::Value << item._Name.toStdString();
                    qEmitter << YAML::Key << "Path" << YAML::Value << item._Path.toStdString();
                    qEmitter << YAML::Key << "Icon" << YAML::Value << item._Icon.toStdString();
                    qEmitter << YAML::EndMap;
                }
                qEmitter << YAML::EndSeq;
            } else
            {
                qEmitter << YAML::Value << qSubValue.toString().toStdString();
            }

            ++qIteratos;
        }

        qEmitter << YAML::EndMap;
        ++qIterator;
    }

    qEmitter << YAML::EndMap;
    auto qStream = std::ofstream(QStringLiteral("%1.yaml").arg(APPLICATION_NAME).toStdString(), std::ios::out | std::ios::binary);
    qStream << qEmitter.c_str();
    qStream.close();
}

void RaptorSettingSuite::invokeInstanceInit()
{
    try
    {
        _Yaml = YAML::LoadFile(QStringLiteral("%1.yaml").arg(APPLICATION_NAME).toStdString());
    } catch (const YAML::Exception &qException)
    {
        qFatal() << qException.msg;
    }
}

void RaptorSettingSuite::invokeLogicInit()
{
    if (_Yaml.IsMap())
    {
        for (const auto &item: _Yaml)
        {
            const auto qKey = QString::fromStdString(item.first.as<std::string>());
            if (const auto qSubValue = item.second;
                qSubValue.IsMap())
            {
                for (const auto &iten: qSubValue)
                {
                    const auto qSubKey = QString::fromStdString(iten.first.as<std::string>());
                    const auto qSubValuf = iten.second;
                    if (qSubValuf.IsSequence())
                    {
                        if ((qKey == "Download" || qKey == "Video" || qKey == "Office") && qSubKey == "Engine")
                        {
                            auto items = QVector<RaptorSettingItem>();
                            for (const auto &iteo: qSubValuf)
                            {
                                auto itep = RaptorSettingItem();
                                itep._Name = QString::fromStdString(iteo["Name"].as<std::string>());
                                itep._Path = QString::fromStdString(iteo["Path"].as<std::string>());
                                itep._Icon = QString::fromStdString(iteo["Icon"].as<std::string>());
                                items << itep;
                            }

                            _Config[qKey][qSubKey] = QVariant::fromValue<QVector<RaptorSettingItem> >(items);
                        } else if (qKey == "Upload")
                        {
                            auto items = QVector<QString>();
                            for (const auto &iteo: qSubValuf)
                            {
                                items << QString::fromStdString(iteo.as<std::string>());
                            }

                            _Config[qKey][qSubKey] = QVariant::fromValue<QVector<QString> >(items);
                        }

                        continue;
                    }

                    _Config[qKey][qSubKey] = QVariant::fromValue<QString>(
                        QString::fromStdString(qSubValuf.as<std::string>()));
                }
            }
        }
    }
}
