/**
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

#ifndef RAPTORRENAME_H
#define RAPTORRENAME_H

#include "../Toast/RaptorToast.h"
#include "../../../Component/Eject/RaptorEject.h"
#include "../../../Delegate/Page/Common/RaptorTableViewDelegate.h"
#include "../../../Header/Page/Common/RaptorTableViewHeader.h"
#include "../../../Model/Page/Space/RaptorSpaceViewModel.h"
#include "../../../Util/RaptorUtil.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class RaptorRename;
}

QT_END_NAMESPACE

class RaptorRename Q_DECL_FINAL : public RaptorEject
{
    Q_OBJECT

public:
    explicit RaptorRename(QWidget *qParent = Q_NULLPTR);

    ~RaptorRename() Q_DECL_OVERRIDE;

    bool eventFilter(QObject *qObject, QEvent *qEvent) Q_DECL_OVERRIDE;

    void invokeEject(const QVariant &qVariant = QVariant()) Q_DECL_OVERRIDE;

private:
    struct Payload
    {
        QString _Parent;
        QVector<RaptorFileItem> _Items;
    };

    void invokeInstanceInit() Q_DECL_OVERRIDE;

    void invokeUiInit() Q_DECL_OVERRIDE;

    void invokeSlotInit() Q_DECL_OVERRIDE;

Q_SIGNALS:
    Q_SIGNAL void itemsRenaming(const QVariant &qVariant) const;

public Q_SLOTS:
    Q_SLOT void onTargetViewModelItemEdited(const QVariant &qVariant);

private Q_SLOTS:
    Q_SLOT void onCloseClicked();

    Q_SLOT void onOkClicked();

    Q_SLOT void onCancelClicked();

private:
    Ui::RaptorRename *_Ui = Q_NULLPTR;
    Payload _Payload;
    RaptorTableViewHeader *_ItemSourceViewHeader = Q_NULLPTR;
    RaptorSpaceViewModel *_ItemSourceViewModel = Q_NULLPTR;
    RaptorTableViewHeader *_ItemTargetViewHeader = Q_NULLPTR;
    RaptorSpaceViewModel *_ItemTargetViewModel = Q_NULLPTR;
    RaptorTableViewDelegate *_ItemSourceViewDelegate = Q_NULLPTR;
    RaptorTableViewDelegate *_ItemTargetViewDelegate = Q_NULLPTR;
};

#endif // RAPTORRENAME_H
