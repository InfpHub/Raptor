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

#ifndef RAPTORDOWNLOAD_H
#define RAPTORDOWNLOAD_H

#include <QButtonGroup>
#include <QRegExpValidator>
#include <QStandardPaths>

#include "../Toast/RaptorToast.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Suite/File/RaptorFileSuite.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorDownload;
}

QT_END_NAMESPACE

class RaptorDownload Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    explicit RaptorDownload(QWidget* qParent = Q_NULLPTR);

    ~RaptorDownload() Q_DECL_OVERRIDE;

    bool eventFilter(QObject* qObject, QEvent* qEvent) Q_DECL_OVERRIDE;

    void invokeEject(const QVariant& qVariant = QVariant()) Q_DECL_OVERRIDE;

private:
    void invokeInstanceInit() Q_DECL_OVERRIDE;

    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

    void invokeIconDrawing() const;

Q_SIGNALS:
    Q_SIGNAL void itemsDownloading(const QVariant& qVariant) const;

private Q_SLOTS:
    Q_SLOT void onCloseClicked();

    Q_SLOT void onSpeedNoLimitStateChanged(const int& qState) const;

    Q_SLOT void onDirSelectClicked();

    Q_SLOT void on3rdPartyEvokeClicked();

    Q_SLOT void onDownloadClicked();

    Q_SLOT void onCancelClicked();

private:
    QButtonGroup* _UnitGroup = Q_NULLPTR;
    QScopedPointer<QSvgRenderer> _SvgRender = QScopedPointer(new QSvgRenderer());
    QString _Icon;
    Ui::RaptorDownload* _Ui = Q_NULLPTR;
    QVariant _Variant;
};

#endif // RAPTORDOWNLOAD_H